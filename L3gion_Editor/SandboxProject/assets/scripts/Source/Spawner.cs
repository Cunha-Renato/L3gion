using System;
using L3gion;

namespace Sandbox
{
    public class Spawner : Entity
    {
        private TransformComponent m_Transform;
        private RigidBody2DComponent m_Rigidbody2D;

        public float testVar;
        void OnCreate()
        {
            Console.WriteLine($"Created - {ID}");
            m_Transform = GetComponent<TransformComponent>();
            m_Rigidbody2D = GetComponent<RigidBody2DComponent>();
        }

        static bool m_Pressed = false;
        void OnUpdate(float ts)
        {
            if (Input.IsKeyDown(LgKey.SPACE) && !m_Pressed)
            {
                DuplicateEntity(ID);

                m_Pressed = true;
            }
            else if (!Input.IsKeyDown(LgKey.SPACE))
                m_Pressed = false;       
        }
    }
}