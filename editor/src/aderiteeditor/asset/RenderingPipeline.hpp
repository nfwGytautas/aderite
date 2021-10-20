#pragma once

#include <string>
#include "aderite/io/SerializableObject.hpp"
#include "aderite/rendering/Forward.hpp"
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
	node::Graph* getGraph() const;

	/**
	 * @brief Returns the rendering pipeline
	 * @return rendering::Pipeline instance
	*/
	rendering::Pipeline* getPipeline() const;

	/**
	 * @brief Compiles the graph to a rendering pipeline
	*/
	void compile();

	// Inherited via SerializableObject
	virtual io::SerializableType getType() const override;
	virtual bool serialize(const io::Serializer* serializer, YAML::Emitter& emitter) override;
	virtual bool deserialize(const io::Serializer* serializer, const YAML::Node& data) override;
private:
	std::string m_name;
	node::Graph* m_graph = nullptr;
	rendering::Pipeline* m_pipeline = nullptr;
};

}
}
