#include "lgpch.h"
#include "ScriptEngine.h"

#include "ScriptGlue.h"
#include "L3gion/Core/UUID.h"
#include "L3gion/Core/Application.h"

#include <FileWatch.h>
#include "mono/jit/jit.h"
#include "mono/metadata/assembly.h"
#include "mono/metadata/object.h"
#include "mono/metadata/tabledefs.h"

#include <glm/glm.hpp>

namespace L3gion
{
	static std::unordered_map<std::string, ScriptFieldType> s_ScriptFieldTypeMap = 
	{
		{ "System.Single",	ScriptFieldType::Float	},
		{ "System.Double",	ScriptFieldType::Double },
		{ "System.Boolean", ScriptFieldType::Bool	},
		{ "System.Char",	ScriptFieldType::Char	},
		{ "System.Int16",	ScriptFieldType::Short	},
		{ "System.Int32",	ScriptFieldType::Int	},
		{ "System.Int64",	ScriptFieldType::Long	},
		{ "System.Byte",	ScriptFieldType::Byte	},
		{ "System.UInt16",	ScriptFieldType::UShort },
		{ "System.UInt32",	ScriptFieldType::UInt	},
		{ "System.UInt64",	ScriptFieldType::ULong	},

		{ "L3gion.Vec2",	ScriptFieldType::Vec2	},
		{ "L3gion.Vec3",	ScriptFieldType::Vec3	},
		{ "L3gion.Vec4",	ScriptFieldType::Vec4	},

		{ "L3gion.Entity",	ScriptFieldType::Entity },
	};

	namespace Utils
	{
		static void printAssemblyTypes(MonoAssembly* assembly)
		{
			MonoImage* image = mono_assembly_get_image(assembly);
			const MonoTableInfo* typeDefinitionsTable = mono_image_get_table_info(image, MONO_TABLE_TYPEDEF);
			int32_t numTypes = mono_table_info_get_rows(typeDefinitionsTable);

			for (int32_t i = 0; i < numTypes; i++)
			{
				uint32_t cols[MONO_TYPEDEF_SIZE];
				mono_metadata_decode_row(typeDefinitionsTable, i, cols, MONO_TYPEDEF_SIZE);

				const char* nameSpace = mono_metadata_string_heap(image, cols[MONO_TYPEDEF_NAMESPACE]);
				const char* name = mono_metadata_string_heap(image, cols[MONO_TYPEDEF_NAME]);

				LG_CORE_INFO("{0}.{1}\n", nameSpace, name);
			}
		}
		
		static char* readBytes(const std::filesystem::path& filepath, uint32_t* outSize)
		{
			std::ifstream stream(filepath, std::ios::binary | std::ios::ate);

			if (!stream)
			{
				// Failed to open the file
				return nullptr;
			}

			std::streampos end = stream.tellg();
			stream.seekg(0, std::ios::beg);
			uint64_t size = end - stream.tellg();

			if (size == 0)
			{
				// File is empty
				return nullptr;
			}

			char* buffer = new char[size];
			stream.read((char*)buffer, size);
			stream.close();

			*outSize = (uint32_t)size;
			return buffer;
		}
		
		static MonoAssembly* loadMonoAssembly(const std::filesystem::path& assemblyPath)
		{
			uint32_t fileSize = 0;
			char* fileData = readBytes(assemblyPath, &fileSize);

			// NOTE: We can't use this image for anything other than loading the assembly because this image doesn't have a reference to the assembly
			MonoImageOpenStatus status;
			MonoImage* image = mono_image_open_from_data_full(fileData, fileSize, 1, &status, 0);

			if (status != MONO_IMAGE_OK)
			{
				const char* errorMessage = mono_image_strerror(status);
				// Log some error message using the errorMessage data
				return nullptr;
			}

			std::string pathStr = assemblyPath.string();
			MonoAssembly* assembly = mono_assembly_load_from_full(image, pathStr.c_str(), &status, 0);
			mono_image_close(image);

			// Don't forget to free the file data
			delete[] fileData;

			return assembly;
		}
	
		ScriptFieldType monoTypeToScriptFieldType(MonoType* monoType)
		{
			std::string typeName = mono_type_get_name(monoType);

			auto it = s_ScriptFieldTypeMap.find(typeName);

			if (it == s_ScriptFieldTypeMap.end())
			{
				LG_CORE_ERROR("Unknown type: {}", typeName);
				return ScriptFieldType::None;
			}

			return it->second;
		}

