#pragma once

#include <string>
#include <vector>

#include <mono/jit/jit.h>

#include "aderite/asset/Forward.hpp"
#include "aderite/audio/Forward.hpp"
#include "aderite/io/ISerializable.hpp"
#include "aderite/scene/Forward.hpp"
#include "aderite/scripting/FieldType.hpp"
#include "aderite/scripting/Forward.hpp"

namespace aderite {
namespace scripting {

/**
 * @brief Script class data object
 */
class ScriptData final : public io::ISerializable {
public:
    ScriptData(scene::Scene* scene);

    const char* getScriptName() const;

    ScriptClass* getClass() const;

    void setScriptName(const std::string& name);

    bool isEmpty() const;

    void clear();

    void load();

    MonoObject* getInstance() const;

    // Inherited via ISerializable
    virtual bool serialize(const io::Serializer* serializer, YAML::Emitter& emitter) const override;
    virtual bool deserialize(io::Serializer* serializer, const YAML::Node& data) override;

private:
    scene::Scene* m_scene = nullptr;
    ScriptClass* m_klass = nullptr;

    MonoObject* m_instance = nullptr;
};

} // namespace scripting
} // namespace aderite
