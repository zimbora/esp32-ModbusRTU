#ifndef MODBUS_RTU_H
#define MODBUS_RTU_H

#include <Arduino.h>

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
	ModbusRTU();
	static uint8_t rs485_read(uint8_t unit_id, uint8_t fc, uint16_t address, uint16_t len, uint8_t* data, uint16_t* size);
	static uint8_t rs485_write(uint8_t unit_id, uint8_t fc, uint16_t address, uint16_t len, uint8_t* data, uint16_t* size);
	static String getLastError();
private:
	static bool valid(uint8_t *data, uint16_t len);
	static bool decode(uint8_t *data, uint16_t len, modbus_rtu* rtu);
	static bool encode(uint8_t slave_id, uint8_t fc, uint16_t address, uint16_t len, uint8_t* data, uint8_t *frame, uint16_t* size);
	static uint16_t encode(uint8_t *message, uint16_t message_size, uint8_t *response);
	static uint16_t crc(uint8_t *buf, uint16_t len);
};

#endif
