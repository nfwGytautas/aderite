using System;
using System.Runtime.CompilerServices;

namespace Aderite
{
    /// <summary>
    /// Transform component
    /// </summary>
    public class Transform
    {
        IntPtr instance = IntPtr.Zero;

        /// <summary>
        /// Position of the transform component
        /// </summary>
        public Vector3 Position { get { return __GetPosition(instance); } set { __SetPosition(instance, value); } }

        /// <summary>
        /// Rotation of the transform component, encoded as a quaternion vector
        /// </summary>
        public Quaternion Rotation { get { return __GetRotation(instance); } set { __SetRotation(instance, value); } }

        /// <summary>
        /// Scale of the transform component
        /// </summary>
        public Vector3 Scale { get { return __GetScale(instance); } set { __SetScale(instance, value); } }

        internal Transform(IntPtr scene, UIntPtr entity)
        {
            instance = __GetInstance(scene, entity);
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern static IntPtr __GetInstance(IntPtr scene, UIntPtr entity);

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
