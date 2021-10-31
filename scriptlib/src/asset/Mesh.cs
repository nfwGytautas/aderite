using System;

namespace Aderite
{
    /// <summary>
    /// Aderite mesh class 
    /// </summary>
    public class Mesh
    {
        IntPtr Instance = IntPtr.Zero;

        internal Mesh(IntPtr instance)
        {
            Instance = instance;
        }

        /// <summary>
        /// Returns the C++ instance of the mesh
        /// </summary>
        /// <returns>IntPtr where the C++ object is located at</returns>
        internal IntPtr GetInstance()
        {
            return Instance;
        }
    }
}
