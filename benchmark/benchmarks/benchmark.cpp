
#include <algorithm>
#include <cstdint>
#include <cstring>
#include <format>
#include <iostream>
#include <unordered_map>
#include <optional>
#include <print>
#include <random>
#include <set>
#include <string>
#include "gperf_neon.h"
#include "counters/bench.h"

// ============================================================================
// Filter support: --filter hits,make_perfect_map,protocol
//   Workloads: hits, misses, mixed
//   Methods:   make_perfect_map, naive, unordered_map, ankerl, absl, frozen, kronuz, gperf, pthash
//   Keysets:   protocol, stock, keyword, header, mime, jsreserved
// Omitting a category means "run all" for that category.
// ============================================================================

struct BenchFilter {
  std::set<std::string> workloads; // hits, misses, mixed
  std::set<std::string> methods;   // make_perfect_map, naive, unordered_map
  std::set<std::string> keysets;   // protocol, stock, keyword, header, mime

  bool run_workload(const std::string &w) const {
    return workloads.empty() || workloads.count(w);
  }
  bool run_method(const std::string &m) const {
    return methods.empty() || methods.count(m);
  }
  bool run_keyset(const std::string &k) const {
    return keysets.empty() || keysets.count(k);
  }
};

BenchFilter parse_filter(const std::string &arg) {
  BenchFilter f;
  static const std::set<std::string> valid_workloads = {"hits", "misses", "mixed"};
  static const std::set<std::string> valid_methods = {"naive", "unordered_map", "gperf"};
  static const std::set<std::string> valid_keysets = {"jsreserved"};

  size_t start = 0;
  while (start < arg.size()) {
    size_t end = arg.find(',', start);
    if (end == std::string::npos) end = arg.size();
    std::string token = arg.substr(start, end - start);
    if (valid_workloads.count(token))      f.workloads.insert(token);
    else if (valid_methods.count(token))   f.methods.insert(token);
    else if (valid_keysets.count(token))    f.keysets.insert(token);
    else std::println(stderr, "Warning: unknown filter token '{}'", token);
    start = end + 1;
  }
  return f;
}


// ============================================================================
// Infrastructure
// ============================================================================

template <class Function1, class Function2>
counters::event_aggregate shuffle_bench(Function1 &&function1,
                                        Function2 &&function2,
                                        size_t min_repeat = 300) {
  static thread_local counters::event_collector collector;
  auto fn = std::forward<Function1>(function1);
  auto fn2 = std::forward<Function2>(function2);
  counters::event_aggregate aggregate{};
  for (size_t i = 0; i < min_repeat; i++) {
    collector.start();
    fn();
    aggregate << collector.end();
    fn2();
  }
  return aggregate;
}

void pretty_print(const std::string &name, size_t num_values,
                  counters::event_aggregate agg) {
  std::print("  {:<48} : ", name);
  std::print(" {:6.3f} ns ", agg.fastest_elapsed_ns() / double(num_values));
  std::print(" {:5.2f} Gv/s", double(num_values) / agg.fastest_elapsed_ns());
  if (counters::has_performance_counters()) {
    std::print("  {:5.2f} c  {:5.2f} i  {:4.2f} i/c  {:4.2f} bm",
               agg.fastest_cycles() / double(num_values),
               agg.fastest_instructions() / double(num_values),
               agg.fastest_instructions() / double(agg.fastest_cycles()),
               agg.fastest_branch_misses() / double(num_values));
  }
  std::print("\n");
}

// Build a shuffled input vector from a pool of candidate strings.
std::vector<std::string_view> build_input(
    const std::vector<std::string_view> &pool, size_t count, uint64_t seed) {
  std::mt19937_64 gen(seed);
  std::vector<std::string_view> result;
  result.reserve(count);
  for (size_t i = 0; i < count; i++)
    result.push_back(pool[gen() % pool.size()]);
  return result;
}

