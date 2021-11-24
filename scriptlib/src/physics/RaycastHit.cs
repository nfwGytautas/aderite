namespace Aderite
{
    /// <summary>
    /// Class used to store information about a physics hit
    /// </summary>
    public class RaycastHit
    {
        /// <summary>
        /// Object that was hit with the ray
        /// </summary>
        public Entity Object { get; }

        /// <summary>
        /// Distance to the object
        /// </summary>
        public float Distance { get; }

        internal RaycastHit(Entity entity, float distance)
        {
            this.Object = entity;
            this.Distance = distance;
        }
    }
}
