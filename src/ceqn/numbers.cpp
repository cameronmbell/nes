#include "eqn/numbers.hpp"

namespace eqn {
	bool isNumber(char q) {
		auto a = static_cast<int>(q);

		return a >= '0' && a <= '9';
	}
}
