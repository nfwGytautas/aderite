using Aderite;

namespace Scripts
{
    class Ground : ScriptedBehavior
    {
        public int Lives = 3;

        GameObject Player = null;
        Shoot ShootBehavior = null;
        bool GameOver = false;

        void Initialize()
        {
            Player = Engine.FindGameObject("Player");

            if (Player != null)
            {
                ShootBehavior = (Shoot)Player.GetBehavior("Scripts.Shoot");
            }
        }

        void Update(float delta)
        {
            if (Lives <= 0 && !GameOver)
            {
                GameOver = true;

                // Stop game
                Engine.StopPhysics();

                // Output score to console
                Log.Trace($"Score: {ShootBehavior.GetScore()}");
            }
        }

        void OnCollisionStart(CollisionEvent ce)
        {
            if (ce.Object1.Name == "Ground")
            {
                ce.Object2.Destroy();
            }
            else
            {
                ce.Object1.Destroy();
            }

            Lives -= 1;

            Log.Trace($"Lives decremented, {Lives} left");
        }
    }
}
