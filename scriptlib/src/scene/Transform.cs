using System;
using System.Runtime.CompilerServices;

namespace Aderite
{
    /// <summary>
    /// Transform component used to control the position, rotation and scale of the game object
    /// </summary>
    public class Transform
    {
        /// <summary>
        /// Position of the transform component
        /// </summary>
        public Vector3 Position { get { return __GetPosition(Instance); } set { __SetPosition(Instance, value); } }

        /// <summary>
        /// Rotation of the transform component, encoded as a quaternion vector
        /// </summary>
        public Quaternion Rotation { get { return __GetRotation(Instance); } set { __SetRotation(Instance, value); } }

        /// <summary>
        /// Scale of the transform component
        /// </summary>
        public Vector3 Scale { get { return __GetScale(Instance); } set { __SetScale(Instance, value); } }

        // The C++ instance of this object
        IntPtr Instance = IntPtr.Zero;

        internal Transform(IntPtr instance)
        {
            Instance = instance;
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern static Vector3 __GetPosition(IntPtr instance);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern static void __SetPosition(IntPtr instance, Vector3 value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern static Quaternion __GetRotation(IntPtr instance);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern static void __SetRotation(IntPtr instance, Quaternion value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern static Vector3 __GetScale(IntPtr instance);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern static void __SetScale(IntPtr instance, Vector3 value);
    }
}
