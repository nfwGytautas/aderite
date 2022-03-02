using System;
using System.Runtime.CompilerServices;

namespace Aderite
{
    /// <summary>
    /// Aderite Prefab class
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

        /// <summary>
        /// Creates an instance of a game object from this prefab in the current scene
        /// </summary>
        /// <returns>GameObject instance</returns>
        public GameObject Instantiate()
        {
            return __Instantiate(Instance);
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern static GameObject __Instantiate(IntPtr instance);
    }
}
