
#include "Radio.h"
#include <Wire.h>
typedef unsigned char uint8_t ;

Radio::Radio()
{
  addr = 0x60 ;
}

Radio::Radio( unsigned char i2cAddr ) {
  addr = i2cAddr ;
}

void Radio::readData() {
  unsigned char c, i ;
  unsigned int w = 0;
  memset( this->data, 0, 5 );
  Wire.requestFrom( addr, 5 );
  i = 0 ;
  while( Wire.available() ) {
    c = Wire.read();
    if( i<4 ) w = (w<<8) + c; 
    this->data[i++] = c ;
  }
  lastStatus = w ;
  pll = ((data[0] & 0x3F) << 8) + data[1] ;
  afcDelta = 100 * (data[2] & 0x7F) ;
  freq = pll * 8192UL - 225000 + afcDelta ;  
  strength = data[3] >> 4 ; 
  rxStatus = (data[0] & 0xC0) | (data[2] & 0x80)>>4 ;
}

bool Radio::isReady()  { return (rxStatus & ISREADY) != 0 ; }
bool Radio::isOutOfBand() { return (rxStatus & ISOFFBAND) != 0 ; }
bool Radio::isStereo() { return (rxStatus & ISSTEREO) != 0 ; }
bool Radio::isMuted()  { return (rxConfig[0] & MUTED) != 0 ; }

void Radio::setStereo( bool onOff ) {
  if( onOff ) rxConfig[1] = rxConfig[1] & (~FORCEMONO) ;
  else rxConfig[1] = rxConfig[1] | FORCEMONO ;
}

void Radio::setMute( bool onOff ) {
  if (onOff)
    rxConfig[0] = rxConfig[0] | MUTED;
  else
    rxConfig[0] = rxConfig[0] & (~MUTED);
}

void Radio::setSearch( int direction ) {
  rxConfig[0] = rxConfig[0] | SEARCH ;
  if( direction > 0 ) rxConfig[1] = rxConfig[1] | SEARCH_UP ;
  else
    rxConfig[1] = rxConfig[1] & ~SEARCH_UP;
}

void Radio::setFrequency( unsigned long frequency, bool highSideFlag ) {
  freq = frequency * 1000 ;
  pll = (freq + 225000UL) / 8192 ;
  rxConfig[1] |= HIGHSIDE ;
}

void Radio::setPllDivider(unsigned int n) {
  pll = n ;
}

unsigned int Radio::getPllDivider() { return pll; }
unsigned long Radio::getFrequency() { return (( freq + 500L ) / 1000) ; }
unsigned char Radio::getStrength() { return strength ; }
unsigned long Radio::getLastStatus() { return lastStatus; }

void Radio::sendData() {
  Wire.beginTransmission( addr );
  Wire.write ((rxConfig[0] & 0xc0) | ((pll >> 8) & 0x3F)) ;
  Wire.write ( pll & 0xFF );
  Wire.write ( rxConfig[1] | HIGHSIDE) ;
  Wire.write ( rxConfig[2] | 0x10 );
  Wire.write (0) ;
  Wire.endTransmission();
  rxConfig[0] &= ~SEARCH ;
}

void Radio::sendData( unsigned long extData ) {
  unsigned long mask = 0xFF000000 ;
  unsigned int  bitShift = 24 ;
  Wire.beginTransmission(addr);
  while( mask > 0 ) {
    Wire.write((extData & mask) >> bitShift );
    mask = mask >> 8 ;
    bitShift -= 8 ;
  }
  Wire.write(0);
  Wire.endTransmission();
}

