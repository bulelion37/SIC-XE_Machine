
#include "20161598.h"

typedef struct _ESTAB {
	char label[31];
	int addr;
	struct _ESTAB* next;
}ESTAB;	// SYM : struct which store SYMBOL table
ESTAB* estab_head;	//estab_head is pointing head of ESTAB
ESTAB* estab_prev;	//estab_prev is pointing previous node of ESTAB

typedef struct _FILENAME {
	char str_filename[31];
}FILENAME;	//FILENAME : struct which store .obj file name
FILENAME obj_name[3];	//arr which store .obj file name
int obj_name_size=1;		//variable which store number of obj_name

int progaddr;	//variable which store PROGADDR
int csaddr;		//variable which store CSADDR
char str_progaddr[31];	//arr which stores progaddr

void command_progaddr();
void command_loader();

/*------------------------------------------*/
/* func_name : command_porgaddr              */
/* purpose : get PROGADDR   */
/*   	     and store into progaddr variable */
/* return  : none                           */
/*------------------------------------------*/
void command_progaddr()
{
	int i=9, j = 0;		//index of command, index of str_proaddr

	//get file_name through under while statement
	while (1)
	{
		if (command[i] == '\0')
			break;

		if (command[i] == ' ')
		{
			//if there is blank in address, error
			printf("Error : Invalid PROGADDR!\n");
			error_flag = 1;
			return;
		}
		//Valid PROGADDR
		else if(distinct_ascii(command[i]==0))
			str_progaddr[j++] = command[i++];
		else
		{
			//if there is not valid char in address, error
			printf("Error : Invalid PROGADDR!\n");
			error_flag = 1;
			return;
		}
	}
	progaddr = strtol(str_progaddr, NULL, 16);
}

/*------------------------------------------*/
/* func_name : command_loader              */
/* purpose : read obj file and execute linking procedure    */
/*   	     store result into the virtual memory, printout load map */
/* return  : none                           */
/*------------------------------------------*/
void command_loader()
{
	int i=7, j = 0, k=0;		//index of command, index of str_filename, index of obj_name

	//get object file name through under while statement
	while (1)
	{
		if (command[i] == '\0')
			break;
		if (command[i] == ' ')
		{
			k++;
			j = 0;
		}
		obj_name[k].str_filename[j++] = command[i++];
	}

	while (obj_name_size)
	{
		obj_name_size--;
	}
}