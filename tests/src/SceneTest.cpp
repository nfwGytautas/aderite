#include <aderite/Aderite.hpp>
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#define private public
#define protected public

#include <aderite/scene/CameraSettings.hpp>
#include <aderite/scene/GameObject.hpp>
#include <aderite/scene/Scene.hpp>
#include <aderite/scene/SceneManager.hpp>
#include <aderite/scene/TransformProvider.hpp>

#define private private
#define protected protected

class SceneTest : public ::testing::Test {
public:
    static void SetUpTestSuite() {
        aderite::Engine::get()->init({});
    }

    static void TearDownTestSuite() {
        aderite::Engine::get()->shutdown();
    }
};

/**
 * @brief Verifies scene manager init method
 */
TEST_F(SceneTest, SceneManager_init) {
    EXPECT_NE(aderite::Engine::getSceneManager(), nullptr);
}

/**
 * @brief Verifies scene manager active scene getter setter methods
 */
TEST_F(SceneTest, SceneManager_getSetActive) {
    aderite::scene::Scene* scene = new aderite::scene::Scene();
    aderite::Engine::getSceneManager()->setActive(scene);
    EXPECT_EQ(scene, aderite::Engine::getSceneManager()->getCurrentScene());
    EXPECT_EQ(scene->m_refCount, 1);
}

/**
 * @brief Verifies scene create game object method
 */
TEST_F(SceneTest, Scene_createGameObject) {
    aderite::scene::Scene* scene = new aderite::scene::Scene();
    aderite::scene::GameObject* go = scene->createGameObject();
    EXPECT_EQ(scene->m_gameObjects.size(), 1);
}

/**
 * @brief Verifies scene destroy game object method
 */
TEST_F(SceneTest, Scene_destroyGameObject) {
    aderite::scene::Scene* scene = new aderite::scene::Scene();

    aderite::scene::GameObject* go = scene->createGameObject();
    EXPECT_EQ(scene->m_gameObjects.size(), 1);

    scene->destroyGameObject(go);
    EXPECT_EQ(scene->m_gameObjects.size(), 0);
}

/**
 * @brief Verifies game object deletion mark functionality
 */
TEST_F(SceneTest, GameObject_deletionMark) {
    aderite::scene::GameObject* go = new aderite::scene::GameObject(nullptr, "asd");
    EXPECT_FALSE(go->isMarkedForDeletion());

    go->markForDeletion();
    EXPECT_TRUE(go->isMarkedForDeletion());
}

/**
 * @brief Verifies scene update method with marked for deletion game objects
 */
TEST_F(SceneTest, Scene_updateMarked) {
    aderite::scene::Scene* scene = new aderite::scene::Scene();

    aderite::scene::GameObject* go = scene->createGameObject();
    EXPECT_EQ(scene->m_gameObjects.size(), 1);
    go->markForDeletion();

    scene->update(0.0f);
    EXPECT_EQ(scene->m_gameObjects.size(), 0);
}

/**
 * @brief Verifies game object add method for transform component
 */
TEST_F(SceneTest, GameObject_addTransform) {
    aderite::scene::GameObject* go = new aderite::scene::GameObject(nullptr, "asd");
    auto component = go->addTransform();
    EXPECT_EQ(component, go->getTransform());
}

/**
 * @brief Verifies game object remove method for transform component
 */
TEST_F(SceneTest, GameObject_removeTransform) {
    aderite::scene::GameObject* go = new aderite::scene::GameObject(nullptr, "asd");
    go->addTransform();
    go->removeTransform();
    EXPECT_EQ(go->getTransform(), nullptr);
}

/**
 * @brief Verifies game object add method for renderable component
 */
TEST_F(SceneTest, GameObject_addRenderable) {
    aderite::scene::GameObject* go = new aderite::scene::GameObject(nullptr, "asd");
    auto component = go->addRenderable();
    EXPECT_EQ(component, go->getRenderable());
}

/**
 * @brief Verifies game object remove method for renderable component
 */
TEST_F(SceneTest, GameObject_removeRenderable) {
    aderite::scene::GameObject* go = new aderite::scene::GameObject(nullptr, "asd");
    go->addRenderable();
    go->removeRenderable();
    EXPECT_EQ(go->getRenderable(), nullptr);
}

/**
 * @brief Verifies game object add method for actor component
 */
TEST_F(SceneTest, GameObject_addActor) {
    aderite::scene::GameObject* go = new aderite::scene::GameObject(nullptr, "asd");
    auto component = go->addActor();
    EXPECT_EQ(component, go->getActor());
}

