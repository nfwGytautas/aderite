#include "ScriptAudio.hpp"

#include <mono/jit/jit.h>

#include "aderite/audio/AudioInstance.hpp"

namespace aderite {
namespace scripting {

void PlayAudio(audio::AudioInstance* instance) {
    instance->start();
}

void StopAudio(audio::AudioInstance* instance) {
    instance->stop();
}

void audioInternals() {
    mono_add_internal_call("Aderite.AudioInstance::__Play(intptr)", reinterpret_cast<void*>(PlayAudio));
    mono_add_internal_call("Aderite.AudioInstance::__Stop(intptr)", reinterpret_cast<void*>(StopAudio));
}

} // namespace scripting
} // namespace aderite