// Generic benchmark: runs PHF, naive, and unordered_map on a given input vector.
template <typename NaiveFn, typename GperfFn>
void bench_workload(const std::string &label,
                    std::vector<std::string_view> &input,
                    size_t num_strings,
                    NaiveFn naive_fn,
                    GperfFn gperf_fn,
                    const BenchFilter &filter,
                    bool describe) {
  std::vector<int> results(num_strings, 0);
  std::mt19937_64 gen(42);
  auto shuffle = [&]() {
    std::shuffle(input.begin(), input.end(), gen);
  };

  // naive if/else
  if (filter.run_method("naive")) {
    gen.seed(42);
    auto naive_bench = [&]() {
      for (size_t i = 0; i < input.size(); i++) {
        auto opt = naive_fn(input[i]);
        if (opt) results[i] = *opt;
      }
    };
    pretty_print(label + " naive", num_strings,
                 shuffle_bench(naive_bench, shuffle));
    volatile auto sum = std::accumulate(results.begin(), results.end(), 0); // prevent optimization
  }


  // gperf (GNU perfect hash)
  if (filter.run_method("gperf")) {
    gen.seed(42);
    auto gperf_bench = [&]() {
      for (size_t i = 0; i < input.size(); i++) {
        auto result = gperf_fn(input[i]);
        if (result) results[i] = *result;
      }
    };
    pretty_print(label + " gperf", num_strings,
                 shuffle_bench(gperf_bench, shuffle));
    volatile auto sum = std::accumulate(results.begin(), results.end(), 0); // prevent optimization
  }
}

std::optional<int> jsreserved_naive(std::string_view s) {
  if (s == "await") return 0;     if (s == "break") return 1;
  if (s == "case") return 2;      if (s == "catch") return 3;
  if (s == "class") return 4;     if (s == "const") return 5;
  if (s == "continue") return 6;  if (s == "debugger") return 7;
  if (s == "default") return 8;   if (s == "delete") return 9;
  if (s == "do") return 10;       if (s == "else") return 11;
  if (s == "enum") return 12;     if (s == "export") return 13;
  if (s == "extends") return 14;  if (s == "false") return 15;
  if (s == "finally") return 16;  if (s == "for") return 17;
  if (s == "function") return 18; if (s == "if") return 19;
  if (s == "import") return 20;   if (s == "in") return 21;
  if (s == "instanceof") return 22; if (s == "new") return 23;
  if (s == "null") return 24;     if (s == "return") return 25;
  if (s == "super") return 26;    if (s == "switch") return 27;
  if (s == "this") return 28;     if (s == "throw") return 29;
  if (s == "true") return 30;     if (s == "try") return 31;
  if (s == "typeof") return 32;   if (s == "var") return 33;
  if (s == "void") return 34;     if (s == "while") return 35;
  if (s == "with") return 36;     if (s == "yield") return 37;
  if (s == "implements") return 38; if (s == "interface") return 39;
  if (s == "package") return 40;  if (s == "private") return 41;
  if (s == "protected") return 42; if (s == "public") return 43;
  if (s == "static") return 44;
  return std::nullopt;
}

auto gperf_jsreserved_fn = [](std::string_view s) -> std::optional<int> {
  char buf[16] = {0};
  size_t copy_len = std::min(s.size(), size_t(16));
  std::memcpy(buf, s.data(), copy_len);
  auto result = JsReservedGperf::lookup(buf, s.size());
  if (result) return static_cast<int>(*result);
  return std::nullopt;
};

// Run a full key-set benchmark with all three workload modes.
template <typename NaiveFn, typename GperfFn>
void run_keyset(const std::string &name,
                NaiveFn naive_fn,
                GperfFn gperf_fn,
                const std::vector<std::string_view> &hit_keys,
                const std::vector<std::string_view> &miss_keys,
                const BenchFilter &filter,
                bool describe,
                size_t num_strings = 200000) {

  // Compute MaxKeyLen for display
  size_t max_len = 0;
  for (auto &k : hit_keys) max_len = std::max(max_len, k.size());

  std::println("\n=== {} (N={}, MaxKeyLen={}) ===", name, hit_keys.size(), max_len);

  // Build unordered_map baseline
  std::unordered_map<std::string_view, int> uset;
  for (size_t i = 0; i < hit_keys.size(); i++)
    uset[hit_keys[i]] = static_cast<int>(i);

  // Build workload vectors
  auto hits   = build_input(hit_keys, num_strings, 42);
  auto misses = build_input(miss_keys, num_strings, 42);

  // Mixed: 50/50 interleaved
  std::vector<std::string_view> mixed_pool;
  mixed_pool.insert(mixed_pool.end(), hit_keys.begin(), hit_keys.end());
  mixed_pool.insert(mixed_pool.end(), miss_keys.begin(), miss_keys.end());
  auto mixed = build_input(mixed_pool, num_strings, 42);

  if (filter.run_workload("hits")) {
    std::println("  --- all hits ---");
    bench_workload("hits  ", hits, num_strings, jsreserved_naive, gperf_jsreserved_fn, filter, describe);
  }
  if (filter.run_workload("misses")) {
    std::println("  --- all misses ---");
    bench_workload("misses", misses, num_strings, jsreserved_naive, gperf_jsreserved_fn, filter, describe);
  }
  if (filter.run_workload("mixed")) {
    std::println("  --- mixed (50/50) ---");
    bench_workload("mixed ", mixed, num_strings, jsreserved_naive, gperf_jsreserved_fn, filter, describe);
  }
}


