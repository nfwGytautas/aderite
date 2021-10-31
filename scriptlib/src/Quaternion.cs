using System.Runtime.InteropServices;

namespace Aderite
{
    /// <summary>
    /// Quaternion struct used to specify rotations
    /// </summary>
    [StructLayout(LayoutKind.Sequential)]
    public struct Quaternion
    {
        public float w;
        public float x;
        public float y;
        public float z;
    }
}
