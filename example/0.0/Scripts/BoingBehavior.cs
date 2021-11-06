using Aderite;

namespace Scripts
{
    class BoingBehavior : ScriptedBehavior
    {
        public Audio Clip;
        public AudioSource Source;

        private Transform Transform;

        void Init()
        {
            Transform = Entity.GetComponent<Transform>();
        }

        void Update(float delta)
        {
            if (Source == null)
            {
                return;
            }

            Source.Position = Transform.Position;
            Source.Rotation = Transform.Rotation;
        }

        void OnCollisionEnter(Entity collision)
        {
            if (Source == null)
            {
                Log.Warn("Source null");
            }
            if (Clip == null)
            {
                Log.Warn("Clip null");
            }

            Source.OneShot(Clip);
        }
    }
}
