#pragma once

#include <bgfx/bgfx.h>
#include "aderite/interfaces/irenderable.hpp"
#include "aderite/core/assets/asset.hpp"

namespace aderite {
	namespace asset {

		/**
		 * @brief Shader asset implementation
		*/
		class shader_asset : public asset_base, public interfaces::irenderable {
		public:
			/**
			 * @brief Editable fields of the asset, this information is stored inside the asset file
			*/
			struct fields {
				std::string VertexPath = "";
				std::string FragmentPath = "";
			};
		public:
			~shader_asset();

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
			shader_asset(const std::string& name);
			shader_asset(const std::string& name, const fields& info);

			virtual bool serialize(YAML::Emitter& out) override;
			virtual bool deserialize(YAML::Node& data) override;

			friend class asset_manager;
		private:
			// Handles
			bgfx::ProgramHandle m_handle = BGFX_INVALID_HANDLE;

			fields m_info = {};

			std::vector<unsigned char> m_vertexSource;
			std::vector<unsigned char> m_fragmentSource;

			bool m_being_prepared = false;
		};

	}
}
