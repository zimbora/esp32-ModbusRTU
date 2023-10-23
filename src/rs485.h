
#ifndef RS485_H
#define RS485_H

#include "Arduino.h"
#define ESP32DEBUGGING
#include <ESP32Logger.h>

#define RTS_NORMAL 1 // set RTS to LOW before write
#define RTS_INVERTED 0 // set RTS to HIGH before write

#define HARDWARE_RS485_READ_WAIT 50

class RS485Comm {

  public:
    // constructor
    RS485Comm(){};
    void setup(HardwareSerial* serial, uint8_t rx_pin, uint8_t tx_pin, uint8_t rts_pin);
    void setup(HardwareSerial* serial, uint8_t rx_pin, uint8_t tx_pin, uint8_t rts_pin, bool logic);
    // initialize RS485 comunication
    void begin(int8_t mode, uint32_t baudrate = 9600, uint32_t config = SERIAL_8N1, uint8_t retries = 3);
    void change_config(int8_t mode, uint32_t baudrate = 9600, uint32_t config = SERIAL_8N1, uint8_t retries = 3);
    // write data to RS485
    void write(uint8_t data[], uint8_t len);
    // read data from RS485
    uint8_t read(uint8_t data[], uint8_t len);
    // check data from RS485 in slave mode operation
    uint8_t read_master(uint8_t data[], uint8_t len);

    // ----- INTERNAL -----

  private:
    // read a byte from RS485
    int read_byte();

};

#endif
