#pragma once
#include "Entity.hpp"
namespace crsp {
	struct IComponentPool {
		IComponentPool() {}
		virtual ~IComponentPool() {}
	};

	template<typename T>
	struct ComponentPool : public IComponentPool {
		ComponentPool() {
			data = new T[MAX_ENTITIES];
		}
		~ComponentPool() {
			delete[] data;
		}

		inline T* get(size_t index) {
			return data + index;
		}

		T* data{};
	};
}