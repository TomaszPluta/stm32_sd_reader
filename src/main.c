
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


const char * taskSD = "task sd";
const char * taskKey = "task key";

void vTask1( )
{
	GLCD_Initialize();
	GLCD_ClearScreen();
	SD_mount();

	char* napis;
	char* secondLine;
	char* otherLines;
    portTickType xLastWakeTime;   xLastWakeTime = xTaskGetTickCount();

	for( ;; )
    {

    		 if (xQueueReceive(xQuFirstLineLCD, &napis, (portTickType) 1))
    		 {
				 GLCD_GoTo(0,0);
			 	 GLCD_WriteString(napis);
			     xSemaphoreGive(xSemaphoreMuteks);

			if ( xQueueReceive(xQuSecondLineLCD, &secondLine, (portTickType) 1))
			{
					GLCD_GoTo(0,1);
					GLCD_WriteString(secondLine);
					xSemaphoreGive(xSemaphoreMuteks);
			}

			if ( xQueueReceive(xQuOtherLinesLCD, &otherLines, (portTickType) 1))
			{
				GLCD_GoTo(0,2);
				GLCD_WriteString(otherLines);
			}



    	 	 }
			vTaskDelayUntil( &xLastWakeTime, 1000 );
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
				//	f_opendir(&katalog,"");
//
//				//	int result;
//					int i =1;
//					do
//					{
//
//						f_readdir(&katalog, &plik);
//
//					//	new_file->name =  (char*) malloc( sizeof(plik.fname)+1);
//					//	strcpy(new_file->name, plik.fname);
//
//
////						char* temp_content;
////						temp_content = SD_open_file(plik.fname);
////						new_file->content =  (char*) malloc( sizeof(temp_content)+1);
////						strcpy (new_file->content, temp_content);
//
//
////						GLCD_GoTo(2,i);
////						GLCD_WriteString(plik.fname);
////						GLCD_GoTo(2,5);
////						GLCD_WriteString(temp_content);
//						i++;
//    	    	// } while (result != SUCCES || plik.fname[0] == 0);
//					} while (i<5);

		//		xSemaphoreGive(xSemaphoreMuteks);
				xQueueSend(xQuSecondLineLCD, (void *) &taskSD, (portTickType) 5);
				xQueueSend(xQuFirstLineLCD, (void *) &test2, (portTickType) 5);
				vTaskDelayUntil( &xLastWakeTime, 2000 );
    	    	// }
    }
}






void vTaskCheckKey (void)
{

	GLCD_Initialize();
	GLCD_ClearScreen();


    portTickType xLastWakeTime;   xLastWakeTime = xTaskGetTickCount();

	for( ;; )
    {
//		 if(xSemaphoreTake(xSemaphoreMuteks, 10) == pdTRUE)
//    	 {

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

			 xQueueSend(xQuSecondLineLCD, (void *) &taskKey, (portTickType) 5);
  //  		 xSemaphoreGive(xSemaphoreMuteks);
    		 vTaskDelayUntil( &xLastWakeTime, 1600 );

    	 }
    }

//
//void vSystemUpTime (void)
//{
//	GLCD_Initialize();
//	GLCD_ClearScreen();
//	SD_mount();
//	xSemaphoreMuteks = xSemaphoreCreateMutex();
//
//    portTickType xLastWakeTime;   xLastWakeTime = xTaskGetTickCount();
//
//	for( ;; )
//    {
//    	 if(xSemaphoreTake(xSemaphoreMuteks, 4) == pdTRUE)
//    	 {
//
//			GPIO_SetBits(GPIOB, GPIO_Pin_9);
//			GLCD_GoTo(0,0);
//			GLCD_WriteString("   Zadanie PIERWSZE  ");
//			GLCD_GoTo(2,2);
//	//		GLCD_WriteString("/");
//			 xSemaphoreGive(xSemaphoreMuteks);
//			vTaskDelayUntil( &xLastWakeTime, 1000 );
//
//    	 }
//    }
//}


int
main()
{
	init();


  xTaskCreate( vTask1, "Task1", 3000, NULL, tskIDLE_PRIORITY + 4, NULL);
  xTaskCreate( vTask2, "Task2", 500, NULL, tskIDLE_PRIORITY + 1, NULL);
  xTaskCreate( vTaskCheckKey, "vTaskCheckKey", 200, NULL,   tskIDLE_PRIORITY + 2, NULL);


	xSemaphoreMuteks = xSemaphoreCreateMutex();
	xQuFirstLineLCD = xQueueCreate(4, sizeof(u32));
	xQuSecondLineLCD = xQueueCreate(4, sizeof(u32));
	xQuOtherLinesLCD = xQueueCreate(4, sizeof(u32));

  vTaskStartScheduler();


  while (1)
  {}
}



