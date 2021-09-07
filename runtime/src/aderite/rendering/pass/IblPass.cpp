#include "IblPass.hpp"

#include <bx/bx.h>
#include <glm/gtc/type_ptr.hpp>

#include "aderite/Aderite.hpp"
#include "aderite/scene/SceneManager.hpp"


ADERITE_PASS_NAMESPACE_BEGIN

static float s_texelHalf = 0.0f;

struct PosColorTexCoord0Vertex
{
	float m_x;
	float m_y;
	float m_z;
	uint32_t m_rgba;
	float m_u;
	float m_v;

	static void init()
	{
		ms_layout
			.begin()
			.add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
			.add(bgfx::Attrib::Color0, 4, bgfx::AttribType::Uint8, true)
			.add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float)
			.end();
	}

	static bgfx::VertexLayout ms_layout;
};

bgfx::VertexLayout PosColorTexCoord0Vertex::ms_layout;

void screenSpaceQuad(float _textureWidth, float _textureHeight, bool _originBottomLeft = false, float _width = 1.0f, float _height = 1.0f)
{
	if (3 == bgfx::getAvailTransientVertexBuffer(3, PosColorTexCoord0Vertex::ms_layout))
	{
		bgfx::TransientVertexBuffer vb;
		bgfx::allocTransientVertexBuffer(&vb, 3, PosColorTexCoord0Vertex::ms_layout);
		PosColorTexCoord0Vertex* vertex = (PosColorTexCoord0Vertex*)vb.data;

		const float zz = 0.0f;

		const float minx = -_width;
		const float maxx = _width;
		const float miny = 0.0f;
		const float maxy = _height * 2.0f;

		const float texelHalfW = s_texelHalf / _textureWidth;
		const float texelHalfH = s_texelHalf / _textureHeight;
		const float minu = -1.0f + texelHalfW;
		const float maxu = 1.0f + texelHalfW;

		float minv = texelHalfH;
		float maxv = 2.0f + texelHalfH;

		if (_originBottomLeft)
		{
			std::swap(minv, maxv);
			minv -= 1.0f;
			maxv -= 1.0f;
		}

		vertex[0].m_x = minx;
		vertex[0].m_y = miny;
		vertex[0].m_z = zz;
		vertex[0].m_rgba = 0xffffffff;
		vertex[0].m_u = minu;
		vertex[0].m_v = minv;

		vertex[1].m_x = maxx;
		vertex[1].m_y = miny;
		vertex[1].m_z = zz;
		vertex[1].m_rgba = 0xffffffff;
		vertex[1].m_u = maxu;
		vertex[1].m_v = minv;

		vertex[2].m_x = maxx;
		vertex[2].m_y = maxy;
		vertex[2].m_z = zz;
		vertex[2].m_rgba = 0xffffffff;
		vertex[2].m_u = maxu;
		vertex[2].m_v = maxv;

		bgfx::setVertexBuffer(0, &vb);
	}
}

struct Settings
{
	Settings()
	{
		m_envRotCurr = 0.0f;
		m_envRotDest = 0.0f;
		m_lightDir[0] = -0.8f;
		m_lightDir[1] = 0.2f;
		m_lightDir[2] = -0.5f;
		m_lightCol[0] = 1.0f;
		m_lightCol[1] = 1.0f;
		m_lightCol[2] = 1.0f;
		m_glossiness = 0.7f;
		m_exposure = 0.0f;
		m_bgType = 3.0f;
		m_radianceSlider = 2.0f;
		m_reflectivity = 0.85f;
		m_rgbDiff[0] = 1.0f;
		m_rgbDiff[1] = 1.0f;
		m_rgbDiff[2] = 1.0f;
		m_rgbSpec[0] = 1.0f;
		m_rgbSpec[1] = 1.0f;
		m_rgbSpec[2] = 1.0f;
		m_lod = 0.0f;
		m_doDiffuse = false;
		m_doSpecular = false;
		m_doDiffuseIbl = true;
		m_doSpecularIbl = true;
		m_showLightColorWheel = true;
		m_showDiffColorWheel = true;
		m_showSpecColorWheel = true;
		m_metalOrSpec = 0;
		m_meshSelection = 0;
	}

	float m_envRotCurr;
	float m_envRotDest;
	float m_lightDir[3];
	float m_lightCol[3];
	float m_glossiness;
	float m_exposure;
	float m_radianceSlider;
	float m_bgType;
	float m_reflectivity;
	float m_rgbDiff[3];
	float m_rgbSpec[3];
	float m_lod;
	bool  m_doDiffuse;
	bool  m_doSpecular;
	bool  m_doDiffuseIbl;
	bool  m_doSpecularIbl;
	bool  m_showLightColorWheel;
	bool  m_showDiffColorWheel;
	bool  m_showSpecColorWheel;
	int32_t m_metalOrSpec;
	int32_t m_meshSelection;
};

struct LightProbe
{
	enum Enum
	{
		Bolonga,
		Kyoto,

		Count
	};

