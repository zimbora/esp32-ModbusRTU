#ifndef MODBUS_RTU_H
#define MODBUS_RTU_H

#include "Arduino.h"

#ifndef UNITTEST
#include "rs485.h"
#else
#include <iostream>
#endif

#define WARNING_LOG
#define DEBUG_LOG
//#define HIGH_DEBUG_LOG

#define ERROR_MODBUS_ENC 				03 // error encoding modbus
#define ERROR_MODBUS_GW_NO_RSP 	11 // Gateway Target Device Failed to Respond
#define ERROR_MODBUS_NES 				12 // not enough space to store response
#define ERROR_MODBUS_DEC 				13 // couldn't decode received frame
#define ERROR_MODBUS_RFNV 			14 // received frame is not valid

struct modbus_rtu{
	uint8_t  slave_id;
	uint8_t  fc;
	uint16_t address;
	uint16_t len;
	uint8_t* data;
	uint16_t crc;
};

class ModbusRTU {

public:
	ModbusRTU(){};
	static void setup(HardwareSerial* serial, uint8_t rx, uint8_t tx, uint8_t rts);
	static void begin(int8_t mode_, uint32_t baudrate_, uint32_t config_, uint8_t retries_=3);
	static void change_config(int8_t mode_, uint32_t baudrate_, uint32_t config_, uint8_t retries_=3);
	#ifdef UNITTEST
	bool rs485_set_response(uint8_t* data, uint16_t size);
	#endif
	static uint8_t rs485_read(uint8_t unit_id, uint8_t fc, uint16_t address, uint16_t len, uint8_t* data, uint16_t* size);
	static uint8_t rs485_write(uint8_t unit_id, uint8_t fc, uint16_t address, uint16_t len, uint8_t* data, uint16_t* size);

	static String getLastError();
	static bool decode(uint8_t *data, uint16_t len, modbus_rtu* rtu);
	static bool valid(uint8_t *data, uint16_t len);
	static uint16_t crc(uint8_t *buf, uint16_t len); // crc-16
private:
	static bool encode(uint8_t slave_id, uint8_t fc, uint16_t address, uint16_t len, uint8_t* data, uint8_t *frame, uint16_t* size);
	static uint16_t encode(uint8_t *message, uint16_t message_size, uint8_t *response);
	template<typename T1>
	static void log(String text,T1 value){
		Serial.print("rs485 - ");
		Serial.print(value);
	}
	static void log(String text){
		Serial.println("rs485 - "+text);
	}
	static void log_hex(String direction,uint8_t* data,size_t size){
		Serial.print("rs485 - ["+direction+"]: ");
		for(uint16_t i=0; i<size; i++){
			Serial.printf("%x ",data[i]);
		}
		Serial.println();
	}
};

#endif
