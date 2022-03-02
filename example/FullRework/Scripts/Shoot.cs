using Aderite;

namespace Scripts
{
    class Shoot : ScriptedBehavior
    {
        public float ShootingSpeed = 2.0f;
        public float MaxRange = 20.0f;

        private Transform Transform = null;
        private Camera Camera = null;
        private float CurrentCooldown = 0.0f;
        private int Score = 0;

        void Initialize()
        {
            Transform = GameObject.GetTransform();
            Camera = GameObject.GetCamera();
        }

        void Update(float delta)
        {
            if (Input.IsMouseButtonDown(MouseKey.LEFT))
            {
                // Left mouse button down
                if (CurrentCooldown <= 0.0f)
                {
                    ShootShot();
                }
            }

            CurrentCooldown -= delta;

            // Reset to 0.0f if less than zero, so that shots could not be buffered
            if (CurrentCooldown < 0.0f)
            {
                CurrentCooldown = 0.0f;
            }
        }

        void ShootShot()
        {
            Log.Trace("Shooting");

            // Cast raycast
            RaycastResult result = Physics.RaycastFirstHit(Transform.Position, Camera.Front, MaxRange);
            if (result != null)
            {
                if (result.Object.Name != "Ground")
                {
                    result.Object.Destroy();
                    Score += 1;
                    Log.Trace($"Hit {result.Object.Name} current score {Score}");
                }
            }

            // Set the cooldown
            CurrentCooldown = ShootingSpeed;
        }

        public int GetScore()
        {
            return Score;
        }
    }
}
