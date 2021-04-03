#include <Arduino.h>
#include <chrono>
#include <ctime>
#include "NonVolatileMemory.h"

#define I2C_ADDRESS 0x50

NonVolatileMemory _memory(I2C_ADDRESS);
char _serialCommand;

void showInstructions() 
{
  Serial.println();
  Serial.println(" Send commands to the serial port:");
  Serial.println(" ---------------------------------");
  Serial.println();
  Serial.println(" i   - Show instructions");
  Serial.println(" r   - Re-load data from EEPROM");
  Serial.println(" c   - Clear the (local) data structure");
  Serial.println(" e   - Erase all data from EEPROM (NUL every address)");
  Serial.println(" q   - Quick erase from EEPROM");
  Serial.println(" m   - Display all (local) memory contents");
  Serial.println(" f   - Flush the data to the EEPROM");
  Serial.println(" 1-4 - Write some sample data to memory");
  Serial.println("     1 - Store current time");
  Serial.println("     2 - Store a Message");
  Serial.println("     3 - Store a reversed message");
  Serial.println("     4 - Store the fibonacci sequence");
  Serial.println(" 5-8 - Read specific data from memory");
  Serial.println("     5 - Read stored time");
  Serial.println("     6 - Read message");
  Serial.println("     7 - Read reversed message");
  Serial.println("     8 - Read the fibonacci sequence");
  Serial.println(" 9-0 - Erase specific data from memory");
  Serial.println("     9 - Erase the time");
  Serial.println("     0 - Erase the fibonacci sequence");
  Serial.println("");
}

void displayMemoryContents() 
{ 
  std::vector<MemoryDatum> the_data = _memory.readAll();
  Serial.print("Memory contents : ");
  for(auto datum : the_data)
  {
    Serial.print((String)" " + datum.raw());
  }
  Serial.println();
}

void writeFibonacciSequence()
{  
  Serial.print("Calculating fibonacci 0,1");

  int left_hand_side = 0;
  int right_hand_side = 1;
  int current = 0;

  String data = "0,1";
  while(current < 4096)
  {
    current = left_hand_side + right_hand_side;
    Serial.print((String)"," + current);
    data += ((String)"," + current);
    left_hand_side = right_hand_side;
    right_hand_side = current;
  };

  Serial.println();
  _memory.write(data, DataMarkers::FS);  
}

void writeTime()
{
  auto time_mark = std::chrono::system_clock::now();
  std::time_t time = std::chrono::system_clock::to_time_t(time_mark);  
  String time_string = String(std::ctime(&time));
  time_string.remove(time_string.length()-1);
  Serial.println("Writing the time to memory :" + time_string);
  _memory.write(time_string, DataMarkers::BEL);
  
}

void writeMessage(bool reverse = false) 
{   
  String message = reverse ? "!dlroW olleH" : "Hello world!";
  Serial.println("Writing " + message + " to memory");
  _memory.write(message, reverse ? DataMarkers::DLE : DataMarkers::ETX);   
}

bool _quickErase = true;
int _erasableMemory = 0;

void addressNullifiedEventHandler(int address) 
{ 
  if(_quickErase)
    Serial.print(".");
}

void eepromEraseStartedEventHandler(int memory_size, bool quick_erase) 
{ 
  _quickErase = quick_erase;
  _erasableMemory = memory_size;
  if(quick_erase) 
  {
    Serial.println("Performing a quick erase of EEPROM data");
    return;
  }
  Serial.println("Performing a deep erase of EEPROM data.");
  Serial.println("Please be patient, this can take upwards of five minutes. You will be notified when the operation has completed");
}

void writeToMemory(char command)
{
  if(command == '1') writeTime();
  if(command == '2') writeMessage();
  if(command == '3') writeMessage(true);
  if(command == '4') writeFibonacciSequence();
}

void readFromMemory(char command)
{
  String stored_value;
  if(command == '5') stored_value = _memory.read(DataMarkers::BEL);
  if(command == '6') stored_value = _memory.read(DataMarkers::ETX);
  if(command == '7') stored_value = _memory.read(DataMarkers::DLE);
  if(command == '8') stored_value = _memory.read(DataMarkers::FS);
  
  Serial.println(stored_value);  
}

void setup() {
  Wire.begin();
  _memory.readEEPROM();  
  _memory.addAddressNullifiedEventHandler(&addressNullifiedEventHandler);
  _memory.addEepromEraseStartedEventHandler(&eepromEraseStartedEventHandler);
  Serial.begin(115200);
  Serial.println();
  showInstructions();
}

void loop() 
{
  if(Serial.available() > 0)
  {
    _serialCommand = Serial.read();

    if((_serialCommand >= '1') && (_serialCommand <= '4') ){ writeToMemory(_serialCommand); }
    if((_serialCommand >= '5') && (_serialCommand <= '8') ){ readFromMemory(_serialCommand); }
    if(_serialCommand == '9'){ _memory.clear(DataMarkers::BEL); }
    if(_serialCommand == '0'){ _memory.clear(DataMarkers::FS); }
    if(_serialCommand == 'c'){ _memory.clearAll(); }
    if(_serialCommand == 'e'){ _memory.eraseEEPROMData(true); }
    if(_serialCommand == 'f'){ _memory.flush(); }
    if(_serialCommand == 'm'){ displayMemoryContents(); }
    if(_serialCommand == 'q'){ _memory.eraseEEPROMData(false); }
    if(_serialCommand == 'r'){ _memory.readEEPROM(); }
    if(_serialCommand == 'i'){ showInstructions(); }
  }
  delay(100);
}