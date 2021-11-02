using System;
using System.Runtime.CompilerServices;

namespace Aderite
{
    /// <summary>
    /// Class that represents a physics actor, be it static or dynamic
    /// </summary>
    public class PhysicsActor
    {
        protected internal IntPtr scene = IntPtr.Zero;
        protected internal UIntPtr entity = UIntPtr.Zero;

        /// <summary>
        /// Move attached object to the specified position instantly
        /// </summary>
        /// <param name="position">Position to move to</param>
        public void TeleportTo(Vector3 position)
        {
            __Move(scene, entity, position);
        }

        /// <summary>
        /// Rotate attached object instantly
        /// </summary>
        /// <param name="rotation">New rotation of the object</param>
        public void Rotate(Quaternion rotation)
        {
            __Rotate(scene, entity, rotation);
        }

        internal PhysicsActor(IntPtr scene, UIntPtr entity)
        {
            this.scene = scene;
            this.entity = entity;
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern static void __Move(IntPtr scene, UIntPtr entity, Vector3 position);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern static void __Rotate(IntPtr scene, UIntPtr entity, Quaternion rotation);
    }
}
