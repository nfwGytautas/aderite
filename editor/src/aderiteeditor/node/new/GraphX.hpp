#pragma once

#include <string>
#include <unordered_map>
#include <vector>

#include <glm/glm.hpp>

#include "aderite/io/ISerializable.hpp"

#include "aderiteeditor/asset/Forward.hpp"
#include "aderiteeditor/compiler/Forward.hpp"

namespace aderite {
namespace node {

class Graph;
class Node;
class InNodePin;
class OutNodePin;
class MaterialInputNode;

enum class PinType { None = 0, Float = 1, Vec2 = 2, Vec3 = 3, Vec4 = 4, Texture2D = 5, TextureCube = 6 };

class Node : public io::ISerializable {
public:
    Node();

    const std::string& getName() const;
    void setName(const std::string& name);

    const glm::vec2& getPosition() const;
    void setPosition(const glm::vec2& position);

    std::vector<InNodePin>& getInPins();
    std::vector<OutNodePin>& getOutPins();

    virtual const char* getTypeName() const = 0;
    virtual void evaluate(compiler::ShaderEvaluator* evaluator) = 0;

    virtual void renderBody() {}

    // Inherited via ISerializable
    bool serialize(const io::Serializer* serializer, YAML::Emitter& emitter) const override;
    bool deserialize(io::Serializer* serializer, const YAML::Node& data) override;

protected:
    std::vector<InNodePin> p_inPins;
    std::vector<OutNodePin> p_outPins;

private:
    std::string m_name = "";
    glm::vec2 m_position = glm::vec2(0.0f, 0.0f);
};

class InNodePin {
public:
    InNodePin(Node* node, PinType type, const std::string& name);
    InNodePin(Node* node, PinType type);

    const std::string& getPinName() const;
    std::string getFullName() const;

    void setType(PinType type);
    PinType getType() const;

    const std::string& getConnectedOutPinName() const;
    void setConnectedOutPin(const std::string& fullOutName);

private:
    PinType m_type = PinType::None;
    Node* m_node = nullptr;
    std::string m_name = "";
    std::string m_outNodeName = "";
};

class OutNodePin {
public:
    OutNodePin(Node* node, PinType type, const std::string& name);
    OutNodePin(Node* node, PinType type);

    const std::string& getPinName() const;
    std::string getFullName() const;
    Node* getNode() const;

    void setType(PinType type);
    PinType getType() const;

private:
    PinType m_type = PinType::None;
    Node* m_node = nullptr;
    std::string m_name = "";
};

class Graph : public io::ISerializable {
public:
    virtual ~Graph();

    void addNode(Node* node);
    void removeNode(Node* node);

    const std::vector<Node*>& getNodes() const;

    void render();

    void clear();

    void evaluate(compiler::ShaderEvaluator* evaluator, Node* node);

    // Inherited via ISerializable
    virtual bool serialize(const io::Serializer* serializer, YAML::Emitter& emitter) const override;
    virtual bool deserialize(io::Serializer* serializer, const YAML::Node& data) override;

protected:
    /**
     * @brief Create a node of the specified type
     * @param type Type of the node
     * @return Node instance
     */
    virtual Node* createNodeInstance(const std::string& type) = 0;

private:
    int getId(const std::string& name);
    int getId(const std::string& name, bool& isNew);
    OutNodePin* getOutputPin(const std::string& name) const;

    void pushEvaluationNode(std::vector<Node*>& list, Node* currentNode);

private:
    std::vector<Node*> m_nodes;

    int m_nextId = 0;

    std::unordered_map<std::string, int> m_idMap;
};

class MaterialInputNode : public Node {
public:
    MaterialInputNode();

    void setMaterial(asset::EditorMaterialType* material);

    // Inherited via Node
    const char* getTypeName() const override;
    void evaluate(compiler::ShaderEvaluator* evaluator) override;
};

class MaterialOutputNode : public Node {
public:
    MaterialOutputNode();

    // Inherited via Node
    const char* getTypeName() const override;
    void evaluate(compiler::ShaderEvaluator* evaluator) override;
};

class AddNode : public Node {
public:
    AddNode();

    void setType(PinType type);
    PinType getType() const;

    // Inherited via Node
    void renderBody() override;
    const char* getTypeName() const override;
    bool serialize(const io::Serializer* serializer, YAML::Emitter& emitter) const override;
    bool deserialize(io::Serializer* serializer, const YAML::Node& data) override;
    void evaluate(compiler::ShaderEvaluator* evaluator) override;

private:
    PinType m_type = PinType::Float;
};

class Sample2DTextureNode : public Node {
public:
    Sample2DTextureNode();

    // Inherited via Node
    const char* getTypeName() const override;
    void evaluate(compiler::ShaderEvaluator* evaluator) override;
};

class VertexUVProviderNode : public Node {
public:
    VertexUVProviderNode();

    // Inherited via Node
    const char* getTypeName() const override;
    void evaluate(compiler::ShaderEvaluator* evaluator) override;
};

class ConstantValueProviderNode : public Node {
public:
    ConstantValueProviderNode();

    void setType(PinType type);
    PinType getType() const;

    // Inherited via Node
    void renderBody() override;
    const char* getTypeName() const override;
    bool serialize(const io::Serializer* serializer, YAML::Emitter& emitter) const override;
    bool deserialize(io::Serializer* serializer, const YAML::Node& data) override;
    void evaluate(compiler::ShaderEvaluator* evaluator) override;

private:
    PinType m_type = PinType::Float;

    union {
        float Fval;
        glm::vec2 v2Val;
        glm::vec3 v3Val;
        glm::vec4 v4Val = glm::vec4(0.0f);
    } m_value;
};

class Vec4Node : public Node {
public:
    enum class Constructor { FourFloats, TwoVec2, Vec3Float, TwoFloatVec2, Count };

public:
    Vec4Node();

    void setConstructor(Constructor ctor);
    Constructor getConstructor() const;

    // Inherited via Node
    void renderBody() override;
    const char* getTypeName() const override;
    void evaluate(compiler::ShaderEvaluator* evaluator) override;
    bool serialize(const io::Serializer* serializer, YAML::Emitter& emitter) const override;
    bool deserialize(io::Serializer* serializer, const YAML::Node& data) override;

private:
    Constructor m_ctor = Constructor::FourFloats;
};

} // namespace node
} // namespace aderite
