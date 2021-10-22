#include "EditorMaterialType.hpp"

#include "aderiteeditor/windows/backend/node/imnodes.h"

#include "aderite/Aderite.hpp"
#include "aderite/utility/Log.hpp"
#include "aderite/io/Serializer.hpp"
#include "aderite/reflection/RuntimeTypes.hpp"
#include "aderiteeditor/asset/property/Property.hpp"
#include "aderiteeditor/node/Graph.hpp"
#include "aderiteeditor/node/material/MaterialOutputNode.hpp"
#include "aderiteeditor/node/material/MaterialInputNode.hpp"

namespace aderite {
namespace asset {

EditorMaterialType::EditorMaterialType() 
	: m_graph(new node::Graph())
{
	node::MaterialOutputNode* mon = m_graph->addNode<node::MaterialOutputNode>();
	node::MaterialInputNode* min = m_graph->addNode<node::MaterialInputNode>();
	min->setType(this);
	m_graph->setLastNode(mon);
	ImNodes::SetNodeGridSpacePos(mon->getId(), ImVec2(400, 200));
	ImNodes::SetNodeGridSpacePos(min->getId(), ImVec2(200, 200));
}

EditorMaterialType::~EditorMaterialType() {
	for (Property* pb : m_properties) {
		delete pb;
	}
}

bool EditorMaterialType::serialize(const io::Serializer* serializer, YAML::Emitter& emitter) {
	MaterialTypeAsset::serialize(serializer, emitter);

	emitter << YAML::Key << "Properties" << YAML::BeginSeq;
	for (Property* pb : m_properties) {
		emitter << YAML::BeginMap;
		emitter << YAML::Key << "Name" << YAML::Value << pb->getName();
		emitter << YAML::Key << "Type" << YAML::Value << static_cast<size_t>(pb->getType());
		emitter << YAML::Key << "Offset" << YAML::Value << pb->getOffset();
		emitter << YAML::EndMap;
	}
	emitter << YAML::EndSeq;

	emitter << YAML::Key << "Graph";
	serializer->writeUntrackedType(emitter, m_graph);

	return true;
}

bool EditorMaterialType::deserialize(const io::Serializer* serializer, const YAML::Node& data) {
	MaterialTypeAsset::deserialize(serializer, data);

	auto properties = data["Properties"];
	if (properties) {
		for (auto property : properties) {
			// Resolve type
			PropertyType type = static_cast<PropertyType>((property["Type"].as<size_t>()));
			std::string name = property["Name"].as<std::string>();
			uint16_t offset = property["Offset"].as<uint16_t>();

			Property* propInstance = new Property(type, name);
			propInstance->setOffset(offset);

			m_properties.push_back(propInstance);
		}
	}

	delete m_graph;
	m_graph = static_cast<node::Graph*>(serializer->parseUntrackedType(data["Graph"]));

	return true;
}

node::Graph* EditorMaterialType::getGraph() const {
	return m_graph;
}

void EditorMaterialType::recalculate() {
	// This method will recalculate offsets so that all properties sit in this configuration
	// although there are multiple loops over properties, most material types won't have that many properties
	// and the method should never be called on runtime
	// vec4
	// vec3 float
	// vec2 vec2
	// vec2 float float
	// float float float float

	uint16_t offset = 0;
	uint16_t elements = 0;
	this->getFieldsMutable().NumSamplers = 0;

	// Temporary vector for storing floats
	std::vector<Property*> floatProperties;
	// Temporary for storing vec2 carry over
	Property* vec2Carry = nullptr;

	// Collect float 1 and set vec4 offsets
	for (Property* prop : m_properties) {
		if (prop->getType() == PropertyType::FLOAT) {
			floatProperties.push_back(prop);
		}
		else if (prop->getType() == PropertyType::VEC4) {
			// Set offset
			prop->setOffset(offset);
			elements += getElementCountForType(prop->getType());
			offset = elements;
		}
		else if (isSampler(prop->getType())) {
			this->getFieldsMutable().NumSamplers += 1;
		}

		// Ignore others
	}

	// Now vec4 are ordered, go to vec3
	for (Property* prop : m_properties) {
		if (prop->getType() == PropertyType::VEC3) {
			prop->setOffset(offset);
			elements += getElementCountForType(prop->getType());
			offset = elements;

			// Check for float to pad the back
			if (floatProperties.size() > 0) {
				Property* fProp = floatProperties.back();
				floatProperties.pop_back();
				fProp->setOffset(offset);
			}

			// No matter what add pad float
			elements += 1;
			offset = elements;
		}
	}

	// Now vec2
	for (Property* prop : m_properties) {
		if (prop->getType() == PropertyType::VEC2) {
			prop->setOffset(offset);
			elements += getElementCountForType(prop->getType());
			offset = elements;
		}
	}

	// Left over floats
	for (Property* prop : m_properties) {
		prop->setOffset(offset);
		elements += getElementCountForType(prop->getType());
		offset = elements;
	}

	if (elements % 4 != 0) {
		this->getFieldsMutable().Size = ((elements - elements % 4) / 4) + 1;
	}
	else {
		this->getFieldsMutable().Size = elements / 4;
	}
}

void EditorMaterialType::compile() {

}

EditorMaterialType::Properties EditorMaterialType::getProperties() const {
	return m_properties;
}

void EditorMaterialType::addProperty(Property* prop) {
	m_properties.push_back(prop);
}

}
}
