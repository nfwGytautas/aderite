using System;
using System.Runtime.CompilerServices;

namespace Aderite
{
    /// <summary>
    /// Object representing an entity in the game world
    /// </summary>
    public class Entity
    {
        public string Name { get { return __GetEntityName(Instance); } }

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

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern static string __GetEntityName(IntPtr instance);
    }
}
