#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <dirent.h>

typedef struct _Node{
	int index;
	char comm[100];
	struct _Node* next;
}Node;	//Node : struct which store history
Node* head;	//head is pointing first of history
Node *prev;	//prev is pointing previous node

typedef struct _Hash {
	char mnemonic[7];
	int value;
	struct _Hash* next;
}Hash;	// Hash : struct which store Hash table
Hash* table[20];	//arr named table storing opcode Hash table

char command[100];	//variable which get input command
int line_num;		//variable which get history's line index
int quit_flag;		//distinct if input gets 'quit', if 1 program end
int error_flag;			//distinct if error occurs
int memory_address;		//variable which stores last printed memory address+16
unsigned char memory[1048576];	//arr which stores memory 


void command_distinct(char* arr);
void command_help();
void command_dir();
void command_history();
void command_dump(int num_flag);
void command_edit();
void edit_add_val(char* arr, int* addr, int* val);
void command_fill();
void fill_addr_val(char* arr, int* start, int* end, int* val);
void command_reset();
int dump_start_end(char* arr, int* start, int* end);
void print_memory(int first_line, int last_line, int dump_start, int dump_end);
void command_quit();
void make_hashtb();
void command_opcodelist();
void command_opcode();
int distinct_ascii(char x);

/*-------------------------------------*/
/* func_name : command_distinct        */
/* purpose : distinct input command,   */
/*   call function related to command  */
/* return  : none                      */
/*-------------------------------------*/
void command_distinct(char* arr)
{
	//if error_flag is 1, wrong commandi input 
	error_flag = 1;

	//Save input command into node and implement as linked list.
	Node* node = (Node*)malloc(sizeof(Node));
	
	//save line information, command information into node.
	node->index = ++line_num;
	strcpy(node->comm, arr);
	node->next = NULL;
	if (head == NULL)
		head = node;
	else
		prev->next = node;
	
	//If help or h entered, call command_help()
	if (!strcmp(arr, "help") || !strcmp(arr, "h"))
	{
		//If properly entered, the error_flag will be zeroed.
		error_flag--;
		command_help();
	}
	//If d or dir entered, call command_dir()
	else if (!strcmp(arr, "d") || !strcmp(arr, "dir"))
	{
		error_flag--;
		command_dir();
	}

	//If q or quit entered, call command_quit()
	else if (!strcmp(arr, "q") || !strcmp(arr, "quit"))
	{
		error_flag--;
		command_quit();
	}
	//If hi or history entered, call command_history()
	else if (!strcmp(arr, "hi") || !strcmp(arr, "history"))
	{
		error_flag--;
		command_history();
	}
	//If du or dump entered, call command_dump()
	else if (!strncmp(arr, "du", 2) || !strncmp(arr, "dump ", 4))
	{
		int dump_flag = 0;	//If a similar string other than du or dump is entered, it becomes 1.
		for (int k = 2; k <= 4; k++)
		{
			if (k == 2 && arr[k] == ' ')
				break;
			else if (k == 2 && arr[k] == '\0')
			{
				error_flag--;
				//only du entered, pass num_flag 1 as parameter to print 10 lines from that memory address
				command_dump(1);
				dump_flag = 1;
			}
			else if (k == 2 && arr[k] == 'm')
				continue;
			else if (k == 3 && arr[k] == 'p')
				continue;
			else if (k == 4 && arr[k] == '\0')
			{
				error_flag--;
				//only dump entered, pass num_flag 1 as parameter to print 10 lines from that memory address
				command_dump(1);
				dump_flag = 1;
			}
			else if (k == 4 && arr[k] == ' ')
				continue;
			else
			{
				//in case an incorrect command has been entered, such as a non-blank character after a dump or du.
				dump_flag = 1;
				break;
			}
		}
		if (!dump_flag)
		{
			//after dump or du, got input at least start index.
			//pass number_flag 0 to calculate start index, end_index.
			error_flag--;
			command_dump(0);
		}
	}
	//If e or edit entered, call command_edit()
	else if (!strncmp(arr, "e", 1) || !strncmp(arr, "edit", 4))
	{
		int edit_flag = 0;	//If a similar string other than e or edit is entered, it becomes 1.
		for (int k = 1; k <= 4; k++)
		{
			if (k == 1 && arr[k] == ' ')
				break;
			else if (k == 1 && arr[k] == 'd')
				continue;
			else if (k == 2 && arr[k] == 'i')
				continue;
			else if (k == 3 && arr[k] == 't')
				continue;
			else if (k == 4 && arr[k] == ' ')
				continue;
			else
			{
				//in case an incorrect command has been entered, such as a non-blank character after a e or edit.
				edit_flag = 1;
				break;
			}

		}
		if (!edit_flag)
		{
			//after e or edit, got address index
			error_flag--;
			command_edit();
		}
	}
	//If f or fill entered, call command_fill()
	else if (!strncmp(arr, "f", 1) || !strncmp(arr, "fill", 4))
	{
		int fill_flag = 0;	//If a similar string other than f or fill is entered, it becomes 1.
		for (int k = 1; k <= 4; k++)
		{
			if (k == 1 && arr[k] == ' ')
				break;
			else if (k == 1 && arr[k] == 'i')
				continue;
			else if (k == 2 && arr[k] == 'l')
				continue;
			else if (k == 3 && arr[k] == 'l')
				continue;
			else if (k == 4 && arr[k] == ' ')
				continue;
			else
			{
				//in case an incorrect command has been entered, such as a non-blank character after a f or fill.
				fill_flag = 1;
				break;
			}
		}
		if (!fill_flag)
		{
			//after f or fill, got address index
			error_flag--;
			command_fill();
		}
	}
	//If reset entered, call command_reset()
	else if (!strcmp(arr, "reset"))
	{
		error_flag--;
		command_reset();
	}
	//If opcodelist entered, call command_opcodelist()
	else if (!strncmp(arr, "opcodelist", 10))
	{
		//if there are no more characters after the ocodelist, if it's right.
		if(arr[10]=='\0'){
		error_flag--;
		command_opcodelist();}
	}
	//If opcode entered, call command_opcode()
	else if (!strncmp(arr, "opcode", 6))
	{
		//if got blank after opcode, got mnemonic input
		if (arr[6] == ' ')
		{
			error_flag--;
			command_opcode();
		}
	}
	//if error_flag is 1 then invalid command, so delete the node from the history.	
	if(error_flag)
	{
		free(node);
		line_num--;	//decrease history line 

		//set head to null again if first input and incorrect command.
		if (prev == NULL)
			head = NULL;
		return;
	}
	//prev is a node that points to a previous one, so insert the current node
	prev = node;
}

