


#include "sd_stm32.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "stm32f10x.h"


static FATFS g_sFatFs;

void SD_mount ()
{
	f_mount(0, &g_sFatFs);
}



void files_list_add (readed_files_t* list_head, readed_files_t* new_element)
{
		readed_files_t* current;
		current = list_head;
		while (current->next != NULL)
			current = current->next;

		current->next = new_element;
		new_element->next = NULL;
}


void files_list_print (readed_files_t* list_head)
{
readed_files_t* current;
		current = list_head;
		int i=0;
		while (current->next != NULL)
		{
		current = current->next;
		GLCD_GoTo(0,i);
		GLCD_WriteString(current->name);
		i++;
		}
}



readed_files_t* print_files (void)
{
	int result;
	static DIR katalog;
	static FILINFO plik;
	result = f_opendir(&katalog,"");
	int i = 0;
	readed_files_t* list_head = malloc(sizeof(readed_files_t));
	list_head->next = NULL;
//	list_head = NULL;
	while(1)
	{
		readed_files_t* new_file =  malloc(sizeof(readed_files_t));

		result = f_readdir(&katalog, &plik);

		new_file->name =  (char*) malloc( sizeof(plik.fname)+1);
		strcpy(new_file->name, plik.fname);


		char* temp_content;
		temp_content = SD_read_file(plik.fname, 0);
		new_file->content =  (char*) malloc( sizeof(temp_content)+1);
		strcpy (new_file->content, temp_content);

		files_list_add(list_head, new_file);
		i++;
		if (result != SUCCES || plik.fname[0] == 0)
		{
			return list_head;
		}
	}
	return list_head;
}





char * SD_read_file (char *fileName, UINT position)
{
    char    bufor[128]={0};
    UINT    odczytanychBajtow=0;
    UINT    bajtowDoOdczytu=45;

 //     f_lseek(&plik, position);
	 //  bajtowDoOdczytu=f_size(&plik);

	   if (bajtowDoOdczytu>127) {bajtowDoOdczytu=127;}
//	    char    bufor[128]={0};
//	   f_gets(bufor, bajtowDoOdczytu, &plik);
	   bufor[bajtowDoOdczytu]=0;

	//   char * content = NULL;
	 //  strcpy(content, bufor);
	   return bufor;
}


//char * SD_read_file (char *fileName, UINT position)
//{
//	FRESULT fresult;
//	FIL     plik;
//    char    bufor[128]={0};
//    UINT    odczytanychBajtow=0;
//    UINT    bajtowDoOdczytu=0;
//
//	   fresult = f_open(&plik,fileName, FA_READ);
//       f_lseek(&plik, position);
//	   bajtowDoOdczytu=f_size(&plik);
//	   if (bajtowDoOdczytu>127) {bajtowDoOdczytu=127;}
//	   fresult = f_read(&plik, bufor, bajtowDoOdczytu, &odczytanychBajtow);
//	   bufor[bajtowDoOdczytu]=0;
//	   fresult = f_close (&plik);
//	//   char * content = NULL;
//	 //  strcpy(content, bufor);
//	   return bufor;
//}


