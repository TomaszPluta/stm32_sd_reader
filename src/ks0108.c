#include "ks0108.h"
#include "stm32f10x_gpio.h"
#include "font5x8.h"

static GPIO_InitTypeDef GPIO_InitStructure;

//-------------------------------------------------------------------------------------------------
// Delay function /for 8MHz/
//-------------------------------------------------------------------------------------------------
void GLCD_Delay(void)
{
  asm("nop");asm("nop");asm("nop");asm("nop");
  asm("nop");asm("nop");asm("nop");asm("nop");
  asm("nop");asm("nop");asm("nop");asm("nop");
  asm("nop");asm("nop");asm("nop");asm("nop");

  asm("nop");asm("nop");asm("nop");asm("nop");
  asm("nop");asm("nop");asm("nop");asm("nop");
  asm("nop");asm("nop");asm("nop");asm("nop");
  asm("nop");asm("nop");asm("nop");asm("nop");

  asm("nop");asm("nop");asm("nop");asm("nop");
  asm("nop");asm("nop");asm("nop");asm("nop");
  asm("nop");asm("nop");asm("nop");asm("nop");
  asm("nop");asm("nop");asm("nop");asm("nop");

  asm("nop");asm("nop");asm("nop");asm("nop");
  asm("nop");asm("nop");asm("nop");asm("nop");
  asm("nop");asm("nop");asm("nop");asm("nop");
  asm("nop");asm("nop");asm("nop");asm("nop");

  asm("nop");asm("nop");asm("nop");asm("nop");
  asm("nop");asm("nop");asm("nop");asm("nop");
  asm("nop");asm("nop");asm("nop");asm("nop");
  asm("nop");asm("nop");asm("nop");asm("nop");

  asm("nop");asm("nop");asm("nop");asm("nop");
  asm("nop");asm("nop");asm("nop");asm("nop");
  asm("nop");asm("nop");asm("nop");asm("nop");
  asm("nop");asm("nop");asm("nop");asm("nop");


}
//-------------------------------------------------------------------------------------------------
// Enalbe Controller (0-2)
//-------------------------------------------------------------------------------------------------
void GLCD_EnableController(unsigned char controller)
{
switch(controller){
	case 0 : GPIO_ResetBits(KS0108_PORT, KS0108_CS1); break;
	case 1 : GPIO_ResetBits(KS0108_PORT, KS0108_CS2); break;
	case 2 : GPIO_ResetBits(KS0108_PORT, KS0108_CS3); break;
	}
}
//-------------------------------------------------------------------------------------------------
// Disable Controller (0-2)
//-------------------------------------------------------------------------------------------------
void GLCD_DisableController(unsigned char controller)
{
switch(controller){
	case 0 : GPIO_SetBits(KS0108_PORT, KS0108_CS1); break;
	case 1 : GPIO_SetBits(KS0108_PORT, KS0108_CS2); break;
	case 2 : GPIO_SetBits(KS0108_PORT, KS0108_CS3); break;
	}
}
//-------------------------------------------------------------------------------------------------
// Read Status byte from specified controller (0-2)
//-------------------------------------------------------------------------------------------------
unsigned char GLCD_ReadStatus(unsigned char controller)
{
unsigned char status;

GPIO_StructInit(&GPIO_InitStructure);
GPIO_InitStructure.GPIO_Pin = 0xFF << KS0108_D0;
GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
GPIO_Init(KS0108_PORT, &GPIO_InitStructure);

GPIO_SetBits(KS0108_PORT, KS0108_RW);
GPIO_ResetBits(KS0108_PORT, KS0108_RS);
GLCD_EnableController(controller);
GLCD_Delay();
GPIO_SetBits(KS0108_PORT, KS0108_EN);
GLCD_Delay();
status = ((GPIO_ReadInputData(KS0108_PORT) >> KS0108_D0) & 0xFF);
GPIO_ResetBits(KS0108_PORT, KS0108_EN);
GLCD_DisableController(controller);
return status;
}
//-------------------------------------------------------------------------------------------------
// Write command to specified controller
//-------------------------------------------------------------------------------------------------
void GLCD_WriteCommand(unsigned char commandToWrite, unsigned char controller)
{
while(GLCD_ReadStatus(controller)&DISPLAY_STATUS_BUSY);
GPIO_StructInit(&GPIO_InitStructure);
GPIO_InitStructure.GPIO_Pin  = (0xFF << KS0108_D0);
GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
GPIO_Init(KS0108_PORT, &GPIO_InitStructure);

GPIO_ResetBits(KS0108_PORT, KS0108_RS | KS0108_RW);
GLCD_Delay();
GLCD_EnableController(controller);
GLCD_Delay();
GPIO_SetBits(KS0108_PORT, (commandToWrite << KS0108_D0));
commandToWrite ^= 0xFF;
GPIO_ResetBits(KS0108_PORT, (commandToWrite << KS0108_D0));
GLCD_Delay();
GPIO_SetBits(KS0108_PORT, KS0108_EN);
GLCD_Delay();
GPIO_ResetBits(KS0108_PORT, KS0108_EN);
GLCD_Delay();
GLCD_DisableController(controller);
}

