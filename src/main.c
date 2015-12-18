
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
QueueHandle_t xQuSecondLineLCD;
QueueHandle_t xQuOtherLinesLCD;

const char * test1 = "01234";
const char * test2 = "56789";
char * systemTime;

const char * taskSD = "task sd";
const char * taskKey = "task key";

void vTask1( )
{
	GLCD_Initialize();
	GLCD_ClearScreen();
	SD_mount();

	char* firstLineLcd;
	char* secondLineLcd;
	char* otherLinesLcd;
    portTickType xLastWakeTime;   xLastWakeTime = xTaskGetTickCount();

	for( ;; )
    {

    		if (xQueueReceive(xQuFirstLineLCD, &firstLineLcd, (portTickType) 5))
    		 {
				 GLCD_GoTo(0,0);
				 GLCD_WriteString("UPTIME: ");
				volatile  char * system = firstLineLcd;
				GLCD_GoTo(75,0);
			 	 GLCD_WriteString(system);
			     xSemaphoreGive(xSemaphoreMuteks);///?? wtf

			if ( xQueueReceive(xQuSecondLineLCD, &secondLineLcd, (portTickType) 1))
			{
					GLCD_GoTo(0,1);
					GLCD_WriteString(secondLineLcd);
					xSemaphoreGive(xSemaphoreMuteks);
			}

			if ( xQueueReceive(xQuOtherLinesLCD, &otherLinesLcd, (portTickType) 1))
			{
				const int startLineOffset = 2;
				const int numberOfLines = 6;
				const int charsInLine = 21;
				int i;
				for (i=0; i<numberOfLines; i++){
					GLCD_GoTo(0,i+startLineOffset);
					GLCD_WriteString((char *)&otherLinesLcd[0+(charsInLine*i)]);
				}
			}
   	 	 }
			vTaskDelayUntil( &xLastWakeTime, 500 );
    }
}

void vTask2()
{

   portTickType xLastWakeTime;
   xLastWakeTime = xTaskGetTickCount();

   char* file_name  =  pvPortMalloc(40*sizeof(char));
   char* file_content =  pvPortMalloc(128* sizeof(char));
 //  char temp_content[128]  = {0};

   int result =0;
   static DIR katalog;
   static FILINFO plik;

   	   f_opendir(&katalog,"");
   	   f_readdir(&katalog, &plik);

    for( ;; )
    {
					//strcpy(file_name, plik.fname);
				//	strncpy (file_name, plik.fname,12);
    				strncpy (file_name, plik.fname,12);

//						temp_content = SD_open_file(plik.fname);
//						new_file->content =  (char*) malloc( sizeof(temp_content)+1);
//						strcpy (new_file->content, temp_content);


//					if (result != SUCCES || plik.fname[0] == 0);
//					} while (i<5)



					if  (xSemaphoreTake(xSemaphoreKeyPressed,0)){
						f_readdir(&katalog, &plik);
					}

				//	temp_content[0] = SD_open_file(file_name);
					strncpy (file_content,  SD_open_file(file_name),128);


				xQueueSend(xQuSecondLineLCD, (void *) &file_name, (portTickType) 5);
				xQueueSend(xQuOtherLinesLCD, (void *) &file_content, (portTickType) 5);
				vTaskDelayUntil( &xLastWakeTime, 2000 );
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
//				GLCD_GoTo(0,6);
//				GLCD_WriteString("   Klawisz wcisnieto  ");

    		 }
			else
			{
				GPIO_SetBits(GPIOB, GPIO_Pin_8);
   			 xSemaphoreGive(xSemaphoreKeyPressed);
//				GLCD_GoTo(0,6);
//				GLCD_WriteString("   Klawisz zwolniony  ");
			}




//			 xQueueSend(xQuSecondLineLCD, (void *) &taskKey, (portTickType) 5);
    		 vTaskDelayUntil( &xLastWakeTime, 200 );

    	 }
    }


void vSystemUpTime (void)
{
	GLCD_Initialize();
	GLCD_ClearScreen();

    portTickType xLastWakeTime;
    xLastWakeTime = xTaskGetTickCount();
	char timeChar[9] = {0};
    int asciOffset =48;

    for( ;; )
    {
            //systemTime = (int*) xTaskGetTickCount();
            // itoa ((int*) xTaskGetTickCount(), systemTime, 10 );

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
    		while (minH > 6)
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

    		pointerToString = &timeChar;

			xQueueSend(xQuFirstLineLCD, (void *) &pointerToString, (portTickType) 1);
			vTaskDelayUntil( &xLastWakeTime, 100 );



    }
}


int
main()
{
	init();


    xTaskCreate( vTask1, "Task1", 3000, NULL, tskIDLE_PRIORITY + 3, NULL);
    xTaskCreate( vTask2, "Task2", 500, NULL, tskIDLE_PRIORITY + 1, NULL);
    xTaskCreate( vTaskCheckKey, "vTaskCheckKey", 200, NULL,   tskIDLE_PRIORITY + 2, NULL);
    xTaskCreate( vSystemUpTime, "vSystemUpTime", 200, NULL,   tskIDLE_PRIORITY + 4, NULL);

	xSemaphoreMuteks = xSemaphoreCreateMutex();
	xSemaphoreKeyPressed = xSemaphoreCreateBinary();
	xQuFirstLineLCD = xQueueCreate(4, sizeof(u32));
	xQuSecondLineLCD = xQueueCreate(4, sizeof(u32));
	xQuOtherLinesLCD = xQueueCreate(4, sizeof(u32));

  vTaskStartScheduler();


  while (1)
  {}
}



