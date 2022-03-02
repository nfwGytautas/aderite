using System;
using System.Runtime.CompilerServices;

namespace Aderite
{
    /// <summary>
    /// Simple cube collider shape
    /// </summary>
    class BoxCollider : Collider
    {
        /// <summary>
        /// Size of the collider
        /// </summary>
        public Vector3 Size { get { return __GetSize(Instance); } set { __SetSize(Instance, value); } }

        // The C++ instance of this object
        IntPtr Instance = IntPtr.Zero;

        internal BoxCollider(IntPtr instance)
            : base(instance)
        {
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern static Vector3 __GetSize(IntPtr instance);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern static void __SetSize(IntPtr instance, Vector3 value);
    }
}
