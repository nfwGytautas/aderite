using Aderite;

namespace Scripts
{
    class Spawner : ScriptSystem
    {
        public Prefab Prefab;

        void Update(float delta, Entity[] entities)
        {
            if (Input.WasKeyReleased(Key.SPACE))
            {
                Instantiate(Prefab, new Vector3(0, 10, 0));
            }
        }
    }
}
