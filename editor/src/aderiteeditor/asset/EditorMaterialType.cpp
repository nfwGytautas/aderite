#include "EditorMaterialType.hpp"
#include <fstream>

#include "aderite/Aderite.hpp"
#include "aderite/io/FileHandler.hpp"
#include "aderite/io/Serializer.hpp"
#include "aderite/reflection/RuntimeTypes.hpp"
#include "aderite/utility/Log.hpp"

#include "aderiteeditor/asset/property/Property.hpp"
#include "aderiteeditor/compiler/ShaderCompiler.hpp"
#include "aderiteeditor/compiler/ShaderEvaluator.hpp"
#include "aderiteeditor/node/Graph.hpp"
#include "aderiteeditor/node/material/MaterialInputNode.hpp"
#include "aderiteeditor/node/material/MaterialOutputNode.hpp"
#include "aderiteeditor/shared/Project.hpp"
#include "aderiteeditor/shared/State.hpp"

namespace aderite {
namespace asset {

EditorMaterialType::EditorMaterialType() : m_graph(new node::Graph()) {
    node::MaterialOutputNode* mon = m_graph->addNode<node::MaterialOutputNode>();
    node::MaterialInputNode* min = m_graph->addNode<node::MaterialInputNode>();
    min->setType(this);
    m_graph->setLastNode(mon);
    mon->setPosition({400, 200});
    min->setPosition({200, 200});
}

EditorMaterialType::~EditorMaterialType() {
    for (Property* pb : m_properties) {
        delete pb;
    }

    for (Sampler* s : m_samplers) {
        delete s;
    }
}

bool EditorMaterialType::serialize(const io::Serializer* serializer, YAML::Emitter& emitter) const {
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

    emitter << YAML::Key << "Samplers" << YAML::BeginSeq;
    for (Sampler* s : m_samplers) {
        emitter << YAML::BeginMap;
        emitter << YAML::Key << "Name" << YAML::Value << s->getName();
        emitter << YAML::Key << "Type" << YAML::Value << static_cast<size_t>(s->getType());
        emitter << YAML::EndMap;
    }
    emitter << YAML::EndSeq;

    emitter << YAML::Key << "Graph";
    serializer->writeObject(emitter, m_graph);

    return true;
}

bool EditorMaterialType::deserialize(io::Serializer* serializer, const YAML::Node& data) {
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

    auto samplers = data["Samplers"];
    if (samplers) {
        for (auto sampler : samplers) {
            // Resolve type
            SamplerType type = static_cast<SamplerType>((sampler["Type"].as<size_t>()));
            std::string name = sampler["Name"].as<std::string>();

            Sampler* sampInstance = new Sampler(type, name);
            m_samplers.push_back(sampInstance);
        }
    }

    delete m_graph;
    m_graph = static_cast<node::Graph*>(serializer->parseObject(data["Graph"]));

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
    this->getFieldsMutable().NumSamplers = m_samplers.size();

    // Temporary vector for storing floats
    std::vector<Property*> floatProperties;
    // Temporary for storing vec2 carry over
    Property* vec2Carry = nullptr;

    // Collect float 1 and set vec4 offsets
    for (Property* prop : m_properties) {
        if (prop->getType() == PropertyType::FLOAT) {
            floatProperties.push_back(prop);
        } else if (prop->getType() == PropertyType::VEC4) {
            // Set offset
            prop->setOffset(offset);
            elements += getElementCountForType(prop->getType());
            offset = elements;
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
    for (Property* prop : floatProperties) {
        prop->setOffset(offset);
        elements += getElementCountForType(prop->getType());
        offset = elements;
    }

    if (elements % 4 != 0) {
        this->getFieldsMutable().Size = ((elements - elements % 4) / 4) + 1;
    } else {
        this->getFieldsMutable().Size = elements / 4;
    }
}

void EditorMaterialType::compile() {
    std::uint64_t vertexDataSize = 0;
    std::uint64_t fragmentDataSize = 0;

    // Recalculate offsets
    this->recalculate();

    const std::filesystem::path projRoot = editor::State::Project->getRootDir();
    const std::filesystem::path headerFile = projRoot / ("Data/material_" + std::to_string(this->getHandle()) + ".sh");
    const std::filesystem::path varyingFile = projRoot / ("Data/varying_" + std::to_string(this->getHandle()) + ".def.sc");
    const std::filesystem::path fragmentFile = projRoot / ("Data/fragment_" + std::to_string(this->getHandle()) + ".fs");
    const std::filesystem::path vertexFile = projRoot / ("Data/vertex_" + std::to_string(this->getHandle()) + ".vs");

    {
        // Open file streams
        std::ofstream of1(headerFile);
        std::ofstream of2(varyingFile);
        std::ofstream of3(fragmentFile);
        std::ofstream of4(vertexFile);

        // Generate sources
        generateMaterialHeader(of1);
        generateVarying(of2);
        generateFragment(of3);
        generateVertex(of4);
    }

    // Compile into binaries
    compiler::ShaderCompiler sc(vertexFile, fragmentFile, varyingFile);

    // TODO: Handle result
    sc.compile();

    // Merge the binaries into a loadable chunk
    io::DataChunk chunk = ::aderite::Engine::getFileHandler()->openLoadable(this->getHandle());

    // Read all contents
    std::ifstream inVertex(sc.getVertexBinPath(), std::ios::binary);
    std::ifstream inFragment(sc.getFragmentBinPath(), std::ios::binary);

    // Get sizes
    vertexDataSize = inVertex.seekg(0, std::ios::end).tellg();
    fragmentDataSize = inFragment.seekg(0, std::ios::end).tellg();

    // Reset pointer
    inVertex.seekg(0, std::ios::beg);
    inFragment.seekg(0, std::ios::beg);

    chunk.Data.resize(vertexDataSize + fragmentDataSize + sizeof(std::uint64_t));

    // Write the vertex length
    std::memcpy(chunk.Data.data(), &vertexDataSize, sizeof(std::uint64_t));

    // Write shader contents
    inVertex.read(reinterpret_cast<char*>(chunk.Data.data() + sizeof(std::uint64_t)), vertexDataSize);
    inFragment.read(reinterpret_cast<char*>(chunk.Data.data() + vertexDataSize + sizeof(std::uint64_t)), fragmentDataSize);

    ::aderite::Engine::getFileHandler()->commit(chunk);

    // Now flag for reload
    this->unload();
}

EditorMaterialType::Properties EditorMaterialType::getProperties() const {
    return m_properties;
}

EditorMaterialType::Samplers EditorMaterialType::getSamplers() const {
    return m_samplers;
}

void EditorMaterialType::addProperty(Property* prop) {
    m_properties.push_back(prop);
}

void EditorMaterialType::addSampler(Sampler* sampler) {
    m_samplers.push_back(sampler);
}

void EditorMaterialType::removeProperty(Property* prop) {
    auto it = std::find(m_properties.begin(), m_properties.end(), prop);
    ADERITE_DYNAMIC_ASSERT(it != m_properties.end(), "Tried to remove property that is not part of the type");
    m_properties.erase(it);
    this->recalculate();
}

void EditorMaterialType::removeSampler(Sampler* sampler) {
    auto it = std::find(m_samplers.begin(), m_samplers.end(), sampler);
    ADERITE_DYNAMIC_ASSERT(it != m_samplers.end(), "Tried to remove sampler that is not part of the type");
    m_samplers.erase(it);
    this->recalculate();
}

void EditorMaterialType::generateMaterialHeader(std::ostream& os) {
    LOG_TRACE("Generating material header");

    // Header comment
    auto t = std::time(nullptr);
    auto tm = *std::localtime(&t);

    os << "/*\n";
    os << " *"
       << " DON'T CHANGE DIRECTLY"
       << "\n";
    os << " *"
       << " This is a shader header file generated by aderite for material " << this->getHandle() << "\n";
    os << " *"
       << " Generated at " << std::put_time(&tm, "%Y-%m-%d %H.%M.%S") << "\n";
    os << " */"
       << "\n\n";

    // Uniform buffer
    std::stringstream properties;
    std::stringstream samplers;

    unsigned int arraySize = this->getFields().Size;
    if (arraySize > 0) {
        properties << "uniform vec4 u_mat_buffer_" << this->getHandle() << "[" << arraySize << "];\n\n";
    }

    // Properties
    properties << "// Properties\n";
    samplers << "\n"
             << "// Samplers\n";

    // Should be packed already by recalculate call
    unsigned int samplerIdx = 0;
    for (asset::Property* prop : this->m_properties) {
        // Need to generate in a packed manner therefor
        // Recalculate method prioritizes this structure:
        // vec4
        // vec3 float
        // vec2 vec2
        // vec2 float float
        // float float float float

        unsigned int arrayRemainderIdx = prop->getOffset() % 4; // This is the current element
        unsigned int arrayIdx = (prop->getOffset() - arrayRemainderIdx) / 4;
        uint8_t elementCount = asset::getElementCountForType(prop->getType());
        std::string access = "";

        switch (elementCount) {
        case 4: {
            // vec4 will always be first so no need to check for packing
            access = "xyzw";
            break;
        }
        case 3: {
            // vec3 will always be first so xyz
            access = "xyz";
            break;
        }
        case 2: {
            // vec2 can be first or second
            if (arrayRemainderIdx == 0) {
                // first
                access = "xy";
            } else {
                // second
                access = "zw";
            }
            break;
        }
        case 1: {
            // Check
            switch (arrayRemainderIdx) {
            case 0: {
                access = "x";
                break;
            }
            case 1: {
                access = "y";
                break;
            }
            case 2: {
                access = "z";
                break;
            }
            case 3: {
                access = "w";
                break;
            }
            default: {
                LOG_ERROR("Incorrect format, remainder {0}", arrayRemainderIdx);
            }
            }
            break;
        }
        }

        switch (prop->getType()) {
        case asset::PropertyType::FLOAT:
        case asset::PropertyType::VEC2:
        case asset::PropertyType::VEC3:
        case asset::PropertyType::VEC4: {
            properties << "#define u_" << this->getHandle() << "_" << prop->getName() << " u_mat_buffer_" << this->getHandle() << "["
                       << arrayIdx << "]." << access << "\n";
            break;
        }
        default:
            LOG_WARN("Unknown property type when generating header");
        }
    }

    for (Sampler* sampler : m_samplers) {
        switch (sampler->getType()) {
        case asset::SamplerType::TEXTURE_2D: {
            samplers << "SAMPLER2D(u_" << this->getHandle() << "_" << sampler->getName() << ", " << samplerIdx++ << ");\n";
            break;
        }
        case asset::SamplerType::TEXTURE_CUBE: {
            samplers << "SAMPLERCUBE(u_" << this->getHandle() << "_" << sampler->getName() << ", " << samplerIdx++ << ");\n";
            break;
        }
        }
    }

    // Merge
    os << properties.str();
    os << samplers.str();
}

void EditorMaterialType::generateVarying(std::ostream& os) {
    LOG_TRACE("Generating varying.def.sc");

    // Outputs
    os << "vec3 v_normal    : NORMAL    = vec3(0.0, 0.0, 1.0);\n";
    os << "vec2 v_texcoord  : TEXCOORD0 = vec2(0.0, 0.0);\n";

    os << "\n";

    // Inputs
    os << "vec3 a_position  : POSITION;\n";
    os << "vec3 a_normal    : NORMAL;\n";
    os << "vec2 a_texcoord0 : TEXCOORD0;\n";
}

void EditorMaterialType::generateFragment(std::ostream& os) {
    LOG_TRACE("Generating fragment shader");
    compiler::ShaderEvaluator fragmentWriter(compiler::ShaderEvaluator::ShaderType::FRAGMENT, this);
    m_graph->resetEvaluateFlag();
    m_graph->getLastNode()->evaluate(&fragmentWriter);
    fragmentWriter.write(os);
}

void EditorMaterialType::generateVertex(std::ostream& os) {
    // TODO: Vertex manipulator graph
    LOG_TRACE("Generating vertex shader");

    // Header comment
    auto t = std::time(nullptr);
    auto tm = *std::localtime(&t);

    // Inputs, outputs
    os << "$input a_position, a_normal, a_texcoord0\n";
    os << "$output v_normal, v_texcoord\n\n";

    os << "/*\n";
    os << " *"
       << " DON'T CHANGE DIRECTLY"
       << "\n";
    os << " *"
       << " This is a vertex shader header file generated by aderite for material " << this->getHandle() << "\n";
    os << " *"
       << " Generated at " << std::put_time(&tm, "%Y-%m-%d %H.%M.%S") << "\n";
    os << " */"
       << "\n\n";

    // Includes
    os << "#include \"bgfx_shader.sh\"\n";
    os << "#include \"shaderLib.sh\"\n";
    os << "#include \"material_" << this->getHandle() << ".sh\"\n";
    os << "\n";

    // Main entry
    os << "void main()\n{\n\t";

    // gl_Position
    os << "gl_Position = mul(u_modelViewProj, vec4(a_position, 1.0));\n\t";

    // Texcoord and normals
    os << "v_texcoord = a_texcoord0;\n\t";
    os << "v_normal = a_normal;\n";

    // Close main
    os << "}\n";
}

} // namespace asset
} // namespace aderite
