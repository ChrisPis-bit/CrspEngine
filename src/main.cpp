// std
#include <cstdlib>
#include <iostream>
#include <stdexcept>
#include <memory>

#include "Public/App.hpp"
#include "SnakeGame/SnakeScene.hpp"

int main()
{
	std::cout << "HelloWorld" << '\n';

	crsp::App app(std::make_unique<crsp::SnakeScene>());

	try {
		app.run();
	}
	catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}

    return EXIT_SUCCESS;
}