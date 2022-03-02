using Aderite;

namespace Scripts
{
    class Destructor : ScriptedBehavior
    {
        public float TimeAlive = 10.0f;

        private float CurrentTime = 0.0f;

        void Update(float delta)
        {
            CurrentTime += delta;
            if (CurrentTime >= TimeAlive)
            {
                // Destroy
                GameObject.Destroy();
            }
        }
    }
}
