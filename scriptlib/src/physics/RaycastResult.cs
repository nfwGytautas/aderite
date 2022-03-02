namespace Aderite
{
    /// <summary>
    /// Class used to store information about a physics hit
    /// </summary>
    public class RaycastResult
    {
        /// <summary>
        /// Object that was hit with the ray
        /// </summary>
        public GameObject Object { get; }

        /// <summary>
        /// Distance to the object
        /// </summary>
        public float Distance { get; }

        internal RaycastResult(GameObject entity, float distance)
        {
            this.Object = entity;
            this.Distance = distance;
        }
    }
}
