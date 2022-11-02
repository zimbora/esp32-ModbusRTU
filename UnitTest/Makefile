

# additional flags
CXXFLAGS += -std=c++11

MACROS = -DDEBUG
#MACROS += -DDEBUG_LOG

all: modbusrtu

modbusrtu: init cp_mobusrtu make_alarm run clean

init:
	mkdir ./tmp
	cp ./Arduino.h	 ./tmp/
	cp ./Arduino.cpp ./tmp/

cp_mobusrtu:
	cp ../src/modbus-rtu.h ./tmp/
	cp ../src/modbus-rtu.cpp ./tmp/

make_alarm:
	@echo "Compiling.."
	clang++ $(CXXFLAGS) -DUNITTEST -DALARMTEST $(MACROS) modbusrtu_runner.cpp ./tmp/Arduino.cpp ./tmp/modbus-rtu.cpp

run:
	@echo "Running test.."
	./a.out

clean:
	@echo "Cleaning up"
	rm -r ./tmp
	rm *.out
