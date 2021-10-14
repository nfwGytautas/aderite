#include "Compiler.hpp"

#include <sstream>
#include <iomanip>
#include <ctime>
#include <fstream>
#include <string>
#include <algorithm>

#include "aderite/Aderite.hpp"
#include "aderite/utility/Log.hpp"
#include "aderite/asset/AssetManager.hpp"
#include "aderite/rendering/Renderer.hpp"
#include "aderiteeditor/compiler/ShaderEvaluator.hpp"
#include "aderiteeditor/compiler/PipelineEvaluator.hpp"
#include "aderiteeditor/node/Graph.hpp"
#include "aderiteeditor/node/Node.hpp"
#include "aderiteeditor/node/InputPin.hpp"

ADERITE_EDITOR_COMPILER_NAMESPACE_BEGIN

void Compiler::compileGraph(node::Graph* graph) {
	// TODO: Change with actual material name
	ShaderEvaluator fragmentWriter(ShaderEvaluator::ShaderType::FRAGMENT, "ImageMaterialType_mtype");
	
	graph->resetEvaluateFlag();
	graph->getLastNode()->evaluate(&fragmentWriter);

	fragmentWriter.writeToFile();

	std::string typeName = "ImageMaterialType_mtype";
	compileShaderSource((::aderite::Engine::getAssetManager()->getRawDir() / (typeName + ".fs")), ::aderite::Engine::getAssetManager()->getRawDir());
}

void Compiler::compilePipeline(node::Graph* graph) {
	LOG_TRACE("Compiling rendering pipeline");
	PipelineEvaluator evaluator;

	graph->resetEvaluateFlag();
	graph->getLastNode()->evaluate(&evaluator);

	::aderite::Engine::getRenderer()->setPipeline(evaluator.constructPipeline());
}

void Compiler::compileMaterialType(asset::MaterialTypeAsset* type) {
	LOG_TRACE("Compiling material type {0}", type->getName());
	
	// Generate sources
	generateMaterialHeader(type);
	generateVarying(type);
	generateVertexShader(type);

	// Execute compiler
	// TODO: Make into a library function
	// TODO: Select correct platform
	LOG_WARN("USING SYSTEM FOR COMPILING");

	std::string typeName = type->getName();
	std::replace_if(std::begin(typeName), std::end(typeName),
		[](std::string::value_type v) { return v == '.'; },
		'_');

	// Vertex
	std::stringstream vcommand;
	vcommand << "tools\\shadercRelease.exe -f ";
	vcommand << (::aderite::Engine::getAssetManager()->getRawDir() / ("vs_" + typeName + ".sc")).string();
	vcommand << " -o " << (::aderite::Engine::getAssetManager()->getRawDir() / ("vs_" + typeName + ".bin")).string();
	vcommand << " --platform windows --type vertex --verbose --profile vs_5_0 --varyingdef ";
	vcommand << (::aderite::Engine::getAssetManager()->getRawDir() / ("varying.def.sc")).string();

	// Compile
	LOG_TRACE("Running {0}", vcommand.str());
	system(vcommand.str().c_str());

	type->unload();
	type->prepareLoad();
	type->load();
}

void Compiler::compileShaderSource(std::filesystem::path sourceDir, std::filesystem::path destinationDir) {
	// TODO: Replace with lib call
	std::string shaderName = sourceDir.parent_path().filename().string();

	if (!std::filesystem::exists(destinationDir)) {
		std::filesystem::create_directory(destinationDir);
	}

	LOG_TRACE("Compiling {0}", shaderName);

	std::string includePath = (sourceDir.parent_path().parent_path() / "include/").string();

	// Create commands
	std::stringstream fCommand;
	fCommand << "tools\\shadercRelease.exe -f ";
	fCommand << (sourceDir / (shaderName  + ".fs")).string();
	fCommand << " -o " << destinationDir.string() + (shaderName + ".fs.bin");
	fCommand << " --platform windows --verbose ";
	fCommand << " --varyingdef " << (sourceDir / (shaderName + "_varying.def.sc")).string();
	fCommand << " --type fragment --profile ps_5_0 -i ";
	fCommand << includePath;

	std::stringstream vCommand;
	vCommand << "tools\\shadercRelease.exe -f ";
	vCommand << (sourceDir / (shaderName + ".vs")).string();
	vCommand << " -o " << destinationDir.string() + (shaderName + ".vs.bin");
	vCommand << " --platform windows --verbose ";
	vCommand << " --varyingdef " << (sourceDir / (shaderName + "_varying.def.sc")).string();
	vCommand << " --type vertex --profile vs_5_0 -i ";
	vCommand << includePath;

	// Compile
	LOG_TRACE("Running {0}", vCommand.str());
	system(vCommand.str().c_str());
	LOG_TRACE("Running {0}", fCommand.str());
	system(fCommand.str().c_str());
}

