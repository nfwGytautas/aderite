#pragma once

#include <vector>
#include "aderite/utility/Macros.hpp"
#include "aderite/audio/Forward.hpp"
#include "aderite/interfaces/ISerializable.hpp"

ADERITE_AUDIO_NAMESPACE_BEGIN

class AudioInstanceList : public interfaces::ISerializable {
public:
	~AudioInstanceList();

	/**
	 * @brief Add an already created audio instance asset to list
	 * @param instance AudioInstance to add
	*/
	void addInstance(AudioInstance* instance);

	/**
	 * @brief Removes audio instance from list
	 * @param instance Audio instance to remove
	*/
	void removeInstance(AudioInstance* instance);

	// Inherited via ISerializable
	virtual bool serialize(YAML::Emitter& out) override;
	virtual bool deserialize(YAML::Node& data) override;

	auto begin() const {
		return m_instances.begin();
	}

	auto end() const {
		return m_instances.end();
	}
private:
	std::vector<AudioInstance*> m_instances;
};

ADERITE_AUDIO_NAMESPACE_END
