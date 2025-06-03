//
// Created by doug on 6/3/25.
//

#ifndef RTKGPIO_H
#define RTKGPIO_H
#include <cstdio>


class RTkGPIO {
    std::string m_ssDevice;
    FILE * m_pDevice;
public:
    RTkGPIO();
    std::string get_version();
    void write_to_i2c(int i2caddr, int i2cblocks, char * pblock);
    void read_from_i2c(int i2caddr, int i2cblocks, int i2cmagic, char * pblock);
    void control_thru_i2c(int pinch);
};



#endif //RTKGPIO_H
