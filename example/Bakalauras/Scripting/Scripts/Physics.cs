using Aderite;

namespace Scripts
{
    class Physics : ScriptedBehavior
    {
        void OnCollisionStart(CollisionEvent ce)
        {
            GameObject collidingWith = ce.Object1 == this.GameObject ? ce.Object2 : ce.Object1;
            Log.Warn($"{this.GameObject.Name} started colliding with {collidingWith.Name}");
        }

        void OnCollisionEnd(CollisionEvent ce)
        {
            GameObject collidingWith = ce.Object1 == this.GameObject ? ce.Object2 : ce.Object1;
            Log.Warn($"{this.GameObject.Name} stopped colliding with {collidingWith.Name}");
        }
    }
}
