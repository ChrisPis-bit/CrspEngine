#pragma once

namespace crsp {
	class GameObject;

	class Component {
	public:
		Component(GameObject& owner) : owner(owner) {}
		virtual ~Component() {}

		virtual void update() {}
		virtual void render() {}

		bool isActive();

	protected:
		GameObject& owner;
	};
}