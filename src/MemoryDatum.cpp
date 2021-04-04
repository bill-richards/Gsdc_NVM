#include "MemoryDatum.h"
MemoryDatum::MemoryDatum(){}

MemoryDatum::MemoryDatum(DataMarkers marker, String value, int index) 
{ 
    this->_index = index;
    this->_data = value;
    this->setDataMarker(marker);
}

/*MemoryDatum::MemoryDatum(char marker, String value, int index) 
{ 
    this->_index = index;
    this->_markerChar = marker; 
    this->_data = value;
    this->_marker = (DataMarkers)marker;
}*/

void MemoryDatum::setData(String value) { _data = value; }
void MemoryDatum::setDataMarker(DataMarkers marker) 
{ 
    _markerChar = (char)marker; 
    _marker = marker; 
}

String MemoryDatum::raw() { return _data; }
String MemoryDatum::value() { return _data + _markerChar; }
char MemoryDatum::markerChar() { return _markerChar; }
DataMarkers MemoryDatum::marker() { return _marker; }
int MemoryDatum::rawLength() { return raw().length(); }
int MemoryDatum::length() { return value().length(); }
