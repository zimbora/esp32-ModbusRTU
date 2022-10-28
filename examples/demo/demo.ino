
#include "modbus-rtu.h"

ModbusRTU modbusrtu;

void setup() {
  // put your setup code here, to run once:

  Serial.begin(115200);

}

void loop() {
  // put your main code here, to run repeatedly:

  uint16_t size = 32;
  uint8_t* data = (uint8_t*)malloc(size);
  if(data != nullptr){
    Serial.println("Reading rs485..");
    uint8_t error = modbusrtu.rs485_read(1,3,0,2,data,&size);
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

    // do something
    free(data);
  }
  Serial.println();
  delay(3000);
}
