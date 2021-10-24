#pragma once

#include <string>

namespace aderite {
namespace asset {

/**
 * @brief Supported types of properties
*/
enum class PropertyType {
	NONE = -1,
	FLOAT = 0,
	VEC2 = 1,
	VEC3 = 2,
	VEC4 = 3,
	COUNT,
};

/**
 * @brief Supported types of samplers
*/
enum class SamplerType {
	TEXTURE_2D = 0,
	TEXTURE_CUBE = 1,
	COUNT,
};

/**
 * @brief Returns the amount of float elements in property type
 * @param type Type of property
 * @return Number of float elements
*/
uint8_t getElementCountForType(PropertyType type);

/**
 * @brief Returns the string representation of the property type
 * @param type Property type
 * @return String representation
*/
const char* getNameForType(PropertyType type);

/**
 * @brief Returns the string representation of the sampler type
 * @param type Sampler type
 * @return String representation
*/
const char* getNameForType(SamplerType type);

/**
 * @brief Base class for material properties
*/
class Property {
public:
	Property(PropertyType type, const std::string& name);
	virtual ~Property() {};

	/**
	 * @brief Returns the type of the property
	*/
	PropertyType getType() const;

	/**
	 * @brief Returns the value of the property
	 * @param data Material data array
	 * @return Pointer to property data
	*/
	float* getValue(float* data) const;

	/**
	 * @brief Sets the value of the property
	 * @param data Material data array
	 * @param value New value
	*/
	void setValue(float* data, float* value);

	/**
	 * @brief Returns the offset into data buffer
	 * @return
	*/
	uint16_t getOffset() const;

	/**
	 * @brief Sets the offset of the property
	 * @param offset New offset
	*/
	void setOffset(uint16_t offset);

	/**
	 * @brief Returns property name
	*/
	std::string getName() const;

	/**
	 * @brief Sets the name of the property
	 * @param name New name of the property
	*/
	void setName(const std::string& name);
private:
	std::string m_name;
	uint16_t m_offset = 0;
	PropertyType m_type;
};

/**
 * @brief Sampler object for material types
*/
class Sampler {
public:
	Sampler(SamplerType type, const std::string& name);
	virtual ~Sampler() {};

	/**
	 * @brief Returns the type of the property
	*/
	SamplerType getType() const;

	/**
	 * @brief Returns property name
	*/
	std::string getName() const;

	/**
	 * @brief Sets the name of the property
	 * @param name New name of the property
	*/
	void setName(const std::string& name);
private:
	std::string m_name;
	SamplerType m_type;
};

}
}
