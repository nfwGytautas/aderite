using Aderite;

namespace Scripts
{
    class RayCaster : ScriptSystem
    {
        public Vector3 From = new Vector3(0.0f, 2.0f, 0.0f);
        public float Distance;

        void Update(float delta, Entity[] entities)
        {
            if (Input.WasKeyReleased(Key.DOWN))
            {
                Vector3 direction = new Vector3(0.0f, -1.0f, 0.0f);
                RaycastHit hit = Physics.RaycastFirstHit(From, direction, Distance);
                if (hit != null)
                {
                    Log.Trace($"There was a hit on {hit.Object.Name} going downwards, ray traveled for {hit.Distance}");
                }
            } else if (Input.WasKeyReleased(Key.UP))
            {
                Vector3 direction = new Vector3(0.0f, 1.0f, 0.0f);
                RaycastHit hit = Physics.RaycastFirstHit(From, direction, Distance);
                if (hit != null)
                {
                    Log.Trace($"There was a hit on {hit.Object.Name} going upwards, ray traveled for {hit.Distance}");
                }
            }
        }
    }
}
