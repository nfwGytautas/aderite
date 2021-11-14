namespace Aderite
{
    /// <summary>
    /// A class that represents a collision between two entities
    /// </summary>
    public class CollisionEvent
    {
        public Entity Actor1;
        public Entity Actor2;

        internal CollisionEvent(Entity actor1, Entity actor2)
        {
            this.Actor1 = actor1;
            this.Actor2 = actor2;
        }
    }
}
