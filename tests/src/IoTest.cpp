#include <aderite/Aderite.hpp>
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#define private public
#define protected public

#include <aderite/input/InputManager.hpp>

#define private private
#define protected protected

class IoTest : public ::testing::Test {
public:
    static void SetUpTestSuite() {
        aderite::Engine::get()->init({});
    }

    static void TearDownTestSuite() {
        aderite::Engine::get()->shutdown();
    }
};

/**
 * @brief Verifies input manager init method
 */
TEST_F(IoTest, InputManager_init) {
    EXPECT_NE(aderite::Engine::getInputManager(), nullptr);
}

/**
 * @brief Verifies input manager update method
 */
TEST_F(IoTest, InputManager_update) {
    aderite::Engine::getInputManager()->m_currentFrameState.MouseScroll = 5.5f;
    aderite::Engine::getInputManager()->update();
    EXPECT_EQ(aderite::Engine::getInputManager()->m_currentFrameState.MouseScroll, 0.0f);
    EXPECT_EQ(aderite::Engine::getInputManager()->m_previousFrameState.MouseScroll, 5.5f);
}

/**
 * @brief Verifies input manager on key state change
 */
TEST_F(IoTest, InputManager_onKeyStateChange) {
    aderite::Engine::getInputManager()->m_currentFrameState.KeyStates[static_cast<size_t>(aderite::input::Key::A)] = false;
    aderite::Engine::getInputManager()->onKeyStateChange(aderite::input::Key::A, aderite::input::KeyAction::PRESS,
                                                         aderite::input::KeyModifier::SUPER);
    EXPECT_EQ(aderite::Engine::getInputManager()->m_currentFrameState.KeyStates[static_cast<size_t>(aderite::input::Key::A)], true);
}

/**
 * @brief Verifies input manager on mouse key state change
 */
TEST_F(IoTest, InputManager_onMouseKeyStateChange) {
    aderite::Engine::getInputManager()->m_currentFrameState.MouseKeyStates[static_cast<size_t>(aderite::input::MouseKey::B1)] = false;
    aderite::Engine::getInputManager()->onMouseKeyStateChange(aderite::input::MouseKey::B1, aderite::input::KeyAction::PRESS,
                                                         aderite::input::KeyModifier::SUPER);
    EXPECT_EQ(aderite::Engine::getInputManager()->m_currentFrameState.MouseKeyStates[static_cast<size_t>(aderite::input::MouseKey::B1)],
              true);
}

/**
 * @brief Verifies input manager on mouse move
 */
TEST_F(IoTest, InputManager_onMouseMove) {
    aderite::Engine::getInputManager()->m_currentFrameState.MousePosition.x = 0;
    aderite::Engine::getInputManager()->onMouseMove(10, 10);
    EXPECT_EQ(aderite::Engine::getInputManager()->m_currentFrameState.MousePosition.x, 10);
}

/**
 * @brief Verifies input manager on mouse scrolled
 */
TEST_F(IoTest, InputManager_onMouseScrolled) {
    aderite::Engine::getInputManager()->m_currentFrameState.MouseScroll = 0;
    aderite::Engine::getInputManager()->onMouseScrolled(10);
    EXPECT_EQ(aderite::Engine::getInputManager()->m_currentFrameState.MouseScroll, 10);
}

/**
 * @brief Verifies input manager is key pressed
 */
TEST_F(IoTest, InputManager_isKeyPressed) {
    aderite::Engine::getInputManager()->m_currentFrameState.KeyStates[static_cast<size_t>(aderite::input::Key::A)] = true;
    EXPECT_TRUE(aderite::Engine::getInputManager()->isKeyPressed(aderite::input::Key::A));
}

/**
 * @brief Verifies input manager was key released
 */
TEST_F(IoTest, InputManager_wasKeyReleased) {
    aderite::Engine::getInputManager()->m_previousFrameState.KeyStates[static_cast<size_t>(aderite::input::Key::A)] = true;
    aderite::Engine::getInputManager()->m_currentFrameState.KeyStates[static_cast<size_t>(aderite::input::Key::A)] = false;
    EXPECT_TRUE(aderite::Engine::getInputManager()->wasKeyReleased(aderite::input::Key::A));
}

/**
 * @brief Verifies input manager is mouse key pressed
 */
TEST_F(IoTest, InputManager_isMouseKeyPressed) {
    aderite::Engine::getInputManager()->m_currentFrameState.MouseKeyStates[static_cast<size_t>(aderite::input::MouseKey::B1)] = true;
    EXPECT_TRUE(aderite::Engine::getInputManager()->isMouseKeyPressed(aderite::input::MouseKey::B1));
}

/**
 * @brief Verifies input manager get mouse position
 */
TEST_F(IoTest, InputManager_getMousePosition) {
    aderite::Engine::getInputManager()->m_currentFrameState.MousePosition = {10, 10};
    EXPECT_EQ(aderite::Engine::getInputManager()->getMousePosition().x, 10);
    EXPECT_EQ(aderite::Engine::getInputManager()->getMousePosition().y, 10);
}

/**
 * @brief Verifies input manager get mouse delta
 */
TEST_F(IoTest, InputManager_getMouseDelta) {
    aderite::Engine::getInputManager()->m_previousFrameState.MousePosition = {5, 5};
    aderite::Engine::getInputManager()->m_currentFrameState.MousePosition = {10, 10};
    EXPECT_EQ(aderite::Engine::getInputManager()->getMouseDelta().x, 5);
    EXPECT_EQ(aderite::Engine::getInputManager()->getMouseDelta().y, -5);
}

/**
 * @brief Verifies input manager get scroll delta
 */
TEST_F(IoTest, InputManager_getScrollDelta) {
    aderite::Engine::getInputManager()->m_currentFrameState.MouseScroll = 10;
    EXPECT_EQ(aderite::Engine::getInputManager()->getScrollDelta(), 10);
}
