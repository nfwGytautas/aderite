#pragma once

#include "aderite/asset/MaterialTypeAsset.hpp"
#include "aderite/asset/property/Property.hpp"
#include "aderiteeditor/utility/Macros.hpp"
#include "aderiteeditor/node/Forward.hpp"
#include "aderiteeditor/compiler/Forward.hpp"

ADERITE_EDITOR_COMPILER_NAMESPACE_BEGIN

/**
 * @brief Various editor specific object compiler
*/
class Compiler
{
public:
	/**
	 * @brief Compiles a graph
	 * @param graph Graph to compiler
	*/
	static void compileGraph(node::Graph* graph);

	/**
	 * @brief Returns access variable for material property
	 * @param material Material where property exists in
	 * @param prop Property instance
	 * @return Variable access
	*/
	static compiler::Variable getMaterialProperty(asset::MaterialTypeAsset* material, asset::prop::Property* prop);

	/**
	 * @brief Compiles material asset to the current raw directory
	 * @param type Type to compile
	*/
	static void compileMaterialType(asset::MaterialTypeAsset* type);

private:
	/**
	 * @brief Generates a varying.def.sc file
	 * @param type Type to generate for
	*/
	static void generateVarying(asset::MaterialTypeAsset* type);

	/**
	 * @brief Generates a material type shader header
	 * @param type Type to generate for
	*/
	static void generateMaterialHeader(asset::MaterialTypeAsset* type);

	/**
	 * @brief Generates a vertex shader for material
	 * @param type Type to generate for
	*/
	static void generateVertexShader(asset::MaterialTypeAsset* type);

	/**
	 * @brief Generates a fragment shader for material
	 * @param type Type to generate for
	 */
	static void generateFragmentShader(asset::MaterialTypeAsset* type);
};

ADERITE_EDITOR_COMPILER_NAMESPACE_END
