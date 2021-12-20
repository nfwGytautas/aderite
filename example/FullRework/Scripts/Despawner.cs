using Aderite;

namespace Scripts
{
    class Despawner
    {
        BoxSpawner boxSpawner;
        StateTracker stateTracker;

        void Initialize()
        {
            boxSpawner = Engine.GetScript<BoxSpawner>();
            stateTracker = Engine.GetScript<StateTracker>();
        }

        void OnCollisionEnter(Geometry g)
        {
            // Call system functions
            stateTracker.DecrementLives();
            boxSpawner.EntityDestroyed();

            // Destroy entity
            g.GetEntity().Destroy();
        }
    }
}
