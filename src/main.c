
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "stm32f10x.h"
#include "font5x8.h"
#include "ks0108.h"
#include "sd_stm32.h"

/*FreeRTOS includes*/
#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "semphr.h"
#include "queue.h"
#include "initialize.h"




xSemaphoreHandle xSemaphoreMuteks;
xSemaphoreHandle xSemaphoreKeyPressed;
xSemaphoreHandle xSemaphoreKeyScroll;
QueueHandle_t xQuFirstLineLCD;
QueueHandle_t xQueueLCD;
QueueHandle_t xQuSecondLineLCD;
QueueHandle_t xQuOtherLinesLCD;


char * napis_src = "kobylka";
char * napis_dst_filled;

void vTaskLCD( )
{
	GLCD_Initialize();
	GLCD_ClearScreen();
	SD_mount();

	dataToDisplay_t *dataToDisplay;
    portTickType xLastWakeTime;   xLastWakeTime = xTaskGetTickCount();

	for( ;; )
    {

		while (xQueueReceive(xQueueLCD, &dataToDisplay, (portTickType) 0)){

				int line = dataToDisplay->line;
				GLCD_ClearLine(line);
				GLCD_GoTo(0, line);
				GLCD_WriteString(dataToDisplay->data);
//				GLCD_WriteString(napis_dst_filled);
		     }

			vTaskDelayUntil( &xLastWakeTime, 400 );
    }
}

void vTaskSD()
{

	const int startLineOffset = 2;
	const int charsInLine = 21;
	const int numberOfLines =6;
	int  numberOfFiles = 6;
	int laste_readed_line=0;
	int i;
	int selected_line =0, line_to_read =  0;

   dataToDisplay_t * fileNameToLCD  =  pvPortMalloc(sizeof(dataToDisplay_t));
   fileNameToLCD->data = pvPortMalloc(40*sizeof(char));
   char* file_content =  pvPortMalloc(128* sizeof(char));
   dataToDisplay_t * FileContentLinesToLCD[6];
   for (i =0; i< 6; i++)
   {
	   FileContentLinesToLCD[i]  =  pvPortMalloc(sizeof(dataToDisplay_t));
	   FileContentLinesToLCD[i]->data = pvPortMalloc(22*sizeof(char));
   }
   dataToDisplay_t * FileNamesLinesToLCD[6];
   for (i =0; i< 6; i++)
   {
	   FileNamesLinesToLCD[i]  =  pvPortMalloc(sizeof(dataToDisplay_t));
	   FileNamesLinesToLCD[i]->data = pvPortMalloc(23*sizeof(char));
	   FileNamesLinesToLCD[i]->line = startLineOffset + i;
   }
   char* file_name_buff =  pvPortMalloc(23* sizeof(char));
	file_name_buff[0] = '>';
	file_name_buff[1] = '>';

   static DIR katalog;
   static FILINFO plik;
   char    bufor[128]={0};
   xSemaphoreGive(xSemaphoreKeyPressed);
   int fileOpened =0;
    for( ;; )
    {
		if  (xSemaphoreTake(xSemaphoreKeyPressed,0)){
			if (selected_line > numberOfFiles){
				selected_line = 0;
			}
			fileOpened = false;

			f_opendir(&katalog,"");
			for (i=0; i<= numberOfLines; i++){
			    f_readdir(&katalog, &plik);
				strncpy (FileNamesLinesToLCD[i]->data, plik.fname,21);
				FileNamesLinesToLCD[i]->line = startLineOffset + i;
				if (plik.fname[0] == 0){
					numberOfFiles = i-1;
					break;
				}
			}
			for (i =0; i<22; i++){
				file_name_buff[i+2] =  FileNamesLinesToLCD[selected_line]->data[i];
			}
			strncpy (FileNamesLinesToLCD[selected_line]->data, file_name_buff,21);

			for (i=0; i<numberOfLines; i++){
			xQueueSend(xQueueLCD, (void *) &FileNamesLinesToLCD[i], (portTickType) 5);
			}
			laste_readed_line =0;
			selected_line++;
		}

		if  (xSemaphoreTake(xSemaphoreKeyScroll,0)){
			line_to_read = selected_line -1;

			FIL    active_file;
			if (fileOpened != true){
			f_open(&active_file,(char*) &FileNamesLinesToLCD[line_to_read]->data[2], FA_READ);// sprobowac bez & i rzuta
			fileOpened = true;
			}
			for (i =0; i <numberOfLines; i++){

			f_gets(file_content, 127, &active_file);

			strncpy (FileContentLinesToLCD[i]->data, file_content,21);
			FileContentLinesToLCD[i]->line = startLineOffset + i;
			xQueueSend(xQueueLCD, (void *) &FileContentLinesToLCD[i], (portTickType) 15);
			}


		}

		vTaskDelay(100 );
    }
}


