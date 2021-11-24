namespace Aderite
{
    /// <summary>
    /// A class that represents a trigger event between two entities
    /// </summary>
    class TriggerEvent
    {
        public Entity Actor;
        public Entity Trigger;

        internal TriggerEvent(Entity actor, Entity trigger)
        {
            this.Actor = actor;
            this.Trigger = trigger;
        }
    }
}
