#pragma once

#include <string>
#include <unordered_map>
#include <vector>

#include <bgfx/bgfx.h>

#include "aderite/asset/MaterialTypeAsset.hpp"
#include "aderite/io/Loader.hpp"
#include "aderite/io/SerializableObject.hpp"
#include "aderite/utility/Macros.hpp"

#include "aderiteeditor/asset/property/Forward.hpp"
#include "aderiteeditor/node/Forward.hpp"
#include "aderiteeditor/node/new/GraphX.hpp"

namespace aderite {
namespace asset {

/**
 * @brief Material type asset implementation
 */
class EditorMaterialType : public asset::MaterialTypeAsset, public node::Graph {
public:
    using Properties = std::vector<Property*>;
    using Samplers = std::vector<Sampler*>;

public:
    EditorMaterialType();
    ~EditorMaterialType();

    virtual bool serialize(const io::Serializer* serializer, YAML::Emitter& emitter) const override;
    virtual bool deserialize(io::Serializer* serializer, const YAML::Node& data) override;

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
     * @brief Returns the samplers of the material type
     */
    Samplers getSamplers() const;

    /**
     * @brief Adds a property to material type
     * @param prop Property to add
     */
    void addProperty(Property* prop);

    /**
     * @brief Adds a sampler to material type
     * @param sampler Sampler to add
     */
    void addSampler(Sampler* sampler);

    /**
     * @brief Removes the specified property from the type
     * @param prop Property to remove
     */
    void removeProperty(Property* prop);

    /**
     * @brief Removes the specified sampler from the type
     * @param sampler Sampler to remove
     */
    void removeSampler(Sampler* sampler);

private:
    /**
     * @brief Generates a material header for this type and outputs it into the specified stream
     * @param os Stream to output to
     */
    void generateMaterialHeader(std::ostream& os);

    /**
     * @brief Generates varying.def.sc file for this material type and outputs it into the specified stream
     * @param os Stream to output to
     */
    void generateVarying(std::ostream& os);

    /**
     * @brief Generates the fragment shader for this material type and outputs it into the specified stream
     * @param os Stream to output to
     */
    void generateFragment(std::ostream& os);

    /**
     * @brief Generates the vertex shader for this material type and outputs it into the specified stream
     * @param os Stream to output to
     */
    void generateVertex(std::ostream& os);

    /**
     * @brief Adds material input, output nodes to the graph
     */
    void addIONodes();

private:
    Properties m_properties;
    Samplers m_samplers;
};

} // namespace asset
} // namespace aderite
