
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

const char * test1 = "01234";
const char * test2 = "56789";
char * systemTime;

const char * taskSD = "task sd";
const char * taskKey = "task key";

void vTaskLCD( )
{
	GLCD_Initialize();
	GLCD_ClearScreen();
	SD_mount();

	char* firstLineLcd;
	char* secondLineLcd;
	char* otherLinesLcd;
	dataToDisplay_t *dataToDisplay;
    portTickType xLastWakeTime;   xLastWakeTime = xTaskGetTickCount();

	for( ;; )
    {

		while (xQueueReceive(xQueueLCD, &dataToDisplay, (portTickType) 0)){

				int line = dataToDisplay->line;
				GLCD_ClearLine(line);
				GLCD_GoTo(0, line);
				GLCD_WriteString(dataToDisplay->data);

		     }

			vTaskDelayUntil( &xLastWakeTime, 400 );
    }
}

void vTaskSD()
{

   portTickType xLastWakeTime;
   xLastWakeTime = xTaskGetTickCount();

	const int startLineOffset = 2;
	const int numberOfLines = 6;
	const int charsInLine = 21;
	int i;

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
	   FileNamesLinesToLCD[i]->data = pvPortMalloc(22*sizeof(char));
   }



   static DIR katalog;
   static FILINFO plik;

   xSemaphoreGive(xSemaphoreKeyPressed);
   xSemaphoreGive(xSemaphoreKeyScroll);


   f_opendir(&katalog,"");


	int laste_readed_line=0;

    for( ;; )
    {

		if  (xSemaphoreTake(xSemaphoreKeyPressed,0)){
//			f_readdir(&katalog, &plik);





			for (i=0; i<numberOfLines; i++){

				   f_readdir(&katalog, &plik);
				memset(FileNamesLinesToLCD[i]->data, 0 , 22*sizeof(char));
				strncpy (FileNamesLinesToLCD[i]->data, plik.fname,21);
				FileNamesLinesToLCD[i]->line = startLineOffset + i;
				xQueueSend(xQueueLCD, (void *) &FileNamesLinesToLCD[i], (portTickType) 5);
				laste_readed_line++;
			}



//
//					if (plik.fname[0] == 0){
//					   	   f_opendir(&katalog,"");
//					   	   f_readdir(&katalog, &plik);
//					}
//
//
//    				fileNameToLCD->line =1;
//	    			strncpy (fileNameToLCD->data, plik.fname,127);
//    				xQueueSend(xQueueLCD, (void *) &fileNameToLCD, (portTickType) 10);
//
//    				strncpy (file_content,  SD_read_file(plik.fname, 0),128);
////
////    				for (i=0; i<numberOfLines; i++){//
////    					memset(FileContentLinesToLCD[i]->data, 0 , 22*sizeof(char));
////    					strncpy (FileContentLinesToLCD[i]->data, (char *)&file_content[0+(charsInLine*i)],21);
////    					FileContentLinesToLCD[i]->line = startLineOffset + i;
////    					xQueueSend(xQueueLCD, (void *) &FileContentLinesToLCD[i], (portTickType) 15);


    				laste_readed_line= 0;
    				xSemaphoreGive(xSemaphoreKeyScroll);
		}



//		if  (xSemaphoreTake(xSemaphoreKeyScroll,0)){
//
//
//
//						strncpy (file_content,  SD_read_file(plik.fname, laste_readed_line * 127), 128);
//
//	    				for (i=0; i<numberOfLines; i++){
//	    					memset(FileContentLinesToLCD[i]->data, 0 , 22*sizeof(char));
//	    					strncpy (FileContentLinesToLCD[i]->data, (char *)&file_content[0+(charsInLine*i)],21);
//	    					FileContentLinesToLCD[i]->line = startLineOffset + i;
//	    					xQueueSend(xQueueLCD, (void *) &FileContentLinesToLCD[i], (portTickType) 15);
//	    					laste_readed_line++;
//			}
//		}

		vTaskDelay(100 );
    }
}






void vTaskCheckKey (void)
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

		 vTaskDelayUntil( &xLastWakeTime, 300 );
    }
}



void vdummytask (void)
{
 portTickType xLastWakeTime;
    xLastWakeTime = xTaskGetTickCount();
		for( ;; )
		{
			vTaskDelayUntil( &xLastWakeTime, 800 );
		    }

}



void vSystemUpTime (void)
{
    portTickType xLastWakeTime;
    xLastWakeTime = xTaskGetTickCount();

    char *timeChar =  pvPortMalloc(9*sizeof(char));
    int asciOffset =48;

	char * uptime = pvPortMalloc(21*sizeof(char));
	char * napis =  "UPTIME:     ";

    dataToDisplay_t * dataToSend  =  pvPortMalloc(sizeof(dataToDisplay_t));
    dataToSend->data = pvPortMalloc(40*sizeof(char));


    for( ;; )
    {
    		char * pointerToString;
    		int upTime = xLastWakeTime / 1000;

    		// ustawianie czasu na  semaforach: jesli jest binarny to zwieksz

    		int secH, secL, minH, minL, hourH, hourL;

    		hourH = ((upTime)/36000);
			hourL =((upTime)/3600);
			while (hourL > 9)
				hourL = hourL / 10;

    		minH = ((upTime)/600);
    		minL = ((upTime)/60);
    		while (minH > 5)
    			minH = minH / 10;

    		while (minL > 9)
    			minL = minL / 10;

			secH = (((upTime)%60)/10);
    		while (secH > 6)
    			secH = secH / 10;

			secL = ((upTime)%60);
    		while (secL > 9)
    			secL = secL % 10;


    		timeChar[0] = hourH  + asciOffset;
    		timeChar[1] = hourL  + asciOffset;
    		timeChar[2] = ':' ;
    		timeChar[3] = minH + asciOffset;
    		timeChar[4] = minL + asciOffset;
    		timeChar[5] = ':' ;
    		timeChar[6] = secH  + asciOffset;
    		timeChar[7] = secL  + asciOffset;
    		timeChar[8] = 0;


    		dataToSend->line = 0;
    		dataToSend->data = timeChar;

   		sprintf(uptime, "%s %s", napis, timeChar);

  		dataToSend->data = uptime;
    	xQueueSend(xQueueLCD, (void *) &dataToSend, (portTickType) 1);
		vTaskDelayUntil( &xLastWakeTime, 1000 );

		int a;
		a = get_fattime;
    }
}




//i inne z elektrody
//
//
//do tego ioctl
//
//int
main()
{
	init();


    xTaskCreate( vTaskLCD, "LCD", 50, NULL, tskIDLE_PRIORITY + 2, NULL);
    xTaskCreate( vTaskSD, "SD", 500, NULL, tskIDLE_PRIORITY + 1, NULL);
    xTaskCreate( vTaskCheckKey, "vTaskCheckKey", 100, NULL,   tskIDLE_PRIORITY + 4, NULL);
    xTaskCreate( vSystemUpTime, "vSystemUpTime", 200, NULL,   tskIDLE_PRIORITY + 3, NULL);


    xTaskCreate( vdummytask, "vdummytask", 1000, NULL,   tskIDLE_PRIORITY + 7, NULL);


	xSemaphoreMuteks = xSemaphoreCreateMutex();
	xSemaphoreKeyPressed = xSemaphoreCreateBinary();
	xSemaphoreKeyScroll = xSemaphoreCreateBinary();
	xQueueLCD = xQueueCreate(32, sizeof(dataToDisplay_t));

  vTaskStartScheduler();

  while (1)
  {}
}



