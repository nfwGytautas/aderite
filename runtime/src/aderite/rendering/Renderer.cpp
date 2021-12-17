#include "Renderer.hpp"
#include <unordered_map>

#include <bgfx/bgfx.h>
#include <bx/string.h>
#include <glm/gtc/type_ptr.hpp>

#include "aderite/Aderite.hpp"
#include "aderite/Config.hpp"
#include "aderite/asset/MaterialAsset.hpp"
#include "aderite/asset/MaterialTypeAsset.hpp"
#include "aderite/asset/MeshAsset.hpp"
#include "aderite/asset/TextureAsset.hpp"
#include "aderite/rendering/DrawCall.hpp"
#include "aderite/scene/Camera.hpp"
#include "aderite/scene/Entity.hpp"
#include "aderite/scene/ITransformProvider.h"
#include "aderite/scene/Scene.hpp"
#include "aderite/scene/SceneManager.hpp"
#include "aderite/scene/Scenery.hpp"
#include "aderite/scene/Visual.hpp"
#include "aderite/utility/Log.hpp"
#include "aderite/utility/LogExtensions.hpp"
#include "aderite/window/WindowManager.hpp"

namespace impl {

/**
 * @brief bgfx logging callback
 */
class bgfxCallback : public bgfx::CallbackI {
public:
    virtual ~bgfxCallback() {}
    virtual void fatal(const char* filePath, uint16_t line, bgfx::Fatal::Enum code, const char* str) override {
        if (code != bgfx::Fatal::DebugCheck) {
            LOG_FATAL("[Rendering] {0}", str);
        }
    }

    virtual void traceVargs(const char* filePath, uint16_t line, const char* format, va_list args) override {
        char buffer[1024];
        int32_t written = bx::vsnprintf(buffer, BX_COUNTOF(buffer), format, args);
        if (written > 0 && written < BX_COUNTOF(buffer)) {
            // bgfx sends lines with newlines, spdlog adds another
            if (buffer[written - 1] == '\n')
                buffer[written - 1] = '\0';
            LOG_TRACE(buffer);
        }
    }

    virtual void profilerBegin(const char*, uint32_t, const char*, uint16_t) override {}
    virtual void profilerBeginLiteral(const char*, uint32_t, const char*, uint16_t) override {}
    virtual void profilerEnd() override {}
    virtual uint32_t cacheReadSize(uint64_t) override {
        return 0;
    }
    virtual bool cacheRead(uint64_t, void*, uint32_t) override {
        return false;
    }
    virtual void cacheWrite(uint64_t, const void*, uint32_t) override {}
    virtual void captureBegin(uint32_t, uint32_t, uint32_t, bgfx::TextureFormat::Enum, bool) override {}
    virtual void captureEnd() override {}
    virtual void captureFrame(const void*, uint32_t) override {}
    virtual void screenShot(const char*, uint32_t, uint32_t, uint32_t, const void*, uint32_t, bool yflip) override {}
};

static bgfxCallback g_cb;
} // namespace impl

