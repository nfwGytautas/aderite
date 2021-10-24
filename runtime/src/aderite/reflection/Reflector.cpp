#include "Reflector.hpp"

#include "aderite/utility/Log.hpp"
#include "aderite/reflection/Reflectable.hpp"
#include "aderite/reflection/Instancer.hpp"

// Assets, needed for linking instancers
#include "aderite/rendering/Pipeline.hpp"

#include "aderite/asset/MeshAsset.hpp"
#include "aderite/asset/MaterialAsset.hpp"
#include "aderite/asset/MaterialTypeAsset.hpp"
#include "aderite/asset/TextureAsset.hpp"
#include "aderite/scene/Scene.hpp"

#include "aderite/physics/ColliderList.hpp"
#include "aderite/physics/Collider.hpp"
#include "aderite/physics/collider/BoxCollider.hpp"

#include "aderite/rendering/operation/CameraProvideOperation.hpp"
#include "aderite/rendering/operation/EntityProvideOperation.hpp"
#include "aderite/rendering/operation/OutputToScreenOperation.hpp"
#include "aderite/rendering/operation/RenderOperation.hpp"
#include "aderite/rendering/operation/TargetProvideOperation.hpp"

namespace aderite {
namespace reflection {

bool Reflector::init() {
	// Runtime instancers
	LOG_TRACE("Setting runtime instancers");

	// Assets
	ADERITE_REFLECTOR_EXPOSE_INSTANCE(this, asset::MeshAsset, RuntimeTypes::MESH);
	ADERITE_REFLECTOR_EXPOSE_INSTANCE(this, asset::MaterialAsset, RuntimeTypes::MATERIAL);
	ADERITE_REFLECTOR_EXPOSE_INSTANCE(this, asset::TextureAsset, RuntimeTypes::TEXTURE);
	ADERITE_REFLECTOR_EXPOSE_INSTANCE(this, scene::Scene, RuntimeTypes::SCENE);
	ADERITE_REFLECTOR_EXPOSE_INSTANCE(this, asset::MaterialTypeAsset, RuntimeTypes::MAT_TYPE);
	ADERITE_REFLECTOR_EXPOSE_INSTANCE(this, rendering::Pipeline, RuntimeTypes::PIPELINE);

	// Colliders
	ADERITE_REFLECTOR_EXPOSE_INSTANCE(this, physics::ColliderList, RuntimeTypes::CLDR_LIST);
	ADERITE_REFLECTOR_EXPOSE_INSTANCE(this, physics::BoxCollider, RuntimeTypes::BOX_CLDR);

	// Operations
	ADERITE_REFLECTOR_EXPOSE_INSTANCE(this, rendering::CameraProvideOperation, RuntimeTypes::OP_CAMERA);
	ADERITE_REFLECTOR_EXPOSE_INSTANCE(this, rendering::EntityProvideOperation, RuntimeTypes::OP_ENTITY);
	ADERITE_REFLECTOR_EXPOSE_INSTANCE(this, rendering::OutputToScreenOperation, RuntimeTypes::OP_SCREEN);
	ADERITE_REFLECTOR_EXPOSE_INSTANCE(this, rendering::RenderOperation, RuntimeTypes::OP_RENDER);
	ADERITE_REFLECTOR_EXPOSE_INSTANCE(this, rendering::TargetProvideOperation, RuntimeTypes::OP_TARGET);

	LOG_DEBUG("Registered {0} runtime instancers", m_instancers.size());
	ADERITE_DEBUG_SECTION(
		this->printInstancers();
	);

	return true;
}

void Reflector::shutdown() {
	for (auto instancer : m_instancers) {
		delete instancer.second;
	}
}

void Reflector::linkInstancer(Type type, InstancerBase* instancer, const std::string& name = "") {
	auto it = m_instancers.find(type);
	if (it != m_instancers.end()) {
		// Already has mapped instancer
		LOG_DEBUG("Instancer for type {0:03d}({1}) has been overridden to ({2})", type, m_names[type], name);

		delete it->second;
		it->second = instancer;

		return;
	}

	m_instancers.insert_or_assign(type, instancer);
	m_names.insert_or_assign(type, name);
}

std::string Reflector::reflectName(Type type) const {
	return m_names.at(type);
}

InstancerBase* Reflector::resolveInstancer(Type type) const {
	auto it = m_instancers.find(type);
	ADERITE_DYNAMIC_ASSERT(it != m_instancers.end(), "Tried to resolve instancer for non registered type {0}", type);
	return it->second;
}

ADERITE_DEBUG_SECTION(
void Reflector::printInstancers() {
	LOG_TRACE("");
	LOG_TRACE("====================================================================================");
	LOG_TRACE("                                    INSTANCERS                                      ");
	LOG_TRACE("====================================================================================");
	for (auto i : m_instancers) {
		if (i.second != nullptr) {
			Reflectable* temp = nullptr;
			temp = i.second->create();
			LOG_TRACE("Type: {0:03d} Instancer: {1:p} Created type: {2:03d} Name: {3}", i.first, static_cast<void*>(i.second), temp->getType(), m_names[temp->getType()]);
			delete temp;
		}
		else {
			LOG_TRACE("Type: {0:03d} Instancer: {1:p} Created type: UNKNOWN", i.first, static_cast<void*>(i.second));
		}
	}
	LOG_TRACE("");
}
)

}
}