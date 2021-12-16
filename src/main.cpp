#include <Arduino.h>
#include <Radio.h>
#include <SPI.h>
#include <Adafruit_PCD8544.h>
#include <Wire.h>

// Initialize PCD8544 using hardware SPI (DC, CE, RST)
Adafruit_PCD8544 display = Adafruit_PCD8544(6, 7, 8);
unsigned char lastButton = 0;
unsigned char lastStrength = 0;
const char BUTTON = D4;
const char BACKLIGHT = D2;

void displayStrength(unsigned char strength);
void recallMemory(unsigned char memoryIndex);
void displayStereo(bool isStereo = true);

Radio radio = Radio();

void setup()
{
  Wire.begin();
  Wire.setClock(100000000UL);
  pinMode(BUTTON, INPUT_PULLUP);
  //pinMode( BACKLIGHT, OUTPUT );
  //digitalWrite(BACKLIGHT, HIGH);
  lastButton = 0;
  display.begin();
  display.setContrast(40);
  display.clearDisplay();
  // displayStereo();
  display.display();
  display.fillRect(0, 0, 84, 10, 1);
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(4, 1);
  display.println("Radio TEA5767");
  display.setTextColor(1);
  display.drawRect(0, 12, 84, 36, 1);
  display.fillRect(3, 15, 78, 30, 1);
  // display.fillCircle(42, 21, 10, 0);
  display.display();
  recallMemory(0);
}

unsigned long frequency[5] = {
    88800,
    89900,
    90900,
    92600,
    104500};

char *label[5] = {
    "CRo Sever",
    "CRo Dvojka",
    "Radiozurnal",
    "CRo PLUS",
    "Vltava"};

char freqIndex = 0;

void recallMemory(unsigned char memoryIndex)
{
  char *rlabel = label[memoryIndex];
  int rn = strlen(rlabel);
  unsigned long f = frequency[memoryIndex];
  radio.setFrequency( f );
  radio.sendData();
  delay(50);
  radio.readData();
  display.fillRect(0, 12, 84, 36, 0);
  display.setTextSize(2);
  f =  radio.getFrequency();
  display.setCursor(6, 13);
  if (f < 100000)
    display.setCursor(18, 13);
  display.print(f * 0.001);
  display.setTextSize(1);
  display.setCursor(44 - rn * 3, 30);
  display.print(rlabel);
  lastStrength = radio.getStrength();
  displayStrength(lastStrength);
  displayStereo(radio.isStereo());
  display.display();
}

void loop()
{
  // put your main code here, to run repeatedly:
  unsigned char button = digitalRead(BUTTON);
  unsigned char s;
  if (button != lastButton)
  {
    if (button == HIGH)
    {
      recallMemory(freqIndex);
      freqIndex = (freqIndex + 1) % 5;
    }
    lastButton = button;
    delay(500);
  }
  else
  { 
    radio.readData();
    s = radio.getStrength();
    if (s != lastStrength)
    {
      displayStrength(s);
      display.display();
      lastStrength = s;
      delay(50);
    }
    else 
      delay(100);
  }
}

void displayStrength(unsigned char strength)
{
  display.fillRect(0, 38, 63, 9, 0);
  strength &= 0x0F ;
  display.fillTriangle(0, 47, 3 * strength, 47 - (strength / 2), 3 * strength, 47, 1);
}

void displayStereo(bool isStereo)
{
  display.fillRect(70, 38, 12, 9, 0);
  if (isStereo)
  {
    display.drawCircle(71, 43, 4, 1);
    display.drawCircle(76, 43, 4, 1);
  }
}