		const char* scriptFieldTypeToString(ScriptFieldType type)
		{
			switch (type)
			{
				case ScriptFieldType::Float:	return "Float";
				case ScriptFieldType::Double:	return "Double";
				case ScriptFieldType::Bool:		return "Bool";
				case ScriptFieldType::Char:		return "Char";
				case ScriptFieldType::Byte:		return "Byte";
				case ScriptFieldType::Short:	return "Short";
				case ScriptFieldType::Int:		return "Int";
				case ScriptFieldType::Long:		return "Long";
				case ScriptFieldType::UByte:	return "UByte";
				case ScriptFieldType::UShort:	return "UShort";
				case ScriptFieldType::UInt:		return "UInt";
				case ScriptFieldType::ULong:	return "ULong";
				case ScriptFieldType::Vec2:		return "Vec2";
				case ScriptFieldType::Vec3:		return "Vec3";
				case ScriptFieldType::Vec4:		return "Vec4";
				case ScriptFieldType::Entity:	return "Entity";
			}

			return "<Invalid>";
		}
	}

	struct ScriptEngineData
	{
		MonoDomain* rootDomain = nullptr;
		MonoDomain* appDomain = nullptr;

		MonoAssembly* coreAssembly = nullptr;
		MonoImage* coreAssemblyImage = nullptr;

		MonoAssembly* appAssembly = nullptr;
		MonoImage* appAssemblyImage = nullptr;

		std::filesystem::path CoreAssemblyFilepath;
		std::filesystem::path AppAssemblyFilepath;

		ScriptClass entityClass;

		std::unordered_map<std::string, ref<ScriptClass>> entityClasses;
		std::vector<std::string> entityClassesName;
		std::unordered_map<UUID, ref<ScriptInstance>> entityInstances;

		scope<filewatch::FileWatch<std::string>> appAssemblyFileWatcher;
		bool assemblyReloadPending = false;

		// Runtime
		ref<Scene> sceneContext = nullptr;
	};

	static ScriptEngineData* s_Data = nullptr;

	static void onAppAssemblyFileSystemEvent(const std::string& path, const filewatch::Event change_type)
	{
		if (!s_Data->assemblyReloadPending && change_type == filewatch::Event::modified)
		{
			s_Data->assemblyReloadPending = true;

			Application::get().submitToMainThread([]()
			{
				s_Data->appAssemblyFileWatcher.reset();
				ScriptEngine::reloadAssembly();
			});
		}
	}

	void ScriptEngine::init()
	{
		s_Data = new ScriptEngineData();

		initMono();
		ScriptGlue::registerFunctions();

		loadAssembly("resources/scripts/L3gion_ScriptCore.dll");
		loadAppAssembly("SandboxProject/assets/scripts/Binaries/Sandbox.dll");
		loadAssemblyClasses();

		ScriptGlue::registerComponents();

		s_Data->entityClass = ScriptClass("L3gion", "Entity", true);
	}
	void ScriptEngine::shutdown()
	{
		shutdownMono();
		delete s_Data;
		s_Data = nullptr;
	}

	void ScriptEngine::initMono()
	{
		mono_set_assemblies_path("mono/lib");

		MonoDomain* rootDomain = mono_jit_init("L3gionJITRuntime");
		LG_CORE_ASSERT(rootDomain, "In initMono(): No MonoDomain!");

		// Store the root domain pointer
		s_Data->rootDomain = rootDomain;
	}

	void ScriptEngine::shutdownMono()
	{
		mono_domain_set(mono_get_root_domain(), false);
		mono_domain_unload(s_Data->appDomain);
		s_Data->appDomain = nullptr;

		mono_jit_cleanup(s_Data->rootDomain);
		s_Data->rootDomain = nullptr;
	}

	void ScriptEngine::loadAssembly(const std::filesystem::path filepath)
	{
		// Create an App Domain
		char appName[] = "L3gionScriptRuntime";
		s_Data->appDomain = mono_domain_create_appdomain(appName, nullptr);
		mono_domain_set(s_Data->appDomain, true);

		// Maybe not here!
		s_Data->CoreAssemblyFilepath = filepath;
		s_Data->coreAssembly = Utils::loadMonoAssembly(filepath);
		s_Data->coreAssemblyImage = mono_assembly_get_image(s_Data->coreAssembly);
	}
	void ScriptEngine::loadAppAssembly(const std::filesystem::path filepath)
	{
		// Maybe not here!
		s_Data->AppAssemblyFilepath = filepath;
		s_Data->appAssembly = Utils::loadMonoAssembly(filepath);
		auto a = s_Data->appAssembly;
		s_Data->appAssemblyImage = mono_assembly_get_image(s_Data->appAssembly);

		s_Data->appAssemblyFileWatcher = createScope<filewatch::FileWatch<std::string>>(filepath.string(), onAppAssemblyFileSystemEvent);
		s_Data->assemblyReloadPending = false;
	}
	void ScriptEngine::reloadAssembly()
	{
		mono_domain_set(mono_get_root_domain(), false);

		mono_domain_unload(s_Data->appDomain);

		loadAssembly(s_Data->CoreAssemblyFilepath);
		loadAppAssembly(s_Data->AppAssemblyFilepath);
		loadAssemblyClasses();

		ScriptGlue::registerFunctions();
		ScriptGlue::registerComponents();

		s_Data->entityClass = ScriptClass("L3gion", "Entity", true);

		s_Data->entityInstances.clear();
		s_Data->sceneContext->refreshScripts();
	}
	ref<ScriptInstance> ScriptEngine::getEntityScriptInstance(UUID entityID)
	{
		auto it = s_Data->entityInstances.find(entityID);

		if (it == s_Data->entityInstances.end())
			return nullptr;

		return it->second;
	}

