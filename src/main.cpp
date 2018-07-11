#include <eqn/numbers.hpp>

#include <iostream>

int main(int argc, char** argv) {
	std::cout << "a is " << eqn::isNumber('a') << std::endl;
	std::cout << "7 is " << eqn::isNumber('7') << std::endl;
	std::cout << "~ is " << eqn::isNumber('~') << std::endl;
	std::cout << "1 is " << eqn::isNumber('1') << std::endl;
  std::cout << "1 is a " << eqn::isNumber('1') << std::endl;

	return 0;
}
