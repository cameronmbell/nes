#include <util/fmt.hpp>

#include <unordered_map>
#include <unordered_set>
#include <forward_list>
#include <iostream>
#include <complex>
#include <sstream>
#include <utility>
#include <vector>
#include <stack>
#include <list>
#include <map>
#include <set>

#include "simple_test.hpp"

struct Vec2 { float x, y; };
struct Vec3 { float x, y, z; };

namespace fmt {
  template <>
  struct Formatter<Vec2> {
    static void cast(std::ostream& fd, const Vec2& t) {
      fd << "(" << t.x << ", " << t.y << ")";
    }
  };
}

int main(int argc, char* argv[]) {
  ASSERT_EQ(fmt::detail::FindPrintMode<int>::value, fmt::detail::PrintMode::OStreamDefault);
  ASSERT_EQ(fmt::detail::FindPrintMode<char>::value, fmt::detail::PrintMode::OStreamDefault);
  ASSERT_EQ(fmt::detail::FindPrintMode<int&>::value, fmt::detail::PrintMode::OStreamDefault);
  ASSERT_EQ(fmt::detail::FindPrintMode<int&&>::value, fmt::detail::PrintMode::OStreamDefault);
  ASSERT_EQ(fmt::detail::FindPrintMode<float>::value, fmt::detail::PrintMode::OStreamDefault);
  ASSERT_EQ(fmt::detail::FindPrintMode<double>::value, fmt::detail::PrintMode::OStreamDefault);
  ASSERT_EQ(fmt::detail::FindPrintMode<const char*>::value, fmt::detail::PrintMode::OStreamDefault);
  ASSERT_EQ(fmt::detail::FindPrintMode<std::string>::value, fmt::detail::PrintMode::OStreamDefault);
  ASSERT_EQ(fmt::detail::FindPrintMode<const std::complex<float>&&>::value, fmt::detail::PrintMode::OStreamDefault);

  ASSERT_EQ(fmt::detail::FindPrintMode<std::vector<int>>::value, fmt::detail::PrintMode::ContainerPrettyPrint);
  ASSERT_EQ((fmt::detail::FindPrintMode<std::map<std::string, int>>::value), fmt::detail::PrintMode::ContainerPrettyPrint);
  ASSERT_EQ(fmt::detail::FindPrintMode<std::list<int>>::value, fmt::detail::PrintMode::ContainerPrettyPrint);
  ASSERT_EQ(fmt::detail::FindPrintMode<std::set<int>>::value, fmt::detail::PrintMode::ContainerPrettyPrint);
  ASSERT_EQ(fmt::detail::FindPrintMode<std::vector<int>&>::value, fmt::detail::PrintMode::ContainerPrettyPrint);
  ASSERT_EQ(fmt::detail::FindPrintMode<std::vector<int>&&>::value, fmt::detail::PrintMode::ContainerPrettyPrint);
  ASSERT_EQ(fmt::detail::FindPrintMode<const std::vector<int>&>::value, fmt::detail::PrintMode::ContainerPrettyPrint);
  ASSERT_EQ((fmt::detail::FindPrintMode<std::array<int, 10>>::value), fmt::detail::PrintMode::ContainerPrettyPrint);
  ASSERT_EQ((fmt::detail::FindPrintMode<std::deque<int>>::value), fmt::detail::PrintMode::ContainerPrettyPrint);
  ASSERT_EQ((fmt::detail::FindPrintMode<std::forward_list<int>>::value), fmt::detail::PrintMode::ContainerPrettyPrint);
  ASSERT_EQ((fmt::detail::FindPrintMode<std::multiset<int>>::value), fmt::detail::PrintMode::ContainerPrettyPrint);
  ASSERT_EQ((fmt::detail::FindPrintMode<std::multimap<std::string, int>>::value), fmt::detail::PrintMode::ContainerPrettyPrint);
  ASSERT_EQ((fmt::detail::FindPrintMode<std::unordered_set<int>>::value), fmt::detail::PrintMode::ContainerPrettyPrint);
  ASSERT_EQ((fmt::detail::FindPrintMode<std::unordered_multiset<int>>::value), fmt::detail::PrintMode::ContainerPrettyPrint);
  ASSERT_EQ((fmt::detail::FindPrintMode<std::unordered_map<std::string, float>>::value), fmt::detail::PrintMode::ContainerPrettyPrint);
  ASSERT_EQ((fmt::detail::FindPrintMode<std::unordered_multimap<std::string, float>>::value), fmt::detail::PrintMode::ContainerPrettyPrint);
  ASSERT_EQ((fmt::detail::FindPrintMode<std::pair<std::string, float>>::value), fmt::detail::PrintMode::ContainerPrettyPrint);
  ASSERT_EQ((fmt::detail::FindPrintMode<std::pair<std::string, float>&>::value), fmt::detail::PrintMode::ContainerPrettyPrint);
  ASSERT_EQ((fmt::detail::FindPrintMode<std::pair<std::string, float>&&>::value), fmt::detail::PrintMode::ContainerPrettyPrint);
  ASSERT_EQ((fmt::detail::FindPrintMode<const std::pair<std::string, float>&>::value), fmt::detail::PrintMode::ContainerPrettyPrint);
  ASSERT_EQ((fmt::detail::FindPrintMode<const volatile std::pair<std::string, float>&>::value), fmt::detail::PrintMode::ContainerPrettyPrint);

  ASSERT_EQ(fmt::detail::FindPrintMode<std::ostringstream>::value, fmt::detail::PrintMode::Unprintable);

  ASSERT_EQ(fmt::format("i am {}", 123), "i am 123");
  ASSERT_EQ(fmt::format("Hello {}!", "world"), "Hello world!");
  ASSERT_EQ(fmt::format("I am {} and named {}!", 100, "steve"), "I am 100 and named steve!");
  ASSERT_EQ(fmt::format("I am {1} and named {0}!", "steve", 100), "I am 100 and named steve!");
  ASSERT_EQ(fmt::format("{0}{0}{0}", "steve"), "stevestevesteve");
  ASSERT_EQ(fmt::format("{0}{}{0}", "steve", "dave"), "stevedavesteve");
  ASSERT_EQ(fmt::format(" {{ "), " { ");
  ASSERT_EQ(fmt::format(" {{}} "), " {} ");
  ASSERT_EQ(fmt::format("{{{0}{}{0}}}", "abra", "cad"), "{abracadabra}");
  ASSERT_EQ(fmt::format("{{{0}{{{}{{{0}}}}}", "abra", "cad"), "{abra{cad{abra}}");
  ASSERT_EQ(fmt::format("{{{}}", 2), "{2}");

  ASSERT_EQ(fmt::format(
			"{16}{14}{5}{21}{13}{15}{14}{15}{21}{12}"
			"{20}{18}{1}{13}{9}{3}{18}{15}{19}{3}{15}"
			"{16}{9}{3}{19}{9}{12}{9}{3}{15}{22}{15}"
			"{12}{3}{1}{14}{15}{3}{15}{14}{9}{15}{19}"
			"{9}{19}", ' ',
			'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h',
			'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p',
			'q', 'r', 's', 't', 'u', 'v', 'w', 'x',
			'y', 'z'),
	  "pneumonoultramicroscopicsilicovolcanoconiosis");

  SHOULD_THROW(fmt::format("{}"));
  SHOULD_THROW(fmt::format("{1}", 2));
  SHOULD_THROW(fmt::format("{", 2));

  std::vector<int> vint = { 1, 2, 3, 4 };
  ASSERT_EQ(fmt::to_string(vint), "[1, 2, 3, 4]");

  std::set<int> sint = { 1, 2, 3, 4 };
  ASSERT_EQ(fmt::to_string(sint), "{1, 2, 3, 4}");

  std::pair<std::string, float> pint = { "hello", 3.14 };
  ASSERT_EQ(fmt::to_string(pint), "(hello, 3.14)");

  std::map<std::string, std::vector<int>> mint = {
    { "bar", { } },
    { "baz", { 7, 8, 9 } },
    { "foo", { 1, 2, 3 } }
  };
  ASSERT_EQ(fmt::to_string(mint), "{(bar, []), (baz, [7, 8, 9]), (foo, [1, 2, 3])}");

  return 0;
}