//-------------------------------------------------------------------------------------------------
// Read data from current position
//-------------------------------------------------------------------------------------------------
unsigned char GLCD_ReadData(void)
{
unsigned char tmp;
while(GLCD_ReadStatus(screen_x / 64)&DISPLAY_STATUS_BUSY);
GPIO_StructInit(&GPIO_InitStructure);
GPIO_InitStructure.GPIO_Pin = 0xFF << KS0108_D0;
GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
GPIO_Init(KS0108_PORT, &GPIO_InitStructure);

GPIO_SetBits(KS0108_PORT, KS0108_RS | KS0108_RW);

GLCD_EnableController(screen_x / 64);
GLCD_Delay();
GPIO_SetBits(KS0108_PORT, KS0108_EN);
GLCD_Delay();
tmp = ((GPIO_ReadInputData(KS0108_PORT) >> KS0108_D0) & 0xFF);
GPIO_ResetBits(KS0108_PORT, KS0108_EN);
GLCD_DisableController(screen_x / 64);
screen_x++;
return tmp;
}
//-------------------------------------------------------------------------------------------------
// Write data to current position
//-------------------------------------------------------------------------------------------------
void GLCD_WriteData(unsigned char dataToWrite)
{
while(GLCD_ReadStatus(screen_x / 64)&DISPLAY_STATUS_BUSY);

GPIO_StructInit(&GPIO_InitStructure);
GPIO_InitStructure.GPIO_Pin = (0xFF << KS0108_D0);
GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
GPIO_Init(KS0108_PORT, &GPIO_InitStructure);

GPIO_ResetBits(KS0108_PORT, KS0108_RW);
GLCD_Delay();
GPIO_SetBits(KS0108_PORT, KS0108_RS);
GLCD_Delay();
KS0108_PORT->BSRR = CLR_BITS(~((dataToWrite << KS0108_D0)) & KS0108_DATA_PINS) |
					SET_BITS((dataToWrite << KS0108_D0) & KS0108_DATA_PINS);
//if (dataToWrite !=0)
//	GPIO_SetBits(KS0108_PORT, (dataToWrite << KS0108_D0));
//dataToWrite ^= 0xFF;
//if (dataToWrite != 0xff)
//	GPIO_ResetBits(KS0108_PORT, (dataToWrite << KS0108_D0));
GLCD_Delay();
GLCD_EnableController(screen_x / 64);
GLCD_Delay();
GPIO_SetBits(KS0108_PORT, KS0108_EN);
GLCD_Delay();
GPIO_ResetBits(KS0108_PORT, KS0108_EN);
GLCD_Delay();
GLCD_DisableController(screen_x / 64);
screen_x++;
}


//void GLCD_WriteData(unsigned char dataToWrite)
//{
//while(GLCD_ReadStatus(screen_x / 64)&DISPLAY_STATUS_BUSY);
//
//GPIO_StructInit(&GPIO_InitStructure);
//GPIO_InitStructure.GPIO_Pin = (0xFF << KS0108_D0);
//GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
//GPIO_Init(KS0108_PORT, &GPIO_InitStructure);
//
//GPIO_ResetBits(KS0108_PORT, KS0108_RW);
//GLCD_Delay();
//GPIO_SetBits(KS0108_PORT, KS0108_RS);
//GLCD_Delay();
//KS0108_PORT->BSRR = CLR_BITS(~((dataToWrite << KS0108_D0)) & KS0108_DATA_PINS) |
//					SET_BITS((dataToWrite << KS0108_D0) & KS0108_DATA_PINS);
////if (dataToWrite !=0)
////	GPIO_SetBits(KS0108_PORT, (dataToWrite << KS0108_D0));
////dataToWrite ^= 0xFF;
////if (dataToWrite != 0xff)
////	GPIO_ResetBits(KS0108_PORT, (dataToWrite << KS0108_D0));
//GLCD_Delay();
//GLCD_EnableController(screen_x / 64);
//GLCD_Delay();
//GPIO_SetBits(KS0108_PORT, KS0108_EN);
//GLCD_Delay();
//GPIO_ResetBits(KS0108_PORT, KS0108_EN);
//GLCD_Delay();
//GLCD_DisableController(screen_x / 64);
//screen_x++;
//}