/*-------------------------------------*/
/* func_name : command_help            */
/* purpose : print command which can   */
/*   	     use on shell	     	   */
/* return  : none                      */
/*-------------------------------------*/
void command_help()
{
	//if h or help entered, print every command list
	printf("\th[elp]\n\td[ir]\n\tq[uit]\n\thi[story]\n\tdu[mp][start, end]\n\te[dit] address, value\n\tf[ill] start, end, value\n\treset\n\topcode mnemonic\n\topcodelist\n");
}


/*-------------------------------------*/
/* func_name : command_dir             */
/* purpose : print files on current    */
/*   	     directory  	     	   */
/* return  : none                      */
/*-------------------------------------*/
void command_dir()
{
	DIR* dir_info = NULL;
	struct dirent* dir_entry = NULL;
	struct stat buf;
	int num_index=0;	//variable which stores printed file's num
	
	if((dir_info = opendir("."))==NULL) //opening directory
	{
		//print error message
		printf("Error! Directory open error\n");
		error_flag = 1;
		return;
	};	

	//Output all files and directories in the directory
	while ((dir_entry = readdir(dir_info)) != NULL)
	{

		// '.' and '..' point to the current folder and the old folder, so exclude them and search them.
		if ((strcmp(dir_entry->d_name, "..") == 0) || (strcmp(dir_entry->d_name, ".") == 0))
			continue;

		stat(dir_entry->d_name, &buf); //read the file corresponding to the first parameter.
		printf("\t%s",dir_entry->d_name);
		if(S_ISDIR(buf.st_mode))	//print '/' if the file that was read is directory
			printf("/");
		else if(S_IXUSR & buf.st_mode)	//print '*' if the file that was read is exe
			printf("*");
		num_index++;
		if(num_index % 4 ==0)	//print '\n' if 4 files have printed
			printf("\n");
	}
	closedir(dir_info);	//closing directory
	if(num_index % 4 !=0)
		printf("\n");

}

