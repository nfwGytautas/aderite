using System;

namespace Aderite
{
    /// <summary>
    /// Aderite material class
    /// </summary>
    public class Material
    {
        IntPtr Instance = IntPtr.Zero;

        internal Material(IntPtr instance)
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
