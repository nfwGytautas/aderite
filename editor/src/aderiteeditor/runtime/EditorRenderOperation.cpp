#include "EditorRenderOperation.hpp"
#include <filesystem>
#include <fstream>

#include <bgfx/bgfx.h>
#include <glm/gtc/type_ptr.hpp>

#include "aderite/Aderite.hpp"
#include "aderite/asset/ColliderListAsset.hpp"
#include "aderite/physics/Collider.hpp"
#include "aderite/physics/collider/BoxCollider.hpp"
#include "aderite/rendering/operation/CameraProvideOperation.hpp"
#include "aderite/rendering/operation/TargetProvideOperation.hpp"
#include "aderite/scene/Scene.hpp"
#include "aderite/scene/SceneManager.hpp"
#include "aderite/scene/components/Transform.hpp"
#include "aderite/utility/Log.hpp"

#include "aderiteeditor/compiler/ShaderCompiler.hpp"
#include "aderiteeditor/runtime/EditorTypes.hpp"
#include "aderiteeditor/runtime/data/Data.hpp"
#include "aderiteeditor/shared/EditorCamera.hpp"
#include "aderiteeditor/shared/Project.hpp"
#include "aderiteeditor/shared/State.hpp"

namespace aderite {
namespace editor {

EditorRenderOperation::EditorRenderOperation() {
    this->setName("EditorRenderHook");
}

EditorRenderOperation::~EditorRenderOperation() {}

void EditorRenderOperation::initialize() {
    ADERITE_DEBUG_SECTION(bgfx::setViewName(c_ViewId, this->getName().c_str());)

    // Load assets
    loadMeshes();
    loadShaders();

    // Create data
    m_editorUniform = bgfx::createUniform("u_editor", bgfx::UniformType::Vec4, 2);
    wfThickness = 4.0f;
    this->updateUniform();

    // Since the editor should be hooked into the entity rendering, only clear the depth
    bgfx::setViewClear(c_ViewId, BGFX_CLEAR_DEPTH, 0x252525FF, 1.0f, 0);
    bgfx::setViewRect(c_ViewId, 0, 0, 1280, 720);
}

void EditorRenderOperation::execute(rendering::PipelineState* state) {
    bgfx::discard(BGFX_DISCARD_ALL);
    this->renderPhysicsObjects();
    bgfx::discard(BGFX_DISCARD_ALL);
}

void EditorRenderOperation::shutdown() {
    bgfx::destroy(m_editorUniform);
    bgfx::destroy(m_wireframeShader);
    bgfx::destroy(m_bcCube);
    bgfx::destroy(m_bcCubeIbo);
}

reflection::Type EditorRenderOperation::getType() const {
    return static_cast<reflection::Type>(reflection::EditorTypes::EditorRenderOp);
}

bool EditorRenderOperation::serialize(const io::Serializer* serializer, YAML::Emitter& emitter) const {
    return true;
}

bool EditorRenderOperation::deserialize(io::Serializer* serializer, const YAML::Node& data) {
    return true;
}

void EditorRenderOperation::updateUniform() {
    bgfx::setUniform(m_editorUniform, &EditorParameters[0], 2);
}

void EditorRenderOperation::renderPhysicsObjects() {
    // Bind state
    bgfx::setViewFrameBuffer(c_ViewId, editor::State::DebugRenderHandle);
    bgfx::touch(c_ViewId);

    // Set persistent matrices
    bgfx::setViewTransform(c_ViewId, glm::value_ptr(editor::State::EditorCamera->getViewMatrix()),
                           glm::value_ptr(editor::State::EditorCamera->getProjectionMatrix()));

    scene::Scene* currentScene = ::aderite::Engine::getSceneManager()->getCurrentScene();

    if (currentScene == nullptr) {
        return;
    }

    // Colliders and triggers
    auto colliderGroup = ::aderite::Engine::getSceneManager()->getCurrentScene()->getEntityRegistry().group<scene::CollidersComponent>(
        entt::get<scene::TransformComponent>);
    for (auto entity : colliderGroup) {
        auto [colliders, transform] = colliderGroup.get<scene::CollidersComponent, scene::TransformComponent>(entity);

        if (colliders.Colliders == nullptr) {
            continue;
        }

        for (physics::Collider* collider : *colliders.Colliders) {
            switch (static_cast<reflection::RuntimeTypes>(collider->getType())) {
            case reflection::RuntimeTypes::BOX_CLDR: {
                this->renderBoxCollider(static_cast<physics::BoxCollider*>(collider), transform);
                break;
            }
            }
        }
    }
}

void EditorRenderOperation::renderBoxCollider(physics::BoxCollider* collider, const scene::TransformComponent& transform) {
    glm::vec3 extents = collider->getSize();

    scene::TransformComponent tempTransform = transform;
    tempTransform.Scale *= extents;

    if (collider->isTrigger()) {
        wfColor[0] = 0.0f;
        wfColor[1] = 1.0f;
        wfColor[2] = 0.0f;
        wfOpacity = 1.0f;
    } else {
        wfColor[0] = 1.0f;
        wfColor[1] = 0.0f;
        wfColor[2] = 0.0f;
        wfOpacity = 1.0f;
    }
    this->updateUniform();

    uint64_t state = 0 | BGFX_STATE_WRITE_RGB | BGFX_STATE_WRITE_A | BGFX_STATE_DEPTH_TEST_LESS | BGFX_STATE_WRITE_Z | BGFX_STATE_CULL_CCW |
                     BGFX_STATE_MSAA | BGFX_STATE_BLEND_FUNC(BGFX_STATE_BLEND_SRC_ALPHA, BGFX_STATE_BLEND_INV_SRC_ALPHA);

    bgfx::setTransform(glm::value_ptr(scene::TransformComponent::computeTransform(tempTransform)));
    bgfx::setState(state);

    bgfx::setVertexBuffer(0, m_bcCube);
    bgfx::setIndexBuffer(m_bcCubeIbo);
    bgfx::submit(c_ViewId, m_wireframeShader, 0);
}

void EditorRenderOperation::loadMeshes() {
    bgfx::VertexLayout cubeLayout;
    cubeLayout.begin().add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float).add(bgfx::Attrib::Color1, 3, bgfx::AttribType::Float).end();

