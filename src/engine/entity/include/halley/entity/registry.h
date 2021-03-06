#pragma once

#include <memory>

namespace Halley {
    class System;
    class String;
    class ConfigNode;
    class CreateComponentFunctionResult;
	class ComponentReflector;
	class EntityFactoryContext;

	std::unique_ptr<System> createSystem(String name);
	CreateComponentFunctionResult createComponent(const EntityFactoryContext& context, const String& name, EntityRef& entity, const ConfigNode& componentData);
	ComponentReflector& getComponentReflector(int componentId);
}
