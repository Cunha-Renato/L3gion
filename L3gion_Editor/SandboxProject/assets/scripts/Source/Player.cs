using System;
using L3gion;
using static System.Runtime.CompilerServices.RuntimeHelpers;

namespace Sandbox
{
    public class Player : Entity
    {
        private TransformComponent m_Transform;
        private RigidBody2DComponent m_Rigidbody2D;

        public float speed = 3.0f;
        public float time = 0.0f;
        void OnCreate()
        {
            Console.WriteLine($"Player created! - {speed}");
            
            m_Transform = GetComponent<TransformComponent>();
            m_Rigidbody2D = GetComponent<RigidBody2DComponent>();
        }

        void OnUpdate(float ts)
        {
            time += ts;

            Vec3 velocity = Vec3.Zero;

            if (Input.IsKeyDown(LgKey.W))
                velocity.y = 1.0f;
            else if (Input.IsKeyDown(LgKey.S))
                velocity.y = -1.0f;
            if (Input.IsKeyDown(LgKey.A))
                velocity.x = -1.0f;
            else if (Input.IsKeyDown(LgKey.D))
                velocity.x = 1.0f;
            
            Entity cameraEntity = FindEntityByName("Camera");
            if (cameraEntity != null)
            {
                Camera camera = cameraEntity.As<Camera>();

                if (Input.IsKeyDown(LgKey.Q))
                    camera.DistanceFromPlayer += speed * 2.0f * ts;
                else if (Input.IsKeyDown(LgKey.E))
                    camera.DistanceFromPlayer -= speed * 2.0f * ts;
            }

            velocity *= speed * ts;

            m_Rigidbody2D?.ApplyLinearImpulse(velocity.xy);

            Vec3 translation = m_Transform.Translation;
            translation += velocity * ts;
            m_Transform.Translation = translation;
        }
    }
}
