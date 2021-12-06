#pragma once

#include "imnodes.h"

/**
 * @brief This file contains extensions for the imnodes library
 */

namespace IMNODES_NAMESPACE {

void SetLinkUserData(int id, void* data);
void* GetLinkUserData(int id);

void SetNodeUserData(int id, void* data);
void* GetNodeUserData(int id);

void SetPinUserData(int id, void* data);
void* GetPinUserData(int id);

} // namespace IMNODES_NAMESPACE
