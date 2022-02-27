#pragma once

#ifndef ADERITE_ASSET_INTERNALS
#define ADERITE_ASSET_INTERNALS
#else
#error "Multiple AssetInternal.hpp include"
#endif

#include <mono/jit/jit.h>

#include "aderite/Aderite.hpp"
#include "aderite/asset/PrefabAsset.hpp"
#include "aderite/scene/GameObject.hpp"
#include "aderite/scene/SceneManager.hpp"

namespace internal_ {

namespace prefab {

MonoObject* Instantiate(aderite::asset::PrefabAsset* prefab) {
    return prefab->instantiate(::aderite::Engine::getSceneManager()->getCurrentScene())->getScriptInstance();
}

void linkPrefab() {
    mono_add_internal_call("Aderite.Prefab::__Instantiate(intptr)", reinterpret_cast<void*>(Instantiate));
}
} // namespace prefab

} // namespace internal_

void linkAsset() {
    // Prefab
    internal_::prefab::linkPrefab();
}
