using Aderite;

namespace Scripts
{
    public class TestBehavior : ScriptedBehavior
    {
        public float TestValue;

        private Transform Transform;

        void Init()
        {
            Transform = Entity.GetComponent<Transform>();
        }

        void Update(float delta)
        {
            Log.Trace($"From behavior update, test value {TestValue}, delta {delta}");

            Vector3 scale = Transform.Scale;
            Log.Trace($"Old scale x: {scale.x} y: {scale.y} z: {scale.z}");

            scale = new Vector3(TestValue);
            Log.Trace($"New scale x: {scale.x} y: {scale.y} z: {scale.z}");

            Transform.Scale = scale;
        }
    }
}
