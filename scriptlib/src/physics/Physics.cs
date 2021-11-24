using System.Runtime.CompilerServices;

namespace Aderite
{
    /// <summary>
    /// Class used to invoke various physics function on the current scene
    /// </summary>
    public class Physics
    {
        /// <summary>
        /// Does a single hit raycast from the specified point in the specified direction for a maximum specified distance.
        /// NOTE: That if the ray is cast from inside a collider then the collider will be returned as a hit
        /// </summary>
        /// <param name="from">From point</param>
        /// <param name="direction">Direction to cast to</param>
        /// <param name="MaxDistance">Max distance of the ray</param>
        /// <returns>True if there was a hit, false otherwise</returns>
        public static RaycastHit RaycastFirstHit(Vector3 from, Vector3 direction, float MaxDistance) 
        {
            return __RaycastSingle(from, direction, MaxDistance);
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern static RaycastHit __RaycastSingle(Vector3 from, Vector3 direction, float distance);
    }
}
