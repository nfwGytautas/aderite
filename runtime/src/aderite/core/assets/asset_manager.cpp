#include "asset_manager.hpp"

namespace aderite {
	namespace asset {

		bool asset_manager::init() {
			return true;
		}

		void asset_manager::shutdown() {
			m_assets.clear();
		}

		asset_handle asset_manager::get_handle() {
			return m_next_handle++;
		}

	}
}
