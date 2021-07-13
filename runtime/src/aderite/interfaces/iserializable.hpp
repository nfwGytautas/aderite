#pragma once

#include <string>

namespace aderite {
	namespace interfaces {

		/**
		 * @brief Interface for serializing objects
		*/
		class iserializable {
		public:
			virtual ~iserializable() {}

			/**
			 * @brief Serialize object to the specified path
			 * @param path File to save to
			 * @return True if serialized without errors, false otherwise
			*/
			virtual bool serialize(const std::string& path) = 0;

			/**
			 * @brief Deserialize object from specified path
			 * @param path File to deserialize from
			 * @return True if deserialized without errors, false otherwise
			*/
			virtual bool deserialize(const std::string& path) = 0;
		};

	}
}