    m_bcCube = bgfx::createVertexBuffer(bgfx::makeRef(bcCubeVertices, sizeof(bcCubeVertices)), cubeLayout);
    m_bcCubeIbo = bgfx::createIndexBuffer(bgfx::makeRef(bcCubeIndices, sizeof(bcCubeIndices)));
}

void EditorRenderOperation::loadShaders() {
    LOG_TRACE("Loading editor shaders");

    const std::filesystem::path projectRoot = editor::State::Project->getRootDir();
    const std::filesystem::path dataRoot = projectRoot / "Data";

    // Create compilers
    compiler::ShaderCompiler sc(dataRoot / "wireframe.vs", dataRoot / "wireframe.fs", dataRoot / "wireframe_varying.def.sc");

    // Compile
    sc.compile();

    // Load
    loadShader(dataRoot / "wireframe.vs", m_wireframeShader, "Wireframe");
}

void EditorRenderOperation::loadShader(const std::filesystem::path& base, bgfx::ProgramHandle& shader, const std::string& name) {
    LOG_TRACE("Loading {0}", name);

    const std::filesystem::path vPath = base.parent_path() / base.filename().replace_extension(".vs.bin");
    const std::filesystem::path fPath = base.parent_path() / base.filename().replace_extension(".fs.bin");

    std::ifstream vin(vPath, std::ios::binary);
    std::ifstream fin(fPath, std::ios::binary);

    size_t vSize = vin.seekg(0, std::ios::end).tellg();
    size_t fSize = fin.seekg(0, std::ios::end).tellg();

    std::vector<unsigned char> vdata;
    std::vector<unsigned char> fdata;

    vdata.resize(vSize);
    fdata.resize(fSize);

    vin.seekg(0, std::ios::beg);
    fin.seekg(0, std::ios::beg);

    vin.read(reinterpret_cast<char*>(vdata.data()), vdata.size());
    vdata.push_back('\0');

    fin.read(reinterpret_cast<char*>(fdata.data()), fdata.size());
    fdata.push_back('\0');

    // Create memory objects
    const bgfx::Memory* memWfvs = bgfx::copy(vdata.data(), vdata.size() + 1);
    memWfvs->data[memWfvs->size - 1] = '\0';
    const bgfx::Memory* memWffs = bgfx::copy(fdata.data(), fdata.size() + 1);
    memWffs->data[memWffs->size - 1] = '\0';

    // Create objects
    bgfx::ShaderHandle vsh = bgfx::createShader(memWfvs);
    bgfx::ShaderHandle fsh = bgfx::createShader(memWffs);

    bgfx::setName(vsh, (name + " vertex shader").c_str());
    bgfx::setName(fsh, (name + " fragment shader").c_str());

    shader = bgfx::createProgram(vsh, fsh, true);
}

} // namespace editor
} // namespace aderite
