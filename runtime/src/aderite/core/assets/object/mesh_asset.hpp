#pragma once

#include "aderite/core/assets/asset.hpp"
#include "aderite/core/rendering/vao/vao_layout.hpp"

namespace aderite {
	class vao;

	namespace asset {
		class mesh_source;

		/**
		 * @brief Mesh asset implementation
		*/
		class mesh_asset : public asset_base {
		public:
			/**
			 * @brief Editable fields of the asset, this information is stored inside the asset file
			*/
			struct fields {
				std::string SourceFile = "";
				vao_layout Layout;
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

			vao* object() {
				return m_vao;
			}

			/**
			 * Returns the underlying asset object
			 */
			vao* operator->() const {
				return m_vao;
			}
		protected:
			mesh_asset(const std::string& name);
			mesh_asset(const std::string& name, const fields& info);

			virtual bool serialize(YAML::Emitter& out) override;
			virtual bool deserialize(YAML::Node& data) override;

			friend class asset_manager;
		private:
			vao* m_vao = nullptr;
			fields m_info = {};
			asset::mesh_source* m_source = nullptr;

			bool m_being_prepared = false;
		};

	}
}
