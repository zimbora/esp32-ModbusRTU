#include "modbus-rtu.h"

#include "rs485.h"

#define RS485_GPIO_RX 27
#define RS485_GPIO_TX 14
#define RS485_GPIO_RTS 13

#define MAX_VALUE_LEN 4 // maximum payload data that can be received and stored through modbusrtu

RS485Comm rs485comm(&Serial1, (uint8_t)RS485_GPIO_RX, (uint8_t)RS485_GPIO_TX, (uint8_t)RS485_GPIO_RTS);

#define MAX_BUFFER_SIZE 255

String error_msg = "";

ModbusRTU::ModbusRTU(){

	rs485comm.begin(1);
}

uint8_t ModbusRTU::rs485_read(uint8_t unit_id, uint8_t fc, uint16_t address, uint16_t len, uint8_t* data, uint16_t* size){

  if(!ModbusRTU::encode(unit_id,fc,address,len,(uint8_t*)nullptr,data, size)){
    Serial.println("Failed enconding modbus..");
    return 0x8A;
  }

  #ifdef DEBUG_LOG
  log("write: ",data,*size);
  #endif
  rs485comm.write(data,*size);
  len = rs485comm.read(data,*size);
  #ifdef DEBUG_LOG
  log("response: ",data,len);
  #endif
  if(len == 0){
    Serial.println("No data returned from slave..");
    return 0x8B; // GW target device failed to respond
  }

  if(ModbusRTU::valid(data,len)){
    modbus_rtu rtu;
    #ifdef DEBUG_LOG
    log("response valid");
    #endif
    if(ModbusRTU::decode(data,len,&rtu)){
      #ifdef DEBUG_LOG
      log("rtu.slave_id: ",rtu.slave_id);
      log("rtu.fc: ",rtu.fc);
      log("rtu.len: ",rtu.len);
      #endif
      if(rtu.fc >= 0x80)
        return rtu.fc;
      else{
        if(rtu.len <= MAX_VALUE_LEN){
          memcpy(&data[0],&rtu.data[0],rtu.len);
          #ifdef DEBUG_LOG
          log("data: ",data,rtu.len);
          #endif
          *size = rtu.len;
          return 0;
        }
        else
          return 0x80; // change this error - log("data returned too big");
      }

    }else
      return 0x80; // change this error - log("error reading rs485 sensor");
  }else{
    Serial.println("Received frame is not valid..");
    Serial.println(ModbusRTU::getLastError());
    return 0x80;  // change this error - log("error reading rs485 sensor");
  }

  return 0;
}

uint8_t ModbusRTU::rs485_write(uint8_t unit_id, uint8_t fc, uint16_t address, uint16_t len, uint8_t* data, uint16_t* size){

  if(*size < 2)
    return 0x03; // no payload

  uint16_t payload_size = ((data[0]-'0')<<4)|(data[1]-'0');
  if(payload_size*2+2 != *size)
    return 0x03; // invalid frame

  uint8_t payload[payload_size+1];
  payload[0] = payload_size;
  for(uint8_t i=1;i<*size;i++){
    payload[i] = ((data[i*2]-'0')<<4)|(data[i*2+1]-'0');
  }
  payload_size += 9;
  uint8_t frame[payload_size];
  if(!ModbusRTU::encode(unit_id,fc,address,len,payload,frame, &payload_size))
      return 0x03;

  rs485comm.write(frame,payload_size);
  memset(frame,0,payload_size);
  payload_size = rs485comm.read(frame,payload_size);
  #ifdef DEBUG_LOG
  log("frame:",frame,payload_size);
  #endif
  if(payload_size == 0)
    return 0x0B; // GW target device failed to respond

  if(ModbusRTU::valid(frame,payload_size)){
    modbus_rtu rtu;
    #ifdef DEBUG_LOG
    log("response valid");
    #endif
    if(ModbusRTU::decode(frame,payload_size,&rtu)){
      #ifdef DEBUG_LOG
      log("rtu.slave_id: ",rtu.slave_id);
      log("rtu.fc: ",rtu.fc);
      log("rtu.len: ",rtu.len);
      #endif
      if(rtu.fc >= 0x80)
        return rtu.fc;
      else{
        if(rtu.len <= MAX_VALUE_LEN){
          memcpy(&data[0],&rtu.data[0],rtu.len);
          #ifdef DEBUG_LOG
          log("data: ",data,rtu.len);
          #endif
          *size = rtu.len;
          return 0;
        }
        else
          return 0x08; // change this error - log("data returned too big");
      }
    }else
      return 0x08; // change this error - log("error reading rs485 sensor");
  }else
    return 0x08;  // change this error - log("error reading rs485 sensor");

  return 0;
}

bool ModbusRTU::valid(uint8_t *data, uint16_t len) {
	if (len < 4) return false;

	uint16_t crc = ModbusRTU::crc(data, len - 2);

	return ( (((uint8_t) crc) == data[len - 2]) &&
	         (((uint8_t) (crc >> 8)) == data[len - 1]) );
}

bool ModbusRTU::decode(uint8_t *data, uint16_t len, modbus_rtu* rtu) {

	rtu->slave_id = data[0];
		rtu->fc = data[1];
	rtu->len = data[2];
	if(rtu->fc > 0x80)
		return true;
	if(rtu->len > (len-5)){
		error_msg = "[decode] - not enough space to store frame";
	  return false;
	}

	rtu->crc          = data[3+rtu->len] | (data[3+rtu->len+1] << 8);
	rtu->data         = &data[3];

	return true;
}

bool ModbusRTU::encode(uint8_t slave_id, uint8_t fc, uint16_t address, uint16_t len, uint8_t* data, uint8_t *response, uint16_t* size) {

	if(*size < 6){
		error_msg = "[encode] - passed frame is too short";
		return false;
	}

	memset(&response[0],0,*size);
	response[0] = slave_id;
	response[1] = fc;
	response[2] = address<<8;
	response[3] = address;
	response[4] = len<<8;
	response[5] = len;

	uint16_t message_size = 6;
	if(fc == 15 || fc == 16){
		message_size += 1+data[0];
		if(*size < message_size){
			error_msg = "[encode] - not enough space to store frame";
			return false;
		}

		memcpy(&response[6],data,data[0]+1);
	}


	uint16_t crc = ModbusRTU::crc(response, message_size);

	response[message_size] = (uint8_t) crc;
	response[message_size + 1] = (uint8_t) (crc >> 8);
	*size = message_size+2;

	return *size;
}

uint16_t ModbusRTU::encode(uint8_t *message, uint16_t message_size, uint8_t *response) {

	memcpy(message,response,message_size);

	uint16_t crc = ModbusRTU::crc(message, message_size);

	response[message_size] = (uint8_t) crc;
	response[message_size + 1] = (uint8_t) (crc >> 8);

	return 2 + message_size;
}

uint16_t ModbusRTU::crc(uint8_t *buf, uint16_t len) {
	uint16_t crc = 0xFFFF;

	for (int pos = 0; pos < len; pos++) {
		crc ^= (uint16_t) buf[pos];

		for (int i = 8; i != 0; i--) {
			if ((crc & 0x0001) != 0) {
				crc >>= 1;
				crc ^= 0xA001;
			} else {
				crc >>= 1;
			}
		}
	}

	return crc;
}

String ModbusRTU::getLastError(){
	String err_bck = error_msg;
	error_msg = "";
	return err_bck;
}