	void ScriptEngine::setContext(ref<Scene> scene)
	{
		s_Data->sceneContext = scene;
	}
	void ScriptEngine::onRuntimeStop()
	{
		s_Data->sceneContext = nullptr;

		//s_Data->entityInstances.clear();
	}

	void ScriptEngine::loadEntity(Entity entity)
	{
		const auto& sc = entity.getComponent<ScriptComponent>();
		if (entityClassExists(sc.name))
		{
			auto it = s_Data->entityInstances.find(entity.getUUID());


			if (it == s_Data->entityInstances.end())
			{
				ref<ScriptInstance> instance = createRef<ScriptInstance>(s_Data->entityClasses[sc.name]);
				s_Data->entityInstances[entity.getUUID()] = instance;
			}
			else
			{
				const auto& scriptClass = it->second->getScriptClass();
				std::string className = fmt::format("{}.{}", scriptClass->m_ClassNamespace, scriptClass->m_ClassName);
				
				if (className != sc.name)
				{
					ref<ScriptInstance> instance = createRef<ScriptInstance>(s_Data->entityClasses[sc.name]);
					s_Data->entityInstances[entity.getUUID()] = instance;
				}
			}
		}
		else
			s_Data->entityInstances.erase(entity.getUUID());
	}
	void ScriptEngine::onCreateEntity(Entity entity)
	{
		const auto& sc = entity.getComponent<ScriptComponent>();
		if (entityClassExists(sc.name))
		{
			auto it = s_Data->entityInstances.find(entity.getUUID());

			LG_CORE_ASSERT(it != s_Data->entityInstances.end(), "Instance not found!");
			
			it->second->invokeConstructor(entity);
			it->second->invokeOnCreate();
		}
	}
	void ScriptEngine::onUpdateEntity(Entity entity, Timestep ts)
	{
		UUID entityUUID = entity.getUUID();

		LG_CORE_ASSERT(s_Data->entityInstances.find(entityUUID) != s_Data->entityInstances.end(), "Instance doesn't exists!");

		auto& instance = s_Data->entityInstances[entityUUID];
		instance->invokeOnUpdate((float)ts);
	}

	ref<Scene> ScriptEngine::getSceneContext()
	{
		return s_Data->sceneContext;
	}

	MonoObject* ScriptEngine::getManagedInstance(UUID entityUUID)
	{
		LG_CORE_ASSERT(s_Data->entityInstances.find(entityUUID) != s_Data->entityInstances.end(), "");

		return s_Data->entityInstances.at(entityUUID)->getManagedObject();
	}

	bool ScriptEngine::entityClassExists(const std::string& fullName)
	{
		return s_Data->entityClasses.find(fullName) != s_Data->entityClasses.end();
	}

	std::unordered_map<std::string, ref<ScriptClass>> ScriptEngine::getEntityClasses()
	{
		return s_Data->entityClasses;
	}

	const std::vector<std::string>& ScriptEngine::getEntityClassesName()
	{
		return s_Data->entityClassesName;
	}

