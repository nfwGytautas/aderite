using Aderite;

namespace Scripts
{
    class CameraLook : ScriptedBehavior
    {
        public float Sensitivity = 2.0f;

        Transform transform = null;

        void Initialize()
        {
            transform = GameObject.GetTransform();
        }

        void Update(float delta)
        {
            Vector2 mouseDelta = Input.GetMouseDelta();

            // Convert from aderite to C# standard
            Quaternion aderiteQuat = transform.Rotation;
            System.Numerics.Quaternion oldRotation = new System.Numerics.Quaternion(aderiteQuat.x, aderiteQuat.y, aderiteQuat.z, aderiteQuat.w);

            // Calculate rotation deltas
            float YawRotation = mouseDelta.x * Sensitivity * delta * -1;
            float PitchRotation = mouseDelta.y * Sensitivity * delta;

            // Rotate
            System.Numerics.Quaternion YawRotationQuaternion = System.Numerics.Quaternion.CreateFromAxisAngle(System.Numerics.Vector3.UnitY, YawRotation);
            System.Numerics.Quaternion PitchRotationQuaternion = System.Numerics.Quaternion.CreateFromAxisAngle(System.Numerics.Vector3.UnitX, PitchRotation);

            // Create new result
            System.Numerics.Quaternion result = YawRotationQuaternion * oldRotation * PitchRotationQuaternion;

            // Convert back to aderite and assign to transform
            Quaternion newTransform = new Quaternion(result.W, result.X, result.Y, result.Z);
            transform.Rotation = newTransform;
        }
    }
}
