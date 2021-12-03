#include "ScriptEventMap.hpp"

namespace aderite {
namespace scripting {

bool ScriptEventMap::serialize(const io::Serializer* serializer, YAML::Emitter& emitter) const {
    emitter << YAML::Key << "ScriptEventMap" << YAML::BeginMap;
    emitter << YAML::EndMap;

    return true;
}

bool ScriptEventMap::deserialize(io::Serializer* serializer, const YAML::Node& data) {
    const YAML::Node& mapNode = data["ScriptEventMap"];
    if (!mapNode || mapNode.IsNull()) {
        return false;
    }

    return true;
}

} // namespace scripting
}
