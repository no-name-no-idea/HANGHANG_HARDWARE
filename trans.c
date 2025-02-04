#include <Manchester.h>
#include <SoftwareSerial.h> //시리얼통신 라이브러리 호출
#include <DHT11.h>
#include <EEPROM.h>

#define EXTERNED 0x00
#define FIND 0x40
#define SET 0x80
#define DATA 0xC0
#define ERROR_BYTE 0xFF

#define ADDRESS_MASK 0x3F

int dht_pin = 5;
int rf_tx = 4;
int blueTx=2;   //Tx (보내는핀 설정)at
int blueRx=3;   //Rx (받는핀 설정)

SoftwareSerial mySerial(blueTx, blueRx);  //시리얼 통신을 위한 객체선언
DHT11 dht11(dht_pin);

byte command = NULL;
uint16_t transmit_data = 0x00;
byte start_address = 0x01;

void BL_Send_float(float);
void RF_Send(void);
float RD_HM(void);
void SET_ADDRESS(byte);
void FIND_ADDRESS(byte);
void EXTERNED_ADDRESS(byte);

void EEPROM_Sort(void);
void EEPROM_Clear(void);
int EEPROM_Check(byte);

void setup() 
{
  Serial.begin(9600);
  man.setupTransmit(rf_tx, MAX_1200);
  mySerial.begin(9600); //블루투스 시리얼
  start_address = EEPROM.read(0);
  /*if(start_address == 0x00)
  {
    EEPROM_Clear();
  }*/
}

void loop()
{
  command = NULL;
  if (mySerial.available()) {
    command = (byte)mySerial.read();
    Serial.println(command);
    switch(command&0xC0)
    {
      case EXTERNED:  EXTERNED_ADDRESS(command&ADDRESS_MASK); break;
      case FIND:  FIND_ADDRESS(command&ADDRESS_MASK); break;
      case SET: SET_ADDRESS(command&ADDRESS_MASK);  Serial.println(EEPROM.read());  break;
      case DATA:  RD_HM();  break;
      default:  break;
    }
  }
  if(Serial.available())
  {
    mySerial.write(Serial.read());
  }
}

void SET_ADDRESS(byte address)
{
  transmit_data |= address;
  for(int i = 0; i < 255; i++)
  {
    man.transmit(transmit_data);
  }
  transmit_data |= SET;
  for(int i = 0; i < 255; i++)
  {
    man.transmit(transmit_data);
  }
  if(EEPROM_Check(address) == -1)
  {
    mySerial.write(ERROR_BYTE);
    return;
  }
  EEPROM.write((int)start_address++, address);
  return;
}

void FIND_ADDRESS(byte address)
{
  transmit_data |= address;
  for(int i = 0; i < 255; i++)
  {
    man.transmit(transmit_data);
  }
  if(!(EEPROM_Check(address) == -1))
  {
    mySerial.write(ERROR_BYTE);
    return;
  }
  transmit_data |= FIND;
  for(int i = 0; i < 255; i++)
  {
    man.transmit(transmit_data);
  }
  EEPROM.write((int)address, 0);
  EEPROM_Sort();
  return;
}

void EXTERNED_ADDRESS(byte address)
{
  //man.transmit();
}

float RD_HM(void)
{
  float humi = 0.0;
  float temp = 0.0;
  int err;

  if((err = dht11.read(humi, temp)) == 0)
  {
    BL_Send_float(humi);
    BL_Send_float(temp);
  }
  else
  {
    while(!(err = dht11.read(humi, temp)) == 0);
    BL_Send_float(humi);
    BL_Send_float(temp);
  }
}

void BL_Send_float(float data)
{
  byte *dump = (byte*)&data;
  for(int i = 0; i < sizeof(float); i++)
  {
    mySerial.write(*(dump+i));
  }
}

int EEPROM_Check(byte address)
{
  if(start_address == 0x01)
    return 0;
  for(int i = 0; i < (int)start_address; i++)
  {
    if(address == EEPROM.read(i))
    {
      return -1;
    }
  }
}

void EEPROM_Clear(void)
{
  for(int i = 0; i < 255; i++)
  {
    EEPROM.write(i, 0);
  }
  EEPROM.write(0, 0x01);
  return;
}