// ============================================================================
// Verification
// ============================================================================

template <typename NaiveFn, typename GperfFn>
bool verify_keyset(const std::string &name,
                   NaiveFn naive_fn,
                   GperfFn gperf_fn,
                   const std::vector<std::string_view> &hit_keys,
                   const std::vector<std::string_view> &miss_keys) {
  bool ok = true;
  auto fail = [&](const std::string &msg) {
    std::println(stderr, "  FAIL [{}]: {}", name, msg);
    ok = false;
  };

  // Build reference map
  std::unordered_map<std::string_view, int> ref;
  for (size_t i = 0; i < hit_keys.size(); i++)
    ref[hit_keys[i]] = static_cast<int>(i);

  // Mixed pool: all hits then all misses
  std::vector<std::string_view> mixed_pool;
  mixed_pool.insert(mixed_pool.end(), hit_keys.begin(), hit_keys.end());
  mixed_pool.insert(mixed_pool.end(), miss_keys.begin(), miss_keys.end());

  for (auto &key : mixed_pool) {
    auto ref_it = ref.find(key);
    bool is_hit = ref_it != ref.end();
    int ref_val = is_hit ? ref_it->second : -1;

    // naive
    auto naive_result = naive_fn(key);
    if (is_hit) {
      if (!naive_result)
        fail(std::format("naive: missed hit key '{}'", key));
      else if (*naive_result != ref_val)
        fail(std::format("naive: key '{}' got {} expected {}", key, *naive_result, ref_val));
    } else {
      if (naive_result)
        fail(std::format("naive: false positive for miss key '{}'", key));
    }

    // gperf (returns first char of matched string, only checks hit/miss for value)
    auto gperf_result = gperf_fn(key);
    if (is_hit) {
      if (!gperf_result)
        fail(std::format("gperf: missed hit key '{}'", key));
    } else {
      if (gperf_result)
        fail(std::format("gperf: false positive for miss key '{}'", key));
    }


  }

  std::println("  {} {}", ok ? "OK" : "FAIL", name);
  return ok;
}

// ============================================================================
// Main
// ============================================================================

int main(int argc, char *argv[]) {
  if (!counters::has_performance_counters())
    std::println("Performance counters not available, run with sudo.");

  BenchFilter filter;
  bool verify = false;
  bool describe = false;
  for (int i = 1; i < argc; i++) {
    std::string arg = argv[i];
    if (arg == "--help" || arg == "-h") {
      std::println("Usage: {} [OPTIONS]\n", argv[0]);
      std::println("Options:");
      std::println("  --filter <tokens>  Comma-separated list of filter tokens");
      std::println("  --verify, -V       Verify all maps return correct results");
      std::println("  --describe, -D     Print algorithm descriptions");
      std::println("  --help, -h         Show this help message\n");
      std::println("Filter tokens (mix and match):");
      std::println("  Workloads: hits, misses, mixed");
      std::println("  Methods:   make_perfect_map, naive, unordered_map, ankerl, absl, frozen, kronuz, gperf, pthash");
      std::println("  Keysets:   protocol, stock, keyword, header, mime, letters, headers50, jsreserved\n");
      std::println("Omitting a category runs all values for that category.\n");
      std::println("Examples:");
      std::println("  {} --filter hits,make_perfect_map,protocol", argv[0]);
      std::println("  {} --filter hits,misses,stock", argv[0]);
      std::println("  {} --verify", argv[0]);
      return 0;
    }
    if (arg == "--verify" || arg == "-V") {
      verify = true;
    }
    if (arg == "--describe" || arg == "-D") {
      describe = true;
    }
    if (arg == "--filter" && i + 1 < argc) {
      filter = parse_filter(argv[++i]);
    }
  }

  // --- JavaScript Reserved Words (45 keys, medium) ---
  if (filter.run_keyset("jsreserved")) {
    run_keyset("JavaScript Reserved Words", jsreserved_naive, gperf_jsreserved_fn,
      {"await","break","case","catch","class","const","continue","debugger","default","delete",
       "do","else","enum","export","extends","false","finally","for","function","if",
       "import","in","instanceof","new","null","return","super","switch","this","throw",
       "true","try","typeof","var","void","while","with","yield","implements","interface",
       "package","private","protected","public","static"},
      {"let","console","alert","document","window","Array","Object","String","Number","Boolean",
       "undefined","NaN","Infinity","Math","Date","RegExp","JSON","Promise","async","arguments"},
      filter, describe);
  }
}
