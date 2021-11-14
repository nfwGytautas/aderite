#pragma once

#include <string>

#include <mono/jit/jit.h>

#include "aderite/scripting/FieldType.hpp"

namespace aderite {
namespace scripting {

/**
 * @brief Wrappper for mono fields
 */
class FieldWrapper final {
public:
    FieldWrapper(MonoClassField* field, MonoObject* instance);

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
    void getValue(void* value) const;

    /**
     * @brief Returns the value of the field in the specified instance
     */
    MonoObject* getValueObject() const;

    /**
     * @brief Sets the value of the field for the specified instance
     * @param instance Instance to set for
     * @param value New value to set
     */
    void setValue(void* value) const;

private:
    MonoClassField* m_field = nullptr;
    MonoObject* m_instance = nullptr;
    std::string m_name;
    FieldType m_type = FieldType::Null;
};

} // namespace scripting
} // namespace aderite
