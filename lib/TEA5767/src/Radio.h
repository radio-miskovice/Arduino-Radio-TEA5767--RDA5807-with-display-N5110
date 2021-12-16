#ifndef _RADIO_H_
#define _RADIO_H_

class Radio
{
public:
  // rxStatus bits
  static const unsigned char ISREADY   = 0x80 ;
  static const unsigned char ISOFFBAND = 0x40 ;
  static const unsigned char ISSTEREO  = 0x08 ;
  // rxConfig1 bits
  static const unsigned char MUTED     = 0x80 ; // mute audio output
  static const unsigned char SEARCH    = 0x40 ; // start search mode 
  // rxConfig2 bits
  static const unsigned char SEARCH_UP = 0x80 ; // set search direction up
  static const unsigned char SSL_LOW   = 0x20 ; // search stop level low (ADC=5)
  static const unsigned char SSL_MED   = 0x40 ; // search stop level medium (ADC=7)
  static const unsigned char SSL_HIGH  = 0x60 ; // search stop level high (ADC=10)
  static const unsigned char HIGHSIDE  = 0x10 ; // if set, FREQ = PLL - IF, otherwise FREQ = PLL + IF
  static const unsigned char FORCEMONO = 0x08 ; // force mono even in stereo reception
  static const unsigned char MUTERIGHT = 0x04 ;
  static const unsigned char MUTELEFT  = 0x02 ;
  static const unsigned char SWP1      = 0x01 ;
  // rxConfig3 bits
  static const unsigned char SWP2 = 0x80 ;      // set PORT2 bit
  static const unsigned char STANDBY  = 0x40 ;  // stand-by mode
  static const unsigned char BAND_JPN = 0x20 ;  // Search only: Japanese FM Band limits (76-95), otherwise 87.5 - 108
  static const unsigned char SOFTMUTE = 0x08 ;  // soft mute on/off (squelch??)
  static const unsigned char HIGHCUT  = 0x04 ;  // high cut control on
  static const unsigned char STNOISE  = 0x02 ;  // stereo noise cancelling
  static const unsigned char SEARCHIND = 0x01 ; // use SWP1 as ready signal

protected:
  unsigned char data[5];
  unsigned char addr ;
  unsigned long lastStatus = 0;
  unsigned char rxStatus = 0;
  unsigned char rxConfig[3] = { 0, 0, 0 };
  unsigned int  pll;
  unsigned int  afcDelta;
  unsigned long freq; // frequency in Hz
  unsigned char strength;
   
public:
  Radio();
  Radio( unsigned char );
  void setStereo( bool onOff );
  void setMute( bool onOff );
  void setFrequency( unsigned long frequency, bool highSideFlag = true );
  void setPllDivider( unsigned int n );
  void setConfig2(unsigned char config2);
  void setConfig3(unsigned char config3);

  // void setPllDirect( unsigned int value);
  void setSearch( int direction = 1 );
  unsigned int  getPllDivider();
  unsigned long getFrequency();
  unsigned char getStrength();
  unsigned long getLastStatus();
  bool isMuted();
  bool isReady();
  bool isOutOfBand();
  bool isStereo();
  void readData();
  void sendData();
  void sendData( unsigned long extData );
};
#endif