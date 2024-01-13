//
// Created by bergschaf on 1/13/24.
//
# include <stdio.h>
# include <unistd.h>
#include <CppLinuxSerial/SerialPort.hpp>
using namespace mn::CppLinuxSerial;
using namespace std;


int main(){
    SerialPort serialPort("/dev/ttyACM0", BaudRate::B_9600, NumDataBits::EIGHT, Parity::NONE, NumStopBits::ONE);
    serialPort.SetTimeout(0);

    serialPort.Open();
    // delay 1s
    usleep(3000000);
    printf("Write to serial port\n");
    serialPort.Write("Hello World!");
    usleep(3000000);

    string data;
    for (int i = 0; i < 10; ++i) {
        printf("Available bytes: %d\n", serialPort.Available());
        serialPort.Read(data);
        printf("Read from serial port: %s\n", data.c_str());
    }
    serialPort.Close();
    return 0;
}