//-------------------------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------------------------
void GLCD_InitializePorts(void)
{
RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
GPIO_StructInit(&GPIO_InitStructure);
GPIO_InitStructure.GPIO_Pin   =  GPIO_Pin_All;
GPIO_InitStructure.GPIO_Speed =  GPIO_Speed_2MHz;
GPIO_InitStructure.GPIO_Mode  =  GPIO_Mode_Out_PP;

GPIO_Init(KS0108_PORT, &GPIO_InitStructure);
GPIO_Write(KS0108_PORT, KS0108_CS1 | KS0108_CS2 | KS0108_EN | KS0108_RS | KS0108_RW | (0xFF << KS0108_D0));
}
//-------------------------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------------------------
unsigned char GLCD_ReadByteFromROMMemory(char * ptr)
{
  return *(ptr);
}
//-------------------------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
// Reference to MCU-specific external functions
//-------------------------------------------------------------------------------------------------
extern void GLCD_InitializePorts(void);
extern void GLCD_WriteData(unsigned char);
extern void GLCD_WriteCommand(unsigned char, unsigned char);
extern unsigned char GLCD_ReadData(void);
extern unsigned char GLCD_ReadStatus(unsigned char);
//-------------------------------------------------------------------------------------------------
unsigned char screen_x = 0, screen_y = 0; // screen coordinates
//-------------------------------------------------------------------------------------------------
// Initialization interface and LCD controller
//-------------------------------------------------------------------------------------------------
void GLCD_Initialize(void)
{
unsigned char i;
GLCD_InitializePorts();
for(i = 0; i < 2; i++)
  GLCD_WriteCommand((DISPLAY_ON_CMD | ON), i);
}
//-------------------------------------------------------------------------------------------------
// Sets screen coordinates and address
//-------------------------------------------------------------------------------------------------
void GLCD_GoTo(unsigned char x, unsigned char y)
{
unsigned char i;
screen_x = x;
screen_y = y;

for(i = 0; i < KS0108_SCREEN_WIDTH/64; i++)
  {
  GLCD_WriteCommand(DISPLAY_SET_Y | 0,i);
  GLCD_WriteCommand(DISPLAY_SET_X | y,i);
  GLCD_WriteCommand(DISPLAY_START_LINE | 0,i);
  }
GLCD_WriteCommand((DISPLAY_SET_Y | (x % 64)), (x / 64));
GLCD_WriteCommand((DISPLAY_SET_X | y), (x / 64));
}
//-------------------------------------------------------------------------------------------------
// Sets text coordinates and address
//-------------------------------------------------------------------------------------------------
void GLCD_TextGoTo(unsigned char x, unsigned char y)
{
GLCD_GoTo(x*6,y);
}
//-------------------------------------------------------------------------------------------------
// Clears screen
//-------------------------------------------------------------------------------------------------
void GLCD_ClearScreen(void)
{
unsigned char i, j;
for(j = 0; j < KS0108_SCREEN_HEIGHT/8; j++)
	{
	GLCD_GoTo(0,j);
	for(i = 0; i < KS0108_SCREEN_WIDTH; i++)
 		GLCD_WriteData(0x00);
	}
}

void GLCD_ClearLine(int line)
{
	if (line <0 || line >8)
	{
		return;
	}

unsigned char i;
	GLCD_GoTo(0,line);
	for(i = 0; i < KS0108_SCREEN_WIDTH; i++)
 		GLCD_WriteData(0x00);
}


//-------------------------------------------------------------------------------------------------
// Writes char to screen memory
//-------------------------------------------------------------------------------------------------
void GLCD_WriteChar(char charToWrite)
{
char i;
if (charToWrite == 10){
	screen_y++;
	charToWrite = ' ';
}
charToWrite -= 32;
for(i = 0; i < 5; i++)
  GLCD_WriteData(GLCD_ReadByteFromROMMemory((char *)(font5x8 + (5 * charToWrite) + i)));
GLCD_WriteData(0);
}
//-------------------------------------------------------------------------------------------------
// Write null-terminated string to screen memory
//-------------------------------------------------------------------------------------------------
void GLCD_WriteString(char * stringToWrite)
{
	int i = 0;
while(*stringToWrite && screen_x < 121)
{
 	GLCD_WriteChar(*stringToWrite++);
 	i++;
}
}


/*
void GLCD_WriteString(char * stringToWrite)
{
	int i = 0;
while(*stringToWrite && screen_x < 121)
{
 	GLCD_WriteChar(*stringToWrite++);
 	i++;
}
}*/


//void GLCD_WriteString(char * stringToWrite)
//{
//	int i = 0;
//while(*stringToWrite && i<21){
// 	GLCD_WriteChar(*stringToWrite++);
// 	i++;
//	}
//}



//-------------------------------------------------------------------------------------------------
// Sets or clears pixel at (x,y)
//-------------------------------------------------------------------------------------------------
void GLCD_SetPixel(unsigned char x, unsigned char y, unsigned char color)
{
unsigned char tmp;
GLCD_GoTo(x, y/8);
tmp = GLCD_ReadData();
GLCD_GoTo(x, y/8);
tmp = GLCD_ReadData();
GLCD_GoTo(x, y/8);
if(color)
	tmp |= (1 << (y%8));
else
	tmp &= ~(1 << (y%8));
GLCD_WriteData(tmp);
}
//-------------------------------------------------------------------------------------------------
// Displays bitmap at (x,y) and (dx,dy) size
//-------------------------------------------------------------------------------------------------
void GLCD_Bitmap(char * bmp, unsigned char x, unsigned char y, unsigned char dx, unsigned char dy)
{
unsigned char i, j;
for(j = 0; j < dy / 8; j++)
  {
  GLCD_GoTo(x,y + j);
  for(i = 0; i < dx; i++)
    GLCD_WriteData(GLCD_ReadByteFromROMMemory(bmp++));
  }
}


//-------------------------------------------------------------------------------------------------
// End of file KS0108.c
//-------------------------------------------------------------------------------------------------
