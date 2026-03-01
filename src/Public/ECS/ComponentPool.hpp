#pragma once
#include "Entity.hpp"
namespace crsp {
	struct ComponentPool {
		ComponentPool(size_t componentSize) : componentSize{ componentSize } {
			data = new char[componentSize * MAX_ENTITIES];
		}
		~ComponentPool() {
			delete[] data;
		}

		inline void* get(size_t index) {
			return data + index * componentSize;
		}

		char* data{};
		size_t componentSize;
	};
}