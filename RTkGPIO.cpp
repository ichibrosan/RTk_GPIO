//
// Created by doug on 6/3/25.
//
#include <cstdio>
#include <string>
#include "RTkGPIO.h"

#include <iostream>
#include <string>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <bits/ostream.tcc>

RTkGPIO::RTkGPIO() {
    m_ssDevice = "/dev/ttyUSB0";

    // Open the device for read/write
    int fd = open(m_ssDevice.c_str(), O_RDWR | O_NOCTTY | O_NDELAY);
    if (fd == -1) {
        std::cerr << "Failed to open device: " << m_ssDevice << std::endl;
        exit(1);
    }

    // Configure serial port settings
    struct termios options;
    tcgetattr(fd, &options);

    // Set baud rate to 230400
    cfsetispeed(&options, B230400);
    cfsetospeed(&options, B230400);

    // Configure 8N1 (8 data bits, no parity, 1 stop bit)
    options.c_cflag &= ~PARENB;   // No parity
    options.c_cflag &= ~CSTOPB;   // 1 stop bit
    options.c_cflag &= ~CSIZE;    // Clear data size bits
    options.c_cflag |= CS8;       // 8 data bits
    options.c_cflag |= CREAD | CLOCAL; // Enable receiver, ignore modem control lines

    // Disable hardware flow control
    options.c_cflag &= ~CRTSCTS;

    // Configure input/output flags for raw mode
    options.c_iflag &= ~(IXON | IXOFF | IXANY); // Disable software flow control
    options.c_iflag &= ~(ICANON | ECHO | ECHOE | ISIG); // Raw input
    options.c_oflag &= ~OPOST; // Raw output

    // Set timeout values
    options.c_cc[VMIN] = 0;   // Minimum characters to read
    options.c_cc[VTIME] = 10; // Timeout in deciseconds (1 second)

    // Apply the configuration
    if (tcsetattr(fd, TCSANOW, &options) != 0) {
        std::cerr << "Failed to configure serial port" << std::endl;
        close(fd);
        exit(1);
    }

    // Convert file descriptor to FILE pointer
    m_pDevice = fdopen(fd, "r+");
    if (m_pDevice == nullptr) {
        std::cerr << "Failed to create FILE pointer from file descriptor" << std::endl;
        close(fd);
        exit(1);
    }

    // Make sure output is unbuffered for immediate transmission
    setbuf(m_pDevice, nullptr);
}

std::string RTkGPIO::get_version() {
    char szTemp[128];
    fputs("V\r", m_pDevice);
    fflush(m_pDevice); // Ensure data is sent immediately
    usleep(100000);  // 100 milliseconds instead of 1 second

    if (fgets(szTemp, sizeof(szTemp), m_pDevice)) {
        return std::string(szTemp);
    }
    return "";
}


// COMMAND PROCESSORS
/*
 * See main.cpp Lines 201-218
 */
void RTkGPIO::write_to_i2c(int i2caddr, int i2cblocks, char * pblock) {
    fputc(i2caddr,   m_pDevice);
    fputc(i2cblocks, m_pDevice);
    char * pBlk = pblock;
    for(int index=0;index<i2cblocks;index++) {
        fputc(*pblock++, m_pDevice);
    }
}

/*
 * See main.cpp Lines 220-244
 */
void RTkGPIO::read_from_i2c(int i2caddr, int i2cblocks, int i2cmagic, char * pBlock) {
    fputc(i2caddr,   m_pDevice);
    fputc(i2cblocks, m_pDevice);
    fputc(i2cmagic,  m_pDevice);
    for (int index=0;index<i2cblocks;index++) {
        *pBlock++ = fgetc(m_pDevice);
    }
}

/*
 * See main.cpp Lines 248-264
 */

 /**
 * Control thru i2c
 * @param pinch - T for start or P for stop
 */
void RTkGPIO::control_thru_i2c(int pinch) {
    fputc(pinch, m_pDevice);
}


// GPIO PROCESSORS

/* Process a GPIO command.
 *
 * GPIO Commands are two characters.
 * <pinch> <cmdch>
 *
 * pinch is the pin character (a..z) where 'a' represents pin 0.
 * cmdch is the command to perform on that pin:
 *   I: Set this pin to a digital input,        eg: aI
 *   O: Set this pin to a digital output,       eg: aO
 *   0: Write a digital low to this output,     eg: a0
 *   1: Write a digital high to this output,    eg: a1
 *   U: Write a pull input high,    eg: aU
 *   D: Write a pull input down,    eg: aD
 *   ?: Read the state of this digital input,   eg: a?
 *      State is returned as pinch + state(0|1) eg: a0
 */


