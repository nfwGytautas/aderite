using System;
using System.Runtime.CompilerServices;

namespace Aderite
{
    /// <summary>
    /// AudioInstance class provides control over an audio clip and actual. If no audio listeners are present there
    /// will be no audio that can be heard, same goes if the entity doesn't have an audio source component
    /// </summary>
    public class AudioInstance
    {
        IntPtr Instance = IntPtr.Zero;

        internal AudioInstance(IntPtr instance)
        {
            this.Instance = instance;
        }

        /// <summary>
        /// Starts playing audio
        /// </summary>
        public void Play()
        {
            __Play(Instance);
        }

        /// <summary>
        /// Stops playing audio
        /// </summary>
        public void Stop()
        {
            __Stop(Instance);
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern static void __Play(IntPtr instance);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern static void __Stop(IntPtr instance);
    }
}
