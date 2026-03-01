// std
#include <cstdlib>
#include <iostream>
#include <stdexcept>
#include <memory>

#include "Public/App.hpp"

int main()
{
	std::cout << "HelloWorld" << '\n';

	crsp::App app;

	try {
		app.run();
	}
	catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}

    return EXIT_SUCCESS;
}