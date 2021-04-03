#ifndef _DataMarkers_h_
#define _DataMarkers_h_

typedef enum 
{ 
    ETX = 0x03,
    ENQ = 0x05,
    BEL = 0x07,
    VT = 0x0B,
    FF,
    SO = 0x0E,
    SI,
    DLE,
    DC1,
    DC2,
    DC3,
    DC4,
    FS = 0x1C,
    GS,
    US = 0x1F
} DataMarkers;


#endif