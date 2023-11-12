#pragma once

#include "L3gion/Scene/Scene.h"
#include "L3gion/Scene/Entity.h"

#include <string>
#include <filesystem>

extern "C" {
	typedef struct _MonoClass MonoClass;
	typedef struct _MonoObject MonoObject;
	typedef struct _MonoMethod MonoMethod;
	typedef struct _MonoAssembly MonoAssembly;
}


namespace L3gion
{
	class ScriptClass
	{
	public:
		ScriptClass() = default;
		ScriptClass(const std::string& classNamespace, const std::string& className);

		MonoObject* instantiate();
		MonoMethod* getMethod(const std::string& name, int parameterCount);
		MonoObject* invokeMethod(MonoObject* instance, MonoMethod* method, void** params = nullptr);

	private:
		std::string m_ClassNamespace;
		std::string m_ClassName;
		MonoClass* m_MonoClass = nullptr;
	};

	class ScriptInstance
	{
	public:
		ScriptInstance(ref<ScriptClass> scriptClass, Entity entity);

		void invokeOnCreate();
		void invokeOnUpdate(float ts);
	private:
		ref<ScriptClass> m_ScriptClass;

		MonoObject* m_Instance = nullptr;
		MonoMethod* m_Constructor = nullptr;
		MonoMethod* m_OnCreateMethod = nullptr;
		MonoMethod* m_OnUpdateMethod = nullptr;
	};
	
	class ScriptEngine
	{
	public:
		static void init();
		static void shutdown();

		static void loadAssembly(const std::filesystem::path filepath);
		
		static void onRuntimeStart(Scene* scene);
		static void onRuntimeStop();

		static bool entityClassExists(const std::string& fullName);
		static void onCreateEntity(Entity entity);
		static void onUpdateEntity(Entity entity, Timestep ts);

		static Scene* getSceneContext();

		static std::unordered_map<std::string, ref<ScriptClass>> getEntityClasses();

	private:
		static void initMono();
		static void shutdownMono();
		
		static MonoObject* instantiateClass(MonoClass* monoClass);
		static void loadAssemblyClasses(MonoAssembly* assembly);

		friend class ScriptClass;
	};
}