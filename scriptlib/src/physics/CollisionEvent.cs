namespace Aderite
{
    /// <summary>
    /// Event sent when a collision happens
    /// </summary>
    public class CollisionEvent
    {
        /// <summary>
        /// First object that collided
        /// </summary>
        public GameObject Object1 { get; }

        /// <summary>
        /// Second object that collided
        /// </summary>
        public GameObject Object2 { get; }

        internal CollisionEvent(GameObject obj1, GameObject obj2)
        {
            this.Object1 = obj1;
            this.Object2 = obj2;
        }
    }
}
