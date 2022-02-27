using System;
using System.Runtime.CompilerServices;

namespace Aderite
{
    /// <summary>
    /// Object used to manipulate game objects in the scene
    /// </summary>
    public class GameObject
    {
        // The C++ instance of this game object
        IntPtr Instance = IntPtr.Zero;

        internal GameObject(IntPtr instance)
        {
            Instance = instance;
        }

        /// <summary>
        /// The name of the game object
        /// </summary>
        public string Name { get { return __GetName(Instance); } }

        /// <summary>
        /// Get the transform component of this game object can return null if the component doesn't exist
        /// </summary>
        /// <returns>Transform instance or null</returns>
        public Transform GetTransform()
        {
            return __GetTransform(Instance);
        }

        /// <summary>
        /// Get the renderable component of this game object can return null if the component doesn't exist
        /// </summary>
        /// <returns>Renderable instance or null</returns>
        public Renderable GetRenderable()
        {
            return __GetRenderable(Instance);
        }

        /// <summary>
        /// Get the rigidbody component of this game object can return null if the component doesn't exist or the game object has a static rigidbody
        /// </summary>
        /// <returns>Rigidbody instance or null</returns>
        public Rigidbody GetRigidbody()
        {
            return __GetRigidbody(Instance);
        }

        /// <summary>
        /// Returns the collider instance of the game object
        /// </summary>
        /// <param name="name">Name of the collider</param>
        /// <returns>Collider instance or null if doesn't have a collider or a collider with the name</returns>
        public Collider GetCollider(string name)
        {
            return __GetCollider(Instance, name);
        }

        /// <summary>
        /// Get the camera component of this game object can return null if the component doesn't exist
        /// </summary>
        /// <returns>Camera instance or null</returns>
        public Camera GetCamera()
        {
            return __GetCamera(Instance);
        }

        /// <summary>
        /// Get the audio source component of this game object can return null if the component doesn't exist
        /// </summary>
        /// <returns>AudioSource instance or null</returns>
        public AudioSource GetAudioSource()
        {
            return __GetAudioSource(Instance);
        }

        /// <summary>
        /// Get the audio listener component of this game object can return null if the component doesn't exist
        /// </summary>
        /// <returns>AudioListener instance or null</returns>
        public AudioListener GetAudioListener()
        {
            return __GetAudioListener(Instance);
        }

        /// <summary>
        /// Destroy this game object, accessing this object after calling this object is illegal and might cause a segmentation fault
        /// </summary>
        public void Destroy()
        {
            __Destroy(Instance);
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern static string __GetName(IntPtr instance);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern static Transform __GetTransform(IntPtr instance);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern static Renderable __GetRenderable(IntPtr instance);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern static Rigidbody __GetRigidbody(IntPtr instance);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern static Collider __GetCollider(IntPtr instance, string name);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern static Camera __GetCamera(IntPtr instance);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern static AudioSource __GetAudioSource(IntPtr instance);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern static AudioListener __GetAudioListener(IntPtr instance);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern static void __Destroy(IntPtr instance);
    }
}
