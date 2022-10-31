
#include "Arduino.h"

uint8_t di_state[] = {false,false,false,false};
std::chrono::steady_clock::time_point begin;

extern global_settings settings;

semaphore* sdi = new semaphore(); // semaphore for digital inputs

void init_program(){
   begin = std::chrono::steady_clock::now();
}


using namespace std::this_thread; // sleep_for, sleep_until
using namespace std::chrono; // nanoseconds, system_clock, seconds

void delay(uint32_t time_ms){
    sleep_for(milliseconds(time_ms));
}

uint32_t millis(){
  std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
  return (uint32_t)std::chrono::duration_cast<std::chrono::milliseconds>(now - begin).count();
}

uint32_t getTimestamp(){
  std::time_t t = std::time(0);   // get time now
  return t;
}

uint16_t year(){
  std::time_t t = std::time(0);   // get time now
  std::tm* now = std::localtime(&t);
  return now->tm_year + 1900;
}

uint16_t month(){
  std::time_t t = std::time(0);   // get time now
  std::tm* now = std::localtime(&t);
  return now->tm_mon + 1;
}

uint16_t day(){
  std::time_t t = std::time(0);   // get time now
  std::tm* now = std::localtime(&t);
  return now->tm_mday;
}

uint16_t hour(){
  std::time_t t = std::time(0);   // get time now
  std::tm* now = std::localtime(&t);
  return now->tm_hour;
}

uint16_t minute(){
  std::time_t t = std::time(0);   // get time now
  std::tm* now = std::localtime(&t);
  return now->tm_min;
}

void digitalWriteOnInput(uint8_t index, uint8_t value){
  sdi->xSemaphoreTake();
  di_state[index] = value;
  sdi->xSemaphoreGive();
}

uint8_t digitalRead(uint8_t index){
  sdi->xSemaphoreTake();
  uint8_t state = di_state[index];
  sdi->xSemaphoreGive();
  return state;
}

void log(String log){
	std::cout << "[" << millis() <<"] " << log << "\n";
}
void log(String log,int value){
	std::cout << "[" << millis() <<"] " << log << value <<"\n";
}

void log(String log,uint8_t* data, int size){
	std::cout << "[" << millis() <<"] " << log << ": ";
  int i = 0;
  while(i<size){
    printf("0x%x ",data[i++]);
  }
  std::cout << std::endl;
}
