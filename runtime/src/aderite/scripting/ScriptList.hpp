#pragma once

#include <vector>

#include "aderite/scripting/Forward.hpp"
#include "aderite/io/SerializableObject.hpp"
#include "aderite/scene/Entity.hpp"

namespace aderite {
namespace scripting {

/**
 * @brief A list of scripts used for an entity
*/
class ScriptList final : public io::ISerializable {
public:
	ScriptList();
	~ScriptList();

	/**
	 * @brief Updates all instances
	 * @param delta Frame delta time
	*/
	void update(float delta) const;

	/**
	 * @brief Adds a behavior to list
	 * @param script Script to add
	*/
	void addScript(Script* script);

	/**
	 * @brief Removes a behavior from list
	 * @param script Script to remove
	*/
	void removeScript(Script* script);

	/**
	 * @brief Pairs scripts with the specified entity
	 * @param entity Entity to pair with
	*/
	void pair(scene::Entity entity);

	Script* get(size_t idx) const {
		return m_scripts[idx];
	}

	Script* operator[](size_t idx) const {
		return m_scripts[idx];
	}

	size_t size() const {
		return m_scripts.size();
	}

	auto begin() const {
		return m_scripts.begin();
	}

	auto end() const {
		return m_scripts.end();
	}

	// Inherited via ISerializable
	virtual reflection::Type getType() const override;
	virtual bool serialize(const io::Serializer* serializer, YAML::Emitter& emitter) override;
	virtual bool deserialize(io::Serializer* serializer, const YAML::Node& data) override;
private:
	std::vector<Script*> m_scripts;
};

}
}