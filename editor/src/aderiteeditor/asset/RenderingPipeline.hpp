#pragma once

#include <string>
#include "aderite/io/SerializableObject.hpp"
#include "aderite/rendering/Forward.hpp"
#include "aderite/rendering/Pipeline.hpp"
#include "aderiteeditor/node/Forward.hpp"

namespace aderite {
namespace asset {

/**
 * @brief Asset used to define a FPRP rendering pipeline
*/
class RenderingPipeline : public rendering::Pipeline {
public:
	RenderingPipeline();
	~RenderingPipeline();

	/**
	 * @brief Returns this pipeline graph representation
	 * @return node::Graph instance
	*/
	node::Graph* getGraph() const;

	/**
	 * @brief Compiles the graph to a rendering pipeline
	*/
	void compile();

	// Inherited via SerializableObject
	virtual bool serialize(const io::Serializer* serializer, YAML::Emitter& emitter) override;
	virtual bool deserialize(const io::Serializer* serializer, const YAML::Node& data) override;
private:
	std::string m_name;
	node::Graph* m_graph = nullptr;
};

}
}
