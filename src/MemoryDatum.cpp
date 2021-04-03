#include "MemoryDatum.h"
MemoryDatum::MemoryDatum(){}

MemoryDatum::MemoryDatum(DataMarkers marker, String value, int index) 
{ 
    _index = index;
    _marker = (char)marker; 
    _data = value;
}
MemoryDatum::MemoryDatum(char marker, String value, int index) 
{ 
    _index = index;
    _marker = marker; 
    _data = value;
}

void MemoryDatum::setData(String value) { _data = value; }
void MemoryDatum::setDataMarker(DataMarkers marker) { _marker = (char)marker; }

String MemoryDatum::raw() { return _data; }
String MemoryDatum::value() { return _data + _marker; }
char MemoryDatum::markerChar() { return _marker; }
int MemoryDatum::rawLength() { return raw().length(); }
int MemoryDatum::length() { return value().length(); }
