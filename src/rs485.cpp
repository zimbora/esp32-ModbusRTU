
#include "rs485.h"

HardwareSerial* rs485;

int8_t TX_GPIO = -1;
int8_t RX_GPIO = -1;
int8_t RTS_GPIO = -1;
bool RTS_LOGIC = RTS_NORMAL; // RTS_NORMAL - set RTS high before write; RTS_INVERTED - set RTS low before write
//bool RTS_LOGIC = RTS_INVERTED; // RTS_NORMAL - set RTS high before write; RTS_INVERTED - set RTS low before write
int8_t mode = 0;
int8_t retries = 0;
uint32_t baudrate = 0;
uint32_t config = 0;

void RS485Comm::setup(HardwareSerial* serial, uint8_t rx_pin, uint8_t tx_pin, uint8_t rts_pin){

  rs485 = serial;
  RX_GPIO = rx_pin;
  TX_GPIO = tx_pin;
  RTS_GPIO = rts_pin;
}

void RS485Comm::setup(HardwareSerial* serial, uint8_t rx_pin, uint8_t tx_pin, uint8_t rts_pin, bool logic){

  rs485 = serial;
  RX_GPIO = rx_pin;
  TX_GPIO = tx_pin;
  RTS_GPIO = rts_pin;
  RTS_LOGIC = logic;
}

void RS485Comm::begin(int8_t mode_, uint32_t baudrate_, uint32_t config_, uint8_t retries_) {

  mode = mode_;
  retries = retries_;
  baudrate = baudrate_;
  config = config_;

  DBGINI(&Serial);
  DBGSTA
  DBGLEV(Debug);

	if(mode == 0){
		DBGLOG(Info,"settings: rs485 disabled");
	}else if(mode == 1){
		DBGLOG(Info,"settings: rs485 configured as master");
	}else if(mode == 2){
		DBGLOG(Info,"settings: rs485 configured as slave");
  }else{
    DBGLOG(Error,"settings: rs485 mode not available");
  }

	if (mode != 1 && mode != 2) return;


	DBGLOG(Info,"rs485: prepare to init");
	DBGLOG(Info,"active: "+String(mode));
	DBGLOG(Info,"retries: "+String(retries));
	DBGLOG(Info,"baudrate: "+String(baudrate));
	DBGLOG(Info,"parity: "+String(config));
	DBGLOG(Info,"SERIAL_8N1: "+String(SERIAL_8N1));

	rs485->begin(baudrate,config,RX_GPIO,TX_GPIO);

	DBGLOG(Info,"rs485: init");

	//pinMode(HARDWARE_RS485_PIN, OUTPUT);
	//digitalWrite(HARDWARE_RS485_PIN, RTS_LOGIC ? 0 : 1);
  pinMode(RTS_GPIO, OUTPUT);
	digitalWrite(RTS_GPIO, RTS_LOGIC ? LOW : HIGH);
}

void RS485Comm::change_config(int8_t mode_, uint32_t baudrate_, uint32_t config_, uint8_t retries_) {

  mode = mode_;
  retries = retries_;
  baudrate = baudrate_;
  config = config_;

	if(mode == 0){
		DBGLOG(Info,"settings: rs485 disabled");
	}else if(mode == 1){
		DBGLOG(Info,"settings: rs485 configured as master");
	}else if(mode == 2){
		DBGLOG(Info,"settings: rs485 configured as slave");
  }else{
    DBGLOG(Error,"settings: rs485 mode not available");
  }

	if (mode != 1 && mode != 2) return;


	DBGLOG(Info,"rs485: prepare to init");
	DBGLOG(Info,"active: "+String(mode));
	DBGLOG(Info,"retries: "+String(retries));
	DBGLOG(Info,"baudrate: "+String(baudrate));
	DBGLOG(Info,"parity: "+String(config));

	rs485->begin(baudrate,config,RX_GPIO,TX_GPIO);

	DBGLOG(Info,"rs485: init");
}

void RS485Comm::write(uint8_t data[], uint8_t len) {
	if (!mode) return;

	digitalWrite(RTS_GPIO, RTS_LOGIC ? HIGH : LOW);

	delay(1);

	Serial.flush();

	rs485->flush();
	rs485->write(data, len);
	rs485->flush();

	delay(1);
	digitalWrite(RTS_GPIO, RTS_LOGIC ? LOW : HIGH);

  String data_str = " >> ";
  uint8_t i = 0;
  while(i<len){
    char buffer[4];
    sprintf(buffer,"%x ",data[i++]);
    data_str += buffer;
  }
	DBGLOG(Debug,data_str);
  /*
  uint32_t timeout = millis()+2000;
  while(timeout > millis()){
    Serial.printf("%x ",read_byte());
  }
  */

}

// works as slave and master for now
uint8_t RS485Comm::read(uint8_t data[], uint8_t len) {
	if (!mode) return 0;

  uint8_t tries = 0;

  while(tries < 15){

    delay(HARDWARE_RS485_READ_WAIT);

    if(rs485->available()){

      uint8_t i = 0;
      String data_str = " << ";
      while(rs485->available()){
        data[i] = rs485->read();
        char buffer[4];
        sprintf(buffer,"%x ",data[i++]);
        data_str += buffer;
      }
      DBGLOG(Debug,data_str);
      return i;
    }
    tries++;
  }
  DBGLOG(Debug,"rs485 no data available");

  return 0;

}

uint8_t RS485Comm::read_master(uint8_t data[], uint8_t len) {
	if (!mode) return 0;

	uint8_t n = 0;
	uint8_t e = 0;

	for (int i = 0; i < len; i++) {
		int r = -1;
		if(rs485->available())
			r = rs485->read();
		if (r == -1) {
			if (++e >= 5) return n;
		} else {
			e = 0;
			data[n++] = r;

			if (n >= len) return n;
		}
	}

	return n;
}

int RS485Comm::read_byte() {
	uint8_t tries = retries;

	while (!rs485->available()) {
		delay(HARDWARE_RS485_READ_WAIT);

		if (--tries == 0) return -1;
	}

	return rs485->read();
}
