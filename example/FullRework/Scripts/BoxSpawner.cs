using Aderite;

namespace Scripts
{
    class BoxSpawner : ScriptedBehavior
    {
        public float SpawnInterval = 5.0f;
        public Prefab Prefab = null;

        private float CurrentTime = 0.0f;

        void Update(float delta)
        {
            CurrentTime += delta;
            if (CurrentTime >= SpawnInterval)
            {
                // Spawn
                Spawn();

                // Decrement timer
                CurrentTime -= SpawnInterval;
            }
        }

        void Spawn()
        {
            Log.Trace("Spawning box");

            if (Prefab == null)
            {
                Log.Error("Prefab is null can't spawn");
                return;
            }

            // Create game object instance
            GameObject go = Prefab.Instantiate();

            // Set its initial position
            Transform objectTransform = go.GetTransform();
            Transform thisTransform = GameObject.GetTransform();

            objectTransform.Position = thisTransform.Position;
        }
    }
}
