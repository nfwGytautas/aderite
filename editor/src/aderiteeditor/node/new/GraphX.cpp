#include "GraphX.hpp"
#include <stack>
#include <unordered_map>

#include "aderite/utility/Macros.hpp"
#include "aderite/utility/Random.hpp"
#include "aderite/utility/YAML.hpp"

#include "aderiteeditor/asset/EditorMaterialType.hpp"
#include "aderiteeditor/asset/property/Property.hpp"
#include "aderiteeditor/compiler/ShaderEvaluator.hpp"
#include "aderiteeditor/platform/pc/backend/node/imnodes.h"
#include "aderiteeditor/platform/pc/backend/node/imnodes_aderite.hpp"

namespace aderite {
namespace node {

static constexpr const char* c_PinTypeMap[] = {"None", "Float", "Vec2", "Vec3", "Vec4", "Texture 2D", "Texture Cube"};
static constexpr const char* c_PinTypeToShader[] = {"UNKNOWN", "float", "vec2", "vec3", "vec4", "BgfxSampler2D", "BgfxSamplerCube"};

Graph::~Graph() {
    this->clear();
}

void Graph::addNode(Node* node) {
    m_nodes.push_back(node);
}

void Graph::removeNode(Node* node) {
    auto it = std::find(m_nodes.begin(), m_nodes.end(), node);

    delete *it;
    m_nodes.erase(it);
}

const std::vector<Node*>& Graph::getNodes() const {
    return m_nodes;
}

void Graph::render() {
    bool isNew = false;

    // Nodes
    for (Node* node : m_nodes) {
        const std::string nodeName = node->getName();
        int id = this->getId(nodeName, isNew);

        if (isNew) {
            // Set initial position
            ImNodes::SetNodeGridSpacePos(id, {node->getPosition().x, node->getPosition().y});
        } else {
            ImVec2 pos = ImNodes::GetNodeGridSpacePos(id);
            node->setPosition({pos.x, pos.y});
        }

        // Render node
        ImNodes::BeginNode(id);
        ImNodes::SetNodeUserData(id, node);

        ImNodes::BeginNodeTitleBar();
        ImGui::TextUnformatted(node->getTypeName());
        ImNodes::EndNodeTitleBar();

        node->renderBody();

        // Render pins
        for (InNodePin& ipin : node->getInPins()) {
            int pinId = this->getId(ipin.getFullName());

            ImNodes::BeginInputAttribute(pinId);
            ImNodes::SetPinUserData(pinId, &ipin);
            ImGui::Text("[In  %s] %s", c_PinTypeMap[static_cast<size_t>(ipin.getType())], ipin.getPinName().c_str());
            ImNodes::EndInputAttribute();
        }

        ImGui::Dummy(ImVec2(0, 10));

        for (OutNodePin& opin : node->getOutPins()) {
            int pinId = this->getId(opin.getFullName());

            ImNodes::BeginOutputAttribute(pinId);
            ImNodes::SetPinUserData(pinId, &opin);
            ImGui::Text("[Out %s] %s", c_PinTypeMap[static_cast<size_t>(opin.getType())], opin.getPinName().c_str());
            ImNodes::EndOutputAttribute();
        }

        ImNodes::EndNode();
    }

    // Links
    for (Node* node : m_nodes) {
        for (InNodePin& ipin : node->getInPins()) {
            int ipinId = this->getId(ipin.getFullName());
            const OutNodePin* opin = this->getOutputPin(ipin.getConnectedOutPinName());
            if (opin != nullptr) {
                if (ipin.getType() != opin->getType()) {
                    // Disconnect, type mismatch
                    ipin.setConnectedOutPin("");
                } else {
                    // Render connection
                    int opinId = this->getId(opin->getFullName());
                    ImNodes::Link(ipinId, opinId, ipinId);
                    ImNodes::SetLinkUserData(ipinId, &ipin);
                }
            } else {
                // Invalid pin
                ipin.setConnectedOutPin("");
            }
        }
    }
}

void Graph::clear() {
    m_idMap.clear();
    m_nextId = 0;

    for (Node* node : m_nodes) {
        delete node;
    }

    m_nodes.clear();
}

void Graph::evaluate(compiler::ShaderEvaluator* evaluator, Node* node) {
    std::vector<Node*> evaluationOrder;

    // Create evaluation order
    pushEvaluationNode(evaluationOrder, node);

    // Evaluate
    for (Node* node : evaluationOrder) {
        node->evaluate(evaluator);
    }
}

int Graph::getId(const std::string& name) {
    bool unused = false;
    return getId(name, unused);
}

int Graph::getId(const std::string& name, bool& isNew) {
    ADERITE_DYNAMIC_ASSERT(!name.empty(), "Empty name passed to getId");
    isNew = false;

    auto it = m_idMap.find(name);

    if (it == m_idMap.end()) {
        m_idMap[name] = m_nextId++;
        isNew = true;
    }

    return m_idMap[name];
}

OutNodePin* Graph::getOutputPin(const std::string& name) const {
    const size_t sepIdx = name.find_first_of('_');

    if (sepIdx == size_t(-1)) {
        // Invalid name
        return nullptr;
    }

    // Parse the name
    const std::string& nodeName = name.substr(0, sepIdx);
    const std::string& pinName = name.substr(sepIdx + 1);

    // Find node
    auto itNode = std::find_if(m_nodes.begin(), m_nodes.end(), [&nodeName](Node* node) {
        return node->getName() == nodeName;
    });

    if (itNode == m_nodes.end()) {
        return nullptr;
    }

    Node* node = *itNode;
    auto& outPins = node->getOutPins();

    // Find pin
    auto pinNode = std::find_if(outPins.begin(), outPins.end(), [&pinName](OutNodePin& pin) {
        return pin.getPinName() == pinName;
    });

    if (pinNode == outPins.end()) {
        return nullptr;
    }

    return &*pinNode;
}

void Graph::pushEvaluationNode(std::vector<Node*>& list, Node* currentNode) {
    // Check if already evaluated
    if (std::find(list.begin(), list.end(), currentNode) != list.end()) {
        // Already evaluated skip
        return;
    }

    for (InNodePin& ipin : currentNode->getInPins()) {
        OutNodePin* opin = this->getOutputPin(ipin.getConnectedOutPinName());

        if (opin != nullptr) {
            pushEvaluationNode(list, opin->getNode());
        }
    }

    list.push_back(currentNode);
}

bool Graph::serialize(const io::Serializer* serializer, YAML::Emitter& emitter) const {
    emitter << YAML::Key << "Graph" << YAML::BeginMap;
    emitter << YAML::Key << "Nodes" << YAML::BeginSeq;
    for (node::Node* node : this->getNodes()) {
        emitter << YAML::BeginMap;

        // Properties
        emitter << YAML::Key << "Type" << YAML::Value << node->getTypeName();

        // Pins
        node->serialize(serializer, emitter);

        emitter << YAML::EndMap;
    }
    emitter << YAML::EndSeq;
    emitter << YAML::EndMap;

    return true;
}

bool Graph::deserialize(io::Serializer* serializer, const YAML::Node& data) {
    this->clear();

    const YAML::Node& graphNode = data["Graph"];
    if (!graphNode || graphNode.IsNull()) {
        return false;
    }

    const YAML::Node& nodesNode = graphNode["Nodes"];
    if (!nodesNode || nodesNode.IsNull()) {
        return false;
    }

    for (const YAML::Node& nodeNode : nodesNode) {
        const std::string& type = nodeNode["Type"].as<std::string>();
        Node* node = this->createNodeInstance(type);
        if (!node->deserialize(serializer, nodeNode)) {
            return false;
        }
        this->addNode(node);
    }

    return true;
}

OutNodePin::OutNodePin(Node* node, PinType type, const std::string& name) : m_node(node), m_type(type), m_name(name) {}

OutNodePin::OutNodePin(Node* node, PinType type) : m_node(node), m_type(type), m_name(utility::generateString(12)) {}

const std::string& OutNodePin::getPinName() const {
    return m_name;
}

std::string OutNodePin::getFullName() const {
    return m_node->getName() + "_" + m_name;
}

Node* OutNodePin::getNode() const {
    return m_node;
}

void OutNodePin::setType(PinType type) {
    m_type = type;
}

PinType OutNodePin::getType() const {
    return m_type;
}

InNodePin::InNodePin(Node* node, PinType type, const std::string& name) : m_node(node), m_type(type), m_name(name) {}

InNodePin::InNodePin(Node* node, PinType type) : m_node(node), m_type(type), m_name(utility::generateString(12)) {}

const std::string& InNodePin::getPinName() const {
    return m_name;
}

std::string InNodePin::getFullName() const {
    return m_node->getName() + "_" + m_name;
}

void InNodePin::setType(PinType type) {
    m_type = type;
}

PinType InNodePin::getType() const {
    return m_type;
}

const std::string& InNodePin::getConnectedOutPinName() const {
    return m_outNodeName;
}

void InNodePin::setConnectedOutPin(const std::string& fullOutName) {
    m_outNodeName = fullOutName;
}

Node::Node() : m_name(utility::generateString(12)) {}

const std::string& Node::getName() const {
    return m_name;
}

void Node::setName(const std::string& name) {
    ADERITE_DYNAMIC_ASSERT(!name.empty(), "Node can't have empty name");
    m_name = name;
}

const glm::vec2& Node::getPosition() const {
    return m_position;
}

void Node::setPosition(const glm::vec2& position) {
    m_position = position;
}

std::vector<InNodePin>& Node::getInPins() {
    return p_inPins;
}

std::vector<OutNodePin>& Node::getOutPins() {
    return p_outPins;
}

bool Node::serialize(const io::Serializer* serializer, YAML::Emitter& emitter) const {
    emitter << YAML::Key << "Name" << YAML::Value << m_name;
    emitter << YAML::Key << "Position" << YAML::Value << m_position;

    emitter << YAML::Key << "Pins" << YAML::BeginMap;

    emitter << YAML::Key << "In" << YAML::BeginSeq;
    for (const InNodePin& inode : p_inPins) {
        emitter << YAML::BeginMap;
        emitter << YAML::Key << "Type" << YAML::Value << static_cast<size_t>(inode.getType());
        emitter << YAML::Key << "Name" << YAML::Value << inode.getPinName();
        emitter << YAML::Key << "Connected" << YAML::Value << inode.getConnectedOutPinName();
        emitter << YAML::EndMap;
    }
    emitter << YAML::EndSeq;

    emitter << YAML::Key << "Out" << YAML::BeginSeq;
    for (const OutNodePin& onode : p_outPins) {
        emitter << YAML::BeginMap;
        emitter << YAML::Key << "Type" << YAML::Value << static_cast<size_t>(onode.getType());
        emitter << YAML::Key << "Name" << YAML::Value << onode.getPinName();
        emitter << YAML::EndMap;
    }
    emitter << YAML::EndSeq;

    emitter << YAML::EndMap;

    return true;
}

bool Node::deserialize(io::Serializer* serializer, const YAML::Node& data) {
    // Verify data
    const YAML::Node& pinsNode = data["Pins"];

    if (!pinsNode || pinsNode.IsNull()) {
        return false;
    }

    const YAML::Node& inPinsNode = pinsNode["In"];
    const YAML::Node& outPinsNode = pinsNode["Out"];

    if (!inPinsNode || inPinsNode.IsNull()) {
        return false;
    }

    if (!outPinsNode || outPinsNode.IsNull()) {
        return false;
    }

    p_inPins.clear();
    p_outPins.clear();

    // Deserialize
    m_name = data["Name"].as<std::string>();
    m_position = data["Position"].as<glm::vec2>();

    // Pins
    for (const YAML::Node& inode : inPinsNode) {
        const PinType& type = static_cast<PinType>(inode["Type"].as<size_t>());
        const std::string& name = inode["Name"].as<std::string>();
        const std::string& connection = inode["Connected"].as<std::string>();

        InNodePin ipin(this, type, name);
        ipin.setConnectedOutPin(connection);

        p_inPins.push_back(ipin);
    }

    for (const YAML::Node& onode : outPinsNode) {
        const PinType& type = static_cast<PinType>(onode["Type"].as<size_t>());
        const std::string& name = onode["Name"].as<std::string>();
        p_outPins.push_back(OutNodePin(this, type, name));
    }

    return true;
}

MaterialInputNode::MaterialInputNode() {}

void MaterialInputNode::setMaterial(asset::EditorMaterialType* material) {
    // Create pins
    p_outPins.clear();

    auto props = material->getProperties();
    for (int i = 0; i < props.size(); i++) {
        asset::Property* prop = props[i];
        p_outPins.push_back(OutNodePin(this, static_cast<PinType>(prop->getType()), prop->getName()));
    }

    auto samplers = material->getSamplers();
    for (int i = 0; i < samplers.size(); i++) {
        asset::Sampler* samp = samplers[i];
        p_outPins.push_back(OutNodePin(this, static_cast<PinType>(samp->getType()), samp->getName()));
    }
}

const char* MaterialInputNode::getTypeName() const {
    return "Material";
}

void MaterialInputNode::evaluate(compiler::ShaderEvaluator* evaluator) {
    evaluator->addComment(this->getName());

    for (OutNodePin& opin : p_outPins) {
        evaluator->getMaterialField(opin.getFullName(), opin.getFullName(), c_PinTypeToShader[static_cast<size_t>(opin.getType())]);
    }
}

MaterialOutputNode::MaterialOutputNode() {
    p_inPins.push_back(InNodePin(this, PinType::Vec4, "Output"));
}

const char* MaterialOutputNode::getTypeName() const {
    return "Output";
}

void MaterialOutputNode::evaluate(compiler::ShaderEvaluator* evaluator) {
    evaluator->addComment(this->getName());
    evaluator->addFragmentColorInstruction(p_inPins[0].getConnectedOutPinName());
}

AddNode::AddNode() {
    p_inPins.push_back(InNodePin(this, PinType::Float, "Lhs"));
    p_inPins.push_back(InNodePin(this, PinType::Float, "Rhs"));

    p_outPins.push_back(OutNodePin(this, PinType::Float, "Sum"));
}

void AddNode::setType(PinType type) {
    for (InNodePin& ipin : p_inPins) {
        ipin.setType(type);
    }

    for (OutNodePin& opin : p_outPins) {
        opin.setType(type);
    }

    m_type = type;
}

PinType AddNode::getType() const {
    return m_type;
}

void AddNode::renderBody() {
    static const PinType types[] = {PinType::Float, PinType::Vec2, PinType::Vec3, PinType::Vec4};

    // Dropdown for type
    ImGui::SetNextItemWidth(125.0f);
    if (ImGui::BeginCombo(("##" + this->getName()).c_str(), c_PinTypeMap[static_cast<size_t>(m_type)])) {
        for (size_t i = 0; i < sizeof(types) / sizeof(types[0]); i++) {
            if (ImGui::Selectable(c_PinTypeMap[static_cast<size_t>(types[i])], m_type == types[i])) {
                this->setType(types[i]);
            }

            if (m_type == types[i]) {
                ImGui::SetItemDefaultFocus();
            }
        }

        ImGui::EndCombo();
    }

    ImGui::Dummy(ImVec2(0, 5));
}

const char* AddNode::getTypeName() const {
    return "Add";
}

bool AddNode::serialize(const io::Serializer* serializer, YAML::Emitter& emitter) const {
    if (!Node::serialize(serializer, emitter)) {
        return false;
    }

    emitter << YAML::Key << "OpType" << YAML::Value << static_cast<size_t>(m_type);

    return true;
}

bool AddNode::deserialize(io::Serializer* serializer, const YAML::Node& data) {
    if (!Node::deserialize(serializer, data)) {
        return false;
    }

    this->setType(static_cast<PinType>(data["OpType"].as<size_t>()));

    return true;
}

void AddNode::evaluate(compiler::ShaderEvaluator* evaluator) {
    evaluator->addComment(this->getName());
    evaluator->addAddInstruction(c_PinTypeToShader[static_cast<size_t>(m_type)], p_inPins[0].getConnectedOutPinName(),
                                 p_inPins[1].getConnectedOutPinName(), p_outPins[0].getFullName());
}

Sample2DTextureNode::Sample2DTextureNode() {
    p_inPins.push_back(InNodePin(this, PinType::Texture2D, "Texture"));
    p_inPins.push_back(InNodePin(this, PinType::Vec2, "UV"));

    p_outPins.push_back(OutNodePin(this, PinType::Vec4, "Color"));
}

const char* Sample2DTextureNode::getTypeName() const {
    return "Sample 2D";
}

void Sample2DTextureNode::evaluate(compiler::ShaderEvaluator* evaluator) {
    evaluator->addComment(this->getName());
    evaluator->add2DSamplingInstruction(p_inPins[0].getConnectedOutPinName(), p_inPins[1].getConnectedOutPinName(),
                                        p_outPins[0].getFullName());
}

VertexUVProviderNode::VertexUVProviderNode() {
    p_outPins.push_back(OutNodePin(this, PinType::Vec2, "v_texcoord"));
}

const char* VertexUVProviderNode::getTypeName() const {
    return "Vertex UV";
}

void VertexUVProviderNode::evaluate(compiler::ShaderEvaluator* evaluator) {
    evaluator->addComment(this->getName());
    evaluator->getAttribute("v_texcoord", p_outPins[0].getFullName(), "vec2");
}

ConstantValueProviderNode::ConstantValueProviderNode() {
    p_outPins.push_back(OutNodePin(this, PinType::Float, "Value"));
}

void ConstantValueProviderNode::setType(PinType type) {
    for (OutNodePin& opin : p_outPins) {
        opin.setType(type);
    }

    m_type = type;
}

PinType ConstantValueProviderNode::getType() const {
    return m_type;
}

void ConstantValueProviderNode::renderBody() {
    static const PinType types[] = {PinType::Float, PinType::Vec2, PinType::Vec3, PinType::Vec4};

    // Dropdown for type
    ImGui::SetNextItemWidth(125.0f);
    if (ImGui::BeginCombo(("##" + this->getName()).c_str(), c_PinTypeMap[static_cast<size_t>(m_type)])) {
        for (size_t i = 0; i < sizeof(types) / sizeof(types[0]); i++) {
            if (ImGui::Selectable(c_PinTypeMap[static_cast<size_t>(types[i])], m_type == types[i])) {
                this->setType(types[i]);
            }

            if (m_type == types[i]) {
                ImGui::SetItemDefaultFocus();
            }
        }

        ImGui::EndCombo();
    }

    // Input for value
    ImGui::SetNextItemWidth(225.0f);
    switch (m_type) {
    case PinType::Float: {
        ImGui::InputFloat("Value", &m_value.Fval);
        break;
    }
    case PinType::Vec2: {
        ImGui::InputFloat2("Value", &m_value.v2Val[0]);
        break;
    }
    case PinType::Vec3: {
        ImGui::InputFloat3("Value", &m_value.v3Val[0]);
        break;
    }
    case PinType::Vec4: {
        ImGui::InputFloat4("Value", &m_value.v4Val[0]);
        break;
    }
    default: {
        ADERITE_ABORT("Unknown value provider type");
    }
    }

    ImGui::Dummy(ImVec2(0, 5));
}

const char* ConstantValueProviderNode::getTypeName() const {
    return "Value";
}

bool ConstantValueProviderNode::serialize(const io::Serializer* serializer, YAML::Emitter& emitter) const {
    if (!Node::serialize(serializer, emitter)) {
        return false;
    }

    emitter << YAML::Key << "ValueType" << YAML::Value << static_cast<size_t>(m_type);
    emitter << YAML::Key << "Value" << YAML::Value;

    switch (m_type) {
    case PinType::Float: {
        emitter << m_value.Fval;
        break;
    }
    case PinType::Vec2: {
        emitter << m_value.v2Val;
        break;
    }
    case PinType::Vec3: {
        emitter << m_value.v3Val;
        break;
    }
    case PinType::Vec4: {
        emitter << m_value.v4Val;
        break;
    }
    default: {
        ADERITE_ABORT("Unknown value provider type");
    }
    }

    return true;
}

bool ConstantValueProviderNode::deserialize(io::Serializer* serializer, const YAML::Node& data) {
    if (!Node::deserialize(serializer, data)) {
        return false;
    }

    this->setType(static_cast<PinType>(data["ValueType"].as<size_t>()));

    switch (m_type) {
    case PinType::Float: {
        m_value.Fval = data["Value"].as<float>();
        break;
    }
    case PinType::Vec2: {
        m_value.v2Val = data["Value"].as<glm::vec2>();
        break;
    }
    case PinType::Vec3: {
        m_value.v3Val = data["Value"].as<glm::vec3>();
        break;
    }
    case PinType::Vec4: {
        m_value.v4Val = data["Value"].as<glm::vec4>();
        break;
    }
    default: {
        ADERITE_ABORT("Unknown value provider type");
    }
    }

    return true;
}

void ConstantValueProviderNode::evaluate(compiler::ShaderEvaluator* evaluator) {
    evaluator->addComment(this->getName());

    std::string value;
    switch (m_type) {
    case PinType::Float: {
        value = std::to_string(m_value.Fval);
        break;
    }
    case PinType::Vec2: {
        value = "vec2(" + std::to_string(m_value.v2Val.x) + ", " + std::to_string(m_value.v2Val.y) + ")";
        break;
    }
    case PinType::Vec3: {
        value = "vec3(" + std::to_string(m_value.v3Val.x) + ", " + std::to_string(m_value.v3Val.y) + ", " +
                std::to_string(m_value.v3Val.z) + ")";
        break;
    }
    case PinType::Vec4: {
        value = "vec4(" + std::to_string(m_value.v4Val.x) + ", " + std::to_string(m_value.v4Val.y) + ", " +
                std::to_string(m_value.v4Val.z) + ", " + std::to_string(m_value.v4Val.w) + ")";
        break;
    }
    default: {
        ADERITE_ABORT("Unknown value provider type");
    }
    }

    evaluator->defineVariable(this->p_outPins[0].getFullName(), value, c_PinTypeToShader[static_cast<size_t>(m_type)]);
}

Vec4Node::Vec4Node() {
    this->setConstructor(Constructor::FourFloats);
    p_outPins.push_back(OutNodePin(this, PinType::Vec4, "Out"));
}

void Vec4Node::setConstructor(Constructor ctor) {
    p_inPins.clear();

    switch (ctor) {
    case Constructor::FourFloats: {
        p_inPins.push_back(InNodePin(this, PinType::Float, "x"));
        p_inPins.push_back(InNodePin(this, PinType::Float, "y"));
        p_inPins.push_back(InNodePin(this, PinType::Float, "z"));
        p_inPins.push_back(InNodePin(this, PinType::Float, "w"));
        break;
    }
    case Constructor::TwoVec2: {
        p_inPins.push_back(InNodePin(this, PinType::Vec2, "xy"));
        p_inPins.push_back(InNodePin(this, PinType::Vec2, "zw"));
        break;
    }
    case Constructor::TwoFloatVec2: {
        p_inPins.push_back(InNodePin(this, PinType::Vec2, "xy"));
        p_inPins.push_back(InNodePin(this, PinType::Float, "z"));
        p_inPins.push_back(InNodePin(this, PinType::Float, "w"));
        break;
    }
    case Constructor::Vec3Float: {
        p_inPins.push_back(InNodePin(this, PinType::Vec3, "xyz"));
        p_inPins.push_back(InNodePin(this, PinType::Float, "w"));
    }
    }

    m_ctor = ctor;
}

Vec4Node::Constructor Vec4Node::getConstructor() const {
    return m_ctor;
}

void Vec4Node::renderBody() {
    static const char* ConstructorNames[] = {"4 float", "2 vec2", "vec3, float", "2 float, vec2"};

    // Dropdown for type
    ImGui::SetNextItemWidth(125.0f);
    if (ImGui::BeginCombo(("##" + this->getName()).c_str(), ConstructorNames[static_cast<size_t>(m_ctor)])) {
        for (size_t i = 0; i < static_cast<size_t>(Constructor::Count); i++) {
            if (ImGui::Selectable(ConstructorNames[i], static_cast<size_t>(m_ctor) == i)) {
                this->setConstructor(static_cast<Constructor>(i));
            }

            if (static_cast<size_t>(m_ctor) == i) {
                ImGui::SetItemDefaultFocus();
            }
        }

        ImGui::EndCombo();
    }

    ImGui::Dummy(ImVec2(0, 5));
}

const char* Vec4Node::getTypeName() const {
    return "Vec4";
}

void Vec4Node::evaluate(compiler::ShaderEvaluator* evaluator) {
    evaluator->addComment(this->getName());

    std::string value = "vec4(";

    switch (m_ctor) {
    case Constructor::FourFloats: {
        value += p_inPins[0].getConnectedOutPinName();
        value += ", ";

        value += p_inPins[1].getConnectedOutPinName();
        value += ", ";

        value += p_inPins[2].getConnectedOutPinName();
        value += ", ";

        value += p_inPins[3].getConnectedOutPinName();

        break;
    }
    case Constructor::TwoFloatVec2: {
        value += p_inPins[0].getConnectedOutPinName();
        value += ", ";

        value += p_inPins[1].getConnectedOutPinName();
        value += ", ";

        value += p_inPins[2].getConnectedOutPinName();

        break;
    }
    case Constructor::TwoVec2: {
        value += p_inPins[0].getConnectedOutPinName();
        value += ", ";

        value += p_inPins[1].getConnectedOutPinName();

        break;
    }
    case Constructor::Vec3Float: {
        value += p_inPins[0].getConnectedOutPinName();
        value += ", ";

        value += p_inPins[1].getConnectedOutPinName();
        break;
    }
    }

    value += ")";
    evaluator->defineVariable(p_outPins[0].getFullName(), value, c_PinTypeToShader[static_cast<size_t>(PinType::Vec4)]);
}

bool Vec4Node::serialize(const io::Serializer* serializer, YAML::Emitter& emitter) const {
    if (!Node::serialize(serializer, emitter)) {
        return false;
    }

    emitter << YAML::Key << "CtorType" << YAML::Value << static_cast<size_t>(m_ctor);

    return true;
}

bool Vec4Node::deserialize(io::Serializer* serializer, const YAML::Node& data) {
    if (!Node::deserialize(serializer, data)) {
        return false;
    }

    m_ctor = static_cast<Constructor>(data["CtorType"].as<size_t>());

    return true;
}

} // namespace node
} // namespace aderite
