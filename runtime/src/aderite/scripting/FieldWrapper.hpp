#pragma once

#include <string>

#include <mono/jit/jit.h>

#include "aderite/io/Forward.hpp"
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

    // ============================================================================
    // Helper functions for converting to and from C++ objects
    // ============================================================================

    /**
     * @brief Get the field value as a value type object
     * @tparam T Type of the field
     * @return Value of the field
     */
    template<typename T>
    T getValueType() const {
        // Just create object and create a copy
        T value;
        this->getValue(&value);
        return value;
    }

    /**
     * @brief Set the field value
     * @tparam T Type of the field
     * @param value New value of the field
     */
    template<typename T>
    void setValueType(T value) const {
        this->setValue(&value);
    }

    /**
     * @brief If the type is of serializable, then this method can be used to directly extract the C++ instance
     * @return ISerializable instance
     */
    io::SerializableAsset* getSerializable() const;

    /**
     * @brief If the type is of serializable, then this method can be used to directly set the instance of it
     * @param serializable New instance of the field
     */
    void setSerializable(io::SerializableAsset* serializable) const;

private:
    MonoClassField* m_field = nullptr;
    MonoObject* m_instance = nullptr;
    std::string m_name;
    FieldType m_type = FieldType::Null;
};

} // namespace scripting
} // namespace aderite