	void ScriptEngine::loadAssemblyClasses()
	{
		s_Data->entityClasses.clear();
		s_Data->entityClassesName.clear();

		const MonoTableInfo* typeDefinitionsTable = mono_image_get_table_info(s_Data->appAssemblyImage, MONO_TABLE_TYPEDEF);
		int32_t numTypes = mono_table_info_get_rows(typeDefinitionsTable);
		MonoClass* entityClass = mono_class_from_name(s_Data->coreAssemblyImage, "L3gion", "Entity");

		s_Data->entityClassesName.reserve(numTypes);
		for (int32_t i = 0; i < numTypes; i++)
		{
			uint32_t cols[MONO_TYPEDEF_SIZE];
			mono_metadata_decode_row(typeDefinitionsTable, i, cols, MONO_TYPEDEF_SIZE);

			const char* nameSpace = mono_metadata_string_heap(s_Data->appAssemblyImage, cols[MONO_TYPEDEF_NAMESPACE]);
			const char* name = mono_metadata_string_heap(s_Data->appAssemblyImage, cols[MONO_TYPEDEF_NAME]);
			
			std::string fullName;
			if (strlen(nameSpace) != 0)	
				fullName = fmt::format("{}.{}", nameSpace, name);
			else
				fullName = name;

			MonoClass* monoClass = mono_class_from_name(s_Data->appAssemblyImage, nameSpace, name);

			if (monoClass == entityClass)
				continue;

			bool isEntity = mono_class_is_subclass_of(monoClass, entityClass, false);

			if (!isEntity)
				continue;
			
			ref<ScriptClass> scriptClass = createRef<ScriptClass>(nameSpace, name);
			s_Data->entityClasses[fullName] = scriptClass;
			s_Data->entityClassesName.push_back(fullName);

			LG_CORE_WARN("Class: {} has: ", name);
			// Geting the field names and types from mono...
			void* iterator = nullptr;
			while (MonoClassField* field = mono_class_get_fields(monoClass, &iterator))
			{
				const char* fieldName = mono_field_get_name(field);
				uint32_t flags = mono_field_get_flags(field);

				if (flags & FIELD_ATTRIBUTE_PUBLIC)
				{
					MonoType* type = mono_field_get_type(field);
					ScriptFieldType fieldType = Utils::monoTypeToScriptFieldType(type);
					LG_CORE_WARN("	{} ({})", fieldName, Utils::scriptFieldTypeToString(fieldType));

					scriptClass->m_Fields[fieldName] = { fieldType, fieldName, field };
				}
			}
		}
	}

	MonoObject* ScriptEngine::instantiateClass(MonoClass* monoClass)
	{
		MonoObject* instance = mono_object_new(s_Data->appDomain, monoClass);
		mono_runtime_object_init(instance);
		return instance;
	}

	MonoImage* ScriptEngine::getCoreAssemblyImage()
	{
		return s_Data->coreAssemblyImage;
	}

// ---------------- ScriptClass ----------------

	ScriptClass::ScriptClass(const std::string& classNamespace, const std::string& className, bool isCore)
		: m_ClassNamespace(classNamespace), m_ClassName(className)
	{
		m_MonoClass = mono_class_from_name(
			isCore ? s_Data->coreAssemblyImage : s_Data->appAssemblyImage, 
			m_ClassNamespace.c_str(), 
			m_ClassName.c_str()
		);
	}

	MonoObject* ScriptClass::instantiate()
	{
		return ScriptEngine::instantiateClass(m_MonoClass);
	}

	MonoMethod* ScriptClass::getMethod(const std::string& name, int parameterCount)
	{
		auto str = name;
		return mono_class_get_method_from_name(m_MonoClass, str.c_str(), parameterCount);
	}

	MonoObject* ScriptClass::invokeMethod(MonoObject* instance, MonoMethod* method, void** params)
	{
		return mono_runtime_invoke(method, instance, params, nullptr);
	}

// ---------------- ScriptInstance ----------------

	ScriptInstance::ScriptInstance(ref<ScriptClass> scriptClass)
		: m_ScriptClass(scriptClass)
	{
		m_Instance = m_ScriptClass->instantiate();
		
		m_Constructor = s_Data->entityClass.getMethod(".ctor", 1);
		m_OnCreateMethod = m_ScriptClass->getMethod("OnCreate", 0);
		m_OnUpdateMethod = m_ScriptClass->getMethod("OnUpdate", 1);
	}

	void ScriptInstance::invokeConstructor(Entity entity)
	{
		uint64_t entityUUID = entity.getUUID();
		void* param = &entityUUID;
		m_ScriptClass->invokeMethod(m_Instance, m_Constructor, &param);
	}
	void ScriptInstance::invokeOnCreate()
	{
		if (m_OnCreateMethod)
			m_ScriptClass->invokeMethod(m_Instance, m_OnCreateMethod);
	}
	void ScriptInstance::invokeOnUpdate(float ts)
	{
		if (m_OnUpdateMethod)
		{
			void* param = &ts;
			m_ScriptClass->invokeMethod(m_Instance, m_OnUpdateMethod, &param);
		}
	}

	bool ScriptInstance::getFieldValueInternal(const std::string& name, bool retainValue)
	{
		const auto& fields = m_ScriptClass->getFields();
		auto it = fields.find(name);

		if (it == fields.end())
			return false;

		const ScriptField& field = it->second;
		
		if (m_FieldValues.find(name) == m_FieldValues.end() && retainValue)
			mono_field_get_value(m_Instance, field.classField, &m_FieldValues[name][0]);
		else if (!retainValue)
			mono_field_get_value(m_Instance, field.classField, &m_Buffer);
			
		return true;
	}
	bool ScriptInstance::setFieldValueInternal(const std::string& name, const void* value)
	{
		const auto& fields = m_ScriptClass->getFields();
		auto it = fields.find(name);

		if (it == fields.end())
			return false;

		const ScriptField& field = it->second;
		mono_field_set_value(m_Instance, field.classField, (void*)value);

		return true;
	}
}