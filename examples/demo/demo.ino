
#include "modbus-rtu.h"


#define RS485_GPIO_RX 27
#define RS485_GPIO_TX 14
#define RS485_GPIO_RTS 13

ModbusRTU modbusrtu;

void setup() {
  // put your setup code here, to run once:

  Serial.begin(115200);

  modbusrtu.setup(&Serial1,RS485_GPIO_RX,RS485_GPIO_TX,RS485_GPIO_RTS);
  modbusrtu.begin(1,9600,SERIAL_8N1);

  modbusrtu.change_config(1,115200,SERIAL_8N1);

  modbusrtu.change_config(1,9600,SERIAL_8N1);
}

uint8_t loop_counter = 1;

void loop() {
  // put your main code here, to run repeatedly:

  uint16_t size = 32;
  uint8_t* data = (uint8_t*)malloc(size);
  if(data != nullptr){
    Serial.println("Reading rs485..");
    uint8_t error = modbusrtu.rs485_read(1,4,0,2,data,&size);
    if(error == 0){
      Serial.print("res: ");
      uint8_t i = 0;
      while(i<size){
        Serial.printf("0x%x ",data[i++]);
      }
      Serial.println();
    }else{
      Serial.printf("error: 0x%x \n",error);
      String error_msg = modbusrtu.getLastError();
      if(error_msg != "")
        Serial.println("error msg: "+error_msg);
    }

    memset(data,0,32);
    Serial.println();
    Serial.println("Writing rs485..");
    data[0] = 0x04;
    data[1] = 0x3F;
    data[2] = 0x80;
    data[3] = 0x00;
    data[4] = 0x00;
    error = modbusrtu.rs485_write(1,16,22,2,data,&size);
    if(error == 0){
      Serial.println("packet written successfully");
    }else{
      Serial.printf("error: 0x%x \n",error);
      String error_msg = modbusrtu.getLastError();
      if(error_msg != "")
        Serial.println("error msg: "+error_msg);
    }

    // do something
    free(data);
  }
  Serial.println();
  delay(3000);
}
