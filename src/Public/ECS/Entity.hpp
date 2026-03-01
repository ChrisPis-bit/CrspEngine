#pragma once
#include <bitset>

namespace crsp {

	constexpr static uint32_t MAX_ENTITIES = 500;
	constexpr static uint32_t MAX_COMPONENTS = 32;
	typedef std::bitset<MAX_COMPONENTS> ComponentMask;

	extern int componentCounter;
	/// <summary>
	/// Get unique identifier for component type.
	/// </summary>
	/// <typeparam name="T">Component struct</typeparam>
	/// <returns>Unique ID</returns>
	template<class T>
	int getId() {
		static int componentId = componentCounter++;
		return componentId;
	}

	typedef unsigned long long EntityID;
	typedef unsigned int EntityIndex;
	typedef unsigned int EntityVersion;
	#define INVALID_ENTITY_ID EntityIndex(-1);

	struct Entity {
		EntityID id;

		bool operator==(const Entity& other) const {
			return id == other.id;
		}

		bool operator<(const Entity& other) const {
			return id < other.id;
		}

		inline EntityIndex getIndex() {
			return id >> 32;
		}

		inline EntityVersion getVersion() {
			return (EntityVersion)id;
		}

		inline bool isValid() {
			return getIndex() != INVALID_ENTITY_ID;
		}
	};
}