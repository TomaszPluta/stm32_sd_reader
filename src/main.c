
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



xSemaphoreHandle xSemaphoreMuteks;




void vInitializeTasks( void * pvParameters )
{

	GLCD_Initialize();
	GLCD_ClearScreen();
	xSemaphoreMuteks = xSemaphoreCreateMutex();

	for( ;; )
    {};
}




void vTask1( void * pvParameters )
{
	GLCD_Initialize();
	GLCD_ClearScreen();
	xSemaphoreMuteks = xSemaphoreCreateMutex();

    portTickType xLastWakeTime;   xLastWakeTime = xTaskGetTickCount();

	for( ;; )
    {
    	 if(xSemaphoreTake(xSemaphoreMuteks, 4) == pdTRUE)
    	 {
			GLCD_ClearScreen();
			GPIO_SetBits(GPIOB, GPIO_Pin_8);
			GLCD_GoTo(2,2);
			GLCD_WriteString("   Zadanie PIERWSZE  ");
			GLCD_GoTo(2,2);
			GLCD_WriteString("ok");
			 xSemaphoreGive(xSemaphoreMuteks);
			vTaskDelayUntil( &xLastWakeTime, 1000 );

    	 }
    }
}

void vTask2( void * pvParameters )
{

   portTickType xLastWakeTime;
   xLastWakeTime = xTaskGetTickCount();


    for( ;; )
    {
    	 if(xSemaphoreTake(xSemaphoreMuteks, 3) == pdTRUE)
    	    	 {
				GLCD_ClearScreen();
				GPIO_ResetBits(GPIOB, GPIO_Pin_8);
				GLCD_GoTo(2,3);
				GLCD_WriteString("   Zadanie DRUGIE   ");
				GLCD_GoTo(2,3);
				GLCD_WriteString("ok");
				xSemaphoreGive(xSemaphoreMuteks);
				vTaskDelayUntil( &xLastWakeTime, 2000 );

    	    	 }
    }
}






int
main()
{
	init();


  xTaskCreate( vTask1, "Task1", 1000, NULL, tskIDLE_PRIORITY + 2, NULL);
  xTaskCreate( vTask2, "Task2", 1000, NULL, tskIDLE_PRIORITY + 1, NULL);
//  xTaskCreate( vInitializeTasks, "InitializeTasks", 100, NULL,   configMAX_PRIORITIES, NULL);





  vTaskStartScheduler();


  while (1)
  {}
}



