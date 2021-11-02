using System;
using System.Runtime.CompilerServices;

namespace Aderite
{
    /// <summary>
    /// Dynamic physics actor that can move in the world, has mass, inertia and forces
    /// </summary>
    public class DynamicActor : PhysicsActor
    {
        /// <summary>
        /// Set actor kinematic property, kinematic objects are handled differently than simple dynamic objects.
        /// These objects are moved by the physics engine regardless of forces affecting them
        /// </summary>
        public bool Kinematic { get { return __GetKinematic(scene, entity); } set { __SetKinematic(scene, entity, value); } }

        /// <summary>
        /// Actor gravity property if this is true then the actor is affected by gravity
        /// </summary>
        public bool HasGravity { get { return __GetGravity(scene, entity); } set { __SetGravity(scene, entity, value); } }

        /// <summary>
        /// Mass of the actor
        /// </summary>
        public float Mass { get { return __GetMass(scene, entity); } set { __SetMass(scene, entity, value); } }

        internal DynamicActor(IntPtr scene, UIntPtr entity) : base(scene, entity)
        {
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern static bool __GetKinematic(IntPtr scene, UIntPtr entity);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern static bool __GetGravity(IntPtr scene, UIntPtr entity);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern static float __GetMass(IntPtr scene, UIntPtr entity);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern static void __SetKinematic(IntPtr scene, UIntPtr entity, bool value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern static void __SetGravity(IntPtr scene, UIntPtr entity, bool value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern static void __SetMass(IntPtr scene, UIntPtr entity, float mass);
    }
}
