#include <iostream>
#include "RTkGPIO.h"

int main() {

    RTkGPIO gpio;
    std::cout <<  gpio.get_version();
    std::cout << std::endl;
    std::cout <<  gpio.get_version();
    return 0;
}