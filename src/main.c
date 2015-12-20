
#include <stdio.h>
#include <stdlib.h>

#include <string.h>
#include "stm32f10x.h"


#include "FreeRTOSConfig.h"

/*Kernel includes*/
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "semphr.h"
#include "queue.h"
#include "initialize.h"

#include "font5x8.h"
#include "ks0108.h"

#include "sd_stm32.h"





xSemaphoreHandle xSemaphoreMuteks;
xSemaphoreHandle xSemaphoreKeyPressed;

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

		if (xQueueReceive(xQueueLCD, &dataToDisplay, (portTickType) 0)){

				int line = dataToDisplay->line;
				GLCD_ClearLine(line);
				GLCD_GoTo(0, line);
				GLCD_WriteString(dataToDisplay->data);

		     }

//
//    		if (xQueueReceive(xQuFirstLineLCD, &firstLineLcd, (portTickType) 5))
//    		 {
//				 GLCD_GoTo(0,0);
//				 GLCD_WriteString("UPTIME: ");
//				 GLCD_GoTo(75,0);
//			 	 GLCD_WriteString(firstLineLcd);
//			if ( xQueueReceive(xQuSecondLineLCD, &secondLineLcd, (portTickType) 1))
//			{
//					GLCD_GoTo(0,1);
//					GLCD_ClearLine(1);
//					GLCD_WriteString(secondLineLcd);
//			}
//
//			if ( xQueueReceive(xQuOtherLinesLCD, &otherLinesLcd, (portTickType) 1))
//			{
//				const int startLineOffset = 2;
//				const int numberOfLines = 6;
//				const int charsInLine = 21;
//				int i;
//				for (i=0; i<numberOfLines; i++){
//					GLCD_ClearLine(i+startLineOffset);
//					GLCD_GoTo(0,i+startLineOffset);
//					GLCD_WriteString((char *)&otherLinesLcd[0+(charsInLine*i)]);
//				}
//			}
//   	 	 }
			vTaskDelayUntil( &xLastWakeTime, 300 );
    }
}

void vTaskSD()
{

   portTickType xLastWakeTime;
   xLastWakeTime = xTaskGetTickCount();

   char* file_name  =  pvPortMalloc(40*sizeof(char));
   char* file_content =  pvPortMalloc(128* sizeof(char));
 //  char temp_content[128]  = {0};


   dataToDisplay_t * dataToSend  =  pvPortMalloc(sizeof(dataToDisplay_t));
   dataToSend->data = pvPortMalloc(40*sizeof(char));
 //  dataToSend->line = pvPortMalloc(sizeof(int));


   int result =0;
   static DIR katalog;
   static FILINFO plik;

   	   f_opendir(&katalog,"");
   	   f_readdir(&katalog, &plik);

    for( ;; )
    {
					//strcpy(file_name, plik.fname);
				//	strncpy (file_name, plik.fname,12);


//						temp_content = SD_open_file(plik.fname);
//						new_file->content =  (char*) malloc( sizeof(temp_content)+1);
//						strcpy (new_file->content, temp_content);

 //   				strncpy (file_name, plik.fname,12);
    				strncpy (dataToSend->data, plik.fname,12);
    				dataToSend->line =3;

					if (plik.fname[0] == 0){
					   	   f_opendir(&katalog,"");
					   	   f_readdir(&katalog, &plik);
		    			   strncpy (file_name, plik.fname,12);
					}



					if  (xSemaphoreTake(xSemaphoreKeyPressed,0)){
						f_readdir(&katalog, &plik);
					}


					strncpy (file_content,  SD_open_file(file_name),128);

				xQueueSend(xQueueLCD, (void *) &dataToSend, (portTickType) 5);

//				xQueueSend(xQuSecondLineLCD, (void *) &file_name, (portTickType) 5);
//				xQueueSend(xQuOtherLinesLCD, (void *) &file_content, (portTickType) 5);
				vTaskDelayUntil( &xLastWakeTime, 800 );
    	    	// }
    }
}






void vTaskCheckKey (void)
{

	GLCD_Initialize();
	GLCD_ClearScreen();





    portTickType xLastWakeTime;
    xLastWakeTime = xTaskGetTickCount();

    taskKey = "123456789a123456789b123456789c123456789d123456789e123456789f123456789g123456789h123456789i123456789j123456789g123456789h123456789i";
    taskKey = "task2: keys_handling         ";
	for( ;; )
    {
    		 if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_9))
    		 {
    			 GPIO_ResetBits(GPIOB, GPIO_Pin_8);
    		 }
			else
			{
				GPIO_SetBits(GPIOB, GPIO_Pin_8);
   			 xSemaphoreGive(xSemaphoreKeyPressed);
			}
    		 vTaskDelayUntil( &xLastWakeTime, 100 );

    	 }
    }


void vSystemUpTime (void)
{
	GLCD_Initialize();
	GLCD_ClearScreen();

    portTickType xLastWakeTime;
    xLastWakeTime = xTaskGetTickCount();
//	char timeChar[9] = {0,1,2,3,4,5,6,7,8};
//	char* file_name  =  pvPortMalloc(40*sizeof(char));
    char *timeChar =  pvPortMalloc(9*sizeof(char));
    int asciOffset =48;

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

//    		xQueueSend(xQuFirstLineLCD, (void *) &timeChar, (portTickType) 1);
			vTaskDelayUntil( &xLastWakeTime, 100 );

    }
}


int
main()
{
	init();


    xTaskCreate( vTaskLCD, "Task1", 3000, NULL, tskIDLE_PRIORITY + 3, NULL);
    xTaskCreate( vTaskSD, "Task2", 500, NULL, tskIDLE_PRIORITY + 1, NULL);
    xTaskCreate( vTaskCheckKey, "vTaskCheckKey", 200, NULL,   tskIDLE_PRIORITY + 2, NULL);
    xTaskCreate( vSystemUpTime, "vSystemUpTime", 200, NULL,   tskIDLE_PRIORITY + 4, NULL);

	xSemaphoreMuteks = xSemaphoreCreateMutex();
	xSemaphoreKeyPressed = xSemaphoreCreateBinary();
//	xSemaphoreKeyPressed = xSemaphoreCreateCounting(5,0);
//	xQuFirstLineLCD = xQueueCreate(4, sizeof(u32));
//	xQuSecondLineLCD = xQueueCreate(4, sizeof(u32));
//	xQuOtherLinesLCD = xQueueCreate(4, sizeof(u32));
	xQueueLCD = xQueueCreate(8, sizeof(dataToDisplay_t));


  vTaskStartScheduler();


  while (1)
  {}
}



