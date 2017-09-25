#include "system.h"

std::default_random_engine& default_generator() {
	static std::default_random_engine generator(std::random_device{}());
	return generator;
}
