#pragma once

#include <bgfx/bgfx.h>
#include "aderite/interfaces/irenderable.hpp"
#include "aderite/core/assets/asset.hpp"

namespace aderite {
	namespace asset {
		class mesh_source;

		/**
		 * @brief Mesh asset implementation, it just wraps around base mesh object, to provide interface with asset manager
		*/
		class mesh_asset : public asset_base, public interfaces::irenderable {
		public:
			/**
			 * @brief Editable fields of the asset, this information is stored inside the asset file
			*/
			struct fields {
				std::string SourceFile = "";
				bool IsStatic = false;
				bool HasPosition = true;
				bool HasIndices = true;
			};
		public:
			~mesh_asset();

			virtual asset_type type() const override;
			virtual bool in_group(asset_group group) const override;

			virtual void prepare_load() override;
			virtual bool ready_to_load() override;
			virtual void load() override;
			virtual void unload() override;
			virtual bool is_preparing() override;
			virtual bool is_loaded() override;

			/**
			 * @brief Returns the info of shader fields
			*/
			fields get_fields() const {
				return m_info;
			}

			/**
			 * @brief Returns mutable field structure
			*/
			fields& get_fields_mutable() {
				return m_info;
			}

			// Inherited via irenderable
			virtual void fill_draw_call(rendering::draw_call* dc) override;
		protected:
			mesh_asset(const std::string& name);
			mesh_asset(const std::string& name, const fields& info);

			virtual bool serialize(YAML::Emitter& out) override;
			virtual bool deserialize(YAML::Node& data) override;

			friend class asset_manager;
		private:
			// BGFX resource handles
			bgfx::VertexBufferHandle m_vbh = BGFX_INVALID_HANDLE;
			bgfx::IndexBufferHandle m_ibh = BGFX_INVALID_HANDLE;

			fields m_info = {};
			asset::mesh_source* m_source = nullptr;

			bool m_being_prepared = false;
		};

	}
}
