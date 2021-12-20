using System;
using System.Runtime.CompilerServices;

namespace Aderite
{
    /// <summary>
    /// Class that represents a piece of geometry attached to a entity
    /// </summary>
    public class Geometry
    {
        protected IntPtr Instance = IntPtr.Zero;

        internal Geometry(IntPtr instance)
        {
            Instance = instance;
        }

        /// <summary>
        /// Returns the entity attached to this geometry
        /// </summary>
        /// <returns>Entity instance</returns>
        public Entity GetEntity()
        {
            return __GetEntity(Instance);
        }

        /// <summary>
        /// Returns the C++ instance of the geometry
        /// </summary>
        /// <returns>IntPtr where the C++ object is located at</returns>
        internal IntPtr GetInstance()
        {
            return Instance;
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern static Entity __GetEntity(IntPtr instance);
    }
}