/*-------------------------------------*/
/* func_name : command_history         */
/* purpose : print all input commands  */
/*   	     in order with number  	   */
/* return  : none                      */
/*-------------------------------------*/
void command_history()
{
	//If hi or history entered, print all input commands in order with number
	if (head)
	{
		Node* cur = head;	//cur is pointing current node
		while (1)
		{
			int i = 0;
			
			//print current history node's line index
			printf("\t %d\t", cur->index);
			
			//print current history node's input command
			while(cur->comm[i] != '\0')
				printf("%c", cur->comm[i++]);
			printf("\n");
			
			//move cur to the next node
			cur=cur->next;
			
			//when cur reaches the end of the linked list, exit.
			if (cur == NULL)
				break;
		}
	}
}

/*------------------------------------------------*/
/* func_name : command_dump                       */
/* purpose : distinct start, end index existence, */
/*   	     call print_memory function by case   */
/* return  : none                                 */
/*------------------------------------------------*/
void command_dump(int num_flag)
{
	//only du or dump entered with start, end index
	if(num_flag)
	{		
		//memory address over FFFFF, if 9 added
		if (memory_address > 0xFFF60)
		{
			//variable that stores the number of lines to be printed after print from the present to 0xFFFF0.
			int difference = 8 - (0xFFFF0 - memory_address) / 16;
			
			//print from current memory_address to 0xFFFF0
			print_memory(memory_address, 0xFFFF0, memory_address, 0xFFFFF);
			
			//reset memory_address to 0
			memory_address = 0;	

			//print memory_address 0 to address that added difference
			print_memory(memory_address, memory_address + 16 * difference, memory_address, memory_address + 16 * difference + 15);
		}
		else
			//do not over FFFFF if 9 added, print 10 lines from current memory address
			print_memory(memory_address, memory_address + 16 * 9, memory_address, (memory_address + 10 * 16) - 1);
	}
	//du or dump entered with start or end index
	else
	{
		int dump_start, dump_end;	//dump_start : start index, dump_end : end_index
		int dump_valid = dump_start_end(command, &dump_start, &dump_end); //if -1, wrong memory index entered, 0 : only start index entered, 1 : both start, end entered
		//use dump_start_end to get a number of start and end addr. 
		//If both come in, get 1 return and run the if statement below
		if (dump_valid == 1)
			//If start, end entered, print start to end
			print_memory((dump_start / 16) * 16, (dump_end / 16) * 16, dump_start, dump_end);
		//print error message
		else if(dump_valid == -1)
		{
			printf("Error : wrong memory index!\n ");
			error_flag=1;
			return;
		}
		
		//only start entered, print 10 lines from start index 
		else
		{
			//memory address over FFFFF, if 9 added
			if ((dump_start / 16) * 16 > 0xFFF60)
			{
				//if start index is over memory boundary
				if(dump_start > 0xFFFFF)
				{
					printf("Error : Memory index out of bound\n");
					error_flag=1;
					return;
				}
				else{
				//insert start index's memory address into memory_address
				memory_address = (dump_start / 16) * 16;
				
				//variable that stores the number of lines to be printed after print from the present to 0xFFFF0.
				int difference = 8 - (0xFFFF0 - memory_address) / 16;
				
				//print from current memory_address to 0xFFFF0
				print_memory(memory_address, 0xFFFF0, dump_start, 0xFFFFF);
				
				//reset memory_address to 0
				memory_address = 0;
				
				//print memory_address 0 to address that added difference
				print_memory(memory_address, memory_address + 16 * difference, memory_address, memory_address + 16 * difference + 15);}
			}
			else
				//print 10 lines from start index
				print_memory((dump_start / 16) * 16, (dump_start / 16) * 16 + 16 * 9, dump_start, ((dump_start / 16) + 10) * 16 - 1);
		}
	}
}

