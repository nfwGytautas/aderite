#pragma once

#include <vector>
#include <string>
#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>
#include <mono/metadata/debug-helpers.h>
#include "aderite/scripting/Forward.hpp"
#include "aderite/scene/Entity.hpp"

namespace aderite {
namespace scripting {

class ScriptManager final {
public:
	/**
	 * @brief Initializes the serializer
	 * @return True if initialized without error, false otherwise
	*/
	bool init();

	/**
	 * @brief Shutdown serializer
	*/
	void shutdown();

	/**
	 * @brief Updates all scripts
	 * @param delta Delta of the frame
	*/
	void update(float delta);

	/**
	 * @brief Loads the game code assembly
	*/
	void loadAssemblies();

	/**
	 * @brief Returns the current domain
	*/
	MonoDomain* getDomain() const;

	/**
	 * @brief Returns all behaviors that the script manager knows off
	*/
	std::vector<BehaviorWrapper*> getAllBehaviors() const;

	/**
	 * @brief Returns the behavior instance with the specified name
	 * @param name Name of the behavior
	 * @return BehaviorWrapper instance or nullptr if doesn't exist
	*/
	BehaviorWrapper* getBehavior(const std::string& name);

	/**
	 * @brief Creates a C# script Entity object from C++ entity
	 * @param entity Entity from which to create
	 * @return MonoObject instance
	*/
	MonoObject* createEntityObject(scene::Entity entity);

	/**
	 * @brief Returns the ScriptedBehavior.Entity field
	*/
	MonoClassField* getBehaviorEntityField() const;
private:
	/**
	 * @brief Resolves all Behavior classes in the loaded assembly
	*/
	void resolveBehaviors();

	/**
	 * @brief Sets up all engine runtime related assemblies and information
	*/
	bool setupEngineAssemblies();

	/**
	 * @brief Sets up all code related assemblies and information
	*/
	bool setupCodeAssemblies();

	/**
	 * @brief Returns true if the specified class has ScriptedBehavior attribute
	*/
	bool classMarkedAsBehavior(MonoClass* klass);

	/**
	 * @brief Cleans up all data
	*/
	void clean();
private:
	ScriptManager() {}
	friend class Engine;

private:
	MonoDomain* m_jitDomain = nullptr;

	MonoDomain* m_currentDomain = nullptr;

	// Engine assemblies
	MonoAssembly* m_scriptlibAssembly = nullptr;
	MonoImage* m_scriptlibImage = nullptr;

	MonoClass* m_sbClass = nullptr; // ScriptedBehavior
	MonoClassField* m_sbEntityField = nullptr;

	MonoClass* m_entityClass = nullptr; // Entity
	MonoClassField* m_entitySceneField = nullptr;
	MonoClassField* m_entityEntityField = nullptr;
	
	// Code assemblies
	MonoAssembly* m_codeAssembly = nullptr;
	MonoImage* m_codeImage = nullptr;

	std::vector<BehaviorWrapper*> m_behaviors;
};

}
}
