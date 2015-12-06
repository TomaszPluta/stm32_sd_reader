
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
QueueHandle_t xQueueLCD;


void vTask1( void * pvParameters )
{
	GLCD_Initialize();
	GLCD_ClearScreen();
	SD_mount();

	char* napis;
    portTickType xLastWakeTime;   xLastWakeTime = xTaskGetTickCount();

	for( ;; )
    {
    	 if(xSemaphoreTake(xSemaphoreMuteks, 4) == pdTRUE)
    	 {

    		 xQueueReceive(xQueueLCD, &napis, (portTickType) 10);
	//		GPIO_SetBits(GPIOB, GPIO_Pin_8);
			GLCD_GoTo(0,0);
			GLCD_WriteString("   Zadanie PIERWSZE  ");
			GLCD_GoTo(2,2);
			GLCD_WriteString(napis);
			 xSemaphoreGive(xSemaphoreMuteks);
			vTaskDelayUntil( &xLastWakeTime, 1000 );

    	 }
    }
}

void vTask2( void * pvParameters )
{

   portTickType xLastWakeTime;
   xLastWakeTime = xTaskGetTickCount();
 //  readed_files_t* new_file =  malloc(sizeof(readed_files_t));

	char *  tescik;
zrobic tablice charow i do niej wpisywac nazwy plikow


    for( ;; )
    {
    	 if(xSemaphoreTake(xSemaphoreMuteks, 3) == pdTRUE)
    	    	 {

	//			GPIO_ResetBits(GPIOB, GPIO_Pin_8);
				GLCD_GoTo(0,0);
				GLCD_WriteString("   Zadanie DRUGIE   ");


					static DIR katalog;
					static FILINFO plik;
					f_opendir(&katalog,"");

					int result;
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


//						GLCD_GoTo(2,i);
//						GLCD_WriteString(plik.fname);
//						GLCD_GoTo(2,5);
//						GLCD_WriteString(temp_content);
						i++;
    	    	// } while (result != SUCCES || plik.fname[0] == 0);
					} while (i<5);






				xSemaphoreGive(xSemaphoreMuteks);
				//tescik = plik.fname;

				//char * test = "1234567890123";

 to nizej nie dziala, bo nazwa pliku jest na stosie. chyba
				xQueueSend(xQueueLCD, (void *) &plik.fname[0], (portTickType) 10);

				vTaskDelayUntil( &xLastWakeTime, 2000 );
    	    	 }
    }
}






void vTaskCheckKey (void)
{

	GLCD_Initialize();
	GLCD_ClearScreen();


    portTickType xLastWakeTime;   xLastWakeTime = xTaskGetTickCount();

	for( ;; )
    {
		 if(xSemaphoreTake(xSemaphoreMuteks, 10) == pdTRUE)
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
    		 xSemaphoreGive(xSemaphoreMuteks);
    		 vTaskDelayUntil( &xLastWakeTime, 300 );

    	 }
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


  xTaskCreate( vTask1, "Task1", 1500, NULL, tskIDLE_PRIORITY + 2, NULL);
  xTaskCreate( vTask2, "Task2", 1500, NULL, tskIDLE_PRIORITY + 1, NULL);
  xTaskCreate( vTaskCheckKey, "vTaskCheckKey", 200, NULL,   tskIDLE_PRIORITY + 3, NULL);


	xSemaphoreMuteks = xSemaphoreCreateMutex();
	xQueueLCD = xQueueCreate(8, sizeof(u32));

  vTaskStartScheduler();


  while (1)
  {}
}



