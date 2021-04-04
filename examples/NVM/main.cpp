#include <Arduino.h>
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
  Serial.println(" 1-3 - Write some sample data to memory");
  Serial.println("     1 - Store a Message");
  Serial.println("     2 - Store a reversed message");
  Serial.println("     3 - Store the fibonacci sequence");
  Serial.println(" 4-6 - Read specific data from memory");
  Serial.println("     4 - Read message");
  Serial.println("     5 - Read reversed message");
  Serial.println("     6 - Read the fibonacci sequence");
  Serial.println(" 7-9 - Erase specific data from memory");
  Serial.println("     7 - Erase the message");
  Serial.println("     8 - Erase the reversed message");
  Serial.println("     9 - Erase the fibonacci sequence");
  Serial.println("");
}

void showMarkers()
{
  Serial.println((String)"DataMarkers::ETX = " + DataMarkers::ETX);
  Serial.println((String)"DataMarkers::DLE = " + DataMarkers::DLE);
  Serial.println((String)"DataMarkers::FS = " + DataMarkers::FS);
}

void displayMemoryContents() 
{ 
  MemoryDataStructure::MemoryDataIterator the_data = _memory.readAll();
  Serial.print("Memory contents : ");
  while(the_data.isValid())
  {
    auto datum = the_data.current_data();
    Serial.print((String)" " + datum->raw());
    the_data.next();
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

String captureSerialInput()
{
  while(Serial.available() < 1) { delay(100); }
  return Serial.readString();
}

String _receivedMessage;

bool getMessage()
{
  Serial.println("Enter the message you want to store or q|quit to cancel");
  Serial.print("Your Message: ");

  String message = "";
  while(message != "q" && message != "quit" && message.length() == 0)
  {
    message = captureSerialInput();
    message.trim();
  }

  if(message == "q" || message == "quit") { Serial.println(); Serial.println("EXIT"); return false; }
  Serial.println(message);

  String response = "";
  while(response != "y" && response != "yes" && response != "n" &&  response != "no" && response != "q" && response != "quit") 
  {
    Serial.println("Is the message correct?");
    response = captureSerialInput();
  }
  if(response == "q" || response == "quit") { Serial.println("EXIT"); return false; }
  if(response == "n" ||  response == "no") { return getMessage(); }
  
  _receivedMessage = message;
  return true;
}

void reverseTheMessage()
{
  String reversed = "";
  for(int idx = _receivedMessage.length()-1; idx >= 0 ; idx--)
  {
    reversed += _receivedMessage[idx];
  }

  _receivedMessage = reversed;
}

void writeMessage(bool reverse = false) 
{
  if(!getMessage()) { return; }
  if(reverse) { reverseTheMessage(); }

  Serial.println("Writing " + _receivedMessage + " to memory");
  _memory.write(_receivedMessage, reverse ? DataMarkers::DLE : DataMarkers::ETX);   
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
  if(command == '1') writeMessage();
  if(command == '2') writeMessage(true);
  if(command == '3') writeFibonacciSequence();
}

void readFromMemory(char command)
{
  String stored_value;
  if(command == '4') stored_value = _memory.read(DataMarkers::ETX);
  if(command == '5') stored_value = _memory.read(DataMarkers::DLE);
  if(command == '6') stored_value = _memory.read(DataMarkers::FS);
  
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

    if((_serialCommand >= '1') && (_serialCommand <= '3') ){ writeToMemory(_serialCommand); }
    if((_serialCommand >= '4') && (_serialCommand <= '7') ){ readFromMemory(_serialCommand); }
    if(_serialCommand == '7'){ Serial.println("Erasing the message"); _memory.clear(DataMarkers::ETX); }
    if(_serialCommand == '8'){ Serial.println("Erasing the reversed message"); _memory.clear(DataMarkers::DLE); }
    if(_serialCommand == '9'){ Serial.println("Erasing the fibonacci sequence"); _memory.clear(DataMarkers::FS); }
    if(_serialCommand == 'c'){ Serial.println("Erasing the in memory data structure"); _memory.clearAll(); }
    if(_serialCommand == 'e'){ _memory.eraseEEPROMData(true); }
    if(_serialCommand == 'f'){ _memory.flush(); }
    if(_serialCommand == 'm'){ displayMemoryContents(); }
    if(_serialCommand == 'q'){ _memory.eraseEEPROMData(false); }
    if(_serialCommand == 'r'){ showMarkers(); _memory.readEEPROM(); }
    if(_serialCommand == 'i'){ showInstructions(); }
  }
  delay(100);
}