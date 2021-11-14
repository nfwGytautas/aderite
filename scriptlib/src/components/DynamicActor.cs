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
        public bool Kinematic { get { return __GetKinematic(Instance); } set { __SetKinematic(Instance, value); } }

        /// <summary>
        /// Actor gravity property if this is true then the actor is affected by gravity
        /// </summary>
        public bool HasGravity { get { return __GetGravity(Instance); } set { __SetGravity(Instance, value); } }

        /// <summary>
        /// Mass of the actor
        /// </summary>
        public float Mass { get { return __GetMass(Instance); } set { __SetMass(Instance, value); } }

        internal DynamicActor(IntPtr instance) : base(instance)
        {
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern static bool __GetKinematic(IntPtr instance);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern static bool __GetGravity(IntPtr instance);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern static float __GetMass(IntPtr instance);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern static void __SetKinematic(IntPtr instance, bool value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern static void __SetGravity(IntPtr instance, bool value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern static void __SetMass(IntPtr instance, float mass);
    }
}
