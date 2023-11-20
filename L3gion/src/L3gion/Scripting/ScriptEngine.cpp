#include "lgpch.h"
#include "ScriptEngine.h"

#include "ScriptGlue.h"
#include "L3gion/Core/UUID.h"
#include "L3gion/Core/Application.h"
#include "L3gion/Core/Buffer.h"
#include "L3gion/Utils/PlatformUtils.h"
#include "L3gion/Project/Project.h"

#include <FileWatch.h>
#include "mono/jit/jit.h"
#include "mono/metadata/assembly.h"
#include "mono/metadata/object.h"
#include "mono/metadata/tabledefs.h"
#include "mono/metadata/threads.h"
#include "mono/metadata/mono-debug.h"

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
				
		static MonoAssembly* loadMonoAssembly(const std::filesystem::path& assemblyPath, bool loadPDB = false)
		{
			ScopedBuffer fileData = FileSystem::readFileBinary(assemblyPath);

			// NOTE: We can't use this image for anything other than loading the assembly because this image doesn't have a reference to the assembly
			MonoImageOpenStatus status;
			MonoImage* image = mono_image_open_from_data_full(fileData.as<char>(), (uint32_t)fileData.size(), 1, &status, 0);

			if (status != MONO_IMAGE_OK)
			{
				const char* errorMessage = mono_image_strerror(status);
				// Log some error message using the errorMessage data
				return nullptr;
			}

			if (loadPDB)
			{
				std::filesystem::path pdbPath = assemblyPath;
				pdbPath.replace_extension(".pdb");

				if (std::filesystem::exists(pdbPath))
				{
					ScopedBuffer pdbFileData = FileSystem::readFileBinary(pdbPath);

					mono_debug_open_image_from_memory(image, (const mono_byte*)pdbFileData.data(), (int)pdbFileData.size());

					LG_CORE_INFO("Loaded PDB {}", pdbPath.string());
				}
			}

			std::string pathStr = assemblyPath.string();
			MonoAssembly* assembly = mono_assembly_load_from_full(image, pathStr.c_str(), &status, 0);
			mono_image_close(image);

			return assembly;
		}
	
		static ScriptFieldType monoTypeToScriptFieldType(MonoType* monoType)
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

		bool enableDebugging = true;

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

		bool status = loadAssembly("resources/scripts/L3gion_ScriptCore.dll");
		if (!status)
		{
			LG_CORE_ERROR("[ScriptEngine]: Init()... Could not load L3gion_ScriptCore assembly");
			return;
		}
		auto path = Project::getRootDir() / Project::getAssetsDir() / Project::getScriptModulePath();
		status = loadAppAssembly(path);
		if (!status)
		{
			LG_CORE_ERROR("[ScriptEngine]: Init()... Could not load app assembly");
			return;
		}
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

		if (s_Data->enableDebugging)
		{
			const char* argv[2] = 
			{
				"--debugger-agent=transport=dt_socket,address=127.0.0.1:2550,server=y,suspend=n,loglevel=3,logfile=MonoDebugger.log",
				"--soft-breakpoints"
			};

			mono_jit_parse_options(2, (char**)argv);
			mono_debug_init(MONO_DEBUG_FORMAT_MONO);
		}

		MonoDomain* rootDomain = mono_jit_init("L3gionJITRuntime");
		LG_CORE_ASSERT(rootDomain, "In initMono(): No MonoDomain!");

		// Store the root domain pointer
		s_Data->rootDomain = rootDomain;

		mono_thread_set_main(mono_thread_current());
	}
	void ScriptEngine::shutdownMono()
	{
		mono_domain_set(mono_get_root_domain(), false);
		mono_domain_unload(s_Data->appDomain);
		s_Data->appDomain = nullptr;

		mono_jit_cleanup(s_Data->rootDomain);
		s_Data->rootDomain = nullptr;
	}

	bool ScriptEngine::loadAssembly(const std::filesystem::path filepath)
	{
		// Create an App Domain
		char appName[] = "L3gionScriptRuntime";
		s_Data->appDomain = mono_domain_create_appdomain(appName, nullptr);
		mono_domain_set(s_Data->appDomain, true);

		s_Data->CoreAssemblyFilepath = filepath;
		s_Data->coreAssembly = Utils::loadMonoAssembly(filepath, s_Data->enableDebugging);
		if (s_Data->coreAssembly == nullptr)
			return false;
		s_Data->coreAssemblyImage = mono_assembly_get_image(s_Data->coreAssembly);

		return true;
	}
	bool ScriptEngine::loadAppAssembly(const std::filesystem::path filepath)
	{
		// Maybe not here!
		s_Data->AppAssemblyFilepath = filepath;
		s_Data->appAssembly = Utils::loadMonoAssembly(filepath, s_Data->enableDebugging);
		if (s_Data->appAssembly == nullptr)
			return false;
		s_Data->appAssemblyImage = mono_assembly_get_image(s_Data->appAssembly);

		s_Data->appAssemblyFileWatcher = createScope<filewatch::FileWatch<std::string>>(filepath.string(), onAppAssemblyFileSystemEvent);
		s_Data->assemblyReloadPending = false;

		return true;
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

		if (s_Data->entityInstances.find(entityUUID) != s_Data->entityInstances.end())
		{
			auto& instance = s_Data->entityInstances[entityUUID];
			instance->invokeOnUpdate((float)ts);
		}
		else
		LG_CORE_ERROR("Could not find ScriptInstance for entity {}", entityUUID);

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
	ref<ScriptClass> ScriptEngine::getEntityClass(const std::string& name)
	{
		if (s_Data->entityClasses.find(name) == s_Data->entityClasses.end())
			return nullptr;

		return s_Data->entityClasses.at(name);
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
		MonoObject* exeption = nullptr;
		return mono_runtime_invoke(method, instance, params, &exeption);
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