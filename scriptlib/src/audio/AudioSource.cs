using System;
using System.Runtime.CompilerServices;

namespace Aderite
{
    /// <summary>
    /// Audio source object, from which one can play audio clips
    /// </summary>
    public class AudioSource
    {
        private IntPtr Instance = IntPtr.Zero;

        public Vector3 Position { get { return __GetPosition(this.Instance); } set { __SetPosition(this.Instance, value); } }
        public Quaternion Rotation { get { return __GetRotation(this.Instance); } set { __SetRotation(this.Instance, value); } }
        public Vector3 Velocity { get { return __GetVelocity(this.Instance); } set { __SetVelocity(this.Instance, value); } }
        public float Volume { get { return __GetVolume(this.Instance); } set { __SetVolume(this.Instance, value); } }

        internal AudioSource(IntPtr instance)
        {
            this.Instance = instance;
        }

        /// <summary>
        /// Returns the C++ instance of the source
        /// </summary>
        /// <returns>IntPtr where the C++ object is located at</returns>
        internal IntPtr GetInstance()
        {
            return Instance;
        }

        /// <summary>
        /// Mute the audio source
        /// </summary>
        public void Mute()
        {
            __Mute(this.Instance);
        }

        /// <summary>
        /// Unmute the audio source
        /// </summary>
        public void Unmute()
        {
            __Unmute(this.Instance);
        }

        /// <summary>
        /// Stop all audio coming from this source
        /// </summary>
        public void Stop()
        {
            __Stop(this.Instance);
        }

        /// <summary>
        /// Create audio instance from this source
        /// </summary>
        /// <param name="audio">Audio to play</param>
        /// <returns>AudioInstance object</returns>
        public AudioInstance CreateInstance(Audio audio)
        {
            return new AudioInstance(__CreateInstance(this.Instance, audio.GetInstance()));
        }

        /// <summary>
        /// Play one shot audio clip
        /// </summary>
        /// <param name="audio">Audio to play</param>
        public void OneShot(Audio audio)
        {
            __OneShot(this.Instance, audio.GetInstance());
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern static Vector3 __GetPosition(IntPtr source);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern static Quaternion __GetRotation(IntPtr source);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern static Vector3 __GetVelocity(IntPtr source);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern static float __GetVolume(IntPtr source);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern static void __SetPosition(IntPtr source, Vector3 position);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern static void __SetRotation(IntPtr source, Quaternion rotation);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern static void __SetVelocity(IntPtr source, Vector3 velocity);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern static void __SetVolume(IntPtr source, float volume);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern static void __Mute(IntPtr source);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern static void __Unmute(IntPtr source);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern static void __Stop(IntPtr source);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern static IntPtr __CreateInstance(IntPtr source, IntPtr audio);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern static void __OneShot(IntPtr source, IntPtr audio);
    }
}
