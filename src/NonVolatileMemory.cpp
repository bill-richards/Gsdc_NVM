#include "NonVolatileMemory.h"

NonVolatileMemory::NonVolatileMemory(int i2c_address) { _i2cAddress = i2c_address; }

NonVolatileMemory::NonVolatileMemory(int i2c_address, int size_in_kb)
{ 
    _memorySize = 125 * size_in_kb;
    _i2cAddress = i2c_address;
}

void NonVolatileMemory::eraseEEPROMData(bool ignoreNUL) 
{
    raiseEepromEraseStartedEvent(_memorySize, !ignoreNUL);

    for(int idx = 0; idx < _memorySize; idx++)
    {
        char value = readFromEEPROM(idx);
        writeToEEPROM(idx, 0);
        raiseAddressNullifiedEvent(idx);
        if(!ignoreNUL && value == 0) break;
    }
    writeToEEPROM(0, _terminator);
    _dataStructure.clear();
    Serial.println();
    Serial.println("Done erasing");
}

void NonVolatileMemory::write(String data, DataMarkers marker)
{
    if(exists(marker))
    { 
        Serial.println((String)"Updating marker:"+marker); 
        _dataStructure.findResult()->setData(data);
    }
    else
    {
        Serial.println((String)"Creating new datum for marker:"+marker); 
        _dataStructure.add(MemoryDatum(marker, data, _dataStructure.length()));
    }
}

void NonVolatileMemory::write(int data, DataMarkers marker)
{
    String string_data(data); 
    write(string_data, marker);
}
void NonVolatileMemory::write(long data, DataMarkers marker)
{
    String string_data(data); 
    write(string_data, marker);
}
void NonVolatileMemory::write(float data, DataMarkers marker)
{
    String string_data(data); 
    write(string_data, marker);
}
void NonVolatileMemory::write(bool data, DataMarkers marker)
{
    String string_data(data); 
    write(string_data, marker);
}
void NonVolatileMemory::write(char data, DataMarkers marker)
{
    String string_data(data); 
    write(string_data, marker);
}

void NonVolatileMemory::flush()
{
    Serial.println("Flushing the data :");
    int address = 0;

    MemoryDataStructure::MemoryDataIterator iterator = readAll();

    while(iterator.isValid())
    {
        auto datum = iterator.current_data();
        int length = datum->length();
        String toWrite = datum->value();
        String memory_packet = datum->value() + _separatorChar;
        
        for(int idx = 0; idx < length; idx++, address++)
        {
            char current_char = toWrite[idx];
            writeToEEPROM(address, current_char);
        }
        writeToEEPROM(address, _separatorChar);
        address += 1;
        iterator.next();
    }

    writeToEEPROM(address, _terminator);
    removeAnyExistingTerminator(address+1);
    Serial.println("Done flushing");
}

void NonVolatileMemory::removeAnyExistingTerminator(int statingAddress)
{
    for(int idx = statingAddress; idx < _memorySize; idx++)
    {
        Serial.print(".");
        char current_char = readFromEEPROM(idx);
        if(current_char == 0) return;
        if(current_char == _terminator) 
        { 
            writeToEEPROM(idx, 0);
            return; 
        }
    }
}

void NonVolatileMemory::writeToEEPROM(int address, char value)
{
    char original_value = readFromEEPROM(address);
    if(original_value == value) return;

    startI2CTransmission(address);
    Wire.write(value);
    endI2CTransmission();
    delay(5);   
}

void NonVolatileMemory::startI2CTransmission(int address)
{
    if(_transmitting) return;
    _transmitting = true;

    Wire.beginTransmission(_i2cAddress);
    Wire.write((int)(address >> 8));   // Most significant bit
    Wire.write((int)(address & 0xFF)); // Least significant bit
}

void NonVolatileMemory::endI2CTransmission()
{
    if(!_transmitting) return;
    _transmitting = false;
    Wire.endTransmission();
}

char NonVolatileMemory::readFromEEPROM(int address)
{
    startI2CTransmission(address);
    endI2CTransmission();
    Wire.requestFrom(_i2cAddress, 1);
    int read = Wire.read();
    delay(5);
    return (char)read;
}

void NonVolatileMemory::clear(DataMarkers marker) 
{ 
    if(!exists(marker)) { return; }
    _dataStructure.remove(marker);
}

void NonVolatileMemory::clearAll() { _dataStructure.clear(); }

bool NonVolatileMemory::exists(DataMarkers marker) { return _dataStructure.find(marker); }

String NonVolatileMemory::read(DataMarkers marker)
{
    return exists(marker) 
        ? _dataStructure.findResult()->raw() 
        : "";
}

MemoryDataStructure::MemoryDataIterator NonVolatileMemory::readAll() { return _dataStructure.iterator(); }

void NonVolatileMemory::readEEPROM()
{
    int datum_index = 0;
    String memory_contents = "";
    _dataStructure.clear();

    for(int idx = 0; idx < _memorySize; idx++)
    {
        char current_character = readFromEEPROM(idx);
        if(current_character == _terminator) { break; }
        if(current_character == 0) { break; }
        if(current_character == _separatorChar)
        {
            int length = memory_contents.length();
            char marker = (char)memory_contents[length-1];
            Serial.println((String)"Creating Datum for marker: " + (DataMarkers)marker);

            MemoryDatum newData((DataMarkers)marker, memory_contents.substring(0, length-1), datum_index);
            _dataStructure.add(newData);

            memory_contents.clear();
            datum_index += 1;
            continue;   
        }
        memory_contents += current_character;
    }
}