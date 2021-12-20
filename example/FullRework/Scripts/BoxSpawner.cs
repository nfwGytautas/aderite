using Aderite;

namespace Scripts
{
    class BoxSpawner
    {
        public int MaxLive = 10;

        void Update(float delta)
        {

        }

        public void EntityDestroyed()
        {
            Log.Trace("Box destroyed creating a new one");
        }
    }
}
