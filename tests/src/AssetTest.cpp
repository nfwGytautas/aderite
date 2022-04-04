#include <aderite/Aderite.hpp>
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#define private public
#define protected public

#include <aderite/asset/AssetManager.hpp>
#include <aderite/asset/AudioAsset.hpp>
#include <aderite/asset/MaterialAsset.hpp>
#include <aderite/asset/MaterialTypeAsset.hpp>
#include <aderite/asset/MeshAsset.hpp>
#include <aderite/asset/PrefabAsset.hpp>
#include <aderite/asset/TextureAsset.hpp>
#include <aderite/reflection/RuntimeTypes.hpp>

#define private private
#define protected protected

class AssetTest : public ::testing::Test {
public:
    static void SetUpTestSuite() {
        aderite::Engine::get()->init({});

        testMesh = new aderite::asset::MeshAsset();
    }

    static void TearDownTestSuite() {
        aderite::Engine::get()->shutdown();
    }

    static aderite::asset::MeshAsset* testMesh;
};

aderite::asset::MeshAsset* AssetTest::testMesh = nullptr;

/**
 * @brief Verify the asset manager init method
 */
TEST_F(AssetTest, AssetManager_init) {
    EXPECT_NE(aderite::Engine::getAssetManager(), nullptr);
}

/**
 * @brief Verify the asset manager track method
 */
TEST_F(AssetTest, AssetManager_track) {
    EXPECT_EQ(::aderite::Engine::getAssetManager()->m_registry.size(), 0);
    EXPECT_EQ(::aderite::Engine::getAssetManager()->m_nextFreeHandle, 0);

    aderite::Engine::getAssetManager()->track(testMesh);

    EXPECT_EQ(testMesh->m_handle, 0);
    EXPECT_EQ(::aderite::Engine::getAssetManager()->m_registry.size(), 1);
    EXPECT_EQ(::aderite::Engine::getAssetManager()->m_nextFreeHandle, 1);
}

/**
 * @brief Verify the asset manager untrack method
 */
TEST_F(AssetTest, AssetManager_untrack) {
    aderite::Engine::getAssetManager()->track(testMesh);

    EXPECT_EQ(::aderite::Engine::getAssetManager()->m_registry.size(), 1);
    EXPECT_EQ(::aderite::Engine::getAssetManager()->m_nextFreeHandle, 1);

    aderite::Engine::getAssetManager()->untrack(testMesh);

    EXPECT_EQ(testMesh->m_handle, 0);
    EXPECT_EQ(::aderite::Engine::getAssetManager()->m_registry.size(), 0);
    EXPECT_EQ(::aderite::Engine::getAssetManager()->m_nextFreeHandle, 1);
}

/**
 * @brief Verify the asset manager has method
 */
TEST_F(AssetTest, AssetManager_has) {
    aderite::Engine::getAssetManager()->track(testMesh);
    EXPECT_TRUE(::aderite::Engine::getAssetManager()->get(testMesh->m_handle));
}

/**
 * @brief Verify the asset manager get method
 */
TEST_F(AssetTest, AssetManager_get) {
    aderite::Engine::getAssetManager()->track(testMesh);
    aderite::io::SerializableAsset* asset = ::aderite::Engine::getAssetManager()->get(testMesh->m_handle);
    EXPECT_EQ(testMesh, asset);
}

/**
 * @brief Verify the asset manager update method
 */
TEST_F(AssetTest, AssetManager_update) {
    aderite::Engine::getAssetManager()->track(testMesh);

    // Make needsLoading return false
    testMesh->m_vbh = {1};
    testMesh->m_ibh = {1};

    // When a ref count is positive the mesh should remain
    testMesh->m_refCount = 1;
    ::aderite::Engine::getAssetManager()->update();
    EXPECT_EQ(::aderite::Engine::getAssetManager()->m_registry.size(), 1);
    EXPECT_NE(::aderite::Engine::getAssetManager()->m_registry[0].Asset, nullptr);

    // Ignore unload call
    testMesh->m_vbh = BGFX_INVALID_HANDLE;
    testMesh->m_ibh = BGFX_INVALID_HANDLE;

    // When ref count 0 should free asset
    testMesh->m_refCount = 0;
    ::aderite::Engine::getAssetManager()->update();
    EXPECT_EQ(::aderite::Engine::getAssetManager()->m_registry.size(), 1);
    EXPECT_EQ(::aderite::Engine::getAssetManager()->m_registry[0].Asset, nullptr);
}

/**
 * @brief Verify the asset ref counting mechanism
 */
TEST_F(AssetTest, Asset_refCount) {
    aderite::asset::AudioAsset aa;
    EXPECT_EQ(aa.m_refCount, 0);

    // Acquire reference
    aa.acquire();
    EXPECT_EQ(aa.m_refCount, 1);

    // Release reference
    aa.release();
    EXPECT_EQ(aa.m_refCount, 0);
}

/**
 * @brief Verify the audio asset event name setter/getter
 */
TEST_F(AssetTest, AudioAsset_getterSetter) {
    aderite::asset::AudioAsset aa;
    aa.setEventName("test");
    EXPECT_EQ("test", aa.getEventName());
}

/**
 * @brief Verify the mesh asset isValid method
 */
TEST_F(AssetTest, MeshAsset_isValid) {
    aderite::asset::MeshAsset ma;

    // By default not valid
    EXPECT_FALSE(ma.isValid());

    ma.m_vbh = {1};
    ma.m_ibh = {1};

    // Should be true now
    EXPECT_TRUE(ma.isValid());

    // Reset to not crash bgfx
    ma.m_vbh = BGFX_INVALID_HANDLE;
    ma.m_ibh = BGFX_INVALID_HANDLE;
}

///**
// * @brief Verify the texture asset isValid method
// */
// TEST_F(AssetTest, TextureAsset_fields) {
//    aderite::asset::TextureAsset ta;
//
//    // By default false
//    EXPECT_FALSE(ta.m_isHDR);
//    EXPECT_FALSE(ta.m_isCubemap);
//
//    ta.setCubemap(true);
//    ta.setHDR(true);
//
//    EXPECT_TRUE(ta.isHDR());
//    EXPECT_TRUE(ta.isCubemap());
//}

/**
 * @brief Verify the assets and their reflection types
 */
TEST_F(AssetTest, ReflectionTypes) {
    aderite::asset::AudioAsset aa;
    aderite::asset::MaterialAsset ma;
    aderite::asset::MaterialTypeAsset mta;
    aderite::asset::MeshAsset msa;
    aderite::asset::PrefabAsset pa;
    aderite::asset::TextureAsset ta;

    EXPECT_EQ(static_cast<aderite::reflection::RuntimeTypes>(aa.getType()), aderite::reflection::RuntimeTypes::AUDIO);
    EXPECT_EQ(static_cast<aderite::reflection::RuntimeTypes>(ma.getType()), aderite::reflection::RuntimeTypes::MATERIAL);
    EXPECT_EQ(static_cast<aderite::reflection::RuntimeTypes>(mta.getType()), aderite::reflection::RuntimeTypes::MAT_TYPE);
    EXPECT_EQ(static_cast<aderite::reflection::RuntimeTypes>(msa.getType()), aderite::reflection::RuntimeTypes::MESH);
    EXPECT_EQ(static_cast<aderite::reflection::RuntimeTypes>(pa.getType()), aderite::reflection::RuntimeTypes::PREFAB);
    EXPECT_EQ(static_cast<aderite::reflection::RuntimeTypes>(ta.getType()), aderite::reflection::RuntimeTypes::TEXTURE);
}
