using Aderite;

namespace Scripts
{
    public class JumpBehavior : ScriptedBehavior
    {
        public float JumpForce;

        private Transform Transform;

        void Init()
        {
            Transform = Entity.GetComponent<Transform>();
        }

        void Update(float delta)
        {
            if (Input.IsKeyDown(Key.SPACE))
            {
                Log.Trace("Space pressed");
                Vector3 newPosition = Transform.Position;
                newPosition.y += JumpForce * delta;
                Transform.Position = newPosition;
            }
        }
    }
}
