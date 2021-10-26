using Aderite;

namespace Scripts
{
    [ScriptedBehavior]
    public class TestBehavior
    {
        public float TestValue;

        void Update(float delta)
        {
            Log.Info($"From behavior update, test value {TestValue}, delta {delta}");
        }
    }
}
