// because the test framework depends on the utils their testing must be
// bootstrapped with this simplified test framework

#include <util/fmt.hpp>

namespace detail {
  static constexpr const char* cleanp(const char* const str, const char* last_slash) {
    return
      *str == '\0' ? last_slash :
      *str == '/'  ? cleanp(str + 1, str + 1) :
      cleanp(str + 1, last_slash);
  }
}

#define __SFILE__ ({constexpr const char* sf__ {detail::cleanp(__FILE__, __FILE__)}; sf__;})
#define ERR(...) fmt::println(std::cerr, "{}:{}\t{}", __SFILE__, __LINE__, fmt::format(__VA_ARGS__));

#define ASSERT_TRUE(a)                                  \
  do {                                                  \
    if ((a) != true) {                                  \
      ERR("{} must be true", #a);                       \
      return 1;                                         \
    }                                                   \
  } while(0)

#define ASSERT_FALSE(a)                                  \
  do {                                                   \
    if ((a) != false) {                                  \
      ERR("{} must be false", #a);                       \
      return 1;                                          \
    }                                                    \
  } while(0)

#define ASSERT_EQ(a, b)                               \
  do {                                                \
    if ((a) != (b)) {                                 \
      ERR("{} != {}", #a, #b);                        \
      ERR("{} != {}", a, b);                        \
      return 1;                                   		\
    }                                                 \
  } while(0)

#define ASSERT_NEQ(a, b)                              \
  do {                                                \
    if ((a) == (b)) {                                 \
      ERR("{} == {}", #a, #b);                        \
      return 1;                                   		\
    }                                                 \
  } while(0)

#define SHOULD_THROW(x)                             \
  do {                                              \
    try {                                           \
      (x);                                          \
    } catch(...) {                                  \
      break;                                        \
    }                                               \
    ERR("{} must throw", #x);                       \
    return 1;                                       \
  } while(0)
