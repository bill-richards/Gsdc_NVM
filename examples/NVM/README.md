# Gsdc Non-Volatile Memory Library Example Scratch

* Upload the example _Sketch_ to your microcontroller
* Connect a _**24C256**_ (or an equivalent) external EEPROM chip to your microcontroller's _I<sup>2</sup>C Bus_ (`SDA & SLC`)
* If you are using a different _I<sup>2</sup>C external EEPROM_ make sure that you modify the _Sketch_ to use the alternate constructor for`NonVolatileMemory` and remember to pass in the size of the EEPROM expressed in **kilobytes**
* Connect to the serial port and send the following commands

 | | EEPROM tasks
---:|:---
**i**|_Show these **instructions**_
**r**|_**Re-load** data from EEPROM_
**c**|_**Clear** the (local) memory_
**e**|_**Erase** all data from EEPROM **i.e.** set every address to`0x00 (NUL)`_
**q**|_**Quick erase** from EEPROM **i.e.** only untill the first`0x00 (NUL)`character is encountered_
**m**|_Display all (local) **memory** contents_
**f**|_**Flush** the data to the EEPROM_

_1-3_|  Write some data to memory
---:|---
**1** | _Store a Message_
**2** | _Store a message in reverse_
**3** | _Store the fibonacci sequence_

_4-6_ | Read specific data from memory
---:|---
**4** | _Read message_
**5** | _Read reversed message_
**6** | _Read the fibonacci sequence_

_7-9_ | Erase data from memory
---:|---
**7** |_Erase the message_
**8** |_Erase the reversed message_
**9** |_Erase the fibonacci sequence_

If you do not call flush (using option _**f**_) the data will not be persisted to the EEPROM.

Once you have written data to the EEPROM you can power down your controller and when you subsequently power it up again, you can read the data from the EEPROM (using option _**m**_)

Because each EEPROM is limited to approximately 1 million write operations, an explicit call should therefore be made to write the data structure to the EEPROM.

Although a number of writes are used in committing the entire structure (_**NB:** one write operation for each byte_), that structure will only be persisted when explicitly instructed rather than after every change to the structure and/or its content.

For this reason it is important to remember that this library operates on an _**in memory data structure**_
