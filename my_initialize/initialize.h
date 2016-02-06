/*
 * initialize.h
 *
 *  Created on: 29 wrz 2015
 *      Author: tomek
 */

#ifndef INITIALIZE_H_
#define INITIALIZE_H_



void init (void);
void GPIO_Config(void);
void RCC_Config(void);
void NVIC_Config(void);
void SPI_Config(void);
void DMA_config (int* data_src, int * data_dst);
unsigned long int SysTick_Config_Mod(unsigned long int SysTick_CLKSource, unsigned long int Ticks);


#endif /* INITIALIZE_H_ */
