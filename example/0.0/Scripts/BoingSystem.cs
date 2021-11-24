using Aderite;

namespace Scripts
{
    class BoingSystem : ScriptSystem
    {
        public AudioSource Source;
        public Audio Boing;

        void Init()
        {

        }

        void Shutdown()
        {

        }

        void Update(float delta, Entity[] entities)
        {

        }

        void OnCollisionStart(CollisionEvent collision)
        {
            if (Source == null || Boing == null)
            {
                Log.Error("Source or Boing is null");
                return;
            }

            Source.OneShot(Boing);

            if (collision.Actor2.Name == "Ground")
            {
                Destroy(collision.Actor1);
            }
            else if (collision.Actor1.Name == "Ground")
            {
                Destroy(collision.Actor2);
            }
        }
    }
}
