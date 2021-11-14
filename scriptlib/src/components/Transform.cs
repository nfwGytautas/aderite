using System;
using System.Runtime.CompilerServices;

namespace Aderite
{
    /// <summary>
    /// Transform component
    /// </summary>
    public class Transform
    {
        IntPtr Instance = IntPtr.Zero;

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

        internal Transform(IntPtr instance)
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
