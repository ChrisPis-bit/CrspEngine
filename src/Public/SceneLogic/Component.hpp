#pragma once

namespace crsp {
	class GameObject;

	class Component {
	public:
		Component(GameObject& owner) : owner(owner) {}

		virtual void start() {}
		virtual void update() {}

		bool isActive();

	protected:
		GameObject& owner;
	};
}