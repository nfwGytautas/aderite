#include "Script.hpp"

#include "aderite/Aderite.hpp"
#include "aderite/scripting/ScriptManager.hpp"
#include "aderite/scripting/BehaviorWrapper.hpp"
#include "aderite/scripting/FieldWrapper.hpp"
#include "aderite/scripting/MonoUtils.hpp"

namespace aderite {
namespace scripting {

Script::Script() {
	
}

Script::~Script() {
	
}

std::string Script::getName() const {
	return m_name;
}

void Script::setName(const std::string& name) {
	m_name = name;

	m_behavior = ::aderite::Engine::getScriptManager()->getBehavior(m_name);
	if (m_behavior != nullptr) {
		m_instance = m_behavior->createInstance();
	}
}

void Script::update(float delta) const {
	if (m_behavior != nullptr && m_behavior->hasUpdateFunction()) {
		m_behavior->update(m_instance, delta);
	}
}

BehaviorWrapper* Script::getBehavior() const {
	return m_behavior;
}

MonoObject* Script::getInstance() const {
	return m_instance;
}

reflection::Type Script::getType() const {
	return static_cast<reflection::Type>(reflection::RuntimeTypes::SCRIPT);
}

bool Script::serialize(const io::Serializer* serializer, YAML::Emitter& emitter) {
	emitter << YAML::Key << "Name" << YAML::Value << m_name;

	emitter << YAML::Key << "Fields" << YAML::BeginMap;
	for (FieldWrapper* fw : m_behavior->getFields()) {
		emitter << YAML::Key << fw->getName() << YAML::Value;

		switch (fw->getType()) {
		case FieldType::Float: {
			emitter << (*(float*)unbox(fw->getValueObject(m_instance)));
			break;
		}
		default: {
			emitter << YAML::Null;
		}
		}
	}
	emitter << YAML::EndMap;

	return true;
}

bool Script::deserialize(io::Serializer* serializer, const YAML::Node& data) {
	m_name = data["Name"].as<std::string>();

	m_behavior = ::aderite::Engine::getScriptManager()->getBehavior(m_name);
	m_instance = m_behavior->createInstance();

	for (auto& fieldData : data["Fields"]) {
		FieldWrapper* fw = m_behavior->getField(fieldData.first.as<std::string>());

		if (fw != nullptr && !fieldData.second.IsNull()) {
			
			switch (fw->getType()) {
			case FieldType::Float: {
				float val = fieldData.second.as<float>();
				fw->setValue(m_instance, &val);
				break;
			}
			}
		}
	}

	return true;
}

}
}
