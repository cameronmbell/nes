// fmt.hpp -- in-string formatted replacement
// allow for python style string formatting instead of c style printf
// with pretty printing of STL iterable containers, once again like python
// this doesnt do: compile-time argument checking or paramter formatting
// because they're mostly unnecciary but may be added later when I see fit
// attribution to tinyformat.h and libfmt, this is derivative of their work

#ifndef _FMT_HPP
#define _FMT_HPP

#include <type_traits>
#include <iostream>
#include <sstream>
#include <vector>

#include <unordered_map>
#include <unordered_set>
#include <set>
#include <map>

namespace fmt {
  template <typename T>
  struct Formatter;

  namespace detail {
    using SFINAEyes=std::true_type;
    using SFINAEno=std::false_type;

    template <typename C>
    struct IsStreamable {
      template <typename T> static constexpr auto test_output_streamop(int)
        -> decltype(std::declval<std::ostream&>() << std::declval<T>(), SFINAEyes());

      template <typename> static constexpr SFINAEno test_output_streamop(...);

      static constexpr bool value = decltype(test_output_streamop<C>(0))::value;
    };

    template <typename C>
    struct IsContainer {
      template <typename T> static constexpr SFINAEyes test_const_iterator(typename T::const_iterator*);

      template <typename T> static constexpr auto test_begin(int)
        -> decltype(std::declval<T>().begin(), SFINAEyes());

      template <typename T> static auto test_end(int)
        -> decltype(std::declval<T>().end(), SFINAEyes());

      template <typename> static constexpr SFINAEno test_const_iterator(...);
      template <typename> static constexpr SFINAEno test_begin(...);
      template <typename> static constexpr SFINAEno test_end(...);

      static constexpr bool value =
           decltype(test_const_iterator<C>(0))::value
        && decltype(test_begin<C>(0))::value
        && decltype(test_end<C>(0))::value;
    };

    template <typename T1, typename T2>
    struct IsContainer<std::pair<T1, T2>> {
      static constexpr bool value = true;
    };

    enum class PrintMode {
      ContainerPrettyPrint,
      OStreamDefault,
      Unprintable
    };

    template <typename T>
    struct FindPrintMode {
      static constexpr PrintMode value =
        (IsStreamable<typename std::decay<T>::type>::value)? PrintMode::OStreamDefault :
        (IsContainer<typename std::decay<T>::type>::value)?  PrintMode::ContainerPrettyPrint :
                                                             PrintMode::Unprintable;
    };

    struct PrintPadding {
      using type=struct {
        const char* lhs;
        const char* ins;
        const char* rhs;
      };

      template <typename T>
      struct ValueHelper { static constexpr type value = { "[", ", ", "]" }; };
      template <typename T, typename Comp, typename Alloc>
      struct ValueHelper<std::set<T, Comp, Alloc>> { static constexpr type value = { "{", ", ", "}" }; };
      template <typename T, typename Comp, typename Alloc>
      struct ValueHelper<std::multiset<T, Comp, Alloc>> { static constexpr type value = { "{", ", ", "}" }; };
      template <typename T, typename Hash, typename Equal, typename Alloc>
      struct ValueHelper<std::unordered_set<T, Hash, Equal, Alloc>> { static constexpr type value = { "{", ", ", "}" }; };
      template <typename T, typename Hash, typename Equal, typename Alloc>
      struct ValueHelper<std::unordered_multiset<T, Hash, Equal, Alloc>> { static constexpr type value = { "{", ", ", "}" }; };
      template <typename Key, typename T, typename Comp, typename Alloc>
      struct ValueHelper<std::map<Key, T, Comp, Alloc>> { static constexpr type value = { "{", ", ", "}" }; };
      template <typename Key, typename T, typename Comp, typename Alloc>
      struct ValueHelper<std::multimap<Key, T, Comp, Alloc>> { static constexpr type value = { "{", ", ", "}" }; };
      template <typename Key, typename T, typename Hash, typename Pred, typename Alloc>
      struct ValueHelper<std::unordered_map<Key, T, Hash, Pred, Alloc>> { static constexpr type value = { "{", ", ", "}" }; };
      template <typename Key, typename T, typename Hash, typename Pred, typename Alloc>
      struct ValueHelper<std::unordered_multimap<Key, T, Hash, Pred, Alloc>> { static constexpr type value = { "{", ", ", "}" }; };
      template <typename T1, typename T2>
      struct ValueHelper<std::pair<T1, T2>> { static constexpr type value = { "(", ", ", ")" }; };
    };

    template <typename T, PrintMode Mode=FindPrintMode<T>::value>
    struct FormatterHelper {
      template <typename U>
      struct SanitizedCast {
        static void cast(std::ostream& fd, const T&) {
//          static_assert(Mode != PrintMode::Unprintable, "Unprintable type");

          fd << "<unprintable>";
        }
      };
    };

