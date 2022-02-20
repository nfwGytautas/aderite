using System;
using System.Runtime.CompilerServices;

namespace Aderite
{
    /// <summary>
    /// Audio listener object used to configure audio listeners
    /// </summary>
    class AudioListener
    {
        /// <summary>
        /// If this is true then this audio listener will be enabled and audio will be heard by this game object, multiple enabled 
        /// listeners the last one will be enabled, the order is unknown however
        /// </summary>
        public bool Enabled { get { return __GetEnabled(Instance); } set { __SetEnabled(Instance, value); } }

        // The C++ instance of this object
        IntPtr Instance = IntPtr.Zero;

        internal AudioListener(IntPtr instance)
        {
            Instance = instance;
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern static bool __GetEnabled(IntPtr instance);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern static void __SetEnabled(IntPtr instance, bool value);
    }
}
