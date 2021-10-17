#pragma once

#include <string>
#include "aderite/io/SerializableObject.hpp"
#include "aderiteeditor/node/Forward.hpp"

namespace aderite {
namespace asset {

/**
 * @brief Asset used to defined a FPRP rendering pipeline
*/
class RenderingPipeline : public io::SerializableObject {
public:
	RenderingPipeline();
	~RenderingPipeline();

	/**
	 * @brief Returns this pipeline graph representation
	 * @return node::Graph instance
	*/
	node::Graph* getGraph();

	// Inherited via SerializableObject
	virtual io::SerializableType getType() override;
	virtual bool serialize(const io::Serializer* serializer, YAML::Emitter& emitter) override;
	virtual bool deserialize(const io::Serializer* serializer, const YAML::Node& data) override;
private:
	std::string m_name;
	node::Graph* m_graph = nullptr;
};

}
}
