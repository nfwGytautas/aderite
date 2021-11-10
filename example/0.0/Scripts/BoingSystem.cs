using Aderite;

namespace Scripts
{
    class BoingSystem : ScriptSystem
    {
        public AudioSource Source;
        public Audio Boing;

        void Init()
        {
            Log.Trace("Initializing BoingSystem");
        }

        void Shutdown()
        {
            Log.Trace("Shutting down BoingSystem");
        }

        void Update(float delta, Entity[] entities)
        {
            Log.Trace($"BoingSystem updating {entities.Length} entities");
        }

        void OnCollisionStart(CollisionEvent collision)
        {
            Log.Trace("Collision start");
        }
    }
}