/**
 * @brief Verifies game object remove method for actor component
 */
TEST_F(SceneTest, GameObject_removeActor) {
    aderite::scene::GameObject* go = new aderite::scene::GameObject(nullptr, "asd");
    go->addActor();
    go->removeActor();
    EXPECT_EQ(go->getActor(), nullptr);
}

/**
 * @brief Verifies game object add method for camera component
 */
TEST_F(SceneTest, GameObject_addCamera) {
    aderite::scene::GameObject* go = new aderite::scene::GameObject(nullptr, "asd");
    auto component = go->addCamera();
    EXPECT_EQ(component, go->getCamera());
}

/**
 * @brief Verifies game object remove method for camera component
 */
TEST_F(SceneTest, GameObject_removeCamera) {
    aderite::scene::GameObject* go = new aderite::scene::GameObject(nullptr, "asd");
    go->addCamera();
    go->removeCamera();
    EXPECT_EQ(go->getCamera(), nullptr);
}

/**
 * @brief Verifies game object add method for audio source component
 */
TEST_F(SceneTest, GameObject_addAudioSource) {
    aderite::scene::GameObject* go = new aderite::scene::GameObject(nullptr, "asd");
    auto component = go->addAudioSource();
    EXPECT_EQ(component, go->getAudioSource());
}

/**
 * @brief Verifies game object remove method for audio source component
 */
TEST_F(SceneTest, GameObject_removeAudioSource) {
    aderite::scene::GameObject* go = new aderite::scene::GameObject(nullptr, "asd");
    go->addAudioSource();
    go->removeAudioSource();
    EXPECT_EQ(go->getAudioSource(), nullptr);
}

/**
 * @brief Verifies game object add method for audio listener component
 */
TEST_F(SceneTest, GameObject_addAudioListener) {
    aderite::scene::GameObject* go = new aderite::scene::GameObject(nullptr, "asd");
    auto component = go->addAudioListener();
    EXPECT_EQ(component, go->getAudioListener());
}

/**
 * @brief Verifies game object remove method for audio listener component
 */
TEST_F(SceneTest, GameObject_removeAudioListener) {
    aderite::scene::GameObject* go = new aderite::scene::GameObject(nullptr, "asd");
    go->addAudioListener();
    go->removeAudioListener();
    EXPECT_EQ(go->getAudioListener(), nullptr);
}

/**
 * @brief Verifies get and set methods for camera fov setting
 */
TEST_F(SceneTest, CameraSettings_fov) {
    aderite::scene::CameraSettings settings;
    settings.setFoV(20.0f);
    EXPECT_EQ(settings.getFoV(), 20.0f);
}

/**
 * @brief Verifies get and set methods for camera far clip setting
 */
TEST_F(SceneTest, CameraSettings_farClip) {
    aderite::scene::CameraSettings settings;
    settings.setFarClip(20.0f);
    EXPECT_EQ(settings.getFarClip(), 20.0f);
}

/**
 * @brief Verifies get and set methods for camera near clip setting
 */
TEST_F(SceneTest, CameraSettings_nearClip) {
    aderite::scene::CameraSettings settings;
    settings.setNearClip(20.0f);
    EXPECT_EQ(settings.getNearClip(), 20.0f);
}

/**
 * @brief Verifies get and set methods for transform position
 */
TEST_F(SceneTest, TransformProvider_position) {
    aderite::scene::TransformProvider transform;
    transform.setPosition({1.0f, 0.0f, 0.0f});
    EXPECT_EQ(transform.getPosition().x, 1.0f);
}

/**
 * @brief Verifies get and set methods for transform rotation
 */
TEST_F(SceneTest, TransformProvider_rotation) {
    aderite::scene::TransformProvider transform;
    transform.setRotation({1.0f, 0.0f, 0.0f, 0.0f});
    EXPECT_EQ(transform.getRotation().w, 1.0f);
}

/**
 * @brief Verifies get and set methods for transform scale
 */
TEST_F(SceneTest, TransformProvider_scale) {
    aderite::scene::TransformProvider transform;
    transform.setScale({1.0f, 0.0f, 0.0f});
    EXPECT_EQ(transform.getScale().x, 1.0f);
}

/**
 * @brief Verifies get and set methods for transform modify flag
 */
TEST_F(SceneTest, TransformProvider_modify) {
    aderite::scene::TransformProvider transform;
    EXPECT_FALSE(transform.wasModified());

    transform.setPosition({});
    EXPECT_TRUE(transform.wasModified());

    transform.resetModifiedFlag();
    EXPECT_FALSE(transform.wasModified());
}
