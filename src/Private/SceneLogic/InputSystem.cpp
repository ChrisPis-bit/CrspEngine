#include "SceneLogic/InputSystem.hpp"

namespace crsp {
	bool InputSystem::isKeyDown(int key)
	{
		return glfwGetKey(window.getGLFWwindow(), key);
	}

	bool InputSystem::isKeyPressed(int key)
	{
		bool curr = currentKeys.find(key) != currentKeys.end();
		bool prev = previousKeys.find(key) != previousKeys.end();
		return curr && !prev;
	}

	void InputSystem::update() {
		previousKeys = currentKeys;
		currentKeys.clear();

		for (int key : keysToTrack)
		{
			if (glfwGetKey(window.getGLFWwindow(), key))
				currentKeys[key] = true;
		}
	}
}
