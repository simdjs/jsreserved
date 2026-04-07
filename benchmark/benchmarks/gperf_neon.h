/**
 * Modified gperf output for JavaScript reserved words,
 * optimized for ARM NEON by Daniel Lemire.
 */
#include <cstring>
#include <optional>
#if defined(__aarch64__)
#include <arm_neon.h>
#include <cstdint>
#endif
/* maximum key range = 64, duplicates = 0 */

class JsReservedGperf
{
public:
  enum class Token {
    AWAIT = 0,
    BREAK = 1,
    CASE = 2,
    CATCH = 3,
    CLASS = 4,
    CONST = 5,
    CONTINUE = 6,
    DEBUGGER = 7,
    DEFAULT = 8,
    DELETE = 9,
    DO = 10,
    ELSE = 11,
    ENUM = 12,
    EXPORT = 13,
    EXTENDS = 14,
    FALSE = 15,
    FINALLY = 16,
    FOR = 17,
    FUNCTION = 18,
    IF = 19,
    IMPORT = 20,
    IN = 21,
    INSTANCEOF = 22,
    NEW = 23,
    NULL_ = 24,
    RETURN = 25,
    SUPER = 26,
    SWITCH = 27,
    THIS = 28,
    THROW = 29,
    TRUE = 30,
    TRY = 31,
    TYPEOF = 32,
    VAR = 33,
    VOID = 34,
    WHILE = 35,
    WITH = 36,
    YIELD = 37,
    IMPLEMENTS = 38,
    INTERFACE = 39,
    PACKAGE = 40,
    PRIVATE = 41,
    PROTECTED = 42,
    PUBLIC = 43,
    STATIC = 44
  };
private:
  static inline unsigned int hash (const char *str, unsigned int len);
public:
  static std::optional<Token> lookup (const char *str, unsigned int len);
};

inline unsigned int
JsReservedGperf::hash (const char *str, unsigned int len)
{
  static const unsigned char asso_values[] =
    {
      66, 66, 66, 66, 66, 66, 66, 66, 66, 66,
      66, 66, 66, 66, 66, 66, 66, 66, 66, 66,
      66, 66, 66, 66, 66, 66, 66, 66, 66, 66,
      66, 66, 66, 66, 66, 66, 66, 66, 66, 66,
      66, 66, 66, 66, 66, 66, 66, 66, 66, 66,
      66, 66, 66, 66, 66, 66, 66, 66, 66, 66,
      66, 66, 66, 66, 66, 66, 66, 66, 66, 66,
      66, 66, 66, 66, 66, 66, 66, 66, 66, 66,
      66, 66, 66, 66, 66, 66, 66, 66, 66, 66,
      66, 66, 66, 66, 66, 66, 66, 15, 40,  0,
       0,  0, 10, 66, 25,  0, 66, 66, 55,  5,
       0, 20,  5, 66, 20, 10, 20, 20, 30, 25,
      40, 35, 66, 66, 66, 66, 66, 66, 66, 66,
      66, 66, 66, 66, 66, 66, 66, 66, 66, 66,
      66, 66, 66, 66, 66, 66, 66, 66, 66, 66,
      66, 66, 66, 66, 66, 66, 66, 66, 66, 66,
      66, 66, 66, 66, 66, 66, 66, 66, 66, 66,
      66, 66, 66, 66, 66, 66, 66, 66, 66, 66,
      66, 66, 66, 66, 66, 66, 66, 66, 66, 66,
      66, 66, 66, 66, 66, 66, 66, 66, 66, 66,
      66, 66, 66, 66, 66, 66, 66, 66, 66, 66,
      66, 66, 66, 66, 66, 66, 66, 66, 66, 66,
      66, 66, 66, 66, 66, 66, 66, 66, 66, 66,
      66, 66, 66, 66, 66, 66, 66, 66, 66, 66,
      66, 66, 66, 66, 66, 66, 66, 66, 66, 66,
      66, 66, 66, 66, 66, 66
    };
  return len + asso_values[(unsigned char)str[1]] + asso_values[(unsigned char)str[0]];
}

