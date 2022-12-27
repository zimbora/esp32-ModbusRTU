# esp32-ModbusRTU

## BUGS
  - !! this packet: 1,16,22,2 used with rs485_read makes program crash

## Description
package to read and write on modbus rtu slaves using a serial connection.

## Implementation
This library is prepared to receive a decoded packet, calculate crc-16, build the frame, send, wait for response
and return data received.
In case of error, the error can be obtained calling getLastError method.

[static uint8_t rs485_read(uint8_t unit_id, uint8_t fc, uint16_t address, uint16_t len, uint8_t* data, uint16_t* size)](#RS485-read)

[static uint8_t rs485_write(uint8_t unit_id, uint8_t fc, uint16_t address, uint16_t len, uint8_t* data, uint16_t* size)](#RS485-write)

[static String getLastError()](#Get-error)

## Examples
  Run programs inside examples folder to check how it works
### demo
  Request data from slave

## Unit Test with clang
  >> chmod u+x make.sh

  >> ./make.sh

  If needed uncomment DEBUG FLAGS in Makefile

## TODO
  Allow configurations for serial port

## Limitations
  rs485_read method can only store MAX_VALUE_LEN bytes of retrieved payload (check modbus-rtu file to see macro value)

## Public Methods - Extension

### RS485 read

* @param unit_id - unit id
* @param fc - function code
* @param address
* @param len - number of registers to be read
* @param data - pointer to received payload
* @param size - maximum size of passed pointer, returns length of data read
* @return error - 0 in case of no error

```
static uint8_t rs485_read(uint8_t unit_id, uint8_t fc, uint16_t address, uint16_t len, uint8_t* data, uint16_t* size)
```
#### Example
```
uint16_t size = 32;
uint8_t* data = (uint8_t*)malloc(size);
if(data != nullptr){
  uint8_t error = modbusrtu.rs485_read(1,3,0,2,data,&size);
}
free(data);
```

### RS485 write

* @param unit_id - unit id
* @param fc - function code
* @param address
* @param len - number of registers to be read
* @param data - pointer to payload to be written
* @param size - size of payload to be written
* @return error - 0 in case of no error

```
static uint8_t rs485_write(uint8_t unit_id, uint8_t fc, uint16_t address, uint16_t len, uint8_t* data, uint16_t* size)
```
#### Example
```
uint16_t size = 32;
uint8_t* data = (uint8_t*)malloc(size);
if(data != nullptr){
  data[0] = 0x00;
  data[1] = 0x00;
  data[2] = 0x00;
  data[3] = loop_counter++;
  uint8_t error = modbusrtu.rs485_write(1,16,0,2,data,&size);
}
free(data);
```

### Get error

* @return error in string format

```
static String getLastError()
```
#### Example
```
String error_msg = modbusrtu.getLastError();
if(error_msg != "")
  Serial.println("error msg: "+error_msg);
```
