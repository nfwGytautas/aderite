using System;
using System.Runtime.CompilerServices;

namespace Aderite
{
    /// <summary>
    /// Audio source component
    /// </summary>
    public class AudioSource
    {
        IntPtr scene = IntPtr.Zero;
        UIntPtr entity = UIntPtr.Zero;

        internal AudioSource(IntPtr scene, UIntPtr entity)
        {
            this.scene = scene;
            this.entity = entity;
        }

        /// <summary>
        /// Creates AudioInstance from the specified audio clip and attaches it to this audio source
        /// </summary>
        /// <param name="audio">Audio clip</param>
        /// <returns>AudioInstance object</returns>
        public AudioInstance CreateInstance(Audio audio)
        {
            return new AudioInstance(__CreateInstance(this.scene, this.entity, audio.GetInstance()));
        }

        /// <summary>
        /// Play one shot audio
        /// </summary>
        /// <param name="audio">Audio to play</param>
        public void OneShot(Audio audio)
        {
            __OneShot(this.scene, this.entity, audio.GetInstance());
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern static IntPtr __CreateInstance(IntPtr scene, UIntPtr entity, IntPtr audio);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern static void __OneShot(IntPtr scene, UIntPtr entity, IntPtr audio);
    }
}
