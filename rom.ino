#include "rom.h"

void initROM() {
  EEPROM.begin(EEPROM_SIZE);
  int countReboot = EEPROM.read(0); // Get current reboot count
  EEPROM.write(0, countReboot + 1); // Update reboot count
  EEPROM.commit();                  // Save to ROM
}

int getROMData(int index) {
  return EEPROM.read(index);
}
