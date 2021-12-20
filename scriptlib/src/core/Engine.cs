using System;
using System.Runtime.CompilerServices;

namespace Aderite
{
    /// <summary>
    /// Class providing some core functionality of aderite
    /// </summary>
    public class Engine
    {
        /// <summary>
        /// Get a script instance
        /// </summary>
        /// <typeparam name="T">Script type</typeparam>
        /// <returns>Script instance</returns>
        public static T GetScript<T>() where T : class
        {
            return (T)__GetScript(typeof(T).Name);
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern static object __GetScript(string name);
    }
}