/*--------------------------------------------------*/
/* func_name : dump_start_end                       */
/* purpose : distinct existence of start, end index */
/*   	  and change it into hex number from string */
/* return  : both exists : 1, only start index : 0  */
/*--------------------------------------------------*/
int dump_start_end(char* arr, int *start, int *end)
{
	char start_num[10] = {0};	//arr which stores start index
	char end_num[10] = {0};		//arr which stores end index

	int i, j=0;				//i:cur arr index, j:cur start_num, end_num index
	int number_flag = 0;	//both start, end entered : 1, only start entered : 0
	
	//du [start,end]: start with index 3, dump [start, end]: start with index 6.
	if (!strncmp(arr, "dump", 4))
		i = 5;
	else
		i = 3;

	while (arr[i] != '\0')
	{
		if (arr[i] == ',')
		{
			number_flag = 1;
			j = 0;
			i++;
			continue;
		}

		//if ',' is not entered : still start index, number_flag is 0
		if (!number_flag)
		{	
			//if arr[i] is not 0~9, or a~f, A~F : error
			if (distinct_ascii(arr[i]))
			{
				error_flag = 1;
				return -1;
			}
			start_num[j++] = arr[i];
		}
		else{
			//if arr[i] is not 0~9, or a~f, A~F : error
			if(distinct_ascii(arr[i])){
				error_flag=1;
				return -1;
			}
			end_num[j++] = arr[i];}
		i++;
	}
	
	//store start index into *start
	*start = strtol(start_num, NULL, 16);
	
	//if end index is entered
	if (number_flag)
	{
		//store end index into *end
		*end = strtol(end_num, NULL, 16);
		return 1;
	}
	else
		return 0;
}

/*-------------------------------------*/
/* func_name : print_memory            */
/* purpose : print all memories from   */
/*   	     start to end index  	   */
/* return  : none                      */
/*-------------------------------------*/
void print_memory(int first_line, int last_line, int dump_start, int dump_end)
{
	//if out of memory bound, print error message
	if (dump_start < 0 || dump_end >0xFFFFF || dump_start > 0xFFFFF || dump_end < 0)
	{
		printf("Error : Memory index out of bound!\n");
		error_flag = 1;
		return;
	}
	//if start index is biggere than end index, print error message
	else if (dump_start > dump_end)
	{
		printf("Error : Start index is bigger than End index!\n");
		error_flag = 1;
		return;
	}

	int line_start = dump_start;	//variable which stores start index
	for (int hex_memory_line = first_line; hex_memory_line <= last_line; hex_memory_line+=16)
	{
		//print 5 digit memory address
		printf("%.5X ", hex_memory_line);

		//Save corresponding memory address into memory_address at the last line output.
		if (hex_memory_line == last_line)
			memory_address = last_line + 16;
		
		//if memory address is over FFFF0, reset to 0
		if(memory_address > 0xFFFF0)
			memory_address=0;

		int hex_line = hex_memory_line / 16;	//variable which stores hex_memory_line's memory address
		
		//if it is first memory address
		if (first_line/16 == hex_line) {
			//if start is not first of memory address, print blank
			if (dump_start != hex_line * 16)
			{
				for (int k = hex_line*16; k < dump_start; k++)
					printf("   ");
			}
		}

		//if it is not last memory address, print all 16 bytes
		if (hex_line != last_line/16) {
			//print memory by half byte
			for (int k = line_start; k <= hex_line * 16 + 15; k++)
				printf("%.2X ", memory[k]);
		}
		//if it is last memory address
		else if (hex_line == last_line/16)
		{
			//print memory by half byte until end index
			for (int k = line_start; k <= dump_end; k++)
				printf("%.2X ", memory[k]);
			
			//print blank until end of memory address
			for (int k = dump_end + 1; k < hex_line * 16 + 16; k++)
				printf("   ");
		}

		printf("; ");

		//if it is first memory address
		if (first_line/16 == hex_line) {
			//if start is not first of memory address, print 0 ascii code
			if (dump_start != hex_line * 16)
			{
				for (int k = hex_line*16; k < dump_start; k++)
					printf(".");
			}
		}

		//if it is not last memory address, print all 16 bytes
		if (hex_line != last_line/16) {
			//print memory by half byte
			for (int k = line_start; k <= hex_line * 16 + 15; k++)
			{
				if ((int)memory[k] >= 32 && (int)memory[k] <= 126)
					printf("%c", memory[k]);
				//if memory is out of bound, print '.'
				else 	
					printf(".");
			}
		}
		//if it is last memory address
		else if (hex_line == last_line/16)
		{
			//print memory by char until end index
			for (int k = line_start; k <= dump_end; k++)
			{
				if ((int)memory[k] >= 32 && (int)memory[k] <= 126)
					printf("%c", memory[k]);
				else
					printf(".");
			}
			//print '.' until end of memory address
			for (int k = dump_end + 1; k < hex_line * 16 + 16; k++)
				printf(".");
		}

		printf("\n");

		//insert next memory address's first index into line_start
		line_start = (++hex_line) * 16;

	}


}