void Compiler::generateVarying(asset::MaterialTypeAsset* type) {
	std::string typeName = type->getName();
	std::replace_if(std::begin(typeName), std::end(typeName),
		[](std::string::value_type v) { return v == '.'; },
		'_');

	std::filesystem::path outPath = ::aderite::Engine::getAssetManager()->getRawDir() / ("varying.def.sc");
	std::stringstream output;
	std::ofstream outFile(outPath);
	LOG_TRACE("Generating varying.def.sc");

	// Outputs
	output << "vec3 v_normal    : NORMAL    = vec3(0.0, 0.0, 1.0);\n";
	output << "vec2 v_texcoord  : TEXCOORD0 = vec2(0.0, 0.0);\n";

	output << "\n";

	// Inputs
	output << "vec3 a_position  : POSITION;\n";
	output << "vec3 a_normal    : NORMAL;\n";
	output << "vec2 a_texcoord0 : TEXCOORD0;\n";

	// Output to file
	LOG_TRACE("Writing varying.def.sc to file {0}", outPath.string().c_str());
	outFile << output.str();
}

void Compiler::generateMaterialHeader(asset::MaterialTypeAsset* type) {
	std::string typeName = type->getName();
	std::replace_if(std::begin(typeName), std::end(typeName),
		[](std::string::value_type v) { return v == '.'; },
		'_');

	std::filesystem::path outPath = ::aderite::Engine::getAssetManager()->getRawDir() / (typeName + ".sh");
	std::stringstream output;
	std::ofstream outFile(outPath);
	LOG_TRACE("Generating material header");

	// Recalculate offsets
	type->recalculate();

	// Header comment
	auto t = std::time(nullptr);
	auto tm = *std::localtime(&t);

	output << "/*\n";
	output << " *" << " DON'T CHANGE DIRECTLY" << "\n";
	output << " *" << " This is a shader header file generated by aderite for material " << typeName << "\n";
	output << " *" << " Generated at " << std::put_time(&tm, "%Y-%m-%d %H.%M.%S") << "\n";
	output << " */" << "\n\n";

	// Uniform buffer
	unsigned int arraySize = 0;

	if (type->getFields().ElementCount % 4 != 0) {
		arraySize = ((type->getFields().ElementCount - type->getFields().ElementCount % 4) / 4) + 1;
	}
	else {
		arraySize = type->getFields().ElementCount / 4;
	}

	output << "uniform vec4 u_mat_buffer_" << typeName << "[" << arraySize << "];\n";

	// Properties
	output << "\n" << "// Properties\n";
	// Should be packed already by recalculate call
	std::string samplers = "";
	unsigned int samplerIdx = 0;
	for (asset::prop::Property* prop : type->getFields().Properties) {
		// Need to generate in a packed manner therefor
		// Recalculate method prioritizes this structure:
		// vec4
		// vec3 float
		// vec2 vec2
		// vec2 float float
		// float float float float

		unsigned int arrayRemainderIdx = prop->getOffset() % 4; // This is the current element
		unsigned int arrayIdx = (prop->getOffset() - arrayRemainderIdx) / 4;
		uint8_t elementCount = asset::prop::getElementCountForType(prop->getType());
		std::string access = "";

		switch (elementCount) {
		case 4: {
			// vec4 will always be first so no need to check for packing
			access = "xyzw";
			break;
		}
		case 3: {
			// vec3 will always be first so xyz
			access = "xyz";
			break;
		}
		case 2: {
			// vec2 can be first or second
			if (arrayRemainderIdx == 0) {
				// first
				access = "xy";
			}
			else {
				// second
				access = "zw";
			}
			break;
		}
		case 1: {
			// Check
			switch (arrayRemainderIdx) {
			case 0: {
				access = "x";
				break;
			}
			case 1: {
				access = "y";
				break;
			}
			case 2: {
				access = "z";
				break;
			}
			case 3: {
				access = "w";
				break;
			}
			default: {
				LOG_ERROR("Incorrect format, remainder {0}", arrayRemainderIdx);
			}
			}
			break;
		}
		}

		switch (prop->getType()) {
		case asset::prop::PropertyType::FLOAT:
		case asset::prop::PropertyType::VEC2:
		case asset::prop::PropertyType::VEC3:
		case asset::prop::PropertyType::VEC4: {
			output << "#define "
				<< typeName << "_" << prop->getName()
				<< " u_mat_buffer_" << typeName
				<< "[" << arrayIdx << "]." << access << "\n";
			break;
		}
		case asset::prop::PropertyType::TEXTURE_2D: {
			samplers += "SAMPLER2D(" + typeName + "_" + prop->getName() + ", " + std::to_string(samplerIdx++) + ");\n";
			break;
		}
		case asset::prop::PropertyType::TEXTURE_CUBE: {
			samplers += "SAMPLERCUBE(" + typeName + "_" + prop->getName() + ", " + std::to_string(samplerIdx++) + ");\n";
			break;
		}
		default:
			LOG_WARN("Unknown property type when generating header");
		}
	}

	// Samplers
	output << "\n" << "// Samplers\n";
	output << samplers << "\n";

	// Output to file
	LOG_TRACE("Writing shader header to file {0}", outPath.string().c_str());
	outFile << output.str();
}

