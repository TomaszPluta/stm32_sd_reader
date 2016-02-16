#include <stdio.h>
#include <stdlib.h>
#include "stm32f10x.h"
#include "initialize.h"

//void GPIO_Config(void);
//void RCC_Config(void);
//void NVIC_Config(void);
//void SPI_Config(void);
//void init (void);
//unsigned long int SysTick_Config_Mod(unsigned long int SysTick_CLKSource, unsigned long int Ticks);

#define DMA_DIR_MemoryToMemory ((uint32_t)0x00000080)



void init (void)
{
	 RCC_Config();
	 GPIO_Config();
	 NVIC_Config();
	 SPI_Config();
	 ADC_config();
}


void GPIO_Config(void)
{

  GPIO_InitTypeDef  GPIO_InitStructure;

 //PORTC - Wyświetlacz
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOC, &GPIO_InitStructure);

  //PORTB - General purpouse
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOB, &GPIO_InitStructure);


  //PORTB - Input
  GPIO_InitStructure.GPIO_Pin =GPIO_Pin_9 | GPIO_Pin_10;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
  GPIO_Init(GPIOB, &GPIO_InitStructure);


  // PA4 - nSS/CS SPI
//  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  //SPI - SCK, MISO, MOSI
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  //linia DETECT zlacza SD
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);


//  GPIO_StructInit(&GPIO_InitStructure);
//  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
//  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
//  GPIO_Init(GPIOA, &GPIO_InitStructure);
}




void RCC_Config(void)
{
  ErrorStatus HSEStartUpStatus;

  RCC_DeInit();	                                         //Reset ustawien RCC
  RCC_HSEConfig(RCC_HSE_ON);                             //Wlaczenie HSE
  HSEStartUpStatus = RCC_WaitForHSEStartUp();            //Odczekaj az HSE bedzie gotowy
  if(HSEStartUpStatus == SUCCESS)
  {
    FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);//
    FLASH_SetLatency(FLASH_Latency_2);                   //ustaw zwloke dla pamieci Flash; zaleznie od taktowania rdzenia
                                                         //0:<24MHz; 1:24~48MHz; 2:>48MHz
    RCC_HCLKConfig(RCC_SYSCLK_Div1);                     //ustaw HCLK=SYSCLK
    RCC_PCLK2Config(RCC_HCLK_Div1);                      //ustaw PCLK2=HCLK
    RCC_PCLK1Config(RCC_HCLK_Div2);                      //ustaw PCLK1=HCLK/2
    RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9); //ustaw PLLCLK = HSE*9 czyli 8MHz * 9 = 72 MHz
    RCC_PLLCmd(ENABLE);                                  //wlacz PLL
    while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET);  //odczekaj na poprawne uruchomienie PLL
    RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);           //ustaw PLL jako zrodlo sygnalu zegarowego
    while(RCC_GetSYSCLKSource() != 0x08);                //odczekaj az PLL bedzie sygnalem zegarowym systemu


    SysTick_Config_Mod(SysTick_CLKSource_HCLK, 720000ul);

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1 , ENABLE);
  }
}



void NVIC_Config(void)
{
#ifdef  VECT_TAB_RAM
  NVIC_SetVectorTable(NVIC_VectTab_RAM, 0x0);
#else  // VECT_TAB_FLASH
  NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0);
#endif
}



void SPI_Config(void)
{
	SPI_InitTypeDef SPI_InitStructure;
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_CRCPolynomial = 7;
	SPI_Init(SPI1, &SPI_InitStructure);
	SPI_SSOutputCmd(SPI1, ENABLE);
	SPI_Cmd(SPI1, ENABLE);
}



unsigned long int SysTick_Config_Mod(unsigned long int SysTick_CLKSource, unsigned long int Ticks)
{
  //inicjalizacja licznika SysTick
  //zastepuje funkcje z bibliotek STM w zwiazku z bledem w funcji SysTick_Config
  unsigned long int Settings;

  assert_param(IS_SYSTICK_CLK_SOURCE(SysTick_CLKSource));

  if (Ticks > SysTick_LOAD_RELOAD_Msk)  return (1);             //Kontrola, czy wartosc poczatkowa nie przekracza max

  SysTick->LOAD = (Ticks & SysTick_LOAD_RELOAD_Msk) - 1;        //Ustaw wartosc poczatkowa licznika
  NVIC_SetPriority (SysTick_IRQn, 0);                           //Ustaw priorytet przerwania
  SysTick->VAL  = 0;                                            //Ustaw wartosc aktualna licznika
  Settings=SysTick_CTRL_TICKINT_Msk | SysTick_CTRL_ENABLE_Msk;  //Ustaw flagi wlaczaenia SysTick IRQ  i samego licznika
  if (SysTick_CLKSource == SysTick_CLKSource_HCLK){             //Wybierz flage ustawien zrodla sygnalu zegarowego
    Settings |= SysTick_CLKSource_HCLK;
  } else {
    Settings &= SysTick_CLKSource_HCLK_Div8;
  }
  SysTick->CTRL = Settings;                                     //Zapisz ustawienia do rejestru sterujacego SysTick (i wlacz licznik)
  return (0);
}



