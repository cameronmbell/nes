#include <util/fmt.hpp>
#include <iostream>

#include "simple_test.hpp"

int main(int argc, char* argv[]) {
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

  return 0;
}
