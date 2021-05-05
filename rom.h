#ifndef ROM_H
#define ROM_H

#include "definitions.h"
#include <EEPROM.h>

void initROM();
int getROMData(int index);

#endif // ROM_H
