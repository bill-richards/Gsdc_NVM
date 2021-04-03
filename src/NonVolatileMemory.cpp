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
    _dataArray.clear();
    Serial.println();
    Serial.println("Done erasing");
}

void NonVolatileMemory::write(String data, DataMarkers marker)
{
    if(exists(marker))
        _dataArray[(char)marker].setData(data);
    else
    {
        _dataArray[(char)marker] = MemoryDatum(marker, data, _dataArray.size());
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
    Serial.println();
    Serial.println("Flushing the data :");
    orderDataByIndex();
    int address = 0;
    std::vector<MemoryDatum> dataArray = orderDataByIndex();
    for(auto& datum : dataArray)
    {
        int length = datum.length();
        String toWrite = datum.value();
        String memory_packet = datum.value() + _separatorChar;
        
        for(int idx = 0; idx < length; idx++, address++)
        {
            char current_char = toWrite[idx];
            Serial.print(current_char);
            writeToEEPROM(address, current_char);
        }
        writeToEEPROM(address, _separatorChar);
        address += 1;
    }

    writeToEEPROM(address, _terminator);
    removeAnyExistingTerminator(address+1);
    Serial.println();
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
    if(!exists(marker)) return; 
    _dataArray.erase(marker);
}

void NonVolatileMemory::clearAll() { _dataArray.clear(); }

bool NonVolatileMemory::exists(DataMarkers marker) { return _dataArray.find((char)marker) != _dataArray.end(); }

String NonVolatileMemory::read(DataMarkers marker)
{
    return exists(marker) ? _dataArray[(char)marker].raw() : "";
}

std::vector<MemoryDatum> NonVolatileMemory::readAll() { return orderDataByIndex(); }

std::vector<MemoryDatum> NonVolatileMemory::orderDataByIndex()
{
    std::map<int, MemoryDatum> index_ordered;
    for(auto& datum : _dataArray)
        index_ordered[datum.second.index()] = datum.second;

    std::vector<MemoryDatum> data_structure;
    for(const auto& datum : index_ordered)
        data_structure.push_back(datum.second);

    return data_structure;
}

void NonVolatileMemory::readEEPROM()
{
    int datum_index = 0;
    String memory_contents = "";
    _dataArray.clear();

    for(int idx = 0; idx < _memorySize; idx++)
    {
        char current_character = readFromEEPROM(idx);
        if(current_character == _terminator) { break; }
        if(current_character == 0) { break; }
        if(current_character == _separatorChar)
        {
            int length = memory_contents.length();
            char marker = (char)memory_contents[length-1];
            Serial.println((String)"Creating Datum for marker: " + (int)marker);
            _dataArray[marker] = MemoryDatum(marker, memory_contents.substring(0, length-1), datum_index);
            memory_contents.clear();
            datum_index += 1;
            continue;   
        }
        memory_contents += current_character;
    }

    orderDataByIndex();
}