    template <typename T>
    struct FormatterHelper<T, PrintMode::ContainerPrettyPrint> {
      template <typename U>
      struct SanitizedCast {
        static constexpr auto padding = PrintPadding::ValueHelper<U>::value;

        static void cast(std::ostream& fd, const T& t) {
          fd << padding.lhs;

          auto beg = std::begin(t);
          auto end = std::end(t);

          while (beg != end) {
            Formatter<decltype(*beg)>::cast(fd, *beg);
            beg++;

            if (beg != end)
              fd << padding.ins;
          }

          fd << padding.rhs;
        }
      };

      template <typename T1, typename T2>
      struct SanitizedCast<std::pair<T1, T2>> {
        static constexpr auto padding = PrintPadding::ValueHelper<std::pair<T1, T2>>::value;

        static void cast(std::ostream& fd, const std::pair<T1, T2>& t) {
          fd << padding.lhs;
          Formatter<T1>::cast(fd, t.first);
          fd << padding.ins;
          Formatter<T2>::cast(fd, t.second);
          fd << padding.rhs;
        }
      };
    };

    template <typename T>
    struct FormatterHelper<T, PrintMode::OStreamDefault> {
      template <typename U>
      struct SanitizedCast {
        static void cast(std::ostream& fd, const T& t) {
          fd << t;
        }
      };
    };

    class Argument {
      const void* m_value;
      void (*m_castptr)(std::ostream& fd, const void* value);

      template <typename T>
      static void m_cast(std::ostream& fd, const void* value) {
        Formatter<T>::cast(fd, *static_cast<const T*>(value));
      }

    public:
      template <typename T>
      Argument(const T& t)
        : m_value(static_cast<const void*>(&t)),
          m_castptr(&m_cast<T>) { }

      void cast(std::ostream& fd) const {
        if (!m_value)
          throw "malformed argument";

        m_castptr(fd, m_value);
      }
    };

    class ArgumentList {
      const std::vector<Argument> m_args;

    public:
      template <typename ...Args>
      ArgumentList(const Args&... args)
        : m_args { Argument(args)... } { }

      void parse(std::ostream& fd, const char* replacement, std::size_t nchars, std::size_t occurence) const {
        std::size_t target = occurence;
        if (nchars != 0) {
          std::size_t it;
          for (it = target = 0; it < nchars; it++) {
            char c = *(replacement + it);

            if (c >= '0' && c <= '9')
              target = target * 10 + static_cast<std::size_t>(c - '0');
            else break;
          }
        }

        if (target >= m_args.size())
          throw "out of range argument reference";

        m_args[target].cast(fd);
      }

      void format(std::ostream& fd, const char* form) const {
        std::size_t occurence=0;

        for (const char* it = form;; it++) {
          switch (*it) {
          case '\0':
            fd.write(form, it - form);
            return;
          case '}':
            if (*(it+1) == '}') {
              fd.write(form, it - form);
              form = ++it;
              break;
            }
            break;
          case '{': {
            if (*(it+1) == '{') {
              fd.write(form, it - form);
              form = ++it;
              break;
            }

            fd.write(form, it - form);
            form = ++it;

            for (;; it++) {
              if (*it == '\0')
                throw "unterminated replacement block";
              else if (*it == '}') {
                break;
              }
            }

            parse(fd, form, it - form, occurence++);

            form = it + 1;

            break;
          }
          default:
            break;
          }
        }
      }
    };
  }

  template <typename T>
  struct Formatter {
    static void cast(std::ostream& fd, const T& t) {
      detail::FormatterHelper<T>::template SanitizedCast<typename std::decay<T>::type>::cast(fd, t);
    }
  };

  template <typename T>
  std::string to_string(const T& t) {
    std::ostringstream s;

    Formatter<T>::cast(s, t);
    return s.str();
  }

  template <typename ...Args>
  void print(std::ostream& fd, const char* form, const Args&... args) {
    detail::ArgumentList(args...).format(fd, form);
  }

  template <typename ...Args>
  void println(std::ostream& fd, const char* form, const Args&... args) {
    print(fd, form, args...);
    fd << '\n';
  }

  template <typename ...Args>
  inline void print(const char* form, const Args&... args) {
    print(std::cout, form, args...);
  }

  template <typename ...Args>
  inline void println(const char* form, const Args&... args) {
    println(std::cout, form, args...);
  }

  template <typename ...Args>
  std::string format(const char* form, const Args&... args) {
    std::ostringstream s;
    print(s, form, args...);

    return s.str();
  }
}

#endif // _FMT_HPP
