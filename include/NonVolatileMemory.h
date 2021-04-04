#ifndef NonVolatileMemory_h_
#define NonVolatileMemory_h_

#include <Arduino.h>
#include <Wire.h>

#include "DataMarkers.h"
#include "MemoryDataStructure.h"


class NonVolatileMemory
{
private:
    const char _separatorChar   = 0x1E; // RS
    const char _terminator      = 0x04; // EOT

    int _memorySize             = 32000;
    bool _transmitting          = false;

    int _i2cAddress;
    MemoryDataStructure _dataStructure;

    void (*_addressNullifiedEventHandler)(int) = { };
    void (*_eepromEraseStartedEventHandler)(int, bool) = { };

    void endI2CTransmission();
    bool exists(DataMarkers marker);
    char readFromEEPROM(int address);
    void removeAnyExistingTerminator(int statingAddress);

    void startI2CTransmission(int address);
    void writeToEEPROM(int address, char val);

    void raiseAddressNullifiedEvent(int address)
    {
        _addressNullifiedEventHandler(address);
    }
    void raiseEepromEraseStartedEvent(int number_of_addresses, bool ignoreNUL)
    {
        _eepromEraseStartedEventHandler(number_of_addresses, ignoreNUL);
    }

public:
    NonVolatileMemory(int i2c_address);
    NonVolatileMemory(int i2c_address, int size_in_kb);

    void clear(DataMarkers marker);
    void clearAll();
    void eraseEEPROMData(bool ignoreNUL);
    void flush();
    String read(DataMarkers marker);
    MemoryDataStructure::MemoryDataIterator readAll();
    void readEEPROM();
    void write(String value, DataMarkers marker);
    void write(int data, DataMarkers marker);
    void write(long data, DataMarkers marker);
    void write(float data, DataMarkers marker);
    void write(bool data, DataMarkers marker);
    void write(char data, DataMarkers marker);

    void addAddressNullifiedEventHandler(void (*func)(int)) { _addressNullifiedEventHandler = func; }
    void addEepromEraseStartedEventHandler(void(*func)(int, bool)) { _eepromEraseStartedEventHandler = func; }
};

#endif

