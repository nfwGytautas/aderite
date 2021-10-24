#pragma once

/**
 * @brief This file contains all versions of the serialization library.
 * These versions are used to define the structure of the serialized files.
 * The serialization system will try to upgrade older version files to newer ones.
*/

namespace aderite {
namespace io {

/**
 * @brief 2021_10_15r1 version.
 * Initial release of the serialization system.
*/
constexpr const char* c_Version_2021_10_15r1 = "2021_10_15r1";

/**
 * @brief Current version of the serialization system
*/
constexpr const char* c_CurrentVersion = c_Version_2021_10_15r1;

}
}
