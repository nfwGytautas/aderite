using System.Runtime.CompilerServices;

namespace Aderite
{
    /// <summary>
    /// Methods used to manipulate the aderite engine itself
    /// </summary>
    public class Engine
    {
        /// <summary>
        /// Stops physics updates from behaviors
        /// </summary>
        public static void StopPhysics()
        {
            __StopPhysics();
        }

        /// <summary>
        /// Starts physics updates from behaviors
        /// </summary>
        public static void StartPhysics()
        {
            __StartPhysics();
        }

        /// <summary>
        /// Finds and returns a game object with the specified name or null if one doesn't exist in current scene
        /// </summary>
        /// <param name="name">Name of the game object</param>
        /// <returns>GameObject instance</returns>
        public static GameObject FindGameObject(string name)
        {
            return __FindGameObject(name);
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern static void __StopPhysics();

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern static void __StartPhysics();

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern static GameObject __FindGameObject(string name);
    }
}
