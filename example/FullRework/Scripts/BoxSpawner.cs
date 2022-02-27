using Aderite;

namespace Scripts
{
    class BoxSpawner : ScriptedBehavior
    {
        public int MaxLive = 10;

        void Initialize()
        {
            Log.Trace($"BoxSpawner attached to {GameObject.Name}");
        }

        void Update(float delta)
        {
            
        }
    }
}
