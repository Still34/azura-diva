#pragma once
#include <Psapi.h>
#include <stdlib.h>

const MODULEINFO& getModuleInfo();

void* sigScan(const char* signature, const char* mask);
