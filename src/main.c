
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


char numbers[] =  {'1', '2', '3', '4', '5', '6', '7', '8', '9', '0'};


xSemaphoreHandle xSemaphoreMuteks;


void vTask1( void * pvParameters )
{
    portTickType xLastWakeTime;   xLastWakeTime = xTaskGetTickCount();
	GLCD_Initialize();
	GLCD_ClearScreen();


	 xSemaphoreMuteks = xSemaphoreCreateMutex();

	int i =0;
    for( ;; )
    {
    	 if(xSemaphoreTake(xSemaphoreMuteks, 4) == pdTRUE)
    	 {
			GLCD_ClearScreen();
			GPIO_SetBits(GPIOB, GPIO_Pin_8);
			i++;
			if (i>10) i = 0;
			GLCD_GoTo(2,2);
			GLCD_WriteString("   Zadanie PIERWSZE  ");
			GLCD_GoTo(2,2);
			GLCD_WriteString("ok");
	//    	vTaskDelay(1500 );
			 xSemaphoreGive(xSemaphoreMuteks);
			vTaskDelayUntil( &xLastWakeTime, 500 );

    	 }
    }
}

void vTask2( void * pvParameters )
{

	 xSemaphoreMuteks = xSemaphoreCreateMutex();

    portTickType xLastWakeTime;
   xLastWakeTime = xTaskGetTickCount();
	int i =0;
    for( ;; )
    {
    	 if(xSemaphoreTake(xSemaphoreMuteks, 3) == pdTRUE)
    	    	 {
				i++;
				if (i>10) i = 0;
				GLCD_ClearScreen();
				GPIO_ResetBits(GPIOB, GPIO_Pin_8);
				GLCD_GoTo(2,3);
				GLCD_WriteString("   Zadanie DRUGIE   ");
				GLCD_GoTo(2,3);
				GLCD_WriteString("ok");
				xSemaphoreGive(xSemaphoreMuteks);
				vTaskDelayUntil( &xLastWakeTime, 800 );

    	    	 }
    }
}






int
main(int argc, char* argv[])
{
	init();



  xTaskCreate( vTask1, "Task1", 1000, NULL, tskIDLE_PRIORITY + 1, NULL);
  xTaskCreate( vTask2, "Task2", 1000, NULL, tskIDLE_PRIORITY + 2, NULL);
  vTaskStartScheduler();


  while (1)
  {}
}



