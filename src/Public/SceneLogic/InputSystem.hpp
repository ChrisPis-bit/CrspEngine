#pragma once
#include "Rendering/Window.hpp"

#include <vector>
#include <unordered_map>

namespace crsp {
	class InputSystem {
	public:
		InputSystem(Window& window) : window{ window } {}

		bool isKeyDown(int key);
		bool isKeyPressed(int key);

		void trackKey(int key) { keysToTrack.push_back(key); }

		void update();

	private:
		Window& window;

		std::unordered_map<int, bool> currentKeys;
		std::unordered_map<int, bool> previousKeys;

		std::vector<int> keysToTrack{ GLFW_KEY_W, GLFW_KEY_A, GLFW_KEY_S, GLFW_KEY_D };
	};
}