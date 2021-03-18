#include <Manchester.h>
#include <EEPROM.h>

#define RX_PIN 3 //= pin 3
#define LED 0
#define BUR 1
#define BUTTON 2
#define TOBYTE(X) (uint8_t)X

uint16_t m = 0;
static const uint8_t op_address = EEPROM.read(0);

void EEPROM_Clear(void);

void setup()
{
  man.setupReceive(RX_PIN, MAN_1200);
  man.beginReceive();
  pinMode(LED, OUTPUT);
  pinMode(BUR, OUTPUT);
  pinMode(BUTTON, INPUT);   
  //EEPROM_CLEAR();                //First Setting
  //EEPROM_ADDRESS_SET(0x01);      //First Setting
  //GET_ADDRESS();
}

void loop() {
  if (man.receiveComplete()) 
  {
    m = man.getMessage();
    man.beginReceive();
    if(EEPROM.read(0) == (byte)0)
    {
      while(m != man.getMessage())
      {
        man.beginReceive();
      }
      if(m&0x80)
      {
        EEPROM.write(0, (byte)m&0x3F);
      }
    }
    else if(m&0x40 && EEPROM.read()&(m&0x3F))
    {
      digitalWrite(BUR, HIGH);
      delay(1000);
      digitalWrite(BUR, LOW);
    }
  }
}

void EEPROM_Clear(void)
{
  int i = 0;
  while(i < EEPROM.length())
  {
    EEPROM.write(i, 0);
  }
}
