using System;
using System.Runtime.CompilerServices;

namespace Aderite
{
    /// <summary>
    /// Collider is used to create a trigger zone or create a collision zone
    /// </summary>
    class Collider
    {
        /// <summary>
        /// If true then the collider will be used as a trigger and will not participate in collision detection
        /// </summary>
        public bool IsTrigger { get { return __GetIsTrigger(Instance); } set { __SetIsTrigger(Instance, value); } }

        // The C++ instance of this object
        IntPtr Instance = IntPtr.Zero;

        internal Collider(IntPtr instance)
        {
            Instance = instance;
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern static bool __GetIsTrigger(IntPtr instance);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern static void __SetIsTrigger(IntPtr instance, bool value);
    }
}
