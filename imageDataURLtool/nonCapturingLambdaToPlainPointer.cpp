#include <iostream>
#include <type_traits>

auto lambda_expression = +[] {}; // noncapturing lambda expression; signature: void()

using TargetFuncPtr = void(*)(); // explicitly defined pointer for comparison

void execute_callback(void(*cb)()) {
	cb();
};

int main() {
	std::cout << "_'unary-plus-implicit' and other conversion types with noncapturing lambdas_" << std::endl;

	// implicit conversion (via unary plus);
	using DeducedType = decltype(lambda_expression);
	std::cout << "'+[]{}' and 'void(*)()' equivalence: " << std::boolalpha << std::is_same_v<DeducedType, TargetFuncPtr> << std::endl;
	TargetFuncPtr ptr_assigned = +[] {};

	// assign lambda directly to a func ptr variable type, and compiler implicitly converts the function
	void(*fp_direct)() = []() {std::cout << "Directly assigned." << std::endl; };
	fp_direct();

	// pass lambda as argument to a function expecting function pointer, and conversion is triggered automatically
	execute_callback([]() {std::cout << "Passed an argument." << std::endl; });

	// interest in implicit conversion of non capturing lambda was inspired 
	// by discovering a unary-plus trick for implicit conversion of non-char types to string 
	std::cout << std::endl << "another use of unary-plus implicit conversion" << std::endl;

	uint8_t charff = 0xff; // char(-1) prints as blank char

	std::cout << std::hex << "'blind' printout of charff = (uint8_t)(-1) with std::cout << charff; : '" << charff << "'" << std::endl;
	std::cout << std::hex << "'correct' printout with std::cout << +charff; : '" << +charff << "'" << std::endl;

	return 0;
}