/*-------------------------------------*/
/* func_name : command_edit            */
/* purpose : insert value into given   */
/*   	     address            	   */
/* return  : none                      */
/*-------------------------------------*/
void command_edit()
{
	int addr = 0;	//variable which stores address
	int val = 0;	//variable which stores value
	
	//call edit_add_val to get information about address and value
	edit_add_val(command, &addr, &val);
	
	//if value out of bound, print error message
	if (val < 0 || val >255)
	{
		printf("Error : Value Error!\n");
		error_flag = 1;
		return;
	}
	
	//if memory out of bound, print error message
	if (addr < 0 || addr>0xFFFFF) {
		printf("Error : Address out of bound!\n");
		error_flag = 1;
		return;
	}
	
	//if right command entered, insert value into address
	if(!error_flag)
		memory[addr] = val;
}

/*-------------------------------------*/
/* func_name : edit_add_val            */
/* purpose : distinct address and value*/
/*   	     after edit command  	   */
/* return  : none                      */
/*-------------------------------------*/
void edit_add_val(char *arr, int *addr, int *val)
{
	int i, number_flag=0, j=0;		//number_flag distinct existence of address&value
	char edit_value[10] = { 0 };	//arr stores value
	char edit_address[10] = { 0 };	//arr stores address

	//if edit entered, start from index 5, if e entered, start from index 2
	if (!strncmp(command, "edit", 4))
		i = 5;
	else
		i = 2;

	while (arr[i] != '\0')
	{
		//detect address or value by ','
		if (arr[i] == ',')
		{
			number_flag++;
			j = 0;
			i++;
			continue;
		}

		//if number_flag is 0, arr[i] is address
		if (!number_flag)
		{
			//if arr[i] is not 0~9, or a~f, A~F : error
			if (distinct_ascii(arr[i]))
			{
				error_flag = 1;
				return;
			}
			edit_address[j++] = arr[i];
		}
		//if number flag is 1 or 2, arr[i] is value
		else if(number_flag == 1 || number_flag == 2)
		{
			//if arr[i] is not 0~9, or a~f, A~F : error
			if (distinct_ascii(arr[i]))
			{
				error_flag = 1;
				return;
			}
			edit_value[j++] = arr[i];
			number_flag = 2;
		}
		else 
		{
			//entered more value than expected
			error_flag = 1;
			return;
		}
		i++;
	}
	//change address from string to hex number
	*addr = strtol(edit_address, NULL, 16);
	
	//if value is not entered, insert -1 into *val
	if (number_flag != 2 && 0 == strtol(edit_value, NULL, 16))
		*val = -1;
	//change value from string to hex number
	else
		*val = strtol(edit_value, NULL, 16);

}

/*-------------------------------------*/
/* func_name : command_fill            */
/* purpose : fill value from start to  */
/*   	     end index          	   */
/* return  : none                      */
/*-------------------------------------*/
void command_fill()
{
	int start, end, val=-1;		//start : start index, end : end index, val : value
	
	//call fill_addr_val to get information about start, end, val
	fill_addr_val(command, &start, &end, &val);

	//value out of bound, print error message
	if (val < 0 || val >255)
	{
		printf("Error : Value error!\n");
		error_flag = 1;
		return;
	}

	//memory out of bound, print error message
	if (start < 0 || end>0xFFFFF) {
		printf("Error : Address out of bound!\n");
		error_flag = 1;
		return;
	}
	//if end index is not entered, print error message
	else if (end == 0 && start != 0)
	{
		printf("Error : Need end index!\n");
		error_flag = 1;
		return;
	}
	//if start is bigger than end, print error message
	else if (start > end)
	{
		printf("Error : Start index is bigger than End index!\n");
		error_flag = 1;
		return;
	}
	//if right command entered, fill value from start to end index
	if(!error_flag){
		for(int k=start; k<=end; k++)
		memory[k] = val;}
}

