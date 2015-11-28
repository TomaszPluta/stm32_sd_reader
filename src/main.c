
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



void vTask1( void * pvParameters )
{
	GLCD_Initialize();
	GLCD_ClearScreen();
	SD_mount();
	xSemaphoreMuteks = xSemaphoreCreateMutex();

    portTickType xLastWakeTime;   xLastWakeTime = xTaskGetTickCount();

	for( ;; )
    {
    	 if(xSemaphoreTake(xSemaphoreMuteks, 4) == pdTRUE)
    	 {

			GPIO_SetBits(GPIOB, GPIO_Pin_8);
			GLCD_GoTo(0,0);
			GLCD_WriteString("   Zadanie PIERWSZE  ");
			GLCD_GoTo(2,2);
	//		GLCD_WriteString("/");
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

				GPIO_ResetBits(GPIOB, GPIO_Pin_8);
				GLCD_GoTo(0,0);
				GLCD_WriteString("   Zadanie DRUGIE   ");


					static DIR katalog;
					static FILINFO plik;
					f_opendir(&katalog,"");

					int result;
					int i =1;
					do
					{
						readed_files_t* new_file =  malloc(sizeof(readed_files_t));

						f_readdir(&katalog, &plik);

						new_file->name =  (char*) malloc( sizeof(plik.fname)+1);
						strcpy(new_file->name, plik.fname);


						char* temp_content;
						temp_content = SD_open_file(plik.fname);
						new_file->content =  (char*) malloc( sizeof(temp_content)+1);
						strcpy (new_file->content, temp_content);


						GLCD_GoTo(2,i);
						GLCD_WriteString(plik.fname);
						GLCD_GoTo(2,5);
						GLCD_WriteString(temp_content);
						i++;
    	    	// } while (result != SUCCES || plik.fname[0] == 0);
					} while (i<4);




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



