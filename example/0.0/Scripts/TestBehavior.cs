using Aderite;

namespace Scripts
{
    public class TestBehavior : ScriptedBehavior
    {
        public bool EnableGravity;

        private Transform Transform;
        private DynamicActor Actor;

        void Init()
        {
            Transform = Entity.GetComponent<Transform>();
            Actor = Entity.GetComponent<DynamicActor>();

            if (Transform == null || Actor == null)
            {
                Log.Error("Failed to initialize TestBehavior");
            }
        }

        void Update(float delta)
        {
            if (Input.IsKeyDown(Key.SPACE))
            {
                Log.Trace("Space pressed");
                Vector3 newPosition = Transform.Position;
                Transform.Position = newPosition;
            }

            if (Actor.HasGravity != EnableGravity)
            {
                Actor.HasGravity = EnableGravity;
            }
        }

        void OnCollisionEnter(Entity trigger)
        {
            Actor.TeleportTo(new Vector3(0.0f, 5.0f, 0.0f));
        }
    }
}
