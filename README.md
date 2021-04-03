# Gsdc Non-Volatile Memory API

## How it works

Because of the limited number of write operations each EEPROM has, this API mostly operates in memory. Making a call to`readFromEEPROM()`loads the entire contents of the EEPROM into an _in memory data structure_ which the remainder of the API calls operate on.

Also, when physically writing data to the EEPROM, each byte is only overwritten if the existing value differs from the value being written.

The end of the data structure (when stored in the EEPROM) is marked by the`0x04 (EOT)`character. 

When the structure being written to the chip is smaller than that which is currently stored there, those addresses beyond the new structure's terminator retain their values, with the exception of the original termination marker, which is itself overwritten with`0x00 (NUL)`

These precautions and optimisations will not be of much help of course if your data are constantly changing size; or if you should chose to erase some data and subsequently write more data back using the same , the underlying structurre will have changed and you will possibly initiate a high number of write operations. It's best to overwrite your data in place if you can keep them to the same length (remember that all data are stored as strings)

### Important Note

All data are returned as a `String` you will need to perform any required conversions in code.

## The API

| API Objects ||
-- | --
| `DataMarkers` | _Enum used to identify each datum_
| `NonVolatileMemory` | _The API object_
| `MemoryDatum` | _Represents each item in the data structure, indexed by a`DataMarkers`value_

### `NonVolatileMemory`

constructors | |
----|--
`NonVolatileMemory(int i2c_address);`|Register the external EEPROM's I<sup>2</sup>C address. Using a default EEPROM size of _**256Kb**_
`NonVolatileMemory(int i2c_address, int size_in_kb);`|Register the external EEPROM's I<sup>2</sup>C address. Using the specified EEPROM size

event handlers||
--|--
`void addEepromEraseStartedEventHandler(std::function<void (int, bool)> func)`|Register an event handler to get notification of when EEPROM erasing is taking place
`void addAddressNullifiedEventHandler(std::function<void (int)> func);`|Register an event handler to receive notification of each EEPROM address having been set to`0x00 (NUL)`

public members||
--|--
`void clear(DataMarkers marker);`|Clears the data marked with the specified`DataMarker` <br>_This call does not involve a round trip to the EEPROM, rather it reads the local data structure_
`void clearAll();`|Clears all data from the storage data structure. <br>_This call does not involve a round trip to the EEPROM, rather it reads the local data structure_
`void eraseEEPROMData(bool ignoreNUL)`|Sets all addresses of the EEPROM to a value of`0x00`When`ignoreNUL`is`false`(the default) overwriting the EEPROM addresses with stop when an existing value of`0x00` is found. Setting this to true will result in the overwrite continuing for every byte of the EEPROM. For the default _256Kb_ this is a possible 32000 write operations.<br>_Remember though: this will not necessarily result in an increase in write operations because any bytes currently set t`0x00`will not be overwritten._
`void flush();`|Writes the entire data structure to the EEPROM
`String read(DataMarkers marker);`|Returns the data marked with the specified`DataMarker` <br>_This call does not involve a round trip to the EEPROM, rather it reads the local data structure_
`std::vector<MemoryDatum> readAll();`|Returns all data assigned to the data structure. <br>_This call does not involve a round trip to the EEPROM, rather it reads the local data structure_
`void readEEPROM();`| _**Clears the local data structure**_, and reads the structure stored in the EEPROM into working memory
`void write(String value, DataMarkers marker);`|Writes data to the storage structure, using the specified`DataMarker`as an index key. <br>_This call does not involve a round trip to the EEPROM, rather it reads the local data structure_
`void write(int data, DataMarkers marker);`|_see above_
`void write(long data, DataMarkers marker);`|_see above_
`void write(float data, DataMarkers marker);`|_see above_
`void write(bool data, DataMarkers marker);`|_see above_
`void write(char data, DataMarkers marker);`|_see above_


| MemoryDatum Members ||
-- | --
`int length();`|Returns the length of the stored data including the termination marker
`char markerChar();`|Returns the`char` being used as the`DataMarkers`index key
`String raw();`|Returns _just_ the stored data
`int rawLength();`|Returns the length of _just_ the stored data
`String value();`|Returns the stored data terminated with the`DataMarkers`index key
`void setData(String value);`|Set the data to be stored in the EEPROM
`void setDataMarker(DataMarkers marker);`|Set the`DataMarkers`index key for this datum