void DMA_for_ADC(int* data_dst_addr)
{



	// włączenie taktowania dla DMA
	#define DMA1EN  1
	RCC->AHBENR |=  DMA1EN;

	DMA1_Channel1->CMAR = (uint32_t) data_dst_addr;

	#define ADC1_Address 0x4001244C
	DMA1_Channel1->CPAR = ADC1_Address;

	// należy konfigurować przed aktywacją kanału
	// patrz: RM str.287
	#define BufferSize 2;
	DMA1_Channel1->CNDTR = BufferSize;


	#define MemoryToMemoryEnable (1<<14)
	DMA1_Channel1->CCR |= MemoryToMemoryEnable;

	#define PriorityHigh (1<<13)
	DMA1_Channel1->CCR |= PriorityHigh;

	#define MemDataSize (1<<11)
	DMA1_Channel1->CCR |= MemDataSize;

	#define PerDataSize (1<<9)
	DMA1_Channel1->CCR |= PerDataSize;

	#define MemInc (1<<7)
	DMA1_Channel1->CCR |= MemInc ;

	#define DataSize (1<<11)
	DMA1_Channel1->CCR |= DataSize;

	#define Circular (1<<5)
	DMA1_Channel1->CCR |= Circular ;

	#define ChannelEnable (1)
	DMA1_Channel1->CCR |= ChannelEnable;






//
//
//	RCC->AHBENR |=  1;
//
//	#define Circular (1<<5)
//	DMA1_Channel1->CCR |= Circular ;
//
//	#define PriorityHigh (1<<13)
//	DMA1_Channel1->CCR |= PriorityHigh;
//
//	#define PerDataSize (1<<9)
//	DMA1_Channel1->CCR |= PerDataSize;
//
//	#define DataSize (1<<11)
//	DMA1_Channel1->CCR |= DataSize;
//
//	#define MemInc (1<<7)
//	DMA1_Channel1->CCR |= MemInc ;
//
//	DMA1_Channel1->CNDTR = 2; // konfigurować przed aktywacją kanału
//	DMA1_Channel1->CPAR = ADC1_Address;
//	DMA1_Channel1->CMAR = (uint32_t) data_dst_addr;
//
//
//	//	DMA_Cmd(DMA1_Channel1, ENABLE);
//	DMA1_Channel1->CCR |= 1;


}




void ADC_config ()
{

	RCC_ADCCLKConfig(RCC_PCLK2_Div8);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);


	ADC_InitTypeDef ADC_InitStructure;
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
	ADC_InitStructure.ADC_NbrOfChannel = 2;
	ADC_InitStructure.ADC_ScanConvMode = ENABLE;
	ADC_Init(ADC1, &ADC_InitStructure);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_239Cycles5);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 1, ADC_SampleTime_239Cycles5);
	ADC_Cmd(ADC1, ENABLE);
	ADC_DMACmd(ADC1,ENABLE);

	ADC_ResetCalibration(ADC1);
	while(ADC_GetResetCalibrationStatus(ADC1));
	ADC_StartCalibration(ADC1);
	while(ADC_GetCalibrationStatus(ADC1));
}




//
//
//
//RCC->AHBENR |=  1;
////	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
//
//DMA_InitTypeDef  DMA_InitStructure;
////	DMA_DeInit(DMA1_Channel1);
//DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
//DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
//DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;
//DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Word;
//DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Word;
//DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
//DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
//	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
//DMA_InitStructure.DMA_BufferSize = 2;
////	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)ADC1_Address;
////	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)data_dst_addr;
//DMA_Init(DMA1_Channel1, &DMA_InitStructure);
//
//
////
////	DMA1_Channel1->CNDTR = 2; // konfigurować przed aktywacją kanału
//
//
//DMA1_Channel1->CPAR = ADC1_Address;
//DMA1_Channel1->CMAR = (uint32_t) data_dst_addr;
//
//
////	DMA_Cmd(DMA1_Channel1, ENABLE);
//DMA1_Channel1->CCR |= 1;
//
//


