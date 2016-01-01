
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

		if (xQueueReceive(xQueueLCD, &dataToDisplay, (portTickType) 5)){

				int line = dataToDisplay->line;
				GLCD_ClearLine(line);
				GLCD_GoTo(0, line);
				GLCD_WriteString(dataToDisplay->data);

		     }

			vTaskDelayUntil( &xLastWakeTime, 300 );
    }
}

void vTaskSD()
{

   portTickType xLastWakeTime;
   xLastWakeTime = xTaskGetTickCount();

//   char* file_name  =  pvPortMalloc(40*sizeof(char));//
   char* file_content =  pvPortMalloc(128* sizeof(char));//
 //  char temp_content[128]  = {0};


   dataToDisplay_t * fileNameToLCD  =  pvPortMalloc(sizeof(dataToDisplay_t));
   fileNameToLCD->data = pvPortMalloc(40*sizeof(char));

   dataToDisplay_t * FileContentLinesToLCD[6];
   int i;
   for (i =0; i< 6; i++)
   {
	   FileContentLinesToLCD[i]  =  pvPortMalloc(sizeof(dataToDisplay_t));
	   FileContentLinesToLCD[i]->data = pvPortMalloc(22*sizeof(char));
   }


   int result =0;
   static DIR katalog;
   static FILINFO plik;

   	   f_opendir(&katalog,"");
   	   f_readdir(&katalog, &plik);

   	   int a=55;
    for( ;; )
    {

					if (plik.fname[0] == 0){
					   	   f_opendir(&katalog,"");
					   	   f_readdir(&katalog, &plik);

					}

					if  (xSemaphoreTake(xSemaphoreKeyPressed,0)){
						f_readdir(&katalog, &plik);
					}
    				fileNameToLCD->line =1;
	    			strncpy (fileNameToLCD->data, plik.fname,12);
    				xQueueSend(xQueueLCD, (void *) &fileNameToLCD, (portTickType) 10);

    				strncpy (file_content,  SD_open_file(plik.fname),128);
    				const int startLineOffset = 2;
    				const int numberOfLines = 6;
    				const int charsInLine = 21;

    				int i;
    				for (i=0; i<numberOfLines; i++){
    					memset(FileContentLinesToLCD[i]->data, 0 , 22*sizeof(char));
    					strncpy (FileContentLinesToLCD[i]->data,  (char *)&file_content[0+(charsInLine*i)],21);
    					FileContentLinesToLCD[i]->line = startLineOffset + i;
    					if (xQueueSend(xQueueLCD, (void *) &FileContentLinesToLCD[i], (portTickType) 15) == errQUEUE_FULL)
    					{
    						a= 1;
    					}
    					else
    					{
    						a=0;
    					}

    				}


    				vTaskDelayUntil( &xLastWakeTime, 900 );

    }
}






void vTaskCheckKey (void)
{

	//GLCD_Initialize();
//	GLCD_ClearScreen();

    portTickType xLastWakeTime;
    xLastWakeTime = xTaskGetTickCount();

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
		vTaskDelayUntil( &xLastWakeTime, 800 );

    }
}


int
main()
{
	init();


    xTaskCreate( vTaskLCD, "LCD", 250, NULL, tskIDLE_PRIORITY + 4, NULL);
    xTaskCreate( vTaskSD, "SD", 500, NULL, tskIDLE_PRIORITY + 2, NULL);
    xTaskCreate( vTaskCheckKey, "vTaskCheckKey", 100, NULL,   tskIDLE_PRIORITY + 1, NULL);
    xTaskCreate( vSystemUpTime, "vSystemUpTime", 200, NULL,   tskIDLE_PRIORITY + 3, NULL);

	xSemaphoreMuteks = xSemaphoreCreateMutex();
	xSemaphoreKeyPressed = xSemaphoreCreateBinary();
//	xSemaphoreKeyPressed = xSemaphoreCreateCounting(5,0);
//	xQuFirstLineLCD = xQueueCreate(4, sizeof(u32));
//	xQuSecondLineLCD = xQueueCreate(4, sizeof(u32));
//	xQuOtherLinesLCD = xQueueCreate(4, sizeof(u32));
	xQueueLCD = xQueueCreate(32, sizeof(dataToDisplay_t));


  vTaskStartScheduler();


  while (1)
  {}
}



