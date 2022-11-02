#include "modbus-rtu.h"

#define RS485_GPIO_RX 27
#define RS485_GPIO_TX 14
#define RS485_GPIO_RTS 13

#define MAX_VALUE_LEN 8 // maximum payload data that can be received and stored through modbusrtu

#ifndef UNITTEST
RS485Comm rs485comm(&Serial1, (uint8_t)RS485_GPIO_RX, (uint8_t)RS485_GPIO_TX, (uint8_t)RS485_GPIO_RTS);
#endif

#define MAX_BUFFER_SIZE 255

String error_msg = "";

ModbusRTU::ModbusRTU(){
	#ifndef UNITTEST
	rs485comm.begin(1);
	#endif
}

#ifdef UNITTEST
char response[256];
uint8_t len_response = 0;
bool ModbusRTU::rs485_set_response(uint8_t* data, uint16_t size){
	if(size > 255)
		return false;

	uint16_t crc_ = crc(data,size);
  data[size+1] = crc_>>8;
  data[size] = (uint8_t)crc_;

	len_response = size+2;
	memset(response,0,256);
	memcpy(response,data,len_response);
	return true;
}
#endif

uint8_t ModbusRTU::rs485_read(uint8_t unit_id, uint8_t fc, uint16_t address, uint16_t len, uint8_t* data, uint16_t* size){

	uint16_t buffer_size = *size;

  if(!ModbusRTU::encode(unit_id,fc,address,len,(uint8_t*)nullptr,data, size)){
		#ifndef UNITTEST
    Serial.println("Failed enconding modbus..");
		#else
		std::cout << "Failed enconding modbus.." << std::endl;
		#endif
    return ERROR_MODBUS_ENC;
  }

  #ifdef DEBUG_LOG
  log("write: ",data,*size);
  #endif
	#ifndef UNITTEST
  rs485comm.write(data,*size);
	*size = buffer_size;
  len = rs485comm.read(data,*size);
	#else
		// fill buffer
		memset(data,0,buffer_size);
		if(len_response <= buffer_size){
			len = len_response;
			memcpy(data,response,len_response);
		}else len = 0;
	#endif
  #ifdef DEBUG_LOG
  log("response: ",data,len);
  #endif
  if(len == 0){
		#ifndef UNITTEST
    Serial.println("No data returned from slave..");
		#else
		std::cout << "No data returned from slave.." << std::endl;
		#endif
    return ERROR_MODBUS_GW_NO_RSP; // GW target device failed to respond
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
        return rtu.len;
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
          return ERROR_MODBUS_NES; // change this error - log("data returned too big");
      }

    }else
      return ERROR_MODBUS_DEC; // change this error - log("error reading rs485 sensor");
  }else{
		#ifndef UNITTEST
    Serial.println("Received frame is not valid..");
		Serial.println(ModbusRTU::getLastError());
		#else
		std::cout << "Received frame is not valid.." << std::endl;
		std::cout << ModbusRTU::getLastError() << std::endl;
		#endif
    return ERROR_MODBUS_RFNV;  // change this error - log("error reading rs485 sensor");
  }

  return 0;
}

uint8_t ModbusRTU::rs485_write(uint8_t unit_id, uint8_t fc, uint16_t address, uint16_t len, uint8_t* data, uint16_t* size){

  if(*size < 2){
		#ifdef DEBUG_LOG
	  log("no payload");
	  #endif
		return ERROR_MODBUS_ENC; // no payload
	}

  uint16_t payload_size = data[0];
  if(len*2 != payload_size){
		#ifdef DEBUG_LOG
	  log("invalid frame");
	  #endif
		return ERROR_MODBUS_ENC; // invalid frame
	}

  payload_size += 9;
  uint8_t frame[payload_size];
  if(!ModbusRTU::encode(unit_id,fc,address,len,data,frame, &payload_size)){
		#ifdef DEBUG_LOG
	  log("error decoding");
	  #endif
		return ERROR_MODBUS_ENC;
	}

	#ifdef DEBUG_LOG
  log(">>",frame,payload_size);
  #endif
	#ifndef UNITTEST
  rs485comm.write(frame,payload_size);
	memset(frame,0,*size);
	*size = rs485comm.read(frame,payload_size);
	#else
	memset(frame,0,payload_size);
	if(len_response <= payload_size){
		memcpy(frame,response,len_response);
		*size = len_response;
	}
	#endif
  #ifdef DEBUG_LOG
  log("<<",frame,*size);
  #endif
  if(*size == 0)
    return ERROR_MODBUS_GW_NO_RSP; // GW target device failed to respond

  if(ModbusRTU::valid(frame,*size)){
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
        return rtu.len;
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
          return ERROR_MODBUS_NES; // change this error - log("data returned too big");
      }
    }else
      return ERROR_MODBUS_DEC; // change this error - log("error reading rs485 sensor");
  }else
    return ERROR_MODBUS_RFNV;  // change this error - log("error reading rs485 sensor");

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
