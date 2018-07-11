// attribution to tinyformat.h and libfmt and Andrzej's compile time parsing
// much of this is a derivative of their work

#ifndef _FMT_HPP
#define _FMT_HPP

#include <iostream>
#include <array>
#include <sstream>
#include <vector>
#include <string>

namespace fmt {
  template <typename T>
  struct Formatter {static void cast(std::ostream& fd, const T& t) {
      fd << t;
    }
  };

  namespace detail {
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

  template <typename ...Args>
  void print(std::ostream& fd, const char* form, const Args&... args) {
    detail::ArgumentList(args...).format(fd, form);
  }

  template <typename ...Args>
  std::string format(const char* form, const Args&... args) {
    std::ostringstream s;
    print(s, form, args...);

    return s.str();
  }

  template <typename ...Args>
  void print(const char* form, const Args&... args) {
    print(std::cout, form, args...);
  }
}

#endif // _FMT_HPP
