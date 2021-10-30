using Aderite;

namespace Scripts
{
    public class JumpBehavior : ScriptedBehavior
    {
        public float JumpForce;
        public Material OnTriggerMaterial;

        private Transform Transform;
        private MeshRenderer MeshRenderer;

        void Init()
        {
            Transform = Entity.GetComponent<Transform>();
            MeshRenderer = Entity.GetComponent<MeshRenderer>();
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

        void OnTriggerEnter(Entity trigger)
        {
            MeshRenderer.Material = OnTriggerMaterial;
        }
    }
}
