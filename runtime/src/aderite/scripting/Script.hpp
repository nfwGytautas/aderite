#pragma once

#include <mono/jit/jit.h>
#include "aderite/io/SerializableObject.hpp"
#include "aderite/scripting/Forward.hpp"
#include "aderite/scene/Entity.hpp"

namespace aderite {
namespace scripting {

/**
 * @brief Class used to wrap a behavior script
*/
class Script final : public io::ISerializable {
public:
	Script();
	~Script();

	/**
	 * @brief Returns the name of the script
	*/
	std::string getName() const;

	/**
	 * @brief Sets the name of the script
	 * @param name Name of the script
	*/
	void setName(const std::string& name);

	/**
	 * @brief Update script instance
	 * @param delta Frame delta time
	*/
	void update(float delta) const;

	/**
	 * @brief Pair the script with entity instance
	 * @param entity Entity with which to pair
	*/
	void pair(scene::Entity entity);

	/**
	 * @brief Function invokes scripts OnTriggerEnter method
	 * @param trigger Trigger that was entered
	*/
	void onTriggerEnter(scene::Entity trigger);

	/**
	 * @brief Function invokes scripts OnTriggerLeave method
	 * @param trigger Trigger that was left
	*/
	void onTriggerLeave(scene::Entity trigger);

	/**
	 * @brief Function invokes scripts OnTriggerEnter method
	 * @param collision Collision that was entered
	*/
	void onCollisionEnter(scene::Entity collision);

	/**
	 * @brief Function invokes scripts OnTriggerLeave method
	 * @param collision Collision that was left
	*/
	void onCollisionLeave(scene::Entity collision);

	/**
	 * @brief Returns the behavior wrapper of this script
	*/
	BehaviorWrapper* getBehavior() const;

	/**
	 * @brief Returns the instance of this script
	*/
	MonoObject* getInstance() const;

	// Inherited via ISerializable
	virtual reflection::Type getType() const override;
	virtual bool serialize(const io::Serializer* serializer, YAML::Emitter& emitter) override;
	virtual bool deserialize(io::Serializer* serializer, const YAML::Node& data) override;
private:
	std::string m_name = "";

	BehaviorWrapper* m_behavior = nullptr;
	MonoObject* m_instance = nullptr;
};

}
}
