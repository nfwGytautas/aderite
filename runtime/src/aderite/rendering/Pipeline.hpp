#pragma once

#include <vector>
#include "aderite/utility/Macros.hpp"
#include "aderite/io/SerializableObject.hpp"
#include "aderite/rendering/operation/OperationBase.hpp"
#include "aderite/rendering/operation/Forward.hpp"

ADERITE_RENDERING_NAMESPACE_BEGIN

/**
 * @brief Rendering pipeline object, the rendering engine loops through the pipeline
 * operations executing them one by one
*/
class Pipeline : public io::SerializableObject
{
public:
	Pipeline();
	virtual ~Pipeline();

	/**
	 * @brief Adds operation to the pipeline, the pipeline takes ownership of the operation
	 * NOTE: This adds the operation to the end of the operation array
	 *       since ordering is important the user must ensure the correct
	 *       order themselves
	 * @param operation Operation to add
	*/
	void addOperation(OperationBase* operation);

	/**
	 * @brief Initialize pipeline and all of it's operations, allocating resources
	*/
	void initialize();

	/**
	 * @brief Execute the pipeline
	*/
	void execute() const;

	/**
	 * @brief Shutdown the pipeline and all of it's operations, freeing resources
	*/
	void shutdown();

	// Inherited via ISerializable
	virtual reflection::Type getType() const override;
	virtual bool serialize(const io::Serializer* serializer, YAML::Emitter& emitter) override;
	virtual bool deserialize(const io::Serializer* serializer, const YAML::Node& data) override;

	ADERITE_DEBUG_SECTION
	(
		void logPipeline() const;
	)
private:
	std::vector<OperationBase*> m_operations;
	PipelineState* m_state = nullptr;
};

ADERITE_RENDERING_NAMESPACE_END
