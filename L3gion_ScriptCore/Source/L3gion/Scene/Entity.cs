using System;

namespace L3gion
{
    public class Entity
    {
        protected Entity() { ID = 0; } 
        public Entity(ulong id)
        {
            ID = id;
        }

        public readonly ulong ID;

        public ulong CreateEntity()
        {
            return InternalCalls.Entity_CreateEntity();
        }
        public void DuplicateEntity(ulong id)
        {
            InternalCalls.Entity_DuplicateEntity(id);
        }
        public bool HasComponent<T>() where T : Component, new()
        {
            Type componentType = typeof(T);
            return InternalCalls.Entity_HasComponent(ID, componentType);
        }
        public T GetComponent<T>() where T : Component, new()
        {
            if (!HasComponent<T>())
                return null;
            
            // TODO: Add caching for this
            T component = new T() { Entity = this };
            
            return component;
        }
    }
}
