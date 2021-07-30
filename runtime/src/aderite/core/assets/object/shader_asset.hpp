#pragma once

#include "aderite/core/assets/asset.hpp"
#include "aderite/core/rendering/shader/shader.hpp"

namespace aderite {
	namespace asset {

		/**
		 * @brief Shader asset implementation
		*/
		class shader_asset : public asset_base {
		public:
			struct fields {
				std::string VertexPath = "";
				std::string FragmentPath = "";
			};
		public:
			~shader_asset();

			virtual asset_type type() const override;
			virtual bool serialize(const std::string& path) override;
			virtual bool deserialize(const std::string& path) override;
			virtual void prepare_load() override;
			virtual bool ready_to_load() override;
			virtual void load() override;
			virtual void unload() override;
			virtual bool in_group(asset_group group) const override;
			
			/**
			 * Returns the underlying asset object
			 */
			shader* operator->() const {
				return m_shader;
			}
		protected:
			shader_asset(asset_handle handle, const fields& info);

			friend class asset_manager;

		private:
			shader* m_shader = nullptr;
			fields m_info = {};

			std::string m_vertexSource = "";
			std::string m_fragmentSource = "";
		};

	}
}
