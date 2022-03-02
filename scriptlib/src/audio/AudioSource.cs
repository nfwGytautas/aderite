using System;
using System.Runtime.CompilerServices;

namespace Aderite
{
    /// <summary>
    /// Audio source object used to manipulate the 
    /// </summary>
    public class AudioSource
    {
        /// <summary>
        /// If this is true then all audio coming from this source will be muted, otherwise the volume of the source will be used
        /// </summary>
        public bool Muted { get { return __GetMuted(Instance); } set { __SetMuted(Instance, value); } }

        /// <summary>
        /// Volume of the source from 0.0(Muted) to 1.0(Full sound)
        /// </summary>
        public float Volume { get { return __GetVolume(Instance); } set { __SetVolume(Instance, value); } }

        // The C++ instance of this object
        IntPtr Instance = IntPtr.Zero;

        internal AudioSource(IntPtr instance)
        {
            Instance = instance;
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern static bool __GetMuted(IntPtr instance);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern static void __SetMuted(IntPtr instance, bool value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern static float __GetVolume(IntPtr instance);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern static void __SetVolume(IntPtr instance, float value);
    }
}
