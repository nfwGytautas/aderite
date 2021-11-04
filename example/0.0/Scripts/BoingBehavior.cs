using Aderite;

namespace Scripts
{
    class BoingBehavior : ScriptedBehavior
    {
        public Audio Clip;

        private AudioSource Source;

        void Init()
        {
            Source = this.Entity.GetComponent<AudioSource>();
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
