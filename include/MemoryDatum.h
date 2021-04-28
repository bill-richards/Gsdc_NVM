#ifndef MemoryDatum_h_
#define MemoryDatum_h_

#include <Arduino.h>
#include "DataMarkers.h"

class MemoryDatum
{
private:
    char _markerChar = 0;
    DataMarkers _marker;
    String _data = "";
    int _index = -1;
public:
    MemoryDatum();
    //MemoryDatum(char marker, String value, int index);
    MemoryDatum(DataMarkers marker, String value, int index);

    int length();           // Returns the length of the value terminated with the marker
    DataMarkers marker();
    char markerChar();
    String raw();           // Returns JUST the value
    int rawLength();        // Returns the length of JUST the value
    String value();         // Returns the value terminated with the marker
    int index() { return _index; }

    void setData(String value);
    void setDataMarker(DataMarkers marker);
};

#endif