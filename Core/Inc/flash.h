/*
 * flash.h
 *
 *  Created on: Sep 13, 2022
 *      Author: Mattia
 */

#ifndef INC_FLASH_H_
#define INC_FLASH_H_

#include "main.h"

void unlock();
void lock();
void erase_sec11();
void program(char *words, uint32_t location);
void program_8(char *words, uint32_t location);
void read_new();
void read(uint32_t, char *,uint8_t);

#endif /* INC_FLASH_H_ */
