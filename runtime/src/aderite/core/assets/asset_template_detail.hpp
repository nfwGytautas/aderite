#pragma once

#include <string>
#include <vector>

#include "aderite/utility/log.hpp"
#include "aderite/core/rendering/fbo/fbo.hpp"

namespace aderite {
	namespace asset {
		using asset_group_set = size_t;

		enum class asset_type : size_t
		{
			FBO,
			UNKNOWN,
		};

		enum class asset_group : size_t
		{
			SYSTEMIC = 1 << 0,
		};

		namespace detail {

			/**
			 * @brief Base asset meta-info
			*/
			struct asset_info {
				asset_type type = asset_type::UNKNOWN;
				asset_group_set group = (asset_group_set)(0);

				asset_info() {}
				asset_info(asset_type aType) : type(aType) {}
				asset_info(asset_type aType, asset_group_set aGroup) : type(aType), group(aGroup) {}
			};

			/**
			 * @brief Returns the asset info for the specified type
			 * @tparam T Type of the asset
			 * @return Asset meta info struct
			*/
			template<typename T>
			asset_info get_asset_info() {
				LOG_WARN("Unknown get_asset_info type {0}", typeid(T).name());

				// Default
				return asset_info();
			}

			template<>
			inline asset_info get_asset_info<aderite::fbo>() {
				return asset_info(asset_type::FBO, (asset_group_set)(asset_group::SYSTEMIC));
			}
		}
	}
}
