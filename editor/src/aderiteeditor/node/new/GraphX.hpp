#pragma once

#include <string>
#include <unordered_map>
#include <vector>

namespace aderite {
namespace node {

class Node;
class InNodePin;
class OutNodePin;

enum class PinType { None = 0, Float = 1 };

class Node {
public:
    Node();

    const std::string& getName() const;

    std::vector<InNodePin>& getInPins();
    std::vector<OutNodePin>& getOutPins();

    virtual const char* getTypeName() const = 0;

    virtual void renderBody() {}

protected:
    std::vector<InNodePin> p_inPins;
    std::vector<OutNodePin> p_outPins;

private:
    std::string m_name = "";
};

class InNodePin {
public:
    InNodePin(Node* node, PinType type);

    const std::string& getPinName() const;
    std::string getFullName() const;

    void setType(PinType type);
    PinType getType() const;

    OutNodePin* getConnectOutPin() const;
    void setConnectedOutPin(OutNodePin* out);

private:
    PinType m_type = PinType::None;
    Node* m_node = nullptr;
    std::string m_name = "";

    OutNodePin* m_outNode = nullptr;
};

class OutNodePin {
public:
    OutNodePin(Node* node, PinType type);

    const std::string& getPinName() const;
    std::string getFullName() const;

    void setType(PinType type);
    PinType getType() const;

private:
    PinType m_type = PinType::None;
    Node* m_node = nullptr;
    std::string m_name = "";
};

class Graph {
public:
    void addNode(Node* node);
    void removeNode(Node* node);

    const std::vector<Node*>& getNodes() const;

    void render();

private:
    int getId(const std::string& name);

private:
    std::vector<Node*> m_nodes;

    int m_nextId = 0;

    std::unordered_map<std::string, int> m_idMap;
};

} // namespace node
} // namespace aderite
