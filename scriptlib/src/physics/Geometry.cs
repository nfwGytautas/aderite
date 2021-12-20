using System;

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
        /// Returns the C++ instance of the geometry
        /// </summary>
        /// <returns>IntPtr where the C++ object is located at</returns>
        internal IntPtr GetInstance()
        {
            return Instance;
        }
    }
}
