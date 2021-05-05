#include "rom.h"

void initROM() {
  EEPROM.begin(EEPROM_SIZE);
  uint32_t countReboot = EEPROM.read(0); // Get current reboot count
  EEPROM.write(0, countReboot + 1); // Update reboot count
  EEPROM.commit();                  // Save to ROM
}

uint32_t getROMData(size_t index) {
  return EEPROM.read(index);
}