void Compiler::generateVertexShader(asset::MaterialTypeAsset* type) {
	std::string typeName = type->getName();
	std::replace_if(std::begin(typeName), std::end(typeName),
		[](std::string::value_type v) { return v == '.'; },
		'_');

	std::filesystem::path outPath = ::aderite::Engine::getAssetManager()->getRawDir() / ("vs_" + typeName + ".sc");
	std::stringstream output;
	std::ofstream outFile(outPath);
	LOG_TRACE("Generating vertex shader");

	// Recalculate offsets
	type->recalculate();

	// Header comment
	auto t = std::time(nullptr);
	auto tm = *std::localtime(&t);

	// Inputs, outputs
	output << "$input a_position, a_normal, a_texcoord0\n";
	output << "$output v_normal, v_texcoord\n\n";

	output << "/*\n";
	output << " *" << " DON'T CHANGE DIRECTLY" << "\n";
	output << " *" << " This is a vertex shader header file generated by aderite for material " << typeName << "\n";
	output << " *" << " Generated at " << std::put_time(&tm, "%Y-%m-%d %H.%M.%S") << "\n";
	output << " */" << "\n\n";

	// Includes
	output << "#include \"bgfx_shader.sh\"\n";
	output << "#include \"shaderLib.sh\"\n";
	output << "#include \""<< typeName << ".sh\"\n";
	output << "\n";

	// Main entry
	output << "void main()\n{\n\t";

	// gl_Position
	output << "gl_Position = mul(u_modelViewProj, vec4(a_position, 1.0));\n\t";

	// Texcoord and normals
	output << "v_texcoord = a_texcoord0;\n\t";
	output << "v_normal = a_normal;\n";

	// Close main
	output << "}\n";

	// Output to file
	LOG_TRACE("Writing vertex shader to file {0}", outPath.string().c_str());
	outFile << output.str();
}

void Compiler::generateFragmentShader(asset::MaterialTypeAsset* type) {
	std::string typeName = type->getName();
	std::replace_if(std::begin(typeName), std::end(typeName),
		[](std::string::value_type v) { return v == '.'; },
		'_');

	std::filesystem::path outPath = ::aderite::Engine::getAssetManager()->getRawDir() / ("fs_" + typeName + ".sc");
	std::stringstream output;
	std::ofstream outFile(outPath);
	LOG_TRACE("Generating fragment shader");

	// Recalculate offsets
	type->recalculate();

	// Header comment
	auto t = std::time(nullptr);
	auto tm = *std::localtime(&t);

	// Inputs, outputs
	output << "$input v_normal, v_texcoord\n\n";

	output << "/*\n";
	output << " *" << " DON'T CHANGE DIRECTLY" << "\n";
	output << " *" << " This is a fragment shader header file generated by aderite for material " << typeName << "\n";
	output << " *" << " Generated at " << std::put_time(&tm, "%Y-%m-%d %H.%M.%S") << "\n";
	output << " */" << "\n\n";

	// Includes
	output << "#include \"bgfx_shader.sh\"\n";
	output << "#include \"shaderLib.sh\"\n";
	output << "#include \"" << typeName << ".sh\"\n";
	output << "\n";

	// Main entry
	output << "void main()\n{\n\t";

	// Fragcolor
	// TODO: Shader graph code goes here
	output << "vec4 texcolor = toLinear(texture2D(s_ImageMaterialType_mtype_NewProperty_edee790d, v_texcoord));\n\t";
	output << "gl_FragColor = " << "texcolor + u_ImageMaterialType_mtype_NewProperty_259e7581;\n";

	// Close main
	output << "}\n";

	// Output to file
	LOG_TRACE("Writing fragment shader to file {0}", outPath.string().c_str());
	outFile << output.str();
}

ADERITE_EDITOR_COMPILER_NAMESPACE_END
