#pragma once
#ifndef ROM_H
#define ROM_H

#include "definitions.h"
#include <EEPROM.h>

void initROM();
uint32_t getROMData(size_t index);

#endif // ROM_H