	void load(const char* _name)
	{
		//char filePath[512];
		//
		//bx::snprintf(filePath, BX_COUNTOF(filePath), "textures/%s_lod.dds", _name);
		//m_tex = loadTexture(filePath, BGFX_SAMPLER_U_CLAMP | BGFX_SAMPLER_V_CLAMP | BGFX_SAMPLER_W_CLAMP);
		//
		//bx::snprintf(filePath, BX_COUNTOF(filePath), "textures/%s_irr.dds", _name);
		//m_texIrr = loadTexture(filePath, BGFX_SAMPLER_U_CLAMP | BGFX_SAMPLER_V_CLAMP | BGFX_SAMPLER_W_CLAMP);
	}

	void destroy()
	{
		bgfx::destroy(m_tex);
		bgfx::destroy(m_texIrr);
	}

	bgfx::TextureHandle m_tex;
	bgfx::TextureHandle m_texIrr;
};

uint32_t m_width;
uint32_t m_height;
uint32_t m_debug;
uint32_t m_reset;

LightProbe m_lightProbes[LightProbe::Count];
LightProbe::Enum m_currentLightProbe;

bgfx::UniformHandle u_mtx;
bgfx::UniformHandle u_params;
bgfx::UniformHandle u_flags;
bgfx::UniformHandle u_camPos;
bgfx::UniformHandle s_texCube;
bgfx::UniformHandle s_texCubeIrr;

bgfx::ProgramHandle m_programMesh;
bgfx::ProgramHandle m_programSky;

Settings m_settings;

bool IblPass::init(rendering::uniform::UniformManager* um) {
	//m_paramsUniform = bgfx::createUniform("u_params", bgfx::UniformType::Vec4, 12);

	PosColorTexCoord0Vertex::init();

	//m_lightProbes[LightProbe::Bolonga].load("bolonga");
	//m_lightProbes[LightProbe::Kyoto].load("kyoto");
	//m_currentLightProbe = LightProbe::Bolonga;

	//u_mtx = bgfx::createUniform("u_mtx", bgfx::UniformType::Mat4);
	//u_params = bgfx::createUniform("u_params", bgfx::UniformType::Vec4);
	//u_flags = bgfx::createUniform("u_flags", bgfx::UniformType::Vec4);
	//u_camPos = bgfx::createUniform("u_camPos", bgfx::UniformType::Vec4);
	//s_texCube = bgfx::createUniform("s_texCube", bgfx::UniformType::Sampler);
	//s_texCubeIrr = bgfx::createUniform("s_texCubeIrr", bgfx::UniformType::Sampler);

	//m_programMesh = loadProgram("vs_ibl_mesh", "fs_ibl_mesh");
	//m_programSky = loadProgram("vs_ibl_skybox", "fs_ibl_skybox");
	//
	//m_meshOrb = meshLoad("meshes/orb.bin");

	return true;
}

void IblPass::shutdown() {
	//bgfx::destroy(m_paramsUniform);
}

void IblPass::pass(interfaces::ICamera* eye) {
	// Set persistent matrices
	bgfx::setViewTransform(m_iblView, glm::value_ptr(eye->computeViewMatrix()), glm::value_ptr(eye->computeProjectionMatrix()));

	// Bind state
	bgfx::setViewFrameBuffer(m_iblView, eye->getOutputHandle());


}

void IblPass::onWindowResized(unsigned int newWidth, unsigned int newHeight) {
	bgfx::setViewRect(m_iblView, 0, 0, newWidth, newHeight);
}

ADERITE_PASS_NAMESPACE_END

//// Construct draw calls
//auto group = scene->getEntityRegistry().group<scene::components::TransformComponent>(entt::get<scene::components::MeshRendererComponent>);
//for (auto entity : group) {
//	// TODO: Layers
//	auto [TransformComponent, mr] = group.get<scene::components::TransformComponent, scene::components::MeshRendererComponent>(entity);
//
//	if (mr.MeshHandle == nullptr || mr.MaterialHandle == nullptr) {
//		continue;
//	}
//
//	// Check if draw call already exists
//	size_t hash = utility::combineHash(mr.MeshHandle->hash(), mr.MaterialHandle->hash());
//	DrawCall& dc = drawCalls[hash];
//
//	if (!bgfx::isValid(dc.VBO) || !bgfx::isValid(dc.Shader)) {
//		mr.MeshHandle->fillDrawCall(&dc);
//		mr.MaterialHandle->fillDrawCall(&dc);
//	}
//
//	glm::mat4 tmat = scene::components::TransformComponent::computeTransform(TransformComponent);
//	dc.Transformations.push_back(tmat);
//}

//void Renderer::executeDrawCall(const DrawCall& dc) {
//	// Check if valid draw call
//	if (!dc.Valid) {
//		return;
//	}
//
//	// Bind buffers
//	bgfx::setVertexBuffer(0, dc.VBO);
//
//	if (bgfx::isValid(dc.IBO)) {
//		bgfx::setIndexBuffer(dc.IBO);
//	}
//
//	if (dc.Instanced) {
//		LOG_WARN("Instanced rendering not implemented yet");
//	}
//	else {
//		for (auto& transform : dc.Transformations) {
//			bgfx::setTransform(glm::value_ptr(transform));
//
//			// Submit draw call
//			uint8_t flags = BGFX_DISCARD_ALL & ~(BGFX_DISCARD_BINDINGS | BGFX_DISCARD_INDEX_BUFFER | BGFX_DISCARD_VERTEX_STREAMS);
//			bgfx::submit(0, dc.Shader, 0, flags);
//		}
//	}
//}
