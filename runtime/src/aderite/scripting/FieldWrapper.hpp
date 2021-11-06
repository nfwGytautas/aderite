#pragma once

#include <string>

#include <mono/jit/jit.h>

namespace aderite {
namespace scripting {

/**
 * @brief Enum for getting the type of a field
 */
enum class FieldType {
    Null,
    Float,
    Boolean,
    Object,
    Mesh,
    Material,
    Behavior,
    Audio,
    AudioSource,
};

/**
 * @brief Wrappper for mono fields
 */
class FieldWrapper final {
public:
    FieldWrapper(MonoClassField* field);

    /**
     * @brief Returns the name of the field
     */
    std::string getName() const;

    /**
     * @brief Returns the type of the field
     */
    FieldType getType() const;

    /**
     * @brief Get value and store it in the specified pointer
     * @param instance Instance of the object
     * @param target Pointer where value will be stored, must be correct size
     */
    void getValue(MonoObject* instance, void* value);

    /**
     * @brief Returns the value of the field in the specified instance
     */
    MonoObject* getValueObject(MonoObject* instance);

    /**
     * @brief Sets the value of the field for the specified instance
     * @param instance Instance to set for
     * @param value New value to set
     */
    void setValue(MonoObject* instance, void* value);

private:
    MonoClassField* m_field = nullptr;
    std::string m_name;
    FieldType m_type = FieldType::Null;
};

} // namespace scripting
} // namespace aderite
