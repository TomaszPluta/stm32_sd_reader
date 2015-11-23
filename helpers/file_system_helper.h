/*
 * globals.h
 *
 *  Created on: 5 paź 2015
 *      Author: tomek
 */
#ifndef GLOBALS_H_
#define GLOBALS_H_


#define SUCCES  0
#define FAIL   -1





	typedef struct readed_files{
		char * name;
		char * content;
		struct readed_files * next;
	}readed_files_t;



/*
	typedef struct node	{
			void* data;
			struct node* next;
		}node_t;
*/

#endif /* GLOBALS_H_ */
