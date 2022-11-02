// Your First C++ Program

#include <stdint.h>
#include <stdio.h>
#include <iostream>

#include "./tmp/modbus-rtu.h"
#include "./libraries/output.h"

// Editable by tester
uint8_t test[] = {
  true, // test1 - testing read
  true, // test2 - testing read
  true, // test3 - testing read
  true, // test4 - testing read
  true, // test5 - testing read
  true, // test6 - testing read
  true  // test7 - testing write
};

#define String std::string
using json = nlohmann::json;

// Editable by tester
// available types for test
uint8_t type_arr[] = {
  //byte_type,
  //uint8_type,
  //int8_type ,
  uint16be_type,
  //uint16le_type,
  int16be_type,
  //int16le_type,
  uint32be_type,
  //uint32le_type,
  int32be_type,
  //int32le_type,
  floatbe_type,
  //floatle_type,
  //hex_arr,
  //char_arr,
  //index_itron
};

ModbusRTU modbusrtu;

uint8_t test_read(uint8_t unit_id,uint8_t fc,uint16_t address,uint8_t len, uint8_t* response, uint8_t size){

    modbusrtu.rs485_set_response(response,size);

    uint16_t max_size = 50;
    uint8_t data[max_size];
    uint8_t error = modbusrtu.rs485_read(unit_id,fc,address,len,data,&max_size);
    if(error){
      printf("error: %d \n",error);
      printf("%s \n",modbusrtu.getLastError().c_str());
    }
    return error;
}

uint8_t test_write(uint8_t unit_id,uint8_t fc,uint16_t address,uint8_t len, uint8_t* payload, uint8_t* response, uint8_t size){

    if(!modbusrtu.rs485_set_response(response,size)){
      printf("couldn't set response \n");
      return 255;
    }

    uint16_t max_size = size+2;
    uint8_t error = modbusrtu.rs485_write(unit_id,fc,address,len,payload,&max_size);
    if(error){
      printf("error: %d \n",error);
      printf("%s \n",modbusrtu.getLastError().c_str());
    }
    return error;
}

// testing null structs
bool modbusrtu_test1(){
  // Null json

  print_colour_text("testing valid response!","white");

  uint8_t unit_id = 1;
  uint8_t fc = 3;
  uint16_t address = 0;
  uint8_t len = 2;
  uint8_t expected_error = 0x00;

  uint8_t response[] = {1,3,4,0,0,0,1,0,0};
  uint8_t size_rsp = (uint8_t)(sizeof(response)/sizeof(uint8_t)-2);

  if(response[1] >= 0x80)
    expected_error = response[2];

  return test_read(unit_id,fc,address,len,response,size_rsp) == expected_error;
}

bool modbusrtu_test2(){

  print_colour_text("testing response with error!","white");

  uint8_t unit_id = 1;
  uint8_t fc = 3;
  uint16_t address = 0;
  uint8_t len = 2;
  uint8_t expected_error = 0x00;

  uint8_t response[] = {1,0x83,1,0,0};
  uint8_t size_rsp = (uint8_t)(sizeof(response)/sizeof(uint8_t)-2);

  if(response[1] >= 0x80)
    expected_error = response[2];

  return test_read(unit_id,fc,address,len,response,size_rsp) == expected_error;

}

bool modbusrtu_test3(){

  print_colour_text("testing response - with different len (packet is valid)!","white");

  uint8_t unit_id = 1;
  uint8_t fc = 3;
  uint16_t address = 0;
  uint8_t len = 2;
  uint8_t expected_error = 0;

  uint8_t response[] = {1,3,0,0,0};
  uint8_t size_rsp = (uint8_t)(sizeof(response)/sizeof(uint8_t)-2);

  if(response[1] >= 0x80)
    expected_error = response[2];

  return test_read(unit_id,fc,address,len,response,size_rsp) == expected_error;

}

bool modbusrtu_test4(){

  print_colour_text("testing response (1 byte missing in payload)!","white");

  uint8_t unit_id = 1;
  uint8_t fc = 3;
  uint16_t address = 0;
  uint8_t len = 2;
  uint8_t expected_error = ERROR_MODBUS_DEC;

  uint8_t response[] = {1,3,5,0,0,0,1,0,0};
  uint8_t size_rsp = (uint8_t)(sizeof(response)/sizeof(uint8_t)-2);

  if(response[1] >= 0x80)
    expected_error = response[2];

  return test_read(unit_id,fc,address,len,response,size_rsp) == expected_error;

}

bool modbusrtu_test5(){

  print_colour_text("testing response - wrong fc in response yet packet is valid !","white");

  uint8_t unit_id = 1;
  uint8_t fc = 3;
  uint16_t address = 0;
  uint8_t len = 2;
  uint8_t expected_error = 0x00;

  uint8_t response[] = {1,2,4,0,0,0,1,0,0};
  uint8_t size_rsp = (uint8_t)(sizeof(response)/sizeof(uint8_t)-2);

  if(response[1] >= 0x80)
    expected_error = response[2];

  return test_read(unit_id,fc,address,len,response,size_rsp) == expected_error;

}

bool modbusrtu_test6(){

  print_colour_text("testing response - wrong unit_id in response yet packet is valid !","white");

  uint8_t unit_id = 1;
  uint8_t fc = 3;
  uint16_t address = 0;
  uint8_t len = 2;
  uint8_t expected_error = 0x00;

  uint8_t response[] = {2,3,4,0,0,0,1,0,0};
  uint8_t size_rsp = (uint8_t)(sizeof(response)/sizeof(uint8_t)-2);

  if(response[1] >= 0x80)
    expected_error = response[2];

  return test_read(unit_id,fc,address,len,response,size_rsp) == expected_error;

}

bool modbusrtu_test7(){

  print_colour_text("testing response - wrong unit_id in response, yet packet is valid !","white");

  uint8_t unit_id = 1;
  uint8_t fc = 16;
  uint16_t address = 0;
  uint8_t len = 2;
  uint8_t data[] = {0x04,0x00,0x00,0x01,0x02};
  uint8_t expected_error = 0x00;

  uint8_t response[] = {1,16,0,0,0,2,0,0};
  uint8_t size_rsp = (uint8_t)(sizeof(response)/sizeof(uint8_t)-2);

  if(response[1] >= 0x80)
    expected_error = response[2];

  return test_write(unit_id,fc,address,len,data,response,size_rsp) == expected_error;

}

typedef bool (*func_test_type)(void);
func_test_type call_test[] = {
  &modbusrtu_test1,
  &modbusrtu_test2,
  &modbusrtu_test3,
  &modbusrtu_test4,
  &modbusrtu_test5,
  &modbusrtu_test6,
  &modbusrtu_test7
};

int main() {
    #ifndef UNITTEST
    print_colour_text("Hello World!","red");
    #else
    print_colour_text("","white");
    print_colour_text("--- --- ---","blue");
    print_colour_text("Hello Unit Test!","blue");
    try{
      uint8_t i = 0;
      while(i<sizeof(test)){
        if(test[i]){
          if(call_test[i]())
            print_colour_text(" Test "+std::to_string(i+1)+" has passed","green");
          else
            print_colour_text(" Test "+std::to_string(i+1)+" has failed","red");
        }
        i++;
      }
    }catch(std::invalid_argument& e){
      std::cerr << e.what() << std::endl;
    }catch(...){
      print_colour_text("!! Exception Ocurred !! - terminating process ","red");
      return -1;
    }
    print_colour_text("Test terminated","blue");
    #endif
    return 0;
}
