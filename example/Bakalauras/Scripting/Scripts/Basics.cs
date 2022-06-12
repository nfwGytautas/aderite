using Aderite;

namespace Scripts
{
    class Basics : ScriptedBehavior
    {
        public bool ShowUpdateMessage = true;

        void Initialize()
        {
            Log.Warn("Basics script Initialize method");
        }

        void Update(float delta)
        {
            if (ShowUpdateMessage)
            {
                Log.Warn("Basics script Update method");
            }
        }

        void Shutdown()
        {
            Log.Warn("Basics script Shutdown method");
        }
    }
}
