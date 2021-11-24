#include "ScriptAudio.hpp"

#include <mono/jit/jit.h>

#include "aderite/asset/AudioAsset.hpp"
#include "aderite/audio/AudioInstance.hpp"
#include "aderite/audio/AudioSource.hpp"

namespace aderite {
namespace scripting {

glm::vec3 GetPosition(audio::AudioSource* source) {
    return source->getPosition();
}

glm::quat GetRotation(audio::AudioSource* source) {
    return source->getRotation();
}

glm::vec3 GetVelocity(audio::AudioSource* source) {
    return source->getVelocity();
}

float GetVolume(audio::AudioSource* source) {
    return source->getVolume();
}

void SetPosition(audio::AudioSource* source, glm::vec3 position) {
    source->setPosition(position);
}

void SetRotation(audio::AudioSource* source, glm::quat rotation) {
    source->setRotation(rotation);
}

void SetVelocity(audio::AudioSource* source, glm::vec3 velocity) {
    source->setVelocity(velocity);
}

void SetVolume(audio::AudioSource* source, float volume) {
    source->setVolume(volume);
}

void Mute(audio::AudioSource* source) {
    source->mute();
}

void Unmute(audio::AudioSource* source) {
    source->unmute();
}

void Stop(audio::AudioSource* source) {
    source->stop();
}

audio::AudioInstance* CreateInstance(audio::AudioSource* source, asset::AudioAsset* audio) {
    return source->createInstance(audio);
}

void OneShot(audio::AudioSource* source, asset::AudioAsset* audio) {
    source->createOneshot(audio);
}

void PlayAudio(audio::AudioInstance* instance) {
    instance->start();
}

void StopAudio(audio::AudioInstance* instance) {
    instance->stop();
}

void audioInternals() {
    // Source
    mono_add_internal_call("Aderite.AudioSource::__GetPosition(intptr)", reinterpret_cast<void*>(GetPosition));
    mono_add_internal_call("Aderite.AudioSource::__GetRotation(intptr)", reinterpret_cast<void*>(GetRotation));
    mono_add_internal_call("Aderite.AudioSource::__GetVelocity(intptr)", reinterpret_cast<void*>(GetVelocity));
    mono_add_internal_call("Aderite.AudioSource::__GetVolume(intptr)", reinterpret_cast<void*>(GetVolume));
    mono_add_internal_call("Aderite.AudioSource::__SetPosition(intptr,Aderite.Vector3)", reinterpret_cast<void*>(SetPosition));
    mono_add_internal_call("Aderite.AudioSource::__SetRotation(intptr,Aderite.Quaternion)", reinterpret_cast<void*>(SetRotation));
    mono_add_internal_call("Aderite.AudioSource::__SetVelocity(intptr,Aderite.Vector3)", reinterpret_cast<void*>(SetVelocity));
    mono_add_internal_call("Aderite.AudioSource::__SetVolume(intptr,single)", reinterpret_cast<void*>(SetVolume));
    mono_add_internal_call("Aderite.AudioSource::__Mute(intptr)", reinterpret_cast<void*>(Mute));
    mono_add_internal_call("Aderite.AudioSource::__Unmute(intptr)", reinterpret_cast<void*>(Unmute));
    mono_add_internal_call("Aderite.AudioSource::__Stop(intptr)", reinterpret_cast<void*>(Stop));
    mono_add_internal_call("Aderite.AudioSource::__CreateInstance(intptr,intptr)", reinterpret_cast<void*>(CreateInstance));
    mono_add_internal_call("Aderite.AudioSource::__OneShot(intptr,intptr)", reinterpret_cast<void*>(OneShot));

    // Instance
    mono_add_internal_call("Aderite.AudioInstance::__Play(intptr)", reinterpret_cast<void*>(PlayAudio));
    mono_add_internal_call("Aderite.AudioInstance::__Stop(intptr)", reinterpret_cast<void*>(StopAudio));
}

} // namespace scripting
} // namespace aderite
