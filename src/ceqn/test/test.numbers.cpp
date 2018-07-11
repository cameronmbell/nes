#include <eqn/numbers.hpp>
#include <iostream>

int main(int argc, char** argv) {
	std::cout << "hey!" << std::endl;
	if (eqn::isNumber('1') == false) return 1;
	if (eqn::isNumber('a') == true) return 1;

	return 0;
}
