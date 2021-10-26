#pragma once

#include <mono/jit/jit.h>
#include "aderite/io/SerializableObject.hpp"
#include "aderite/scripting/Forward.hpp"

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
