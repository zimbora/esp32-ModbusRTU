
#ifndef ARDUINO_H
#define ARDUINO_H

#include "../libraries/nlohmann/json.hpp"
#include "../libraries/semaphore.hpp"

#include <stdint.h>
#include <iostream>

#include <chrono>
#include <thread>
#include <ctime>

#define JsonObject nlohmann::json
#define JsonArray nlohmann::json::array

#define containsKey contains
#define String std::string

#define byte_type                    0  // not known
#define uint8_type                   1
#define int8_type                    2
#define uint16be_type                3
#define uint16le_type                4
#define int16be_type                 5
#define int16le_type                 6
#define uint32be_type                9
#define uint32le_type                10
#define int32be_type                 11
#define int32le_type                 12
#define floatbe_type                 48
#define floatle_type                 49
#define hex_arr                      64
#define char_arr                     65
#define index_itron                  80

void      init_program();
void      delay(uint32_t time_ms);
uint32_t  millis();
uint32_t  getTimestamp();
uint16_t  year();
uint16_t  month();
uint16_t  day();
uint16_t  hour();
uint16_t  minute();

void      digitalWriteOnInput(uint8_t index, uint8_t value);
uint8_t   digitalRead(uint8_t index);

void      log(String log);
void      log(String log,int value);
void      log(String log,uint8_t* data, int size);

#define OUTPUTS_MAX 8

struct global_settings {
    struct fw{
      uint8_t      model; // boot
      char         version[12]; // boot
      char         hash[33]; // on settings load
      char         uid[16]; // on settings load
    }fw;

    struct system{
      uint8_t      reboot_cause; // boot
      uint8_t      mode; // on settings load
    }system;

    struct wifi { // on settings load
      uint16_t     mode;
  		char         ssid[32];
  		char         pwd[32];
  	} wifi;

    struct nb { // on settings load
  		char         apn[32];
  		char         user[32];
  		char         pwd[32];
  		uint8_t      band;
  		uint16_t     cops;
  	} nb;

  	struct gprs { // on settings load
  		char         apn[32];
  		char         user[32];
  		char         pwd[32];
  		uint8_t      band;
  		uint16_t     cops;
  	}gprs;

    struct catm1 { // on settings load
  		char         apn[32];
  		char         user[32];
  		char         pwd[32];
  		uint8_t      band;
  		uint16_t     cops;
  	}catm1;

    struct modem {
      bool        active; // on boot
      uint8_t	    uart;  // on boot
      uint32_t    baudrate; // on boot
      uint32_t    config; // on boot
      uint16_t    radio; // tech in use - on settings load
      uint16_t    desired_radio; // tech desired - on settings load
    }modem;

    struct mqtt { // on settings load
      char        host[64];
      char        user[32];
      char        pass[32];
      char        prefix[32];
      uint16_t    port; //1883
      bool        backup; //false
    } mqtt;

  	struct log { // on settings load
  		bool         active;
  		uint8_t      uart;
  		uint32_t     baudrate;
  		uint8_t      level;
  		bool         leds;
  	} log;

    struct int_sensors { // on settings load
        uint32_t   mask;
        uint32_t   period;
    } int_sensors;

  	struct rs485 { // on settings load
  		uint8_t      active;
  		uint8_t      retries;
  		uint16_t     retry_delay;
  		uint32_t     baudrate;
  		uint32_t     config; // not implemented for now
  		uint16_t     period; // not implemented for now
  		int8_t       align;
  		uint16_t     delay;     // not implemented for now
  		uint8_t      parity;    // not implemented for now
  		uint8_t      databits;  // not implemented for now
  		uint8_t      stopbits;  // not implemented for now
  	} rs485;

    struct keepalive {  // on settings load
      bool         active;
  		uint32_t     period;
  	} keepalive;

    struct analyzer {
  		bool 				active;
  		uint8_t 		mode;
  		uint32_t 		refresh; // seconds
  		uint16_t 		multiplier; // multiplier
	} analyzer;

  struct pulsecounter {
		bool         active;
		uint16_t     period;
		uint32_t     counters[16];
		uint16_t     boot_wait;
		uint16_t     trigger;
		uint16_t     filter;
		uint16_t     limit;
		uint32_t     accum[16];
	} pulsecounter;

	struct output {
		uint8_t 		 state[OUTPUTS_MAX];		 	// DO || PWM || DAC
		uint8_t 		 value[OUTPUTS_MAX]; 	 		// output value - 8 bits
		uint16_t 		 timeout[OUTPUTS_MAX];  	// seconds 0 - no timeout
		uint32_t 		 freq[OUTPUTS_MAX];		 		// pwm freq
	}output;

	struct andon {
		bool 				active;
		uint16_t 		blinking;
	} andon;

};

extern global_settings settings;
#endif
