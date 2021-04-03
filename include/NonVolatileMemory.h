#ifndef NonVolatileMemory_h_
#define NonVolatileMemory_h_

#include <Arduino.h>
#include <Wire.h>
#include <map>

#include "DataMarkers.h"
#include "MemoryDatum.h"

class NonVolatileMemory
{
private:
    const char _separatorChar   = 0x1E; // RS
    const char _terminator      = 0x04; // EOT

    int _memorySize             = 32000;
    bool _transmitting          = false;

    int _i2cAddress;
    std::map<char, MemoryDatum> _dataArray;
    std::vector<std::function<void (int)>> _addressNullifiedEventHandlers;
    std::vector<std::function<void (int, bool)>> _eepromEraseStartedEventHandlers;

    void endI2CTransmission();
    bool exists(DataMarkers marker);
    char readFromEEPROM(int address);
    void removeAnyExistingTerminator(int statingAddress);

    std::vector<MemoryDatum> orderDataByIndex();

    void startI2CTransmission(int address);
    void writeToEEPROM(int address, char val);

    void raiseAddressNullifiedEvent(int address)
    {
        for(const auto handler : _addressNullifiedEventHandlers)
            handler(address);
    }
    void raiseEepromEraseStartedEvent(int number_of_addresses, bool ignoreNUL)
    {
        for(const auto handler : _eepromEraseStartedEventHandlers)
            handler(number_of_addresses, ignoreNUL);
    }

public:
    NonVolatileMemory(int i2c_address);
    NonVolatileMemory(int i2c_address, int size_in_kb);

    void clear(DataMarkers marker);
    void clearAll();
    void eraseEEPROMData(bool ignoreNUL);
    void flush();
    String read(DataMarkers marker);
    std::vector<MemoryDatum> readAll();
    void readEEPROM();
    void write(String value, DataMarkers marker);
    void write(int data, DataMarkers marker);
    void write(long data, DataMarkers marker);
    void write(float data, DataMarkers marker);
    void write(bool data, DataMarkers marker);
    void write(char data, DataMarkers marker);

    void addAddressNullifiedEventHandler(std::function<void (int address)> func)
    {
        _addressNullifiedEventHandlers.push_back(func);
    }
    void addEepromEraseStartedEventHandler(std::function<void (int memory_size, bool ignore_NUL)> func)
    {
        _eepromEraseStartedEventHandlers.push_back(func);
    }
};
#endif

