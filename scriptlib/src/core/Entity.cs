using System;
using System.Runtime.CompilerServices;

namespace Aderite
{
    /// <summary>
    /// Object representing an entity in the game world
    /// </summary>
    public class Entity
    {
        IntPtr Instance = IntPtr.Zero;

        internal Entity(IntPtr instance)
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
    }
}