namespace aderite {
namespace rendering {

bgfx::TextureFormat::Enum findDepthFormat(uint64_t textureFlags, bool stencil = true) {
    const bgfx::TextureFormat::Enum depthFormats[] = {bgfx::TextureFormat::D16, bgfx::TextureFormat::D32};
    const bgfx::TextureFormat::Enum depthStencilFormats[] = {bgfx::TextureFormat::D24S8};

    const bgfx::TextureFormat::Enum* formats = stencil ? depthStencilFormats : depthFormats;
    size_t count = stencil ? BX_COUNTOF(depthStencilFormats) : BX_COUNTOF(depthFormats);

    bgfx::TextureFormat::Enum depthFormat = bgfx::TextureFormat::Count;
    for (size_t i = 0; i < count; i++) {
        if (bgfx::isTextureValid(0, false, 1, formats[i], textureFlags)) {
            depthFormat = formats[i];
            break;
        }
    }

    assert(depthFormat != bgfx::TextureFormat::Enum::Count);
    return depthFormat;
}

bgfx::FrameBufferHandle createFramebuffer(bool blittable = false, bool hdr = false, bool depth = true, bool stencil = true) {
    bgfx::TextureHandle textures[2];
    uint8_t attachments = 0;

    const uint64_t textureFlags = (blittable ? BGFX_TEXTURE_BLIT_DST : 0) | 0;

    const uint64_t samplerFlags = BGFX_SAMPLER_MIN_POINT | BGFX_SAMPLER_MAG_POINT | BGFX_SAMPLER_MIP_POINT | BGFX_SAMPLER_U_CLAMP |
                                  BGFX_SAMPLER_V_CLAMP | 0;

    // BGRA is often faster (internal GPU format)
    bgfx::TextureFormat::Enum format = hdr ? bgfx::TextureFormat::RGBA16F : bgfx::TextureFormat::BGRA8;
    assert(bgfx::isTextureValid(0, false, 1, format, BGFX_TEXTURE_RT | samplerFlags));
    textures[attachments++] =
        bgfx::createTexture2D(bgfx::BackbufferRatio::Equal, false, 1, format, BGFX_TEXTURE_RT | samplerFlags | textureFlags);

    if (depth) {
        bgfx::TextureFormat::Enum depthFormat = findDepthFormat(BGFX_TEXTURE_RT_WRITE_ONLY | samplerFlags, stencil);
        assert(depthFormat != bgfx::TextureFormat::Enum::Count);
        textures[attachments++] = bgfx::createTexture2D(bgfx::BackbufferRatio::Equal, false, 1, depthFormat,
                                                        BGFX_TEXTURE_RT_WRITE_ONLY | samplerFlags | textureFlags);
    }

    bgfx::FrameBufferHandle handle = bgfx::createFrameBuffer(attachments, textures, true);
    return handle;
}

inline glm::mat4 calculateTransformationMatrix(scene::ITransformProvider* transform) {
    glm::vec3 position = transform->getPosition();
    glm::quat rotation = transform->getRotation();
    glm::vec3 scale = transform->getScale();

    glm::mat4 rMat = glm::toMat4(rotation);
    return glm::translate(glm::mat4(1.0f), position) * rMat * glm::scale(glm::mat4(1.0f), scale);
}

bool Renderer::init() {
    ADERITE_LOG_BLOCK;
    LOG_DEBUG("[Rendering] Initializing BGFX Renderer");

    auto windowManager = ::aderite::Engine::getWindowManager();
    glm::i32vec2 size = windowManager->getSize();

    // Platform data
    bgfx::PlatformData pd;
    pd.nwh = windowManager->getNativeHandle();

    bgfx::Init bgfxInit;
    bgfxInit.platformData = pd;
    bgfxInit.type = bgfx::RendererType::Count; // Automatically choose a backend
    bgfxInit.resolution.width = size.x;
    bgfxInit.resolution.height = size.y;
    bgfxInit.resolution.reset = BGFX_RESET_VSYNC;
    bgfxInit.callback = &::impl::g_cb;

    // TODO: Add multi threaded

    if (!bgfx::init(bgfxInit)) {
        LOG_ERROR("[Rendering] Failed to initialize BGFX");
    }

    // Initial view rect
    auto windowSize = ::aderite::Engine::getWindowManager()->getSize();
    this->onWindowResized(windowSize.x, windowSize.y, false);

    if (!this->createTargets()) {
        LOG_ERROR("[Rendering] Failed to create targets");
        return false;
    }

    // Finish any queued operations
    bgfx::frame();

    m_isInitialized = true;
    ::aderite::Engine::get()->onRendererInitialized();

    LOG_INFO("[Rendering] BGFX renderer initialized");

    return true;
}

void Renderer::shutdown() {
    ADERITE_LOG_BLOCK;
    LOG_TRACE("[Rendering] Shutting down");

    bgfx::destroy(m_mainFbo);

    bgfx::shutdown();

    LOG_INFO("[Rendering] Renderer shutdown");
}

void Renderer::setVsync(bool enabled) {
    ADERITE_UNIMPLEMENTED;
}

void Renderer::onWindowResized(unsigned int newWidth, unsigned int newHeight, bool reset) {
    LOG_TRACE("[Rendering] Resizing window (width: {0}, height: {1})", newWidth, newHeight);
    this->setResolution(glm::uvec2(newWidth, newHeight));

    if (reset) {
        bgfx::reset(newWidth, newHeight);
    }
}

void Renderer::render() {
    if (!this->isReady()) {
        return;
    }

    // Get scene
    scene::Scene* currentScene = ::aderite::Engine::getSceneManager()->getCurrentScene();
    if (currentScene == nullptr) {
        return;
    }

    // Valid scene do rendering

    // 1. Aggregate all objects into a drawlist, this is used to create instanced rendering, by adding only unique renderable
    // configurations
    static std::unordered_map<size_t, DrawCall> drawCalls;
    drawCalls.clear();

    for (scene::Visual* visual : currentScene->getVisuals()) {
        if (visual->isValid()) {
            DrawCall& dc = drawCalls[visual->hash()];
            dc.Renderable = visual;
            dc.Transformations.push_back(calculateTransformationMatrix(visual));
        }
    }

    for (scene::Scenery* scenery : currentScene->getScenery()) {
        if (scenery->isValid()) {
            DrawCall& dc = drawCalls[scenery->hash()];
            dc.Renderable = scenery;
            dc.Transformations.push_back(calculateTransformationMatrix(scenery));
        }
    }

    for (scene::Entity* entity : currentScene->getEntities()) {
        if (entity->isValid()) {
            DrawCall& dc = drawCalls[entity->hash()];
            dc.Renderable = entity;
            dc.Transformations.push_back(calculateTransformationMatrix(entity));
        }
    }

    bgfx::discard(BGFX_DISCARD_ALL);

    uint8_t viewIdx = 0;
    auto perCamera = [&](scene::Camera* camera) {
        // 2. Setup view
        this->setupView(viewIdx, camera->getName());
        bgfx::setViewFrameBuffer(viewIdx, m_mainFbo);
        bgfx::touch(viewIdx);
        bgfx::touch(viewIdx + 1);

        // 3. Setup persistent matrices
        bgfx::setViewTransform(viewIdx, glm::value_ptr(camera->getViewMatrix()), glm::value_ptr(camera->getProjectionMatrix()));

        // Discard previous state
        bgfx::discard(BGFX_DISCARD_ALL);

        // 4. Submit draw calls
        for (auto& kvp : drawCalls) {

            // Extract assets
            const Renderable* renderable = kvp.second.Renderable;
            const asset::MaterialAsset* material = renderable->getMaterial();
            const asset::MeshAsset* mesh = renderable->getMesh();
            const asset::MaterialTypeAsset* mType = material->getMaterialType();

            // Uniform
            bgfx::setUniform(mType->getUniformHandle(), material->getPropertyData(), UINT16_MAX);

            // Samplers
            for (size_t i = 0; i < material->getSamplerCount(); i++) {
                bgfx::setTexture(i, mType->getSampler(i), material->getSampler(i)->getTextureHandle());
            }

            // Bind buffers
            bgfx::setVertexBuffer(0, mesh->getVboHandle());
            bgfx::setIndexBuffer(mesh->getIboHandle());

            // Set render state
            uint64_t state = 0 | BGFX_STATE_WRITE_R | BGFX_STATE_WRITE_G | BGFX_STATE_WRITE_B | BGFX_STATE_WRITE_A | BGFX_STATE_WRITE_Z |
                             BGFX_STATE_DEPTH_TEST_LESS | BGFX_STATE_CULL_CCW | BGFX_STATE_MSAA;

            bgfx::setState(state);

            // TODO: Instancing
            // Submit rendering
            for (auto& transform : kvp.second.Transformations) {
                bgfx::setTransform(glm::value_ptr(transform));

                // Submit draw call
                uint8_t flags = BGFX_DISCARD_ALL &
                                ~(BGFX_DISCARD_BINDINGS | BGFX_DISCARD_INDEX_BUFFER | BGFX_DISCARD_VERTEX_STREAMS | BGFX_DISCARD_STATE);
                bgfx::submit(viewIdx, mType->getShaderHandle(), 0, flags);
            }
        }

        // 5. Copy result
        bgfx::blit(viewIdx + 1, camera->getOutputHandle(), 0, 0, bgfx::getTexture(m_mainFbo));

        viewIdx += 2;
    };

    for (scene::Camera* camera : currentScene->getCameras()) {
        perCamera(camera);
    }

    if (m_editorCamera != nullptr) {
        perCamera(m_editorCamera);
    }

    bgfx::discard(BGFX_DISCARD_ALL);
}

bool Renderer::isReady() const {
    return m_isInitialized;
}

void Renderer::setResolution(const glm::uvec2& size) {
    m_resolution = size;
}

void Renderer::commit() const {
    // Commit
    bgfx::frame(false);
    // TODO: Display stats in editor
    // const bgfx::Stats* stats = bgfx::getStats();
    // LOG_INFO("Commiting {0} draw calls", stats->numDraw);
}

void Renderer::setEditorCamera(scene::Camera* camera) {
    m_editorCamera = camera;
}

bool Renderer::createTargets() {
    // Create
    m_mainFbo = createFramebuffer();
    if (!bgfx::isValid(m_mainFbo)) {
        LOG_WARN("[Rendering] Failed to create main framebuffer");
        return false;
    }

    // Set name
    bgfx::setName(m_mainFbo, "Main pass framebuffer");

    return true;
}

void Renderer::prepareTargets() {
    // bgfx::
}

void Renderer::setupView(uint8_t idx, const std::string& name) {
    ADERITE_DYNAMIC_ASSERT((((uint16_t)idx) + 1) <= 255, "To many cameras view index >255");

    //static const uint32_t ColorClearColor = 0x252525FF;
    static const uint32_t ColorClearColor = 0x9ACBFFFF;
    static const float DepthClearValue = 1.0f;
    static const uint8_t StencilClearValue = 0;

    // Object render
    bgfx::setViewClear(idx, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, ColorClearColor, DepthClearValue, StencilClearValue);
    bgfx::setViewRect(idx, 0, 0, m_resolution.x, m_resolution.y);
    bgfx::setViewName(idx, (name + " - Object rendering").c_str());
    bgfx::setViewName(idx + 1, (name + " - Output copy").c_str());
}

} // namespace rendering
} // namespace aderite
