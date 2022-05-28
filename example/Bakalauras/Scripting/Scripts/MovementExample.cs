using Aderite;

namespace Scripts
{
    class MovementExample : ScriptedBehavior
    {
        public float Speed = 5.0f;

        private Transform Transform = null;

        void Initialize()
        {
            Transform = GameObject.GetTransform();
        }

        void Update(float delta)
        {
            Vector3 newPosition = Transform.Position;

            if (Input.IsKeyDown(Key.W))
            {
                // Move up
                newPosition.y += Speed * delta;
            }

            if (Input.IsKeyDown(Key.A))
            {
                // Move left
                newPosition.x -= Speed * delta;
            }

            if (Input.IsKeyDown(Key.S))
            {
                // Move down
                newPosition.y -= Speed * delta;
            }

            if (Input.IsKeyDown(Key.D))
            {
                // Move right
                newPosition.x += Speed * delta;
            }

            Transform.Position = newPosition;
        }
    }
}
