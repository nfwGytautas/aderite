using System;
using System.Runtime.CompilerServices;

namespace Aderite
{
    /// <summary>
    /// Transform component
    /// </summary>
    public class Transform
    {
        IntPtr scene = IntPtr.Zero;
        UIntPtr entity = UIntPtr.Zero;

        /// <summary>
        /// Position of the transform component
        /// </summary>
        public Vector3 Position { get { return __GetPosition(scene, entity); } set { __SetPosition(scene, entity, value); } }

        /// <summary>
        /// Rotation of the transform component, encoded as a quaternion vector
        /// </summary>
        public Quaternion Rotation { get { return __GetRotation(scene, entity); } set { __SetRotation(scene, entity, value); } }

        /// <summary>
        /// Scale of the transform component
        /// </summary>
        public Vector3 Scale { get { return __GetScale(scene, entity); } set { __SetScale(scene, entity, value); } }

        internal Transform(IntPtr scene, UIntPtr entity)
        {
            this.scene = scene;
            this.entity = entity;
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern static Vector3 __GetPosition(IntPtr scene, UIntPtr entity);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern static void __SetPosition(IntPtr scene, UIntPtr entity, Vector3 value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern static Quaternion __GetRotation(IntPtr scene, UIntPtr entity);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern static void __SetRotation(IntPtr scene, UIntPtr entity, Quaternion value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern static Vector3 __GetScale(IntPtr scene, UIntPtr entity);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern static void __SetScale(IntPtr scene, UIntPtr entity, Vector3 value);
    }
}
