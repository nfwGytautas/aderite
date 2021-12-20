using Aderite;

namespace Scripts
{
    class Despawner
    {
        void OnCollisionEnter(Geometry g)
        {
            Log.Warn("Geometry enter collision");
        }
    }
}
