using Aderite;

namespace Scripts
{
    class BoxSpawner
    {
        public int MaxLive = 10;

        void Update(float delta)
        {
            Log.Trace("Will spawn " + MaxLive.ToString());
        }
    }
}