std::optional<JsReservedGperf::Token>
JsReservedGperf::lookup (const char *str, unsigned int len)
{
  enum
    {
      TOTAL_KEYWORDS = 45,
      MIN_WORD_LENGTH = 2,
      MAX_WORD_LENGTH = 10,
      MIN_HASH_VALUE = 2,
      MAX_HASH_VALUE = 65
    };

  static const std::optional<Token> token_map[66] = {
    std::nullopt, // 0
    std::nullopt, // 1
    Token::IN, // 2
    Token::NEW, // 3
    Token::ENUM, // 4
    std::nullopt, // 5
    Token::DELETE, // 6
    Token::DEFAULT, // 7
    Token::DEBUGGER, // 8
    Token::INTERFACE, // 9
    Token::INSTANCEOF, // 10
    Token::IMPORT, // 11
    Token::IF, // 12
    std::nullopt, // 13
    std::nullopt, // 14
    Token::IMPLEMENTS, // 15
    std::nullopt, // 16
    Token::FINALLY, // 17
    std::nullopt, // 18
    Token::CASE, // 19
    Token::CATCH, // 20
    std::nullopt, // 21
    Token::DO, // 22
    std::nullopt, // 23
    Token::NULL_, // 24
    Token::CONST, // 25
    Token::RETURN, // 26
    Token::PACKAGE, // 27
    Token::CONTINUE, // 28
    Token::WITH, // 29
    Token::FALSE, // 30
    Token::PUBLIC, // 31
    Token::PRIVATE, // 32
    Token::FOR, // 33
    Token::PROTECTED, // 34
    Token::SUPER, // 35
    Token::STATIC, // 36
    std::nullopt, // 37
    Token::FUNCTION, // 38
    std::nullopt, // 39
    Token::YIELD, // 40
    Token::SWITCH, // 41
    std::nullopt, // 42
    Token::TRY, // 43
    Token::TRUE, // 44
    Token::AWAIT, // 45
    Token::EXPORT, // 46
    Token::EXTENDS, // 47
    Token::VAR, // 48
    Token::THIS, // 49
    Token::THROW, // 50
    std::nullopt, // 51
    std::nullopt, // 52
    std::nullopt, // 53
    Token::VOID, // 54
    Token::WHILE, // 55
    std::nullopt, // 56
    std::nullopt, // 57
    std::nullopt, // 58
    Token::ELSE, // 59
    Token::CLASS, // 60
    Token::TYPEOF, // 61
    std::nullopt, // 62
    std::nullopt, // 63
    std::nullopt, // 64
    Token::BREAK // 65
  };

  static const char wordlist[][16] =
    {
      "", "",
      "in",
      "new",
      "enum",
      "",
      "delete",
      "default",
      "debugger",
      "interface",
      "instanceof",
      "import",
      "if",
      "", "",
      "implements",
      "",
      "finally",
      "",
      "case",
      "catch",
      "",
      "do",
      "",
      "null",
      "const",
      "return",
      "package",
      "continue",
      "with",
      "false",
      "public",
      "private",
      "for",
      "protected",
      "super",
      "static",
      "",
      "function",
      "",
      "yield",
      "switch",
      "",
      "try",
      "true",
      "await",
      "export",
      "extends",
      "var",
      "this",
      "throw",
      "", "", "",
      "void",
      "while",
      "", "", "",
      "else",
      "class",
      "typeof",
      "", "", "",
      "break"
    };

  if (len <= MAX_WORD_LENGTH && len >= MIN_WORD_LENGTH)
    {
      unsigned int key = hash (str, len);

      if (key <= MAX_HASH_VALUE)
        {
#if defined(__aarch64__) 
          static constexpr std::uint8_t masks[11][16] = {
              {0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80},
              {0,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80},
              {0,1,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80},
              {0,1,2,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80},
              {0,1,2,3,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80},
              {0,1,2,3,4,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80},
              {0,1,2,3,4,5,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80},
              {0,1,2,3,4,5,6,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80},
              {0,1,2,3,4,5,6,7,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80},
              {0,1,2,3,4,5,6,7,8,0x80,0x80,0x80,0x80,0x80,0x80,0x80},
              {0,1,2,3,4,5,6,7,8,9,0x80,0x80,0x80,0x80,0x80,0x80},
          };
          uint8x16_t raw = vld1q_u8(reinterpret_cast<const uint8_t*>(str));
          uint8x16_t mask_vec = vld1q_u8(masks[len]);
          uint8x16_t input = vqtbl1q_u8(raw, mask_vec);
          uint8x16_t stored_vec = vld1q_u8(reinterpret_cast<const uint8_t*>(wordlist[key]));
          uint8x16_t cmp = vceqq_u8(input, stored_vec);
          uint8x8_t narrowed = vshrn_n_u16(vreinterpretq_u16_u8(cmp), 4);
          if (vget_lane_u64(vreinterpret_u64_u8(narrowed), 0) == ~0ULL)
            return token_map[key];
#else
          const char *s = wordlist[key];

          if (*str == *s && !strncmp (str + 1, s + 1, len - 1) && s[len] == '\0')
            return token_map[key];
#endif
        }
    }
  return std::nullopt;
}