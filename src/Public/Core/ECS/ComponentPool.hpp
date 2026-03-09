#pragma once
#include "Entity.hpp"
#include "Utils.hpp"
#include <queue>

namespace crsp {
	/// <summary>
	/// Base non-generic class.
	/// </summary>
	struct IComponentPool {
		IComponentPool() {}
		virtual ~IComponentPool() {}

		inline virtual void remove(EntityIndex entity) {}
	};

	/// <summary>
	/// Generic component pool.
	/// TODO: Move to archetypes.
	/// </summary>
	/// <typeparam name="T">Component type</typeparam>
	template<typename T>
	struct ComponentPool : public IComponentPool {
		ComponentPool(uint32_t pageCapacity = 1024) : pageCapacity(pageCapacity) {
			data = new T*;
			data[0] = new T[pageCapacity];
			pagesCount = 1;
			currentUsedData = 0;

			sparseArray = new size_t[MAX_ENTITIES];
		}
		~ComponentPool() {
			for (size_t i = 0; i < pagesCount; i++)
			{
				delete[] data[i];
			}

			delete[] data;
			delete[] sparseArray;
		}

		inline T* get(EntityIndex entity) {

			size_t dataIndex = sparseArray[entity];
			return data[dataIndex / pageCapacity] + (dataIndex % pageCapacity);
			//return data + index;
		}
		
		inline T* create(EntityIndex entity) {
			if (!unusedData.empty()) { // Use from deleted pool
				sparseArray[entity] = unusedData.back();
				unusedData.pop_back();
			}
			else if (currentUsedData < poolSize()) { // Use from current newest page
				sparseArray[entity] = currentUsedData;
				currentUsedData++;
			}
			else { // Grow pool
				grow();
				sparseArray[entity] = currentUsedData;
				currentUsedData++;
			}

			return get(entity);
		}
		
		inline void remove(EntityIndex entity) override {
			size_t index = sparseArray[entity];
			unusedData.push_back(index);
			get(entity)->~T();
		}

		void grow() {
			// Grow page pointer buffer
			data = (T**)std::realloc(data, (pagesCount + 1) * sizeof(T*));
			data[pagesCount] = new T[pageCapacity];
			pagesCount++;
		}

		inline uint32_t poolSize() { return pagesCount * pageCapacity; }

		T** data{};
		uint32_t pagesCount;
		uint32_t pageCapacity;

		size_t* sparseArray;
		size_t currentUsedData;

		std::vector<size_t> unusedData;
	};
}