/*----------------------------------------*/
/* func_name : fill_addr_val              */
/* purpose : distinct start, end index    */
/*   	     and value after fill command */
/* return  : none                         */
/*----------------------------------------*/
void fill_addr_val(char* arr, int*start, int*end, int* val)
{
	int i, number_flag = 0, j = 0;
	char fill_val[10] = { 0 };
	char fill_start[10] = { 0 };
	char fill_end[10] = { 0 };

	//if fill entered, start from index 5, if f entered, start from index 2
	if (!strncmp(command, "fill", 4))
		i = 5;
	else
		i = 2;

	while (arr[i] != '\0')
	{
		//detect address or value by ','
		if (arr[i] == ',')
		{
			number_flag++;
			j = 0;
			i++;
			continue;
		}

		//if number_flag is 0, arr[i] is start index
		if (number_flag == 0)
		{
			//if arr[i] is not 0~9, or a~f, A~F : error
			if (distinct_ascii(arr[i]))
			{
				error_flag = 1;
				return;
			}
			fill_start[j++] = arr[i];
		}
		//if number_flag is 1, arr[i] is end index
		else if (number_flag == 1)
		{
			//if arr[i] is not 0~9, or a~f, A~F : error
			if (distinct_ascii(arr[i]))
			{
				error_flag = 1;
				return;
			}
			fill_end[j++] = arr[i];
		}
		//if number_flag is 2, arr[i] is value
		else if(number_flag ==2)
		{	
			//if arr[i] is not 0~9, or a~f, A~F : error
			if (distinct_ascii(arr[i]))
			{
				error_flag = 1;
				return;
			}
			fill_val[j++] = arr[i];
		}
		else
		{
			//got more value than expected
			error_flag=1;
			return;
		}
		i++;
	}
	
	//change start, end index from string to hex number
	*start = strtol(fill_start, NULL, 16);
	*end = strtol(fill_end, NULL, 16);
	
	//if start, end entered, val is not entered, insert -1 into *val
	if (number_flag == 1 && strtol(fill_val, NULL, 16) == 0)
		*val = -1;
	//right command entered, change value from string to hex number
	else
		*val = strtol(fill_val, NULL, 16);
}


/*----------------------------------------*/
/* func_name : command_reset              */
/* purpose : reset every memory value     */
/*   	     into zero                    */
/* return  : none                         */
/*----------------------------------------*/
void command_reset()
{
	//use memset func, change every memory value into (char)0
	memset(memory, (char)0, sizeof(memory));
}

/*----------------------------------------*/
/* func_name : command_quit               */
/* purpose : end sicsim,                  */
/*   	     free all linked list         */
/* return  : none                         */
/*----------------------------------------*/
void command_quit()
{
	//change quit_flag to 1, to exit while loop in 20161598.c
	quit_flag = 1;
	
	//if history's linked list exists, free every memory
	if(head)
	{
		Node* cur = head;	//cur points current node
		while (1)
		{
			if (cur == NULL)
				break;
			Node* prev = cur;	//prev is used to free node
			cur = cur->next;
			free(prev);
		}
	}

	//if Hash_Table's linked list exists, free every memory
	for (int i = 0; i < 20; i++)
	{
		if (table[i])
		{
			Hash* cur = table[i];	//cur points current table node
			while (1)
			{
				if (cur == NULL)
					break;
				Hash* prev = cur;	//prev is used to free node
				prev = cur;	
				cur = cur->next;
				free(prev);
			}
		}
	}
}

