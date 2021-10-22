#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include <bgfx/bgfx.h>
#include "aderite/utility/Macros.hpp"
#include "aderite/io/SerializableObject.hpp"
#include "aderite/io/Loader.hpp"
#include "aderite/asset/MaterialTypeAsset.hpp"
#include "aderiteeditor/asset/property/Forward.hpp"
#include "aderiteeditor/node/Forward.hpp"

namespace aderite {
namespace asset {

/**
 * @brief Material type asset implementation
*/
class EditorMaterialType : public asset::MaterialTypeAsset {
public:
	using Properties = std::vector<Property*>;
public:
	EditorMaterialType();
	~EditorMaterialType();

	virtual bool serialize(const io::Serializer* serializer, YAML::Emitter& emitter) override;
	virtual bool deserialize(const io::Serializer* serializer, const YAML::Node& data) override;

	/**
	 * @brief Returns this pipeline graph representation
	 * @return node::Graph instance
	*/
	node::Graph* getGraph() const;

	/**
	 * @brief Recalculates property offsets
	*/
	void recalculate();

	/**
	 * @brief Compiles the material type to a shader
	*/
	void compile();

	/**
	 * @brief Returns the properties of the material type
	*/
	Properties getProperties() const;

	/**
	 * @brief Adds a property to material type
	 * @param prop Property to add
	*/
	void addProperty(Property* prop);
private:
	Properties m_properties;
	node::Graph* m_graph = nullptr;
};

}
}
