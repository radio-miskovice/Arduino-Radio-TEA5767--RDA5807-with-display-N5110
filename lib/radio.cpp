
#include "Radio.h"
#include <Wire.h>

Radio::Radio() {
  addr = 0x60 ;
}

Radio::Radio( unsigned char i2cAddr ) {
  addr = i2cAddr ;
}

void Radio::readData() {
  unsigned char c, i ;
  unsigned int w = 0;
  memset( this->data, 0, 5 );
  Wire.requestFrom( addr, 5u );
  i = 0 ;
  while( Wire.available() ) {
    c = Wire.read();
    if( i<4 ) w = (w<<8) + c; 
    this->data[i++] = c ;
  }
  lastStatus = w ;
  pll = (data[0] & 0x3F) + data[1] ;
  afcDelta = 100 * (data[2] & 0x7F) ;
  freq = pll / 8192 + ( isHighSide!=0 ? -1 : 1 ) * 225000 + afcDelta ;  
  strength = data[3] >> 4 ; 
  rxStatus = (data[0] & 0xC0) | (data[2] & 0x80)>>4 ;
}

bool Radio::isReady()  { return (rxStatus & ISREADY) != 0 ; }
bool Radio::isOutOfBand() { return (rxStatus & ISOFFBAND) != 0 ; }
bool Radio::isStereo() { return (rxStatus & ISSTEREO) != 0 ; }
bool Radio::isMuted()  { return (rxConfig1 & MUTED) != 0 ; }

void Radio::setStereo( bool onOff ) {
  if( onOff ) rxConfig2 = rxConfig2 & (~FORCEMONO) ;
  else rxConfig2 = rxConfig2 | FORCEMONO ;
}
void Radio::setMute( bool onOff ) {
  if (onOff)
    rxConfig1 = rxConfig1 | MUTED;
  else
    rxConfig1 = rxConfig1 & (~MUTED);
}
void Radio::setSearch( int direction ) {
  rxConfig1 = rxConfig1 | SEARCH ;
  if( direction > 0 ) rxConfig2 = rxConfig2 | SEARCH_UP ;
  else
    rxConfig2 = rxConfig2 & ~SEARCH_UP;
}

void Radio::setFrequency( unsigned long frequency, bool highSideFlag ) {
  freq = frequency * 1000 ;
  if( highSideFlag ) {
    frequency += 225000;
    rxConfig2 = rxConfig2 | HIGHSIDE ;
  }
  else {
    frequency -= 225000;
    rxConfig2 = rxConfig2 & ~HIGHSIDE ;
  }
  pll = frequency / 8192;
}

unsigned int Radio::getPllDivider() { return pll ; }
unsigned long Radio::getFrequency() { return ( freq + 500 ) / 1000 ; }
unsigned char Radio::getStrength() { return strength ; }
unsigned long Radio::getLastStatus() { return lastStatus; }

void Radio::sendData() {
  Wire.beginTransmission( addr );
  Wire.write ((rxConfig1 & 0xc0) | (pll >> 8)) ;
  Wire.write ( pll & 0xFF );
  Wire.write ( rxConfig2 ) ;
  Wire.write ( rxConfig3 );
  Wire.write (0) ;
  Wire.endTransmission();
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

