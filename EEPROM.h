#include <EEPROM.h>

bool isCharValid(char* c) {
  for(int i = 0; i < sizeof(c) - 1; i++) {
    if ( c[i] != '.' && !isalnum(c[i]) )
      return false;
  }
  return true;
} 

void writeEEPROM(int StartAddress, int Length, char* StringToWrite) {
  for (int i = 0; i < Length; i++)
    EEPROM.write(StartAddress + i, StringToWrite[i]);

  EEPROM.commit();
}

char * readEEPROM(int StartAddress, int MaxLength) {
  char * Buffer = (char *) malloc (MaxLength);
  for (int i = 0; i < MaxLength; i++)
    Buffer[i] = char(EEPROM.read(StartAddress + i));

  return Buffer;
}
