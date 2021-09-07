#include "UniformManager.hpp"

#include "aderite/utility/Log.hpp"

// Uniform implementation
#include "aderite/rendering/uniform/SkyboxUniform.hpp"

#if DEBUG_RENDER == 1
#include "aderite/rendering/uniform/DebugUniform.hpp"
#endif

ADERITE_UNIFORM_NAMESPACE_BEGIN

bool UniformManager::init() {
	// Create instances
#if DEBUG_RENDER == 1
	m_debugUniform = new DebugUniform();
	m_debugUniform->create();
#endif

	m_uniforms[0] = new SkyboxUniform();

	// Verify for name conflicts
	uint8_t conflicts = 0;
	for (uint8_t i = 0; i < c_NumUniforms; i++) {
		for (uint8_t j = i + 1; j < c_NumUniforms; j++) {
			if (std::strcmp(m_uniforms[i]->getName(), m_uniforms[j]->getName())) {
				LOG_ERROR("Uniform name conflict {0}", m_uniforms[i]->getName());
				conflicts += 1;
			}
		}
	}

	if (conflicts > 0) {
		LOG_ERROR("Failed to init UniformManager due to uniform name conflicts");
		return false;
	}
	
	// Create uniforms
	for (uint8_t i = 0; i < c_NumUniforms; i++) {
		m_uniforms[i]->create();
	}

	return true;
}

void UniformManager::shutdown() {
	for (uint8_t i = 0; i < c_NumUniforms; i++) {
		m_uniforms[i]->destroy();
		delete m_uniforms[i];
	}
}

template<typename T>
T* UniformManager::getUniform() const {
	LOG_FATAL("Requested unknown uniform type");
	return nullptr;
}

#if DEBUG_RENDER == 1
template<>
DebugUniform* UniformManager::getUniform() const {
	return static_cast<DebugUniform*>(m_debugUniform);
}
#endif

template<>
SkyboxUniform* UniformManager::getUniform() const {
	return static_cast<SkyboxUniform*>(m_uniforms[0]);
}

ADERITE_UNIFORM_NAMESPACE_END
