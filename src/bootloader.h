#ifndef BOOTLOADER_H
#define BOOTLOADER_H

#include "mbed.h"
#include <cstdint>
#include <fstream>
#include <sstream>

void await_data();

void process_data();

void init_slave();

int update_application(std::stringstream &data, uint32_t address);

#endif // BOOTLOADER_H