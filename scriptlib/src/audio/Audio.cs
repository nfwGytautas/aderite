using System;

namespace Aderite
{
    /// <summary>
    /// Aderite Audio class used to spawn instances of audio files
    /// </summary>
    public class Audio
    {
        IntPtr Instance = IntPtr.Zero;

        internal Audio(IntPtr instance)
        {
            Instance = instance;
        }

        /// <summary>
        /// Returns the C++ instance of the audio
        /// </summary>
        /// <returns>IntPtr where the C++ object is located at</returns>
        internal IntPtr GetInstance()
        {
            return Instance;
        }
    }
}
