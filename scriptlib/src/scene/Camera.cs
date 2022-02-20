using System;
using System.Runtime.CompilerServices;

namespace Aderite
{
    /// <summary>
    /// Camera component used to manipulate a camera that is attached to a game object
    /// </summary>
    class Camera
    {
        /// <summary>
        /// FoV of the camera
        /// </summary>
        public float FoV { get { return __GetFoV(Instance); } set { __SetFoV(Instance, value); } }

        /// <summary>
        /// Near clip of the camera
        /// </summary>
        public float NearClip { get { return __GetNearClip(Instance); } set { __SetNearClip(Instance, value); } }

        /// <summary>
        /// Far clip of the camera
        /// </summary>
        public float FarClip { get { return __GetFarClip(Instance); } set { __SetFarClip(Instance, value); } }

        // The C++ instance of this object
        IntPtr Instance = IntPtr.Zero;

        internal Camera(IntPtr instance)
        {
            Instance = instance;
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern static float __GetFoV(IntPtr instance);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern static void __SetFoV(IntPtr instance, float value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern static float __GetNearClip(IntPtr instance);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern static void __SetNearClip(IntPtr instance, float value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern static float __GetFarClip(IntPtr instance);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern static void __SetFarClip(IntPtr instance, float value);
    }
}
