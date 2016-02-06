#ifndef _KS0108_H
#define _KS0108_H

#define KS0108_PORT  GPIOC
#define KS0108_DATA_PINS  (GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | \
							GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7)

#define KS0108_RS    GPIO_Pin_8
#define KS0108_RW    GPIO_Pin_9
#define KS0108_EN    GPIO_Pin_10

#define KS0108_CS1   GPIO_Pin_11
#define KS0108_CS2   GPIO_Pin_12
#define KS0108_CS3   GPIO_Pin_13

#define KS0108_D0    0

#define KS0108_SCREEN_WIDTH	128
#define KS0108_SCREEN_HEIGHT	64

#define DISPLAY_SET_Y       0x40
#define DISPLAY_SET_X       0xB8
#define DISPLAY_START_LINE  0xC0
#define DISPLAY_ON_CMD		0x3E
#define ON	0x01
#define OFF	0x00
#define DISPLAY_STATUS_BUSY	0x80

#define SET_BITS(x) ((uint32_t)(x))
#define CLR_BITS(x) ((uint32_t)(x)<<16)


extern unsigned char screen_x;
extern unsigned char screen_y;

void GLCD_Delay(void);
void GLCD_EnableController(unsigned char controller);
void GLCD_DisableController(unsigned char controller);
unsigned char GLCD_ReadStatus(unsigned char controller);
void GLCD_WriteCommand(unsigned char commandToWrite, unsigned char controller);
unsigned char GLCD_ReadData(void);
void GLCD_WriteData(unsigned char dataToWrite);
void GLCD_InitializePorts(void);
unsigned char GLCD_ReadByteFromROMMemory(char * ptr);
void GLCD_Initialize(void);
void GLCD_GoTo(unsigned char x, unsigned char y);
void GLCD_TextGoTo(unsigned char x, unsigned char y);
void GLCD_ClearScreen(void);
void GLCD_WriteChar(char charToWrite);
void GLCD_WriteString(char * stringToWrite);
void GLCD_SetPixel(unsigned char x, unsigned char y, unsigned char color);
void GLCD_Bitmap(char * bmp, unsigned char x, unsigned char y, unsigned char dx, unsigned char dy);
void GLCD_ClearLine(int line);


 typedef struct dataToDisplay_t{
	int line;
	char* data;
}dataToDisplay_t;


#endif
