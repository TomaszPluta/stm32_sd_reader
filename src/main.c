
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
//				volatile  int prepare = (int)  firstLineLcd;
				volatile  char * system = firstLineLcd;
//				while (system > 128)
//					system = system/10;
//				if (system <48)
//					system += 48;
//				prepare =  atoi (&firstLineLcd);
//				volatile char * systemTime   = (char*) &system ;
			 	 GLCD_WriteString(system);
			     xSemaphoreGive(xSemaphoreMuteks);

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
 //  readed_files_t* new_file =  malloc(sizeof(readed_files_t));


    for( ;; )
    {
    //	 if(xSemaphoreTake(xSemaphoreMuteks, 3) == pdTRUE)
    //	    	 {

	//			GPIO_ResetBits(GPIOB, GPIO_Pin_8);
	//			GLCD_GoTo(1,0);
	//			GLCD_WriteString("   Zadanie DRUGIE   ");


					static DIR katalog;
					static FILINFO plik;
					f_opendir(&katalog,"");

				//	int result;
					int i =1;
					do
					{

						f_readdir(&katalog, &plik);

					//	new_file->name =  (char*) malloc( sizeof(plik.fname)+1);
					//	strcpy(new_file->name, plik.fname);


//						char* temp_content;
//						temp_content = SD_open_file(plik.fname);
//						new_file->content =  (char*) malloc( sizeof(temp_content)+1);
//						strcpy (new_file->content, temp_content);
						i++;
    	    	// } while (result != SUCCES || plik.fname[0] == 0);
					} while (i<5);

		//		xSemaphoreGive(xSemaphoreMuteks);
				xQueueSend(xQuSecondLineLCD, (void *) &taskSD, (portTickType) 5);
				vTaskDelayUntil( &xLastWakeTime, 2000 );
    	    	// }
    }
}






void vTaskCheckKey (void)
{

	GLCD_Initialize();
	GLCD_ClearScreen();


    portTickType xLastWakeTime;   xLastWakeTime = xTaskGetTickCount();

    taskKey = "123456789a123456789b123456789c123456789d123456789e123456789f123456789g123456789h123456789i123456789j123456789g123456789h123456789i";

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

//				GLCD_GoTo(0,6);
//				GLCD_WriteString("   Klawisz zwolniony  ");
			}


			 xQueueSend(xQuOtherLinesLCD, (void *) &taskKey, (portTickType) 5);
    		 vTaskDelayUntil( &xLastWakeTime, 600 );

    	 }
    }


void vSystemUpTime (void)
{
	GLCD_Initialize();
	GLCD_ClearScreen();

    portTickType xLastWakeTime;
    xLastWakeTime = xTaskGetTickCount();

    for( ;; )
    {
            //systemTime = (int*) xTaskGetTickCount();
            // itoa ((int*) xTaskGetTickCount(), systemTime, 10 );

    		char * pointerToString;
    		int upTime = xLastWakeTime / 1000;

    		char timeChar[9] = {0};



//    		pobierac czas w sekundach z timera, nie w tickach

    		int secH, secL, minH, minL, hourH, hourL;

    		minH = ((upTime)/600) +48;
    		minL = ((upTime)/60) +48;
    		if (minL >= 60)
    			minL = 0;

			secH = (((upTime)%100)/10) +48;
			secL = ((upTime)%10) +48;
    		if (secH >= 60)
    			secH = 0;


    		timeChar[0] = ((upTime)/600) +48;
    		timeChar[1] = ((upTime)/60) +48;
    		timeChar[2] = ':' ;
    		timeChar[3] = (((upTime)%100)/10) +48;
    		timeChar[4] = ((upTime)%10) +48;



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
	xQuFirstLineLCD = xQueueCreate(4, sizeof(u32));
	xQuSecondLineLCD = xQueueCreate(4, sizeof(u32));
	xQuOtherLinesLCD = xQueueCreate(4, sizeof(u32));

  vTaskStartScheduler();


  while (1)
  {}
}



