using System;

namespace Aderite
{
    /// <summary>
    /// Prefab class used to spawn entities in aderite
    /// </summary>
    public class Prefab
    {
        IntPtr Instance = IntPtr.Zero;

        internal Prefab(IntPtr instance)
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
