using Aderite;

namespace Scripts
{
    class StateTracker
    {
        public int StartingLives = 3;

        private int Lives;

        void Initialize()
        {
            Lives = StartingLives;
        }

        public void DecrementLives()
        {
            Log.Trace("Decrementing life point");

            Lives--;

            if (Lives == 0)
            {
                Log.Warn("Game over!");
            }
        }
    }
}