void vTaskKeys (void)
{
    portTickType xLastWakeTime;
    xLastWakeTime = xTaskGetTickCount();

 	for( ;; )
    {
		if (!(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_9))) {
			GPIO_SetBits(GPIOB, GPIO_Pin_8);
			xSemaphoreGive(xSemaphoreKeyPressed);
		}

		if (!(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_10))) {
			GPIO_SetBits(GPIOB, GPIO_Pin_8);
			xSemaphoreGive(xSemaphoreKeyScroll);
		}

		if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_9)
		&&  GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_10) ) {
			GPIO_ResetBits(GPIOB, GPIO_Pin_8);
		}

		vTaskDelayUntil( &xLastWakeTime, 220 );
    }
}



void vdummytask (void)
{
 portTickType xLastWakeTime;
    xLastWakeTime = xTaskGetTickCount();
		for( ;; )	{
			vTaskDelayUntil( &xLastWakeTime, 800 );
		    }
}



void vTaskTime (void)
{
    portTickType xLastWakeTime;
    xLastWakeTime = xTaskGetTickCount();

	char * uptime = pvPortMalloc(21*sizeof(char));
	char * napis =  "Uptime:     ";

    dataToDisplay_t * dataToSend  =  pvPortMalloc(sizeof(dataToDisplay_t));
    dataToSend->data = pvPortMalloc(40*sizeof(char));

    for( ;; )
    {
    	int upTime = xLastWakeTime / 1000;
    	int sec, min, hour;

    	sec = upTime % 60;
    	min = upTime / 60;
    	hour = min / 60;
    	min =min % 60;

    	sprintf (uptime, "%s %02i:%02i:%02i", napis, hour, min, sec);
  		dataToSend->data = uptime;
    	xQueueSend(xQueueLCD, (void *) &dataToSend, (portTickType) 1);
		vTaskDelayUntil( &xLastWakeTime, 1000 );
    }
}



void vTaskTemp (void)
{
    portTickType xLastWakeTime;
    xLastWakeTime = xTaskGetTickCount();

	ADC_SoftwareStartConvCmd(ADC1, ENABLE);


    dataToDisplay_t* dataToSend  =  pvPortMalloc(sizeof(dataToDisplay_t));
    dataToSend->data = pvPortMalloc(22*sizeof(char));
	char * temp = pvPortMalloc(21*sizeof(char));
	char* napis =  "Temp:     ";
	int adc;


    for( ;; )
    {
    	adc = ADC_GetConversionValue(ADC1);
    	sprintf (temp, "%s %d", napis, adc);
  		dataToSend->data = temp;
  		dataToSend->line = 1;
    	xQueueSend(xQueueLCD, (void *) &dataToSend, (portTickType) 1);
		vTaskDelayUntil( &xLastWakeTime, 900 );
    }
}




void main()
{
   init();



//   DMA_config(table_src, table_dst);

    xTaskCreate( vTaskLCD, "LCD", 50, NULL, tskIDLE_PRIORITY + 2, NULL);
    xTaskCreate( vTaskSD, "SD", 500, NULL, tskIDLE_PRIORITY + 1, NULL);
    xTaskCreate( vTaskTemp, "vTaskTemp", 200, NULL,   tskIDLE_PRIORITY + 5, NULL);
    xTaskCreate( vTaskKeys, "vTaskKeys", 100, NULL,   tskIDLE_PRIORITY + 4, NULL);
    xTaskCreate( vTaskTime, "vTaskTime", 200, NULL,   tskIDLE_PRIORITY + 3, NULL);

	xSemaphoreMuteks = xSemaphoreCreateMutex();
	xSemaphoreKeyPressed = xSemaphoreCreateBinary();
	xSemaphoreKeyScroll = xSemaphoreCreateBinary();
	xQueueLCD = xQueueCreate(32, sizeof(dataToDisplay_t));


  vTaskStartScheduler();

  while (1)
  {}
}



