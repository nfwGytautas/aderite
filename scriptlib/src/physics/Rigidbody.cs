using System;
using System.Runtime.CompilerServices;

namespace Aderite
{
    /// <summary>
    /// Rigidbody component used to control the physical properties of game objects
    /// </summary>
    class Rigidbody
    {
        /// <summary>
        /// Gravity flag of the game object if true then the object is affected by gravity, otherwise gravity will not affect this object
        /// </summary>
        public bool HasGravity { get { return __GetHasGravity(Instance); } set { __SetHasGravity(Instance, value); } }

        /// <summary>
        /// Mass of the game object
        /// </summary>
        public float Mass { get { return __GetMass(Instance); } set { __SetMass(Instance, value); } }

        // The C++ instance of this object
        IntPtr Instance = IntPtr.Zero;

        internal Rigidbody(IntPtr instance)
        {
            Instance = instance;
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern static bool __GetHasGravity(IntPtr instance);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern static void __SetHasGravity(IntPtr instance, bool value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern static float __GetMass(IntPtr instance);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern static void __SetMass(IntPtr instance, float value);
    }
}
