
#ifndef OUTPUT_H
#define OUTPUT_H

#include <iostream>

void print_colour_text(std::string text,std::string colour){

  if(colour == "red")
    std::cout << "\033[1;31m "+text+"\033[0m\n";
  else if(colour == "green")
    std::cout << "\033[1;32m "+text+"\033[0m\n";
  else if(colour == "yellow")
    std::cout << "\033[1;33m "+text+"\033[0m\n";
  else if(colour == "blue")
    std::cout << "\033[1;34m "+text+"\033[0m\n";
  else if(colour == "magenta")
    std::cout << "\033[1;35m "+text+"\033[0m\n";
  else if(colour == "white")
    std::cout << "\033[1;37m "+text+"\033[0m\n";
}

void print_test_init(std::string test){

  print_colour_text("\n\n","white");
  print_colour_text("--- --- ---","white");
  print_colour_text("Hello Unit Test!","blue");

  print_colour_text("--- Initing test ---","white");
  print_colour_text("--- "+test+" ---","blue");
}

void print_test_end(std::string test){

  print_colour_text("--- --- ---","white");
}

#endif