/*----------------------------------------*/
/* func_name : make_hashtb                */
/* purpose : read opcode.txt and make     */
/*   	     hash table by linked list    */
/* return  : none                         */
/*----------------------------------------*/
void make_hashtb() {

	//open file
	FILE *fp = fopen("opcode.txt", "r");
	
	while (1)
	{
		int hash_index = 1;		//variable stores each mnemonic's hash_table index
		char temp_str[30] = { 0 };	//arr stores 1 line from opcode.txt
		char temp_val[2] = { 0 };	//arr stores value
		char temp_mnemonic[7] = { 0 };	//arr stores mnemonic
		int i;	//variable stores current index
		
		//read 1 line
		if (fgets(temp_str, 30, fp) == NULL)
			break;
		
		//copy value from temp_str to temp_val
		for (i = 0; i <= 1; i++)
			temp_val[i] = temp_str[i];
		
		while (temp_str[i] < (char)65)
			i++;
		
		int j = 0, mnemonic_size=0;	// j:temp_mnemonic's index, mnemonic_size:mnemonic's length
		
		//copy mnemonic from temp_str to temp_mnemonic
		for (; temp_str[i] >= (char)65 && temp_str[i] <= (char)90; i++)
		{
			temp_mnemonic[j++] = temp_str[i];
			
			//add mnemonic's int value to hash_index
			hash_index += (int)temp_str[i];
			
			mnemonic_size++;
		}
		//add mnemonic's size*19 to hash_index
		hash_index += mnemonic_size*19;

		//hash_table has a size of 20, so after adding each alphabetic integer value from all of the preceding commands,
		//add the length of the mnemonic multiplied by 19 and set hash _index as the remainder divided by 20.
		hash_index %= 20;

	
		Hash* Hash_Node;
		Hash_Node = (Hash*)malloc(sizeof(Hash));
		
		//put value and mnemonic into current Hash node
		strcpy(Hash_Node->mnemonic, temp_mnemonic);
		Hash_Node->value = strtol(temp_val, NULL, 16);
		Hash_Node->next = NULL;
		
		//if table[hash_index] has nothing, put point current node
		if (table[hash_index] == NULL)
			table[hash_index] = Hash_Node;
		else
		{
			Hash* cur = table[hash_index];
			
			//repeat to reach end of linked list
			while (cur->next != NULL)
				cur = cur->next;
			
			//put node into the end of linked list
			cur->next = Hash_Node;
		}
	}
}

/*----------------------------------------*/
/* func_name : command_opcodelist         */
/* purpose : opcodelist entered           */
/*   	     print hash table             */
/* return  : none                         */
/*----------------------------------------*/
void command_opcodelist() {
	for (int i = 0; i < 20; i++)
	{
		//print hash table's index
		printf("\t%d : ", i);
		if (table[i])
		{
			Hash* cur = table[i];	//cur points current hash table node
			
			//print every mnemonic and value until end of hash index's linked list
			while (1)
			{
				printf("[%s,%.2X]", cur->mnemonic, cur->value);
				cur = cur->next;
				if (cur == NULL)
					break;
				printf(" -> ");
			}
			printf("\n");
		}
	}
}

/*----------------------------------------*/
/* func_name : command_opcode             */
/* purpose : find input mnemonic from     */
/*   	     hash table and print value   */
/* return  : none                         */
/*----------------------------------------*/
void command_opcode() {
	
	char temp_mnemonic[7] = { 0 };	//arr stores input mnemonic
	int mnemonic_num = 1;	//variable to detect hash_table's index
	int i = 7;	//i starts from index 7 which is after mnemonic and blank

	int j = 0, mnemonic_length = 0;	//j:temp_mnemonic index, mnemonic_length: mnemonic's length
	
	//copy input mnemonic into temp_mnemonic
	while (command[i] != '\0')
	{
		//if wrong mnemonic entered
		if(j==6)
		{
			printf("Error ! No such Mnemonic on the opcodelist.\n");
			error_flag = 1;
			return;
		}
		temp_mnemonic[j++] = command[i];
		mnemonic_num += (int)command[i++];
		mnemonic_length++;
	}
	
	//calculate mnemonic's hash_index
	mnemonic_num += mnemonic_length * 19;
	mnemonic_num %= 20;

	Hash* cur = table[mnemonic_num];	//cur points current hash table node
	while (1)
	{
		//if not found, print error message
		if (cur == NULL)
		{
			printf("Error ! No such Mnemonic on the opcodelist.\n");
			error_flag = 1;
			break;
		}
		//if found, print mnemonic's value
		if (!(strcmp(cur->mnemonic, temp_mnemonic)))
		{
			printf("opcode is %.2X\n", cur->value);
			break;
		}
		cur = cur->next;
	}
}

/*-------------------------------------*/
/* func_name : distinct_ascii          */
/* purpose : distinct char if it is in */
/*   memory ascii boundary             */
/* return  : if not return 1, else 0   */
/*-------------------------------------*/
int distinct_ascii(char x){

	//if x is blank
	if(x==(char)32)
		return 0;
	else if(x < (char)48)
		return 1;
	else if(x>=(char)58 && x <= (char)64)
		return 1;
	else if (x >= (char)71 && x <= (char)96)
		return 1;
	else if (x >= (char)103)
		return 1;
	//if x is 0~9, A~F, a~f, return 0
	else
		return 0;
}

