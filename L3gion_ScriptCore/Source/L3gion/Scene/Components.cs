namespace L3gion
{
    public abstract class Component
    {
        public Entity Entity { get; internal set; }
    }

    public class TransformComponent : Component
    {
        public Vec3 Translation
        { 
            get
            {
                InternalCalls.TransformComponent_GetTranslation (Entity.ID, out Vec3 translation);
                return translation;
            }
            set
            {
                InternalCalls.TransformComponent_SetTranslation(Entity.ID, ref value);
            }
        }
    }
    public class RigidBody2DComponent : Component
    {
        public bool Dynamic
        {
            get
            {
                return InternalCalls.RigidBody2DComponent_IsDynamic(Entity.ID);
            }
            set
            {
                InternalCalls.RigidBody2DComponent_SetDynamic(Entity.ID, value);
            }
        }
        public bool FixedRotation
        {
            get
            {
                return InternalCalls.RigidBody2DComponent_HasFixedRotation(Entity.ID);
            }
            set
            {
                InternalCalls.RigidBody2DComponent_SetFixedRotation(Entity.ID, value); 
            }
        }
        public void SetTransform(Vec2 worldPosition, float angle)
        {
            InternalCalls.RigidBody2DComponent_SetTransform(Entity.ID, ref worldPosition, angle);
        }

        public void ApplyLinearImpulse(Vec2 impulse, Vec2 worldPoint, bool wake = true)
        {
            InternalCalls.RigidBody2DComponent_ApplyLinearImpulse(Entity.ID, ref impulse, ref worldPoint, wake);
        }
        public void ApplyLinearImpulse(Vec2 impulse, bool wake = true)
        {
            InternalCalls.RigidBody2DComponent_ApplyLinearImpulseToCenter(Entity.ID, ref impulse, wake);
        }
    }
}
