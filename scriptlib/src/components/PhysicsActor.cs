using System;
using System.Runtime.CompilerServices;

namespace Aderite
{
    /// <summary>
    /// Class that represents a physics actor, be it static or dynamic
    /// </summary>
    public class PhysicsActor
    {
        protected IntPtr Instance = IntPtr.Zero;

        /// <summary>
        /// Move attached object to the specified position instantly
        /// </summary>
        /// <param name="position">Position to move to</param>
        public void TeleportTo(Vector3 position)
        {
            __Move(Instance, position);
        }

        /// <summary>
        /// Rotate attached object instantly
        /// </summary>
        /// <param name="rotation">New rotation of the object</param>
        public void Rotate(Quaternion rotation)
        {
            __Rotate(Instance, rotation);
        }

        internal PhysicsActor(IntPtr instance)
        {
            Instance = instance;
        }

        /// <summary>
        /// Returns the C++ instance of the audio
        /// </summary>
        /// <returns>IntPtr where the C++ object is located at</returns>
        internal IntPtr GetInstance()
        {
            return Instance;
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern static void __Move(IntPtr instance, Vector3 position);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern static void __Rotate(IntPtr instance, Quaternion rotation);
    }
}
