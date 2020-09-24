/**
 * @file nsd.h
 * @brief (nsd) service IPC wrapper.
 * @author HookedBehemoth
 * @copyright libnx Authors
 */
#pragma once
#include "../types.h"
#include "../sf/service.h"

Result nsdInitialize(void);
void nsdExit(void);

Service* nsdGetServiceSession(void);

Result nsdGetEnvionmentIdentifier(char *out, size_t out_size);
