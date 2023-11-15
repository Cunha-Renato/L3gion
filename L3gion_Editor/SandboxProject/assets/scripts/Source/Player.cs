using System;
using L3gion;

namespace Sandbox
{
    public class Player : Entity
    {
        private TransformComponent m_Transform;
        private RigidBody2DComponent m_Rigidbody2D;

        public float speed = 3.0f;
        void OnCreate()
        {
            Console.WriteLine($"Player created! - {ID}");
            
            m_Transform = GetComponent<TransformComponent>();
            m_Rigidbody2D = GetComponent<RigidBody2DComponent>();
        }

        void OnUpdate(float ts)
        {
            //Console.WriteLine($"Player update: {ts}");
            Vec3 velocity = Vec3.Zero;

            if (Input.IsKeyDown(LgKey.W))
                velocity.y = 1.0f;
            else if (Input.IsKeyDown(LgKey.S))
                velocity.y = -1.0f;
            if (Input.IsKeyDown(LgKey.A))
                velocity.x = -1.0f;
            else if (Input.IsKeyDown(LgKey.D))
                velocity.x = 1.0f;

            velocity *= speed * ts;

            m_Rigidbody2D?.ApplyLinearImpulse(velocity.xy);

            Vec3 translation = m_Transform.Translation;
            translation += velocity * ts;
            m_Transform.Translation = translation;
        }
    }
}
