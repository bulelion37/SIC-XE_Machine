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
	char format;
	struct _Hash* next;
}Hash;	// Hash : struct which store Hash table
Hash* table[20];	//arr named table storing opcode Hash table

typedef struct _SYM {
	char label[31];
	int locctr;
	struct _SYM* next;
}SYM;	// SYM : struct which store SYMBOL table
SYM* first;	//first is pointing first of SYMTAB
SYM* before;	//before is pointing previous SYM node

typedef struct _Modi {
	int start_addr;
	struct _Modi* next;
}Modi;	//Modi : struch which store modication info
Modi* modi_head;	//modi_head is pointing first of Modification linked list
Modi* modi_prev;	//modi_prev is pointing previous node of Modification linked list

typedef struct _ESTAB {
	char control_section[31];
	char symbol_name[31];
	int addr;
	int cslth;
	struct _ESTAB* next;
}ESTAB;	// ESTAB에 대한 정보를 저장하는 구조체이다. Symbol의 이름, control section, 주소, 길이에 대한 정보를 저장할 수 있다. 
ESTAB* estab_head;	//ESTAB linked list의 처음을 가리키는 포인터이다. 
ESTAB* estab_prev;	//ESTAB linked list의 이전 노드를 가리키는 포인터이다.

typedef struct _FILENAME {
	char str_filename[31];
	int cslth;
}FILENAME;	//loader 뒤에 들어온 각 obj 파일의 이름과 해당 obj 파일의 총 길이에 대한 정보를 저장하는 구조체이다. 
FILENAME obj_name[3];	//.obj 파일의 이름 및 길이에 대한 정보를 저장하는 구조체 배열이다. 
int obj_name_size = 1;		//loader 뒤에 들어온 obj 파일의 수를 저장하는 변수이다. 

typedef struct _bp {
	int breakpoint;
	struct _bp* next;
}bp;	//breakpoint에 대한 정보를 저장하는 구조체이다. 지정하고 싶은 breakpoint의 값을 정수로 저장한다. 
bp* bp_head;	//bp linked list의 처음을 가리키는 포인터이다. 
bp* bp_prev;	//bp linked list의 이전 노드를 가리키는 포인터이다. 

typedef struct _nixbpe {
	int ni;
	int xbpe;
}nixbpe;			//해당 메모리 값에서 ni와 xbpe 값을 정수로 저장하는 구조체이다. 
nixbpe nixbpe_flag;	//해당 메모리의 nixbpe의 값을 저장하는 변수이다. 

char command[100];	//variable which get input command
int line_num;		//variable which get history's line index
int quit_flag;		//distinct if input gets 'quit', if 1 program end
int error_flag;			//distinct if error occurs
int memory_address;		//variable which stores last printed memory address+16
unsigned char memory[1048576];	//arr which stores memory 
int loc;	//variable which stores LOC
int start_address; //variable which stores start address at .asm file.
int program_length; //variable which stores program length.
int pc;	//variable which stores PC
int symbol_flag;	// flag, if it's 1 there is symbol, 0 : no symbol
char file_name[31];	//arr which stores filename
unsigned char line_opcode[31];		//string which stores opcode
unsigned char line_symbol[31];		//string which stores symbol 
unsigned char line_operand[31];	//string which stores operand
int opcode_val;	//variable which stores opcode's value
int text_flag;		//if same text line in object code, it is 1
char text_record[61];	//arr which stores object text info 
int text_index;		// index of text
char result_obj_code[31];	//arr which stores obj code
char BASE_str[31];	//arr which stores BASE symbol
int obj_loc;		//variable which stores obj code's starting address
int asm_start_flag;	//flag if there is start in the .asm file, it becomes 1
int lst_line_num = 5;	// variable which stores line_number in lst file
int pass_1_flag;		//flag if pass1 is success, becomes 1
int SYMTAB_flag;		//flag if SYMTAB succesfully made, becomes 1
int progaddr;	//PROGADDR에 대한 정보를 저장하는 변수이다. 
int csaddr;		//CSADDR에 대한 정보를 저장하는 변수이다. 
int execaddr;	//EXECADDR에 대한 정보를 저장하는 변수이다. 
int estab_number;	//ESTAB linked list의 총 크기에 대한 정보를 저장하는 변수이다.
char str_progaddr[31];	//progaddr의 char 정보를 저장하는 배열이다. 
char line_obj[270];		//obj 파일의 한 줄에 대한 정보를 저장하는 배열이다.
char modi_value[7] = { 0 };	//modified 되야 하는 값에 대한 정보를 저장하는 배열이다. 
int* reference_number; //각 obj 파일의 reference number에 대한 정보를 저장하는 배열 가리키는 포인터이다.
int REG[13];		//Register에 대한 값을 저장하는 배열이다. 
int TA;	//계산한 target address에 대한 값을 저장하는 변수이다. 
int format4_flag = 0;		//만일 해당 값이 format4이면 1이 되고, 아니면 0이다. 
int run_end_flag = 0;		//run이 끝났는지 판별하는 flag이다. 끝나지 않았으면 0, 끝났으면 1이다.
int bp_zero = 0;		//bp에 0이 들어온 경우 예외처리해주기 위한 변수이다. 	

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
void command_type();
void command_assemble();
void get_filename(char* arr, int i);
int divide_one_line(char* arr, int pass_num);
int  find_opcode(char* arr, int i);
void command_symbol();
void free_symtab();
void distinct_format(char* opcode, char* operand);
void num_to_hexchar(char* value, int num);
int  find_symbol(char* operand);
void change_hex(char* bin, char* hex, int format_num);
void disp_addr_set(char* arr, int num, int format_type);
char hex_char_return(int num);
void xbpe_setting(char* value, int x, int b, int p, int e);
void register_num(char* arr, char* reg);
void command_progaddr();
void command_loader();
void pass1_line_distinct(char* one_line_obj, int obj_index);
void print_loadmap();
void free_estab();
void pass2_line_distinct(char* one_line_obj, int obj_index);
int find_estab(char* operand);
void calulate_modi(char* operand, int change_num, int estab_symbol, int arith_flag);
int string_to_negin(char* operand, int length);
void print_bp();
void command_bp();
void free_bp(int clear_flag);
int check_bp_addr(char c);
void command_run();
int find_pc_bp();
int get_TA_3();
void get_TA_2();
void ST_opcode(int ta, int register_num);

/*-------------------------------------*/
/* func_name : command_distinct        */
/* purpose : distinct input command,   */
/*   call function related to command  */
/* return  : none                      */
/*-------------------------------------*/
void command_distinct(char* arr)
{
	//if error_flag is 1, wrong command input 
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
	//If type entered, call command_type()
	else if (!strncmp(arr, "type", 4))
	{
		//If got blank after type
		if (arr[4] == ' ')
		{
			error_flag--;
			command_type();
		}
	}
	//If assemble entered, call command_assemble()
	else if (!strncmp(arr, "assemble", 8))
	{
		//if first exists, it means we assembled other file in the past. so we need to free symtab
		free_symtab();

		//Initialize string arrays which stores info about opcode, operand, symbol and symbol_flag
		memset(line_opcode, 0, sizeof(line_opcode));
		memset(line_operand, 0, sizeof(line_operand));
		memset(line_symbol, 0, sizeof(line_symbol));
		memset(file_name, 0,sizeof(file_name));
		memset(BASE_str, 0, sizeof(BASE_str));
		memset(text_record, 0, sizeof(text_record));
		symbol_flag = 0;
		asm_start_flag = 0;
		first = NULL;
		before = NULL;
		modi_head = NULL;
		modi_prev = NULL;
		pass_1_flag=0;
		lst_line_num=5;
		SYMTAB_flag = 0;

		//If got blank after assemble
		if (arr[8] == ' ')
		{
			error_flag--;
			command_assemble();
			if (error_flag == 0)
			{
					printf("Successfully assemble %s.\n", file_name);
					SYMTAB_flag = 1;
			}
			else
			{	
					if(pass_1_flag==0)
					{
							//remove intermediate file
							remove("intermediate");
					}
			}
		}
	}
	// If symbol entered, call command_symbol()
	else if (!strncmp(arr, "symbol", 6))
	{
		//if there was error while assemble
		if(!SYMTAB_flag)
			printf("Symbol Error ! : No SYMTAB!\n");
		else{
				error_flag--;
				command_symbol();}
	}
	//If progaddr entered, call command_progaddr()
	else if (!strncmp(arr, "progaddr", 8))
	{
		//If got blank after progaddr
		if (arr[8] == ' ')
		{
			error_flag--;
			command_progaddr();
		}
	}
	//If loader entered, call command_loader()
	else if (!strncmp(arr, "loader", 6))
	{

		//If got blank after loader, call command_loader
		if (arr[6] == ' ')
		{
			error_flag--;
			command_loader();
			bp_zero = 0;
		}
	}
	//If bp entered, call command_bp()
	else if (!strncmp(arr, "bp", 2))
	{
		//only bp entered, call print_bp()
		if (arr[2] == '\0')
		{
			error_flag--;
			print_bp();
		}
		else if (arr[2] == ' ')
		{
			error_flag--;
			command_bp();
		}
	}
	//If run entered, call command_run()
	else if (!strncmp(arr, "run", 3))
	{
		//If got EOF after run
		if (arr[3] == '\0')
		{
			error_flag--;
			command_run();
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
	printf("h[elp]\nd[ir]\nq[uit]\nhi[story]\ndu[mp][start, end]\ne[dit] address, value\nf[ill] start, end, value\nreset\nopcode mnemonic\nopcodelist\nassemble filename\ntype filename\nsymbol\n");
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
		printf("\t\t%s",dir_entry->d_name);
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
	free_symtab();
	free_bp(0);
	free_estab();
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
		char temp_str[31] = { 0 };	//arr stores 1 line from opcode.txt
		char temp_val[2] = { 0 };	//arr stores value
		char temp_mnemonic[7] = { 0 };	//arr stores mnemonic
		char temp_format;
		int i;	//variable stores current index
		
		//read 1 line
		if (fgets(temp_str, 31, fp) == NULL)
			break;
		
		//copy value from temp_str to temp_val
		for (i = 0; i <= 1; i++)
			temp_val[i] = temp_str[i];
		
		while (temp_str[i] < (char)65)
			i++;
		
		int j = 0, mnemonic_size=0;	// j:temp_mnemonic's index, mnemonic_size:mnemonic's length
		
		//copy mnemonic from temp_str to temp_mnemonic
		for(; temp_str[i] >= (char)65 && temp_str[i] <= (char)90; i++)
		{
			temp_mnemonic[j++] = temp_str[i];
			
			//add mnemonic's int value to hash_index
			hash_index += (int)temp_str[i];
			
			mnemonic_size++;
		}

		//skip blank or tab
		while (temp_str[i] == ' ' || temp_str[i] == '\t')
			i++;

		//store possible format type
		temp_format = temp_str[i];


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
		Hash_Node->format = temp_format;
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
	fclose(fp);
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

/*-------------------------------------*/
/* func_name : command_type            */
/* purpose : read the filename file    */
/*   	     and print file's contents */
/* return  : none                      */
/*-------------------------------------*/
void command_type()
{
	//get filename throught get_filename funtion
	get_filename(file_name, 5);
	
	char temp;	//variable which gets file's character
	FILE *fp = fopen(file_name, "r");
	if(fp == NULL)
			printf("Error : No such file in the directory!\n");
	else{
				while(fscanf(fp, "%c", &temp)!=EOF)
					printf("%c", temp);
		}
}

/*------------------------------------------*/
/* func_name : get_filename                 */
/* purpose : get the filename information   */
/*   	     and store into file_name array */
/* return  : none                           */
/*------------------------------------------*/
void get_filename(char* arr, int i)
{
	int j = 0;		//index of file_name

	//get file_name through under while statement
	while (1)
	{
		if (command[i] == '\0')
			break;
		file_name[j++] = command[i++];
	}
}

/*------------------------------------------------------------------*/
/* func_name : divide_one_line             							*/
/* purpose : get one line in the assembly code   					*/
/*   	      	 and get information about opcode, operand, symbol  */
/* return  : 0 : opcode=start, 1 : opcode=end, -1 : comment line    */
/*------------------------------------------------------------------*/
int divide_one_line(char* arr, int pass_num)
{
	int i = 0, j = 0, k = 0, l = 0, cnt_blank = 0, comma_flag = 0;	//comma_flag : 1 -> need to get another operand even if there is blank
	
	//if this function is called in pass 2
	if (pass_num == 2)
	{
		char line_loc[4] = { 0 };

		//skip line number and blank
		while (arr[i++] != '\t');

		//copy loc of this line
		while (arr[i] != '\t')
			line_loc[j++] = arr[i++];

		//store loc in hex number 
		loc = strtol(line_loc, NULL, 16);

		//skip blank after loc
		i++;
		
		j = 0;
	}

	//if line starts with . this line is comment line. 
	if (arr[i] == '.')
		return -1;

	//if line starts with blank, this line has only opcode and operand
	else if (arr[i] == ' ' || arr[i]=='\t')
		symbol_flag = 0;

	//else, it is symbol, so store in line_symbol string
	else
	{
		symbol_flag = 1;
		line_symbol[j++] = arr[i++];
	}
	int opcode_cnt = 0;	//variable to detect it is opcode or operand

	//Divide line by blank to get information about opcode, symbol
	while (arr[i] != '\n')
	{
		if (arr[i] == ',')
			comma_flag = 1;

		if ((arr[i] == ' ' && comma_flag==0) || (arr[i]=='\t' && comma_flag==0))
		{
			if (symbol_flag == 1)
				cnt_blank++;
			else if (symbol_flag == 0 && opcode_cnt != 0)
				cnt_blank++;
			while (arr[i] == ' ' || arr[i]=='\t')
				i++;
		}

		//If arr[i] is symbol
		if (symbol_flag == 1 && cnt_blank == 0)
			line_symbol[j++] = arr[i];

		//if arr[i] is opcode
		else if ((symbol_flag==1 &&cnt_blank == 1) || (symbol_flag == 0 && cnt_blank == 0))
		{
			line_opcode[k++] = arr[i];
			opcode_cnt++;
		}
		
		//if arr[i] is operand
		else if ((symbol_flag == 1 && cnt_blank == 2) || (symbol_flag == 0 && cnt_blank == 1))
		{
			line_operand[l++] = arr[i];

			//if there is comma, need to get another operand
			if (comma_flag)
			{
				i++;
				while (arr[i] == ' ' || arr[i]=='\t')
					i++;
				comma_flag = 0;
				continue;
			}
		}

		i++;
	}
	
	//If opcode is 'START'
	if (!asm_start_flag) {
		//if START found
		if (!strcmp(line_opcode, "START"))
		{
			asm_start_flag = 1;
			return 0;
		}
		//else print error message
		else
		{
			printf("Assemble Error in line %d : No START opcode\n", lst_line_num);
			error_flag = 1;
			return -4;
		}
	}
	//If opcode is 'END'
	else if (!strcmp(line_opcode, "END"))
		return 1;
	else return 2;
}


/*-----------------------------------------------*/
/* func_name : command_assemble             	 */
/* purpose : assemble file_name, through pass1,2 */
/*   	     make .obj, .lst file 	     	   	 */
/* return  : none                      			 */
/*-----------------------------------------------*/
void command_assemble()
{
	char obj_name[31] = { 0 };
	char lst_name[31] = { 0 };
	get_filename(file_name, 9);

	int file_index = 0, x=0;	//index of file_name, obj_name, lst_name

	//copy filename without .asm to obj_name and lst_name
	while (1)
	{
		if (file_name[file_index] == '.' && file_name[file_index+1] == 'a')
			break;
		if(file_name[file_index] == '\0')
			break;
		obj_name[x] = file_name[file_index];
		lst_name[x++] = file_name[file_index++];
	}
	strcat(obj_name, ".obj");
	strcat(lst_name, ".lst");
	

	//Pass1
	FILE* fp = fopen(file_name, "r");
	FILE* inter = fopen("intermediate", "w");
	char one_line[100] = { 0 };	//arr which gets one line of .asm file
	
	//if no such file name in the directory, print error message
	if(fp == NULL)
	{
			printf("File Open error : No such file in the directory!\n");
			error_flag=1;
			return;
	}

	//read first input line
	fgets(one_line, 100, fp);

	//If OPCODE = 'START'
	if (divide_one_line(one_line,1) == 0)
	{
		//save #[OPERAND] as staring address
		start_address = strtol(line_operand, NULL, 16);
		//initialize LOCCTR to starting address
		loc = start_address;
	}
	//Else, initialize LOCCTR to 0
	else
	{
		loc = 0;
		error_flag = 1;
		return;
	}
	//write line to intermediate file
	fprintf(inter, "%d\t%.4X\t%s", lst_line_num, loc, one_line);
	lst_line_num += 5;		//add 5 to line number

	while (1)
	{
		//Initialize string arrays which stores info about line, opcode, operand, symbol and symbol_flag
		memset(one_line, 0, sizeof(one_line));
		memset(line_opcode, 0, sizeof(line_opcode));
		memset(line_operand, 0, sizeof(line_operand));
		memset(line_symbol, 0, sizeof(line_symbol));
		symbol_flag = 0;

		//Get one line
		if (fgets(one_line, 100, fp) == NULL)
		{
			//if one_line is NULL, it means there is no END opcode, so print error message
			printf("Assemble Error on line %d : No END opcode in the .asm file!\n", lst_line_num);
			error_flag = 1;
			return;
		}

		//Get info about symbol, opcode, operand by calling divide_one_line function
		int end_flag = divide_one_line(one_line,1);

		//OPCODE = END
		if (end_flag == 1)
		{
			//write last line to intermediate file
			fprintf(inter, "%d\t    \t%s", lst_line_num, one_line);
			break;
		}
		//COMMENT STATEMENTS
		else if (end_flag == -1)
		{
			//write line to intermediate file
			fprintf(inter, "%d\t    \t%s", lst_line_num, one_line);
			lst_line_num += 5;		//add 5 to line number
			continue;
		}
		else if (end_flag == 2)
		{
			//If there is symbol in the LABEL field
			if (symbol_flag)
			{
				//search SYMTAB for LABEL
				SYM* sym_cur = first;
				if (first)
				{
					while (sym_cur != NULL) {
						if (!strcmp(sym_cur->label, line_symbol))
						{
							//if LABEL is found in SYMTAB, print error message
							printf("Assemble Error on line %d : same label has many LOC\n", lst_line_num);
							error_flag = 1;
							return;
						}
						sym_cur = sym_cur->next;
					}
				}

			//insert (LABEL,LOCCTR) into SYMTAB

				int insert_flag = 0;		//change to 1 if insert new node into SYMTAB
				
				//Making new node insert (LABEL,LOCCTR) into node
				SYM* node = (SYM*)malloc(sizeof(SYM));
				strcpy(node->label, line_symbol);
				node->locctr = loc;
				node->next = NULL;

				//If first is NULL, make the initial SYMTAB and link first, before to node
				if (first == NULL)
				{
					first = node;
					insert_flag = 1;
				}
					
				SYM* cur = first;	//pointing current node at SYMTAB
				before = first;
				
				int cnt = 0;	//variable to count how many node has passed to insert node into SYMTAB
				
				//Repeat until insert new node into SYMTAB
				while (!insert_flag) {

					//If new node label is lower than current node label, insert new node into SYMTAB
					int ret = strcmp(node->label, cur->label);
					if (ret < 0)
					{
						//insert into the first of SYMTAB
						if (before == first && cnt==0){
							node->next = cur;
							first = node;
						}
						//insert in the middle of SYMTAB
						else{
							before->next = node;
							node->next = cur;
						}
						insert_flag = 1;
						break;
					}

					//Else new node label is bigger than current node label, move cur to next node
					
					//if cur and before is not same
					if (cur != before)
						before = before->next;
						
					//move cur to next node
					cur = cur->next;
					cnt++;

					//insert in the last node
					if(cur == NULL)
					{
						before->next = node;
						insert_flag = 1;
						break;
					}
				}
		}
		
		if (!strcmp(line_opcode, "WORD"))
		{
			//if word constant definition, there is not decimal number, print error message
			if ((int)line_operand[0] >= 58)
			{
				printf("Assemble Error in line %d : Only Decimal number is possible for WORD constant\n", lst_line_num);
				error_flag = 1;
				return;
			}

			//write line to intermediate file
			fprintf(inter, "%d\t%.4X\t%s", lst_line_num, loc, one_line);
			lst_line_num += 5;		//add 5 to line number
			
			loc += 3;			//add 3 to LOCCTR
		}
		else if (!strcmp(line_opcode, "RESW"))
		{
			//write line to intermediate file
			fprintf(inter, "%d\t%.4X\t%s", lst_line_num, loc, one_line);
			lst_line_num += 5;		//add 5 to line number

			loc += 3 * atoi(line_operand); //add decimal number of operand * 3 to LOCCTR
		}
		else if (!strcmp(line_opcode, "RESB"))
		{
			//write line to intermediate file
			fprintf(inter, "%d\t%.4X\t%s", lst_line_num, loc, one_line);
			lst_line_num += 5;		//add 5 to line number

			loc += atoi(line_operand); //add decimal number of operand to LOCCTR
		}
		else if (!strcmp(line_opcode, "BYTE"))
		{
			//find length of constant in bytes
			int constant_length = 0;
			//if operand is character constant
			if (line_operand[0] == 'C')
			{
				int i = 2;		//index of line_operand
				//get length of constant through under while statement
				while (line_operand[i++] != '\'')
					constant_length++;
					
				//write line to intermediate file
				fprintf(inter, "%d\t%.4X\t%s", lst_line_num, loc, one_line);
				lst_line_num += 5;		//add 5 to line number

				//char is 1 byte, so loc + 1*constant_legnth
				loc += constant_length;
			}
			//if operand is hexa number
			else if (line_operand[0] == 'X')
			{
				int i = 2;		//index of line_operand
				//get length of constant through under while statement
				while (line_operand[i++] != '\'')
					constant_length++;
				
				//write line to intermediate file
				fprintf(inter, "%d\t%.4X\t%s", lst_line_num, loc, one_line);
				lst_line_num += 5;		//add 5 to line number
			
				//constant is written in Half Byte. so constant length is divided by 2
				if (constant_length % 2 == 1)
					constant_length += 1;
				loc += ((constant_length) / 2);

			}
		}
		else if (!strcmp(line_opcode, "BASE"))
		{
			//copy BASE operand in BASE_str
			strcpy(BASE_str, line_operand);

			//write line to intermediate file
			fprintf(inter, "%d\t    \t%s", lst_line_num, one_line);
			lst_line_num += 5;		//add 5 to line number
			continue;
		}
		else if (line_opcode[0] == '+') {
						
			//search temp_opcode in the opcode table, if 0, there is no such opcode
			if (find_opcode(line_opcode, 1) == 0)
			{
				printf("Assembly File Error :  No %s on the opcodelist\n", line_opcode);
				error_flag = 1;
				return;
			}

			//write line to intermediate file
			fprintf(inter, "%d\t%.4X\t%s", lst_line_num, loc, one_line);
			lst_line_num += 5;		//add 5 to line number

			//store format 4's loc for modification if it is not immediate number addressing
			if (!(line_operand[0] == '#' && (line_operand[1] >= '0' && line_operand[1] <= '9'))) {
				//make dynamic memory about modification information
				Modi* modi_node = (Modi*)malloc(sizeof(Modi));
				modi_node->start_addr = loc+1;
				modi_node->next = NULL;

				//make linked list about format 4's modification info.
				if (modi_head == NULL)
					modi_head = modi_node;
				else
				{
					Modi* cur = modi_head;
					modi_prev = modi_head;
					while (1)
					{
						cur = cur->next;
						if (cur == NULL)
						{
							modi_prev->next = modi_node;
							break;
						}
						modi_prev = modi_prev->next;
					}
				}
			}

			//If found in the opcode table, format 4, add 4 to LOCCTR
			loc += 4;
		}
		//format 1, 2, 3 instruction
		else{
			//search OPTAB for OPCODE
			int opcode_found_flag = find_opcode(line_opcode, 0);

			//if wrong opcode, print error message
			if (opcode_found_flag == 0)
			{
				printf("Assembly Error in line %d :  No %s on the opcodelist\n",lst_line_num, line_opcode);
				error_flag = 1;
				return;
			}

			//if opcode is found into OPTAB
			
			//write line to intermediate file
			fprintf(inter, "%d\t%.4X\t%s", lst_line_num, loc, one_line);
			lst_line_num += 5;		//add 5 to line number
			
			if (opcode_found_flag == 1)
				loc += 1;		//add 1(format 1) to LOCCTR
			else if (opcode_found_flag == 2)
				loc += 2;		//add 2 (register instruction) to LOCCTR
			else
				loc += 3;			//add 3 (instruction length) to LOCCTR
		}
	}
	}

	//Close File Pointer
	fclose(fp);
	fclose(inter);

	//save program total length in program_length
	program_length = loc - start_address;

	
	//Pass2 if error does not occur, execute
	if(!error_flag){
	pass_1_flag=1;
	FILE* interm = fopen("intermediate", "r");
	FILE* obj_fp = fopen(obj_name, "w");
	FILE* lst_fp = fopen(lst_name, "w");
	 
	//Initialize string arrays which stores info about line, opcode, operand, symbol and symbol_flag
	memset(one_line, '\0', sizeof(one_line));
	memset(line_opcode, '\0', sizeof(line_opcode));
	memset(line_operand, '\0', sizeof(line_operand));
	memset(line_symbol, '\0', sizeof(line_symbol));
	symbol_flag = 0;
	int variable_flag = 0;	//if there is variable, so ends object line and print in the object code, then its 1
	int printed_flag = 0;	//if there remains object code, when ends came in, print it. 
	asm_start_flag = 0;

	//read first input line
	fgets(one_line, 100, interm);

	//If OPCODE = 'START'
	if (divide_one_line(one_line,2) == 0)
	{
		//Write listing line
		fprintf(lst_fp, "%s", one_line);

		//Write Header record to object program
		fprintf(obj_fp, "H%-6s%.6X%.6X\n", line_symbol, start_address, program_length);
	}
	lst_line_num = 5;
	//while opcode is not END, get line. 
	while (!error_flag)
	{
		//Initialize string arrays which stores info about line, opcode, operand, symbol, result_obj_code
		memset(one_line, 0, sizeof(one_line));
		memset(line_opcode, 0, sizeof(line_opcode));
		memset(line_operand, 0, sizeof(line_operand));
		memset(line_symbol, 0, sizeof(line_symbol));
		memset(result_obj_code, 0, sizeof(result_obj_code));
		symbol_flag = 0;
		lst_line_num += 5;

		//Get one line
		fgets(one_line, 100, fp);

		//Get info about symbol, opcode, operand by calling divide_one_line function
		int end_flag = divide_one_line(one_line, 2);

		//OPCODE = END
		if (end_flag == 1)
		{
			//END line do not make object code

			//if object line which didn't printed remains, print it
			if (printed_flag != 0) {
				fprintf(obj_fp, "T%.6X", obj_loc);
				text_record[0] = hex_char_return(((text_index - 2)/2) / 16);
				text_record[1] = hex_char_return(((text_index - 2)/2) % 16);
				fprintf(obj_fp, "%s\n", text_record);
			}			

			//print modification
			Modi* modi_cur = modi_head;
			while(modi_cur != NULL)
			{
				fprintf(obj_fp, "M%.6X05\n", modi_cur->start_addr);
				modi_cur = modi_cur->next;
			}

			//Write listing line
			fprintf(lst_fp, "%s", one_line);

			fprintf(obj_fp, "E%.6X\n", start_address);
			break;
		}
		//COMMENT STATEMENTS
		else if (end_flag == -1)
		{
			//comments line do not make object code

			//Write listing line
			fprintf(lst_fp, "%s", one_line);
		}
		else {
			//if opcode is BASE, do not make object code
			if (!(strcmp(line_opcode, "BASE")))
			{
				//If there is no symbol about the BASE directive, print error message
				int find = find_symbol(line_operand);
				if (find == -1)
				{
					printf("Assemble Error in line %d : No such symbol in SYMTAB!\n", lst_line_num);
					error_flag = 1;
					return;
				}
				fprintf(lst_fp, "%s", one_line);
				continue;
			}
			//if opcode is RESW, do not make object code
			else if (!(strcmp(line_opcode, "RESW")))
			{
				fprintf(lst_fp, "%s", one_line);

				if (!variable_flag) {
				//if there is variable, ends object line and start next
				fprintf(obj_fp, "T%.6X", obj_loc);
				text_record[0] = hex_char_return(((text_index - 2)/2) / 16);
				text_record[1] = hex_char_return(((text_index - 2)/2) % 16);
				fprintf(obj_fp, "%s\n", text_record);
				text_flag = 0;
				variable_flag = 1;
				printed_flag = 0;
				}
				continue;
			}
			//if opcode is RESB, do not make object code
			else if (!(strcmp(line_opcode, "RESB")))
			{
				fprintf(lst_fp, "%s", one_line);

				if (!variable_flag) {
					//if there is variable, ends object line and start next
					fprintf(obj_fp, "T%.6X", obj_loc);
					text_record[0] = hex_char_return(((text_index - 2)/2) / 16);
					text_record[1] = hex_char_return(((text_index - 2)/2) % 16);
					fprintf(obj_fp, "%s\n", text_record);
					text_flag = 0;
					variable_flag = 1;
					printed_flag = 0;
				}
		continue;
			}

			//calculate obj code
			distinct_format(line_opcode, line_operand);

			//delete \n in the one_line
			int one_line_index = 0;
			while (1)
			{
				if (one_line[one_line_index] == '\n')
				{
					one_line[one_line_index] = '\0';
					break;
				}
				one_line_index++;
			}

			//Write listing line
			fprintf(lst_fp, "%-50s %-35s\n", one_line, result_obj_code);
			printed_flag++;

			//f there was variable before, then already printed object code, so make flag to 0
			if (variable_flag)
				variable_flag = 0;
	
			int obj_index = 0, cnt=0;		//index of result_obj_code and cnt : length of object code
			while(result_obj_code[obj_index++] != '\0')
				cnt++;

			//if this object code is written, then it is over max length
			if (text_index + cnt - 1 > 60)
			{
				if (!variable_flag)
				{
					fprintf(obj_fp, "T%.6X", obj_loc);
					text_record[0] = hex_char_return(((text_index - 2)/2) / 16);
					text_record[1] = hex_char_return(((text_index - 2)/2) % 16);
					fprintf(obj_fp, "%s\n", text_record);
					printed_flag = 0;
					text_flag = 0;
				}
			}

			//making new record line
			if (!text_flag)
			{
				//initialize text_record and text_index
				memset(text_record, 0, sizeof(text_record));
				text_index = 2;
				text_flag = 1;
				obj_loc = loc;
			}

			obj_index = 0;

			//copy object code into the text_record array
			while (result_obj_code[obj_index] != '\0')
				text_record[text_index++] = result_obj_code[obj_index++];

		}
	}
	//Close File Pointer
	fclose(interm);
	fclose(obj_fp);
	fclose(lst_fp);
	}
	//remove intermediate file
	remove("intermediate");
	if(error_flag)
	{
			//if there is error during pass2, remove .obj, .lst file
			remove(obj_name);
			remove(lst_name);
	}
}

/*----------------------------------------------------------------------------------*/
/* func_name : find_opcode             												*/
/* purpose : find input mnemonic from     											*/
/*   	     hash table  														    */
/* return  : 0 if there is no such input mnemonic, if found return num of format    */
/*----------------------------------------------------------------------------------*/
int  find_opcode(char *arr, int i) {

	char temp_mnemonic[7] = { 0 };	//arr stores input mnemonic
	int mnemonic_num = 1;	//variable to detect hash_table's index
	int mnemonic_format = 0;		// variable which stores number of format
	int j = 0, mnemonic_length = 0;	//j:temp_mnemonic index, mnemonic_length: mnemonic's length

	//copy input mnemonic into temp_mnemonic
	while (arr[i] != '\0')
	{
		//if wrong mnemonic entered
		if (j == 6)
			return 0;
		temp_mnemonic[j++] = arr[i];
		mnemonic_num += (int)arr[i++];
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
			return 0;
			break;
		}

		//if found
		if (!(strcmp(cur->mnemonic, temp_mnemonic)))
		{
			//store format number and value
			mnemonic_format = (int)cur->format - 48;
			opcode_val = cur->value;
			break;
		}
		cur = cur->next;
	}
	return mnemonic_format;
}

/*---------------------------------------------*/
/* func_name : command_symbol         		   */
/* purpose : print all symbol's label and LOC  */
/*   	    by ascending order                 */
/* return  : none                              */
/*---------------------------------------------*/
void command_symbol() {
	SYM* cur = first;
	if (first) {
		for (; cur != NULL; cur = cur->next)
			printf("\t%s\t%.4X\n", cur->label, cur->locctr);
	}
	else
	{
		//if there is no SYMTAB
		error_flag = 1;
		printf("Error : No SYMTAB!\n");
	}
}

/*--------------------------------------------------------------------*/
/* func_name : free_symtab        									  */
/* purpose : free all symbol table and modification linked list       */
/* return  : none                         							  */
/*--------------------------------------------------------------------*/
void free_symtab()
{
	//if SYMTAB's linked list exists, free every memory
	if (first)
	{
		SYM* cur = first;	//cur points current node
		while (1)
		{
			if (cur == NULL)
				break;
			SYM* previous = cur;	//previous is used to free node
			cur = cur->next;
			free(previous);
		}
	}

	//If Modification's linked list exists, free every memory
	if (modi_head)
	{
		Modi* modi_cur = modi_head;	//cur points current node
		while (1)
		{
			if (modi_cur == NULL)
				break;
			modi_prev = modi_cur;	//prev is used to free node
			modi_cur = modi_cur->next;
			free(modi_prev);
		}
	}
}


/*----------------------------------------*/
/* func_name : distinct_format        	  */
/* purpose : get opcode and operand       */
/*			and make object code 		  */
/* return  : object code character array  */
/*----------------------------------------*/
void distinct_format(char* opcode, char*operand)
{
	char result_obj_before_bind[130] = { 0 };	//arr which stores obj code before binding
	int opcode_format = 0;	//variable which stores opcode's format number
	int x_flag = 0;	//flag which use x register or not
	int b_flag = 0;	//flag which use b register or not
	int p_flag = 0;	//flag pc relative or not
	int e_flag = 0; //flag e 
	
	//Format4
	if (opcode[0] == '+')
	{
		int addr = 0; 
		find_opcode(opcode, 1);

		//put opcode value in result_obj_before_bind array
		num_to_hexchar(result_obj_before_bind, opcode_val);

		//format 4, extension , using absolute value of address
		e_flag = 1;
		
		//Indirect addresssing
		if (operand[0] == '@')
		{
			char pure_operand[31] = { 0 };	//string which stores operand without @

			//store n, i value for indirect addressing
			result_obj_before_bind[6] = '1'; result_obj_before_bind[7] = '0';

			//store x,b,p,e value
			xbpe_setting(result_obj_before_bind, x_flag, b_flag, p_flag, e_flag);

			int operand_index = 1, pure_index = 0;
			//copy operand into pure_operand array without @
			while (operand[operand_index] != '\0')
				pure_operand[pure_index++] = operand[operand_index++];

			int find = find_symbol(pure_operand);
			if (find == -1)
			{
				printf("Assemble Error in line %d : No such symbol in SYMTAB\n", lst_line_num);
				error_flag = 1;
				return;
			}
			else
				addr = find;
		}
		//Immediate addressing
		else if (operand[0] == '#')
		{
			char pure_operand[31] = { 0 };	//string which stores operand without #

			//store n, i value
			result_obj_before_bind[6] = '0'; result_obj_before_bind[7] = '1';

			int s_flag = 0;
			//if got symbol for operand, s_flag = 1
			if ((int)operand[1] >= 65)
				s_flag = 1;
		

			int operand_index = 1, pure_index = 0;
			//copy operand into pure_operand array without #
			while (operand[operand_index] != '\0')
				pure_operand[pure_index++] = operand[operand_index++];

			//store x,b,p,e value
			xbpe_setting(result_obj_before_bind, x_flag, b_flag, p_flag, e_flag);

			//store operand's disp
			if (s_flag)
			{
				int find = find_symbol(pure_operand);
				if (find == -1)
				{
					printf("Assemble Error in line %d : No such symbol in SYMTAB!\n", lst_line_num);
					error_flag = 1;
					return;
				}
				else 
					addr = find;
			}
			else
				addr = atoi(pure_operand);
		}
		else {
			//store n, i value
			result_obj_before_bind[6] = '1'; result_obj_before_bind[7] = '1';

			//store x,b,p,e value
			xbpe_setting(result_obj_before_bind, x_flag, b_flag, p_flag, e_flag);

			//find symbol in SYMTAB 
			addr = find_symbol(operand);

			if (addr == -1)
			{
				printf("Assemble Error in line %d : No such symbol in SYMTAB!\n", lst_line_num);
				error_flag = 1;
				return;
			}
		}

		if (addr >= 1048576)
		{
			printf("Assemble Error in the line %d : Address out of bound!\n", lst_line_num);
			error_flag = 1;
			return;
		}

		//change binary number into hexa number
		change_hex(result_obj_before_bind, result_obj_code, 3);
		
		//store disp number
		disp_addr_set(result_obj_code, addr,4);

	}
	//if Byte constant input
	else if (!strcmp(opcode, "BYTE"))
	{
		int i = 2;		//index of operand
		int j = 0;		//index of result_object_code

		//if operand is character constant
		if (operand[0] == 'C')
		{
			//store hex value of constant through under while statement
			while (operand[i] != '\'')
			{
				result_obj_code[j++] = hex_char_return(((int)operand[i] / 16));
				result_obj_code[j++] = hex_char_return(((int)operand[i++] % 16));
			}
		}
		//if operand is hexa number
		else if (operand[0] == 'X')
		{
			int operand_length = 0;
			
			//count operand's length
			while(operand[i++] != '\'')
				operand_length++;

			//if it is odd number, put 0 in the front
			if (operand_length % 2 == 1)
				result_obj_code[j++] = '0';

			i = 2;
			//store hex value of constant through under while statement
			while (operand[i] != '\'')
				result_obj_code[j++] = operand[i++];
		}
	}
	//if WORD constant input
	else if (!strcmp(opcode, "WORD"))
	{
		//store operand in decimal number to num
		int num = atoi(operand);

		int result_index = 0;	//index of result_obj_code

		//change num into 6 byte hex character array
		int hexx = 65536;
		while (1)
		{
			if (num == 0)
				break;
			int quotient = num / hexx;
			num = num - (quotient * hexx);
			result_obj_code[result_index++] = hex_char_return(quotient);
			hexx /= 16;
		}
	}
	else
	{
		//get opcode's format number and value
		opcode_format = find_opcode(opcode, 0);

		//put opcode value in result_obj_before_bind array
		num_to_hexchar(result_obj_before_bind, opcode_val);
		//Format 1
		if (opcode_format == 1)
		{
			//only opcode, 1 byte
			if (operand[0] != '\0')
			{
				printf("Assemble Error in the line %d : Format 1 cannot have operand!\n", lst_line_num);
				error_flag = 1;
				return;
			}
			//pc is pointing next instruction
			pc = loc + 1;

			//change binary number into hexa number
			change_hex(result_obj_before_bind, result_obj_code, 1);
		}
		//Format2
		else if (opcode_format == 2)
		{
			//pc is pointing next instruction
			pc = loc + 2;

			//Register instruction
			int comma_flag = 0;		//if 1: there is two register in the line

			int operand_index = 0;	//oeprand index
			char register_arr[3] = { 0 };	//string which stores register type
			int register_index = 0;	//index of register
			while (operand[operand_index] != '\0')
			{
				if (operand[operand_index] == ',')
				{
					//store register number into the result_obj_before_bind arr
					register_num(result_obj_before_bind, register_arr);
					register_index = 0;
					operand_index++;
					comma_flag++;
				}
				register_arr[register_index++] = operand[operand_index++];
			}
			
			//store register number into the result_obj_before_bind arr
			if (comma_flag)
				register_num(result_obj_before_bind, register_arr);
			else
			{
				//if only one register is in line
				register_num(result_obj_before_bind, register_arr);

				//store all zero to the register 2 number
				strcat(result_obj_before_bind, "0000");
			}

			//change binary number into hexa number
			change_hex(result_obj_before_bind, result_obj_code,2);
		}
		//Format3
		else
		{
			//pc is pointing next instruction
			pc = loc + 3;
			p_flag = 1;
			int disp = 0;
			//If there is no operand
			if(operand[0]=='\0')
			{
				//store n, i value
				result_obj_before_bind[6] = '1'; result_obj_before_bind[7] = '1';
				
				//not pc relative
				p_flag = 0;

				//store x,b,p,e value
				xbpe_setting(result_obj_before_bind, x_flag, b_flag, p_flag, e_flag);

				//change binary number into hexa number
				change_hex(result_obj_before_bind, result_obj_code, 3);

				//concatenate "000" to the last of result_obj_code
				strcat(result_obj_code, "000");

				return;
			}
			//Indirect addresssing
			else if(operand[0]=='@')
			{
				char pure_operand[31] = { 0 };	//string which stores operand without @

				//store n, i value
				result_obj_before_bind[6] = '1'; result_obj_before_bind[7] = '0';

				//indirect addressing, so pc relative
				p_flag = 1;
				
				//store x,b,p,e value
				xbpe_setting(result_obj_before_bind, x_flag, b_flag, p_flag, e_flag);

				int operand_index = 1, pure_index = 0;
				//copy operand into pure_operand array without #, @
				while (operand[operand_index] != '\0')
					pure_operand[pure_index++] = operand[operand_index++];
				
				int find = find_symbol(pure_operand);
				if (find == -1)
				{
					printf("Assemble Error in line %d : No such symbol in SYMTAB!\n", lst_line_num);
					error_flag = 1;
					return;
				}
				else
					disp = find - pc;
			}
			//Immediate addressing
			else if(operand[0]=='#')
			{
				char pure_operand[31] = { 0 };	//string which stores operand without #

				//store n, i value
				result_obj_before_bind[6] = '0'; result_obj_before_bind[7] = '1';

				//if got symbol for operand, pc relative
				if ((int)operand[1] >= 65)
					p_flag = 1;
				//if got constant for operand, not pc relative
				else
					p_flag = 0;

				int operand_index = 1, pure_index=0;
				//copy operand into pure_operand array without #, @
				while (operand[operand_index] != '\0')
					pure_operand[pure_index++] = operand[operand_index++];

				//store x,b,p,e value
				xbpe_setting(result_obj_before_bind, x_flag, b_flag, p_flag, e_flag);

				//store operand's disp
				if (p_flag)
				{
					int find = find_symbol(pure_operand);
					if (find == -1)
					{
						printf("Assemble Error in line %d : No such symbol in SYMTAB!\n",lst_line_num);
						error_flag = 1;
						return;
					}
					else
						disp = find_symbol(pure_operand) - pc;
				}
				else
					disp = atoi(pure_operand);
			}
			//Simple addressing
			else
			{
				//store n, i value
				result_obj_before_bind[6] = '1'; result_obj_before_bind[7] = '1';

				char pure_operand[31] = { 0 };		//char array which store operand without , 

				int operand_index = 0, pure_index = 0, comma_flag=0;	 //index of operand and pure_operand, comma_flag is 1 when there is comma in operand
				//copy operand into pure_operand array without ,
				while (operand[operand_index] != '\0')
				{
					if (operand[operand_index] == ',')
					{
						comma_flag = 1;
						operand_index++;
					}
					//if there is comma and X, then it x flag become 1
					if (comma_flag == 1 && operand[operand_index] == 'X')
						x_flag = 1;
					else if (comma_flag == 1 && operand[operand_index] != 'X' && operand[operand_index] != ' ')
					{
						//Wrong instruction after comma
						printf("Assemble Error in the line %d : wrong register after , !\n",lst_line_num);
						error_flag = 1;
						return;
					}
					if(!comma_flag)
						pure_operand[pure_index++] = operand[operand_index];

					operand_index++;
				}
				
				//find symbol in SYMTAB
				int symbol_length = find_symbol(pure_operand);

				if (symbol_length == -1)
				{
					printf("Assemble Error in line %d : No such symbol in SYMTAB!\n",lst_line_num);
					error_flag = 1;
					return;
				}
				disp = symbol_length - pc;

				//if absolute value of difference between operand and pc is over 2047, it is impossible in format 3, so make it BASE relative
				if (abs(disp)>2047)
				{
					if (disp < 0 && disp == 0x8000)
					{
						b_flag = 0;
						p_flag = 0;
					}
					else{
					b_flag = 1;
					p_flag = 0;}
				}

				//store x,b,p,e value
				xbpe_setting(result_obj_before_bind, x_flag, b_flag, p_flag, e_flag);

				//If BASE relative
				if (b_flag)
				{
					int find = find_symbol(BASE_str);
					if (find == -1)
					{
						printf("Assemble Error in line %d : No such symbol in SYMTAB!\n",lst_line_num);
						error_flag = 1;
						return;
					}
					else
					{
						disp = symbol_length - find;

						//if disp is base relative and still out of bound print error message. 
						if (abs(disp) > 2047)
						{
								if(disp > 0 || (disp < 0 && abs(disp)>2048 ) )
								{printf("Assemble Error in line %d : Address out of bound!\n", lst_line_num);
								error_flag = 1;
								return;}
						}
					}
					
				}

			}
			
			//change binary number into hexa number
			change_hex(result_obj_before_bind, result_obj_code, 3);
			
			//store disp number
			disp_addr_set(result_obj_code, disp, 3);

		}
	}
}

/*----------------------------------------*/
/* func_name : num_to_hexchar        	  */
/* purpose : change int number into hex   */
/*			and store in character array  */
/* return  :	none             		  */
/*----------------------------------------*/
void num_to_hexchar(char* value, int num)
{
	int x = 0, i=0;	//x stores quotient or remainder divided by 16, i is index of value arr
	for (int cnt = 0; cnt < 2; cnt++)
	{
		if (cnt == 0)
			x = num / 16;
		else
			x = num % 16;		
		switch (x) {
		
		case 0:
			value[i + 0] = '0'; value[i + 1] = '0'; value[i + 2] = '0'; value[i + 3] = '0';
			break;
		case 1:
			value[i + 0] = '0'; value[i + 1] = '0'; value[i + 2] = '0'; value[i + 3] = '1';
			break;
		case 2:
			value[i + 0] = '0'; value[i + 1] = '0'; value[i + 2] = '1'; value[i + 3] = '0';
			break;
		case 3:
			value[i + 0] = '0'; value[i + 1] = '0'; value[i + 2] = '1'; value[i + 3] = '1';
			break;
		case 4:
			value[i + 0] = '0'; value[i + 1] = '1'; value[i + 2] = '0'; value[i + 3] = '0';
			break;
		case 5:
			value[i + 0] = '0'; value[i + 1] = '1'; value[i + 2] = '0'; value[i + 3] = '1';
			break;
		case 6:
			value[i + 0] = '0'; value[i + 1] = '1'; value[i + 2] = '1'; value[i + 3] = '0';
			break;
		case 7:
			value[i + 0] = '0'; value[i + 1] = '1'; value[i + 2] = '1'; value[i + 3] = '1';
			break;
		case 8:
			value[i + 0] = '1'; value[i + 1] = '0'; value[i + 2] = '0'; value[i + 3] = '0';
			break;
		case 9:
			value[i + 0] = '1'; value[i + 1] = '0'; value[i + 2] = '0'; value[i + 3] = '1';
			break;
		case 10:
			value[i + 0] = '1'; value[i + 1] = '0'; value[i + 2] = '1'; value[i + 3] = '0';
			break;
		case 11:
			value[i + 0] = '1'; value[i + 1] = '0'; value[i + 2] = '1'; value[i + 3] = '1';
			break;
		case 12:
			value[i + 0] = '1'; value[i + 1] = '1'; value[i + 2] = '0'; value[i + 3] = '0';
			break;
		case 13:
			value[i + 0] = '1'; value[i + 1] = '1'; value[i + 2] = '0'; value[i + 3] = '1';
			break;
		case 14:
			value[i + 0] = '1'; value[i + 1] = '1'; value[i + 2] = '1'; value[i + 3] = '0';
			break;
		case 15:
			value[i + 0] = '1'; value[i + 1] = '1'; value[i + 2] = '1'; value[i + 3] = '1';
			break;
		}

		i += 4;	//go to next 4
	}
}

/*-------------------------------------------*/
/* func_name : xbpe_setting       		  	 */
/* purpose : set value int the value array   */
/*				depends on x,b,p,e value     */
/* return  :	none             			 */
/*-------------------------------------------*/
void xbpe_setting(char *value, int x, int b, int p, int e)
{
	if (x == 1)
		value[8] = '1';
	else
		value[8] = '0';

	if (b == 1)
		value[9] = '1';
	else
		value[9] = '0';

	if (p == 1)
		value[10] = '1';
	else
		value[10] = '0';

	if (e == 1)
		value[11] = '1';
	else
		value[11] = '0';
}

/*----------------------------------------*/
/* func_name : find_symbol                */
/* purpose : find symbol in the SYMTAB    */
/*   	     return symbol's LOCCTR   	  */
/* return  : symbol's LOCCTR              */
/*----------------------------------------*/
int find_symbol(char* operand) {
	SYM* cur=first;
	while (cur != NULL)
	{
		if (!(strcmp(cur->label, operand)))
			return cur->locctr;
		cur = cur->next;
	}
	
	//if not found, return -1
	return -1;
}

/*-------------------------------------------------------*/
/* func_name : change_hex            					 */
/* purpose : change binary number character array into   */
/*   	     hex array   								 */
/* return  : none                         				 */
/*-------------------------------------------------------*/
void change_hex(char* bin, char* hex, int format_num)
{
	int k = 0;			//index of hex array
	//if format_number is 1, change only 8 bits into hex
	if (format_num == 1)
	{
		for (int i = 0; i < 8; )
		{
			int two = 8, num = 0;
			for (int j = 0; j < 4; j++)
			{
				num += two * ((int)bin[i + j] - 48);
				two /= 2;
			}
			hex[k++] = hex_char_return(num);
			i += 4;
		}
	}
	else if (format_num == 2)
	{
		for (int i = 0; i < 15; )
		{
			int two = 8, num = 0;
			for (int j = 0; j < 4; j++)
			{
				num += two * ((int)bin[i + j] - 48);
				two /= 2;
			}
			hex[k++] = hex_char_return(num);
			i += 4;
		}
	}
	//else change 12 bits into hex
	else
	{
		for (int i = 0; i < 11; )
		{
			int two = 8, num=0;
			for (int j = 0; j < 4; j++)
			{
				num += two * ((int)bin[i + j] - 48);
				two /= 2;
			}
			hex[k++]=hex_char_return(num);
			i += 4;
		}
	}


}

/*----------------------------------------		  */
/* func_name : hex_char_return            		  */
/* purpose : change int number into hex character */
/* return  : none                        		  */
/*------------------------------------------------*/
char hex_char_return(int num)
{
	switch (num)
	{
	case 0:
		return '0';
		break;
	case 1:
		return '1';
		break;
	case 2:
		return '2';
		break;
	case 3:
		return '3';
		break;
	case 4:
		return '4';
		break;
	case 5:
		return '5';
		break;
	case 6:
		return '6';
		break;
	case 7:
		return '7';
		break;
	case 8:
		return '8';
		break;
	case 9:
		return '9';
		break;
	case 10:
		return 'A';
		break;
	case 11:
		return 'B';
		break;
	case 12:
		return 'C';
		break;
	case 13:
		return 'D';
		break;
	case 14:
		return 'E';
		break;
	case 15:
		return 'F';
		break;
	}
}


/*----------------------------------------------*/
/* func_name : disp_addr_set            		*/
/* purpose : put disp or address number into    */
/*   	     result object code character array */
/* return  : symbol's LOCCTR                    */
/*----------------------------------------------*/
void disp_addr_set(char *arr, int num, int format_type)
{
	int i = 3, quotient = 0; // index of arr, and variable which stores quotient 
	int hexx = 0;	//variable which stores pow of 16
	
	//if disp is negative, change it into hex
	if (num < 0)
	{
		//if num is negative, multiply -1
		num *= -1, hexx = 256;
		char temp_num[13] = { 0 };

		//change num*-1 into binary number 
		for (int j = 0; j < 12; )
		{
			quotient = num / hexx;
			num = num - (quotient * hexx);
			switch (hex_char_return(quotient)) {
			case '0':
				temp_num[j + 0] = '0'; temp_num[j + 1] = '0'; temp_num[j + 2] = '0'; temp_num[j + 3] = '0';
				break;
			case '1':
				temp_num[j + 0] = '0'; temp_num[j + 1] = '0'; temp_num[j + 2] = '0'; temp_num[j + 3] = '1';
				break;
			case '2':
				temp_num[j + 0] = '0'; temp_num[j + 1] = '0'; temp_num[j + 2] = '1'; temp_num[j + 3] = '0';
				break;
			case '3':
				temp_num[j + 0] = '0'; temp_num[j + 1] = '0'; temp_num[j + 2] = '1'; temp_num[j + 3] = '1';
				break;
			case '4':
				temp_num[j + 0] = '0'; temp_num[j + 1] = '1'; temp_num[j + 2] = '0'; temp_num[j + 3] = '0';
				break;
			case '5':
				temp_num[j + 0] = '0'; temp_num[j + 1] = '1'; temp_num[j + 2] = '0'; temp_num[j + 3] = '1';
				break;
			case '6':
				temp_num[j + 0] = '0'; temp_num[j + 1] = '1'; temp_num[j + 2] = '1'; temp_num[j + 3] = '0';
				break;
			case '7':
				temp_num[j + 0] = '0'; temp_num[j + 1] = '1'; temp_num[j + 2] = '1'; temp_num[j + 3] = '1';
				break;
			case '8':
				temp_num[j + 0] = '1'; temp_num[j + 1] = '0'; temp_num[j + 2] = '0'; temp_num[j + 3] = '0';
				break;
			case '9':
				temp_num[j + 0] = '1'; temp_num[j + 1] = '0'; temp_num[j + 2] = '0'; temp_num[j + 3] = '1';
				break;
			case 'A':
				temp_num[j + 0] = '1'; temp_num[j + 1] = '0'; temp_num[j + 2] = '1'; temp_num[j + 3] = '0';
				break;
			case 'B':
				temp_num[j + 0] = '1'; temp_num[j + 1] = '0'; temp_num[j + 2] = '1'; temp_num[j + 3] = '1';
				break;
			case 'C':
				temp_num[j + 0] = '1'; temp_num[j + 1] = '1'; temp_num[j + 2] = '0'; temp_num[j + 3] = '0';
				break;
			case 'D':
				temp_num[j + 0] = '1'; temp_num[j + 1] = '1'; temp_num[j + 2] = '0'; temp_num[j + 3] = '1';
				break;
			case 'E':
				temp_num[j + 0] = '1'; temp_num[j + 1] = '1'; temp_num[j + 2] = '1'; temp_num[j + 3] = '0';
				break;
			case 'F':
				temp_num[j + 0] = '1'; temp_num[j + 1] = '1'; temp_num[j + 2] = '1'; temp_num[j + 3] = '1';
				break;
			}
			j += 4;
			hexx /= 16;
		}
		//XOR every binary number
		for (int j = 0; j < 12; j++)
		{
			if (temp_num[j] == '0')
				temp_num[j] = '1';
			else
				temp_num[j] = '0';
		}
		//add 1 to num*-1
		int temp_index = 11;
		while (temp_index >= 0)
		{
			//if temp_num[temp_index] is 0, just add 1
			if (temp_num[temp_index] == '0')
			{
				temp_num[temp_index] = '1';
				break;
			}
			//if temp_num[temp_index] is 1, there is carry
			else
				temp_num[temp_index] = '0';

			temp_index--;
		}

		//change temp_number into hex number and store in temp_hex array 
		char temp_hex[3] = { 0 };
		change_hex(temp_num, temp_hex, 3);

		//store hexa number in arr
		for (int j = 0; j < 3; j++)
			arr[i++] = temp_hex[j];

	}
	//format 4, address
	else if (num > 4095 || format_type == 4)
	{
		hexx = 65536;
		for (int j = 0; j < 5; j++) {
			quotient = num / hexx;
			num = num - (quotient * hexx);
			arr[i++] = hex_char_return(quotient);
			hexx /= 16;
		}
	}
	//format 3, disp
	else
	{
		hexx = 256;
		for (int j = 0; j < 3; j++) {
			quotient = num / hexx;
			num = num - (quotient * hexx);
			arr[i++] = hex_char_return(quotient);
			hexx /= 16;
		}
	}
}

/*----------------------------------------*/
/* func_name : register_num               */
/* purpose : get register character and   */
/*   	     store number in the array    */
/* return  : none                         */
/*----------------------------------------*/
void register_num(char *arr, char* reg)
{
	if (!strcmp(reg, "A"))
		strcat(arr, "0000");
	else if (!strcmp(reg, "X"))
		strcat(arr, "0001");
	else if (!strcmp(reg, "L"))
		strcat(arr, "0010");
	else if (!strcmp(reg, "PC"))
		strcat(arr, "1000");
	else if (!strcmp(reg, "SW"))
		strcat(arr, "1001");
	else if (!strcmp(reg, "B"))
		strcat(arr, "0101");
	else if (!strcmp(reg, "S"))
		strcat(arr, "0100");
	else if (!strcmp(reg, "T"))
		strcat(arr, "0101");
	else if(!strcmp(reg, "F"))
		strcat(arr, "0110");
	else
	{
		printf("Assemble Error on line %d : No such register!\n", lst_line_num);
		error_flag;
		return;
	}
}


/*--------------------------------------------*/
/* func_name : command_porgaddr               */
/* purpose : Progaddr 명령어 뒤에 나온 문자를 읽어서 숫자로 변환한 뒤에 progaddr 변수에 저장한다.   */
/* return  : none                             */
/*--------------------------------------------*/
void command_progaddr()
{
	int i = 9, j = 0;		//command와 str_progaddr의 index

	//file 이름을 아래의 while문으로 얻는다. 
	while (1)
	{
		if (command[i] == '\0')
			break;

		if (command[i] == ' ')
		{
			//address에 빈칸이 있으면 error
			printf("Error : Invalid PROGADDR!\n");
			error_flag = 1;
			return;
		}
		//Valid PROGADDR
		else if (distinct_ascii(command[i] == 0))
			str_progaddr[j++] = command[i++];
		else
		{
			//타당하지 않은 주소 있으면 에러
			printf("Error : Invalid PROGADDR!\n");
			error_flag = 1;
			return;
		}
	}
	//string을 16진수로 변환
	progaddr = strtol(str_progaddr, NULL, 16);
	csaddr = progaddr;	//csaddr에 progaddr 저장
	REG[8] = progaddr;	//PC 레지스터에 progaddr 저장
}

/*-------------------------------------------------------------------*/
/* func_name : command_loader              							 */
/* purpose : Loader 명령어 뒤에 들어온 obj 파일을 link하고 결과를 가상 메모리에 저장한다. 또한 load map도 출력한다. */
/* return  : none                          							 */
/*-------------------------------------------------------------------*/
void command_loader()
{
	//obj_name과 obj_name_size를 처음 loader를 받은 것이 아니면 초기화 
	memset(obj_name, 0, sizeof(obj_name));
	obj_name_size = 1;

	int i = 7, j = 0, k = 0;		//command, str_filename, obj_name의 index
	
	//object file이름을 아래의 while문을 통해 얻는다. 
	while (1)
	{
		if (command[i] == '\0')
			break;
		if (command[i] == ' ')
		{
			k++;
			j = 0;
			i++;
			obj_name_size++;
		}
		obj_name[k].str_filename[j++] = command[i++];
	}

	//Pass1
	int pass1_obj_name_size = obj_name_size;
	for(i=0;i<pass1_obj_name_size;i++)
	{
		FILE* fp_obj = fopen(obj_name[i].str_filename, "r");
		if (fp_obj == NULL)
		{
			printf("Error : No such file in the directory!\n");
			error_flag = 1;
			return;
		}
		else {
			if (i == 0)
			{
				//만일 존재하면 ESTAB free
				free_estab();
			}
			while (1) {
				//read 1 line
				if (fgets(line_obj, 270, fp_obj) == NULL)
					break;
				//아래 함수 호출을 통해 record에 따라 구별한다. 
				pass1_line_distinct(line_obj, i);
			}
		}
		fclose(fp_obj);
	}
	print_loadmap();
	//Pass2
	csaddr = progaddr;		//CSADDR에 PROGADDR 저장
	execaddr = progaddr;	//ESECADDR에 PROGADDR 저장
	for (i = 0; i < obj_name_size; i++)
	{
		FILE* fp_obj = fopen(obj_name[i].str_filename, "r");
		//각 obj 파일을 열 때마다 reference number에 관한 배열 동적 할당
		reference_number = (int*)malloc(sizeof(int) * (estab_number+1));
			while (1) {
			//read 1 line
			if (fgets(line_obj, 270, fp_obj) == NULL)
				break;
			//아래 함수 호출을 통해 record에 따라 구별
			pass2_line_distinct(line_obj, i);
			memset(modi_value, 0, sizeof(modi_value));
		}
		fclose(fp_obj);
		free(reference_number);
	}
	
	//프로그램이 load 되면 REGISTER 값들 초기화
	for(int i=0; i<13; i++)
	{
		//PC Register
		if(i==8)
			REG[i] = progaddr;
		//L Register
		else if(i==2)
			REG[i] = program_length;
		else
			REG[i] = 0;
	}	
}

/*----------------------------------------------*/
/* func_name : pass1_line_distinct              */
/* purpose : 각 줄에 대한 Pass1 실행한다. 한 줄을 읽어들였을 때,   */
/*	해당 줄의 첫 번째 문자를 통해 Head, Define, End Record를 나눠준다. 그리고 각 줄에 대한 정보를 ESTAB에 저장한다		*/
/* return  : none                               */
/*----------------------------------------------*/
void pass1_line_distinct(char* one_line_obj, int obj_index)
{
	int line_index = 1;		//one_line_obj 의 index
	int record_flag = 0;	//flag : 1-> Head Record, 2-> Define Record, 3->End Record
	char _name[7] = { 0 };	// 해당 줄의 program 또는 symbol 이름을 저장하는 배열
	char _addr[7] = { 0 };		// 해당 줄의 program 또는 symbol 주소를 저장하는 배열
	int str_index = 0;		//  _name 또는 _addr의 index
	if (one_line_obj[0] == 'H')
		record_flag = 1;
	else if (one_line_obj[0] == 'D')
		record_flag = 2;
	else if (one_line_obj[0] == 'E')
		record_flag = 3;
	else	
		return;

	//Head Record
	if (record_flag == 1)
	{
		char _length[7] = { 0 };		//프로그램의 길이를 저장하는 배열

		//Program Name 저장
		for (line_index; line_index <= 6; line_index++)
		{
			if (one_line_obj[line_index] == ' ')
				continue;
			_name[str_index++] = one_line_obj[line_index];
		}
		str_index = 0;
		
		//Program Address 저장
		for(line_index; line_index<=12;line_index++)
			_addr[str_index++] = one_line_obj[line_index];
		str_index = 0;

		//Program Length 저장
		for (line_index; line_index <= 18; line_index++)
			_length[str_index++] = one_line_obj[line_index];
		
		obj_name[obj_index].cslth = strtol(_length, NULL, 16);

		//linked list 통해서 ESTAB에 저장
		ESTAB* es_node = (ESTAB*)malloc(sizeof(ESTAB));
		strcpy(es_node->control_section, _name);		//control section name 저장
		es_node->addr = strtol(_addr, NULL, 16) + csaddr;	//CSADDR을 address에 더해서 저장
		es_node->cslth = strtol(_length, NULL, 16);		//head symbol의 CSLTH 저장
		es_node->symbol_name[0] = '\0';	//symbol_name에는 '\0' 저장.
		es_node->next = NULL;
		if (estab_head == NULL)
		{
			estab_head = es_node;
			estab_prev = es_node;
			estab_number++;
		}
		else
		{
			//동일한 이름이 ESTAB에 있으면 에러 메시지 출력
			if (find_estab(_name) != -1)
			{
				printf("Error : Same external symbol in the ESTAB!\n");
				error_flag = 1;
				return;
			}
			estab_prev->next = es_node;
			estab_prev = es_node;
			estab_number++;
		}
	}
	//Define Record
	else if(record_flag ==2)
	{ 
		while (one_line_obj[line_index] != '\n') {
			int six_name = line_index + 5;	//line_index의 6 char 뒤의 정보 저장하는 index
			int six_addr = line_index + 11; //line_index의 11 char 뒤의 정보 저장하는 index

			//External Symbol Name 저장
			for (line_index; line_index <= six_name; line_index++)
			{
				if (one_line_obj[line_index] == ' ')
					continue;
				_name[str_index++] = one_line_obj[line_index];
			}
			str_index = 0;

			//Relative address of symbol 저장
			for (line_index; line_index <= six_addr; line_index++)
				_addr[str_index++] = one_line_obj[line_index];
			str_index = 0;

			//linked list 통해서 ESTAB에 저장
			ESTAB* es_node = (ESTAB*)malloc(sizeof(ESTAB));
			strcpy(es_node->symbol_name, _name);
			es_node->addr = strtol(_addr, NULL, 16) + csaddr;	//CSADDR을 address에 더해서 저장
			es_node->control_section[0] = '\0';	//control_section에는 '\0' 저장
			es_node->next = NULL;
			if (estab_head == NULL)
			{
				estab_head = es_node;
				estab_prev = es_node;
				estab_number++;
			}
			else
			{
				//동일한 이름이 ESTAB에 있으면 에러 메시지 출력
				if (find_estab(_name) != -1)
				{
					printf("Error : Same external symbol in the ESTAB!\n");
					error_flag = 1;
					return;
				}
				estab_prev->next = es_node;
				estab_prev = es_node;
				estab_number++;
			}

			//리셋 _name, _addr
			memset(_name, 0, sizeof(_name));
			memset(_addr, 0, sizeof(_addr));
		}
	}
	//End Record, CSLTH을 CSADDR에 더함
	else if (record_flag == 3)
		csaddr += obj_name[obj_index].cslth;
}

/*------------------------------------------*/
/* func_name : print_loadmap            	*/
/* purpose : Pass1을 성공적으로 수행했을 때, 나오는 ESTAB에 관한 load map을 출력한다. 		*/
/* return  : none                           */
/*------------------------------------------*/
void print_loadmap()
{
	int total_length = 0;	//link한 프로그램의 총 길이를 저장하는 변수
	printf("control  symbol  address length\n");
	printf("section  name  \n");
	printf("--------------------------------\n");
	ESTAB* cur = estab_head;
	while (cur != NULL)
	{
		//Head ESTAB
		if (cur->symbol_name[0] == '\0')
		{
			printf("%-8s          %5.4X%7.4X\n", cur->control_section, cur->addr, cur->cslth);
			total_length += cur->cslth;
		}
		//Define ESTAB
		else
			printf("        %7s   %5.4X\n",  cur->symbol_name, cur->addr);
		cur = cur->next;
	}
	printf("--------------------------------\n");
	printf("          total  length %6.4X\n", total_length);
	
	//total program length을 L REGISTER에 저장
	REG[2] = total_length;
	program_length = total_length;
}

/*--------------------------------------------------------------------*/
/* func_name : free_estab        									  */
/* purpose :	만든 ESTAB을 모두 free 시켜준다.     						  */
/* return  : none                         							  */
/*--------------------------------------------------------------------*/
void free_estab()
{
	//만일 ESTAB linked list가 존재하면 전부 free 시켜준다.
	if (estab_head)
	{
		ESTAB* cur = estab_head;	//현재 노드 가리키는 포인터
		while (1)
		{
			if (cur == NULL)
				break;
			ESTAB* prev = cur;	//prev는 free 하기 위해 사용된다.
			cur = cur->next;
			free(prev);
		}
		estab_head = NULL;
	}
}

/*------------------------------------------*/
/* func_name : pass2_line_distinct          */
/* purpose :각 줄에 대한 Pass1 실행한다. 한 줄을 읽어들였을 때, */
/*해당 줄의 첫 번째 문자를 통해 Head, Reference, Text, Modification, End Record를 나눠준다. */
/*Head 또는 Reference 일 때는 나온 reference number에 대한 배열을 동적할당한 뒤 거기에 주소에 대한 정보를 저장한다. */
/*Text 일 때는 해당 정보들을 읽어서 가상 메모리에 저장하고, modification을 통해 수정하여 준다. 그리고 end일 때는 csaddr, execaddr를 변화시켜준다	*/
/* return  : none                           */
/*------------------------------------------*/
void pass2_line_distinct(char* one_line_obj, int obj_index)
{
	int line_index = 1;		//one_line_obj의 index
	int record_flag = -1;	//flag : 0-> Head Record, 4->Refernce Record, 1-> Text Record, 2-> Modification Record, 3->End Record
	char _addr[7] = { 0 };		//program 또는 symbol 주소를 저장하는 배열
	char modi_symbol[31] = { 0 };	//modification line에 대한 정보를 저장하는 배열
	int str_index = 0;		// _name 또는 _addr의 index
	int dump_index = 0;	// virtual memory의 index
	if (one_line_obj[0] == 'H')
		record_flag = 0;
	else if (one_line_obj[0] == 'R')
		record_flag = 4;
	else if (one_line_obj[0] == 'T')
		record_flag = 1;
	else if (one_line_obj[0] == 'M')
		record_flag = 2;
	else if (one_line_obj[0] == 'E')
		record_flag = 3;
	else
		return;
	//Head Record
	if(record_flag == 0)
	{
		//Program Name을 Reference Number 01로 저장
		for (line_index; line_index <= 6; line_index++)
		{
			if (one_line_obj[line_index] == ' ')
				continue;
			modi_symbol[str_index++] = one_line_obj[line_index];
		}
		//SProgram Name's PROGADDR 을 해당 reference_number 배열에 저장
		reference_number[1] = find_estab(modi_symbol);
	}
	//Reference Record
	else if(record_flag == 4)
	{
		int out_flag = 0;	///해당 줄의 끝을 알기 위한 flag
		
		//모든 reference number를 아래 while 문 통해서 저장. 
		while (!out_flag) {
			char _length[3] = { 0 };		//reference number의 char로 저장하는 배열
			
			int i = line_index, end_i = line_index +2;		//reference number를 정수로 변환한 것을 저장하는 변수
			
			//reference number에 관한 정보 저장
			for (i; i < end_i; i++)
				_length[str_index++] = one_line_obj[i];
			
			str_index = 0;

			end_i = i + 6
					;
			//reference record의 끝 판단
			for (i; i < end_i; i++)
			{
				if (one_line_obj[i] == ' ')
					continue;
				if (one_line_obj[i] == '\n')
				{
					out_flag = 1;
					break;
				}
				modi_symbol[str_index++] = one_line_obj[i];
			}
			int refer_index = (int)strtol(_length, NULL, 16);

			//Symbol's ADDR을 reference_number 배열에 저장
			reference_number[refer_index] = find_estab(modi_symbol);

			memset(modi_symbol, 0, sizeof(modi_symbol));	//modi_symbol array 초기화
			str_index = 0;
			line_index += 8;

			//만일 마지막 define record symbol이 6 words이면, 다음이 \n인지 확인
			if (one_line_obj[line_index] == '\n')
				out_flag = 1;
		}
	}
	//Text Record
	else if (record_flag == 1)
	{
		int text_length=0;	//text 줄의 길이를 저장하는 변수
		char _length[3] = { 0 };		//text record 길이를 char로 저장하는 배열
		char obj_value[3] = { 0 };		//해당 줄의 1바이트의 object code를 저장하는 배열
		
		//Text Address 저장
		for (line_index; line_index <= 6; line_index++)
		{
			if (one_line_obj[line_index] == ' ')
				continue;
			_addr[str_index++] = one_line_obj[line_index];
		}
		str_index = 0;

		//Text Length 저장
		for (line_index; line_index <= 8; line_index++)
			_length[str_index++] = one_line_obj[line_index];
		str_index = 0;

		//start addr와 length 16진수로 변환
		dump_index = strtol(_addr, NULL, 16);
		text_length = strtol(_length, NULL, 16);

		int format_index = 0; 	//line index 뒤에 얼마나 많은 char를 읽어야 하는지에 대한 정보 저장하는 변수
		for(int cnt_obj=0; cnt_obj < text_length ; )
		{
			//Format 4, 8 characters 읽어야함. 
			if(one_line_obj[line_index+2] == '1')
				format_index = line_index + 8;
			//format 4 아니면 6 characters 읽어야함
			else
				format_index = line_index + 6;	
			for (int i = line_index; i < format_index;)
			{
				//obj_value에 text record의 1 바이트 정보 저장
				for (int j = i; j < i + 2; j++)
					obj_value[j - i] = one_line_obj[line_index++];
			
				//obj_value를 virtual memory에 저장
				memory[dump_index + csaddr + cnt_obj] = (unsigned char)strtol(obj_value, NULL, 16);
				cnt_obj++;
				i += 2;
				memset(obj_value, 0, sizeof(obj_value));
			}
		}

	}
	//Modification Record
	else if(record_flag == 2)
	{
		int modi_addr=0, modi_num = 0;	// address of modification과 modified가 얼마나 많은 half byte가 되야하는가에 대한 정보 저장하는 변수
		int arith_flag = 0;		//flag : 0 -> +(add) 1 -> -(minus)
		char _length[3] = { 0 };		//length of program 저장하는 배열
		memset(modi_value, 0, sizeof(modi_value));

		//Text Address 저장
		for (line_index; line_index <= 6; line_index++)
		{
			if (one_line_obj[line_index] == ' ')
				continue;
			_addr[str_index++] = one_line_obj[line_index];
		}
		str_index = 0;

		//modi_num 저장
		for (line_index; line_index <= 8; line_index++)
			_length[str_index++] = one_line_obj[line_index];
		str_index = 0;

		//Distinct + or - arithmetic
		if (one_line_obj[line_index++] == '-')
			arith_flag = 1;

		//Symbol's reference number를 modi_symbol에 저장
		while (one_line_obj[line_index] != '\n')
			modi_symbol[str_index++] = one_line_obj[line_index++];


		//addr과 number 16진수로 변환
		modi_addr = strtol(_addr, NULL, 16);
		modi_num = strtol(_length, NULL, 16);
		modi_addr += csaddr;
		
		//modified 되어야 할 값을 string으로 저장. 
		for (int i = 0; i < 3; i++)
		{
			int divide_modi = 0;	//ascii 코드 판별하기 위한 변수
			divide_modi = (int)memory[modi_addr + i] / 16;
			if (divide_modi >= 10)
				modi_value[i * 2] = (unsigned char)(divide_modi + 55);
			else
				modi_value[i*2] = (unsigned char)(divide_modi + 48);

			divide_modi = (int)memory[modi_addr + i] % 16;
			if (divide_modi >= 10)
				modi_value[i * 2+1] = (unsigned char)(divide_modi + 55);
			else
			modi_value[i*2 + 1] = (unsigned char)((int)memory[modi_addr + i] % 16 + 48);
		 }

		//reference number string을 16진수 정수로 변환into the integer
		int refer_index = (int)strtol(modi_symbol , NULL, 16);

		//calculate_modi을 불러서 modification record 계산
		calulate_modi(modi_value, modi_num, reference_number[refer_index], arith_flag);

		for (int i = 0; i < 3; i++)
		{
			char byte_value[2] = { 0 };	//modified value의 1바이트 정보 저장하는 배열

			//modified value의 1바이트 정보를 byte_value 배열에 저장
			for (int j = i*2; j < i*2 + 2; j++)
				byte_value[j - i*2] = modi_value[j];

			//byte_value를 virtual memory에 저장
			memory[modi_addr + i] = (unsigned char)strtol(byte_value, NULL, 16);
		}

	}
	//End Record is read, CSLTH을 CSADDR에 더해줌
	else if (record_flag == 3)
	{
		//executable instruction의 주소가 존재하면 그 주소 저장
		if (one_line_obj[1] != ' ' && one_line_obj[1] != '\n' && one_line_obj[1] != '\0') {
			//End Address 저장
			for (line_index; line_index <= 6; line_index++)
			{
				if (one_line_obj[line_index] == ' ')
					continue;
				_addr[str_index++] = one_line_obj[line_index];
			}
		//EXECADDR = (CSADDR + specified address)
		execaddr = csaddr + strtol(_addr, NULL, 16);
		}
		//CSLTH을 CSADDR에 더함. 
		csaddr += obj_name[obj_index].cslth;
	}
	}

/*---------------------------------------*/
/* func_name : find_estab                */
/* purpose : ESTAB에서 operand에 해당 내용 일치하는 내용 찾으면 해당 주소 return, 없으면 -1 return     */
/* return  : symbol's address            */
/*---------------------------------------*/
int find_estab(char* operand) {
	ESTAB* cur = estab_head;	////현재 노드 가리키는 포인터
	while (cur != NULL)
	{
		if (!(strcmp(cur->control_section, operand))  || !(strcmp(cur->symbol_name,operand)) )
			return cur->addr;
		cur = cur->next;
	}
	//if not found, return -1
	return -1;
}

/*----------------------------------------*/
/* func_name : calculate_modi             */
/* purpose : Modification 해줘야 하는 값에 + 또는 – 연산을 해준다.   */
/* return  : symbol's address             */
/*----------------------------------------*/
void calulate_modi(char* operand, int change_num, int estab_symbol, int arith_flag) {
	
	char* temp_modi_operand;		// modified된 값을 저장하는 배열
	int oper = 0;			//operand의 정수 값을 저장하는 변수
	int temp_modi_index = 0;	// temp_modi_operand의 index
	
	//Format 4, 5개의 half byte 바꿈, 배열 왼쪽으로 전부 shift 
	if (change_num == 5)
	{
		for (int i = 0; i < 5; i++)
			operand[i] = operand[i + 1];
		operand[5] = '\0';
	}

	//modi value를 저장하기 위해 동적할당
	temp_modi_operand = (char*)malloc(sizeof(char) * (change_num+1));
	
	//만약 양수면, operand integer로 바꾸어서 계산
	if (operand[0] < '8')
		oper = strtol(operand, NULL, 16);
	//음수면, 아래 함수 호출
	else
		oper=string_to_negin(operand, change_num);
	memset(operand, 0, sizeof(operand));
	//Add
	if (arith_flag == 0)
		oper += estab_symbol;
	//Minus
	else
		oper -= estab_symbol;

	//oper가 negative, 8 character string이 된다. 
	if (oper < 0)
	{
		temp_modi_operand = (char*)realloc(temp_modi_operand, 9 * sizeof(int));
		//oper를 8 단어 string으로 변환하고 temp_modi_operand에 저장
		sprintf(temp_modi_operand, "%X", oper);

		//oper가 음수면, 8 word string이 되므로 바꾸어야한다. 
		int temp_modi_oper_index = 2;
		if (change_num == 5)
			temp_modi_oper_index = 3;
		
		//아래 for문을 통해 바뀐 값을 operand 배열에 저장
		for (int i = temp_modi_oper_index; i < 8; i++)
		{
			char te[2];	//임시 값 저장하는 배열 
			te[0] = temp_modi_operand[i];
			te[1] = temp_modi_operand[i + 1];
			strncat(operand, te, 1);
		}
		
	}
	else {

		if (change_num == 5)
		{
			//oper을 5 word string으로 변환한뒤 temp_modi_operand에 저장
			sprintf(temp_modi_operand, "%5X", oper);
			strcat(operand, "1");
		}
		else {
			//oper을 6 word string으로 변환한뒤 temp_modi_operand에 저장
			sprintf(temp_modi_operand, "%6X", oper);
		}

		//빈칸 있으면 해당 자리에 0 대입
		while (temp_modi_operand[temp_modi_index] == ' ')
			temp_modi_operand[temp_modi_index++] = '0';

		//최종 결과를 operand에 저장
		strcat(operand, temp_modi_operand);
	}
	free(temp_modi_operand);
}

/*----------------------------------------*/
/* func_name : string_to_negint           */
/* purpose : 만일 음수인 경우, 음수 정수 값으로 변환   */
/* return  : integer value of string      */
/*----------------------------------------*/
int string_to_negin(char* operand, int length)
{
	char *binary_operand= (char*)calloc(length*4, sizeof(char));		//operand의 binary 값 저장하는 배열
	int result = 0, two = 1;	//operand의 결과 값, 2의 지수 승 저장하는 변수
	for (int i = 0; i < length; i++)
	{
		//각 단어를 2진수로 바꾼뒤 아래 switch 문을 통해 xor 연산을 한다. (2's complement 연산해줌)
		switch (operand[i]) {
		case 'F':
			binary_operand[i * 4] = '0';
			binary_operand[i * 4 + 1] = '0';
			binary_operand[i * 4 + 2] = '0';
			binary_operand[i * 4 + 3] = '0';
			break;
		case 'E':
			binary_operand[i * 4] = '0';
			binary_operand[i * 4 + 1] = '0';
			binary_operand[i * 4 + 2] = '0';
			binary_operand[i * 4 + 3] = '1';
			break;
		case 'D':
			binary_operand[i * 4] = '0';
			binary_operand[i * 4 + 1] = '0';
			binary_operand[i * 4 + 2] = '1';
			binary_operand[i * 4 + 3] = '0';
			break;
		case 'C':
			binary_operand[i * 4] = '0';
			binary_operand[i * 4 + 1] = '0';
			binary_operand[i * 4 + 2] = '1';
			binary_operand[i * 4 + 3] = '1';
			break;
		case 'B':
			binary_operand[i * 4] = '0';
			binary_operand[i * 4 + 1] = '1';
			binary_operand[i * 4 + 2] = '0';
			binary_operand[i * 4 + 3] = '0';
			break;
		case 'A':
			binary_operand[i * 4] = '0';
			binary_operand[i * 4 + 1] = '1';
			binary_operand[i * 4 + 2] = '0';
			binary_operand[i * 4 + 3] = '1';
			break;
		case '9':
			binary_operand[i * 4] = '0';
			binary_operand[i * 4 + 1] = '1';
			binary_operand[i * 4 + 2] = '1';
			binary_operand[i * 4 + 3] = '0';
			break;
		case '8':
			binary_operand[i * 4] = '0';
			binary_operand[i * 4 + 1] = '1';
			binary_operand[i * 4 + 2] = '1';
			binary_operand[i * 4 + 3] = '1';
			break;
		case '7':
			binary_operand[i * 4] = '1';
			binary_operand[i * 4+1] = '0';
			binary_operand[i * 4+2] = '0';
			binary_operand[i * 4+3] = '0';
			break;
		case '6':
			binary_operand[i * 4] = '1';
			binary_operand[i * 4 + 1] = '0';
			binary_operand[i * 4 + 2] = '0';
			binary_operand[i * 4 + 3] = '1';
			break;
		case '5':
			binary_operand[i * 4] = '1';
			binary_operand[i * 4 + 1] = '0';
			binary_operand[i * 4 + 2] = '1';
			binary_operand[i * 4 + 3] = '0';
			break;
		case '4':
			binary_operand[i * 4] = '1';
			binary_operand[i * 4 + 1] = '0';
			binary_operand[i * 4 + 2] = '1';
			binary_operand[i * 4 + 3] = '1';
			break;
		case '3':
			binary_operand[i * 4] = '1';
			binary_operand[i * 4 + 1] = '1';
			binary_operand[i * 4 + 2] = '0';
			binary_operand[i * 4 + 3] = '0';
			break;
		case '2':
			binary_operand[i * 4] = '1';
			binary_operand[i * 4 + 1] = '1';
			binary_operand[i * 4 + 2] = '0';
			binary_operand[i * 4 + 3] = '1';
			break;
		case '1':
			binary_operand[i * 4] = '1';
			binary_operand[i * 4 + 1] = '1';
			binary_operand[i * 4 + 2] = '1';
			binary_operand[i * 4 + 3] = '0';
			break;
		case '0':
			binary_operand[i * 4] = '1';
			binary_operand[i * 4 + 1] = '1';
			binary_operand[i * 4 + 2] = '1';
			binary_operand[i * 4 + 3] = '1';
			break;
		default:
			break;
		}
	}
	for (int i = length * 4 - 1; i >= 0; i--)
	{
		if (binary_operand[i] == '1')
			result += two;
		two *= 2;
	}
	//2's complement을 위해 1을 더해줌
	result += 1;
	//value of result에 -1 곱한 값 return
	return (-1) * result;
}

/*------------------------------------------*/
/* func_name : print_bp                     */
/* purpose :  저장한 모든 breakpoint 출력			*/
/* return  : none                           */
/*------------------------------------------*/
void print_bp()
{
	//만일 bp linked list가 존재하면, 해당 내용 전부 출력
	if (bp_head) {
		printf("\t     breakpoint\n");
		printf("\t     ----------\n");
		bp* cur = bp_head;
		while (cur != NULL)
		{
			//breakpoint 출력
			printf("\t     %X\n", cur->breakpoint);
			cur = cur->next;
		}
	}
	//만일 존재하지 않으면 에러 메시지 출력
	else
	{
		printf("\t     Error! : No existing breakpoints!\n");
		return;
	}
}

/*----------------------------------------------------------------------------------*/
/* func_name : command_bp                   										*/
/* purpose : Bp 명령어 뒤에 온 숫자에 대한 정보를 bp linked list에 저장. 		*/
/* return  : none                           										*/
/*----------------------------------------------------------------------------------*/
void command_bp()
{
	char temp_bp_str[7] = { 0 };	 ///bp 명령어 뒤에 들어온 숫자 정보 저장하는 배열
	int temp_bp_size = 0, bp_index = 3, bp_num=0; //bp 뒤에 들어온 숫자 사이즈, temp_bp_str의 index, bp 뒤에 들어온 숫자의 정수값 저장하는 변수
	
	//bp와 빈칸 뒤 아무것도 안들어오면 에러 메시지 출력
	if(command[3]=='\0')
	{
		error_flag = 1;
		printf("Error ! : Wrong breakpoint command!\n");
		return;
	}

	while(command[bp_index] != '\0')
	{
		//너무 많은 값이 들어오면 에러 메시지 출력
		if(bp_index == 8 || command[bp_index] == ' ')
		{
			error_flag = 1;
			printf("Error ! : Wrong breakpoint command!\n");
			return;
		}
		temp_bp_str[bp_index - 3] = command[bp_index];
		bp_index++;
		temp_bp_size++;
	}

	//bp뒤에 clear가 들어온 경우
	if (!strncmp(temp_bp_str, "clear", 5))
		free_bp(1);
	else {
		//temp_bp_str 내용이 타당한 주소인가를 아래 for문 통해 확인
		for (int i = 0; i < temp_bp_size; i++)
		{
			//char가 타당하지 않으면 에러 메시지 출력 
			if (check_bp_addr(temp_bp_str[i]) == 0)
			{
				error_flag = 1;
				printf("Error ! : Wrong breakpoint command!\n");
				return;
			}
		}
	
		//16진수로 변환
		bp_num = strtol(temp_bp_str, NULL, 16);

		//bp_num이 프로그램의 메모리 주소 범위 내에 있으면, linked list에 추가
		if (bp_num >= REG[8] && bp_num <= REG[8] + REG[2])
		{
			printf("\t     [ok] create breakpoint %X\n", bp_num);
			//linked list에 저장. 
			bp* node = (bp*)malloc(sizeof(bp));
			node->breakpoint = bp_num;
			node->next = NULL;
			if (bp_head == NULL)
			{
				bp_head = node;
				bp_prev = node;
			}
			else
			{
				bp_prev->next = node;
				bp_prev = node;
			}
		}
		//만일 bp_num이 프로그램 메모리 주소안에 있지 않으면, 에러 메시지 출력
		else
		{
			error_flag = 1;
			printf("Error ! : Breakpoint Number out of Program Memory Bound!\n");
			return;
		}
	}

}
/*--------------------------------------------------------------------*/
/* func_name : free_bp        										  */
/* purpose : Bp linked list에 해당 내용 전부 free     							  */
/* return  : none                         							  */
/*--------------------------------------------------------------------*/
void free_bp(int clear_flag)
{
	//만일 bp의 linked list가 존재하면 전부 free
	if (bp_head)
	{
		bp* cur = bp_head;	//현재 노드 가리키는 포인터
		while (1)
		{
			if (cur == NULL)
				break;
			bp* previous = cur;	//previous는 free하기 위해 사용된다. 
			cur = cur->next;
			free(previous);
		}
		//clear command에서 이 function이 불린 경우, 아래 메시지 출력
		if(clear_flag)
			printf("\t     [ok] clear all breakpoints\n");
		bp_head = NULL;
	}
	//bp linked list가 없는 경우, 에러 메시지 출력
	else
	{
		//clear command에서 이 function이 불린 경우, 아래 메시지 출력
		if(clear_flag)	
			printf("\t     Error! : No existing breakpoints!\n");
		return;
	}
}
/*--------------------------------------------------------------------*/
/* func_name : check_bp_addr        								  */
/* purpose : 해당 char가 16진수 숫자가 될 수 있는지 판별,    			  */
/* return  : //if c is valid address char, return 1, else 0           */
/*--------------------------------------------------------------------*/
int check_bp_addr(char c)
{
	//if c가 타당한 주소 char이면 return 1
	if (c >= '0' && c <= '9')
		return 1;
	else if (c >= 'A' && c <= 'F')
		return 1;
	else if (c >= 'a' && c <= 'f')
		return 1;
	else
		return 0;
}
/*------------------------------------------*/
/* func_name : command_run           		*/
/* purpose : 메모리에 로드된 내용을 run 시켜준다. 					*/
/* return  : none                           */
/*------------------------------------------*/
void command_run()
{
	char opcode, xbpe_flag;		//opcode와 xbpe flag setting 정보를 저장하는 변수  
	int opcode_val = 0;	//opcode의 integer 값을 저장하는 변수
	int first_run = 0;		//run 명령어가 들어왔을 때, 첫번째로 해당 내용을 실행하는지 판별하는 변수
	/* Register Number : 
		A : 0
		X : 1
		L : 2
		B : 3
		S : 4
		T : 5
		F : 6
		PC : 8
		SW : 9	r1 > r2 : 2 , r1 == r2 : 1 , r1 < r2 : 0  
		r1 : 10
		r2 : 11
		*/
	while (1) {


		//만일 breakpoint이고, 처음 도는 것이 아니면 break
		if (first_run != 0 && find_pc_bp() == 1)
			break;
		
		//0이 breakpoint인 경우 예외 처리
		if(find_pc_bp() == 1 && REG[8]==0 && bp_zero == 0)
		{
				bp_zero =1;
				break;
		}

		//pc가 프로그램 끝이면 에러 처리. 
		if (REG[8] == program_length)
		{
			run_end_flag = 1;
			break;
		}

		//첫번째 run인지 아닌지 판별
		if (first_run == 0)
			first_run = 1;

		opcode = memory[REG[8]];
		xbpe_flag = memory[REG[8] + 1];

		//0xFC와의 AND 연산 통해 opcode 값 가져옴
		opcode_val = ((int)opcode & 0xFC);

		//0x03과의 AND 연산 통해 ni 값 가져옴
		nixbpe_flag.ni = ((int)opcode & 0x03);

		//0xF0와의 AND 연산 통해 xbpe 값 가져옴
		nixbpe_flag.xbpe = ((int)xbpe_flag & 0xF0) >> 4;

		//ADD
		if (opcode_val == 0x18)
			REG[0] = REG[0] + get_TA_3();
		//ADDR
		else if (opcode_val == 0x90)
		{
			get_TA_2();
			REG[REG[11]] += REG[REG[10]];
		}
		//AND
		else if (opcode_val == 0x40)
			REG[0] = REG[0] & get_TA_3();
		//CLEAR
		else if (opcode_val == 0xB4)
		{
			get_TA_2();
			REG[REG[10]] = 0;
		}
		//COMP
		else if (opcode_val == 0x28)
		{
			int m_m2 = get_TA_3();		//메모리 값 저장하는 변수

			//A , m...m+2 비교

			if (REG[0] > m_m2)
				REG[9] = 2;
			else if (REG[0] == m_m2)
				REG[9] = 1;
			else
				REG[9] = 0;
		}
		//COMPR
		else if (opcode_val == 0xA0)
		{
			get_TA_2();

			//r1, r2 비교
			if (REG[REG[10]] >REG[REG[11]])
				REG[9] = 2;
			else if (REG[REG[10]] == REG[REG[11]])
				REG[9] = 1;
			else
				REG[9] = 0;
		}
		//DIV
		else if (opcode_val == 0x24)
			REG[0] = REG[0] / get_TA_3();
		//DIVR
		else if (opcode_val == 0x9C)
		{
			get_TA_2();
			REG[REG[11]] /= REG[REG[10]];
		}
		// J
		else if (opcode_val == 0x3C)
		{
			get_TA_3();
			REG[8] = TA;
		}
		// JEQ
		else if (opcode_val == 0x30)
		{
			get_TA_3();
			//CC set to =
			if(REG[9] == 1)
				REG[8] = TA;
		}
		// JGT
		else if (opcode_val == 0x34)
		{
			get_TA_3();
			//CC set to >
			if (REG[9] == 2)
				REG[8] = TA;
		}
		// JLT
		else if (opcode_val == 0x38)
		{
			get_TA_3();
			//CC set to <
			if (REG[9] == 0)
				REG[8] = TA;
		}
		// JSUB
		else if (opcode_val == 0x48)
		{
			get_TA_3();
			REG[2] = REG[8];
			REG[8] = TA;
		}
		// LDA
		else if (opcode_val == 0x00)
			REG[0] = get_TA_3();
		// LDB
		else if (opcode_val == 0x68)
			REG[3] = get_TA_3();
		// LDCH
		else if (opcode_val == 0x50)
		{
			REG[0] &= 0xFFFF00;
			get_TA_3();
			REG[0] |= memory[TA];
		}
		// LDL
		else if (opcode_val == 0x08)
			REG[2] = get_TA_3();
		// LDS
		else if (opcode_val == 0x6C)
			REG[4] = get_TA_3();
		// LDT
		else if (opcode_val == 0x74)
			REG[5] = get_TA_3();
		// LDX
		else if (opcode_val == 0x04)
			REG[1] = get_TA_3();
		// MUL
		else if (opcode_val == 0x20)
			REG[0] *= get_TA_3();
		//MULR
		else if (opcode_val == 0x90)
		{
			get_TA_2();
			REG[REG[11]] *= REG[REG[10]];
		}
		// OR
		else if (opcode_val == 0x44)
			REG[0] |= get_TA_3();
		// RD
		else if (opcode_val == 0xD8)
		{
			get_TA_3();
			REG[9] = 1;
		}
		//RMO
		else if (opcode_val == 0xAC)
		{
			get_TA_2();
			REG[REG[11]] = REG[REG[10]];
		}
		//RSUB
		else if (opcode_val == 0x4C)
			REG[8] =REG[2];
		//SHIFTL
		else if (opcode_val == 0xA4)
		{
			get_TA_2();
			REG[REG[10]] = (REG[REG[10]] << REG[11]);
		}
		//SHIFTR
		else if (opcode_val == 0xA8)
		{
			get_TA_2();

			//right shift n bits, 
			for (int i = 0; i < REG[11]; i++)
				REG[REG[10]] = REG[REG[10]] >> 1;
		}
		// STA, STB, STCH, STl, STS, STSW, STT, STX
		else if (opcode_val == 0x0C || opcode_val == 0x78 || opcode_val == 0x54 || opcode_val == 0x14 || opcode_val == 0x7C || opcode_val == 0xE8 || opcode_val == 0x84 || opcode_val == 0x10)
		{
			int immediate_flag = 0;	// flag : 1 -> immediate addressing, 0 : not immediate

			//get TA by calling get_TA_3()
			get_TA_3();
			
			//n : 0, i : 1 immediate addressing
			if (nixbpe_flag.ni == 1)
				immediate_flag = 1;

			//STA
			if (opcode_val == 0x0C)
				ST_opcode(TA, 0);
			// STB
			else if (opcode_val == 0x78)
				ST_opcode(TA, 3);
			// STCH
			else if (opcode_val == 0x54)
				memory[TA] = (unsigned char)(REG[0] & 0x0000FF);
			// STL
			else if (opcode_val == 0x14)
				ST_opcode(TA, 2);
			// STS
			else if (opcode_val == 0x7C)
				ST_opcode(TA, 4);
			// STSW
			else if (opcode_val == 0xE8)
				ST_opcode(TA, 9);
			// STT
			else if (opcode_val == 0x84)
				ST_opcode(TA, 5);
			// STX
			else if (opcode_val == 0x10)
				ST_opcode(TA, 1);
			}
		// SUB
		else if (opcode_val == 0x1C)
			REG[0] -= get_TA_3();
		//SUBR
		else if (opcode_val == 0x94)
		{
			get_TA_2();
			REG[REG[11]] -= REG[REG[10]];
		}
		//TD
		else if (opcode_val == 0xE0)
		{
			get_TA_3();
			REG[9] = 0;
		}
		//TIX
		else if (opcode_val == 0x2C)
		{
			int temp_memory = get_TA_3();	//비교하기 위한 메모리 값 저장하는 변수
			REG[1] += 1;
			if (REG[1] > temp_memory)
				REG[9] = 2;
			if (REG[1] == temp_memory)
				REG[9] = 1;
			else
				REG[9] = 0;
		}
		//TIXR
		else if (opcode_val == 0xB8)
		{
			get_TA_2();
			REG[1] += 1;
			if (REG[1] > REG[REG[10]])
				REG[9] = 2;
			if (REG[1] == REG[REG[10]])
				REG[9] = 1;
			else
				REG[9] = 0;
		}
		//ADDF, COMPF, DIVF, LPS, MULF, SSK, STF, STI, SUBF, SVC, WD
		else if (opcode_val == 0x58 || opcode_val == 88 || opcode_val == 64 || opcode_val == 0xD0 || opcode_val == 0x60 || opcode_val == 0xEC || opcode_val == 0x80 || opcode_val == 0xD4 || opcode_val == 0x5C || opcode_val == 0xB0 || opcode_val == 0xDC)
			get_TA_3();
		//FIX, FLOAT, HIO, NORM, SIO, TIO
		else if (opcode_val == 0xC4 || opcode_val == 0xC0 || opcode_val == 0xF4 || opcode_val == 0xC8 || opcode_val == 0xF0 || opcode_val == 0xF8)
			REG[8] += 1;

			//initialize settings.
			format4_flag = 0;
			nixbpe_flag.ni = 0;
			nixbpe_flag.xbpe = 0;

	}
	printf("A : %.6X\tX : %.6X\n", REG[0], REG[1]);
	printf("L : %.6X     PC : %.6X\n", REG[2], REG[8]);
	printf("B : %.6X\tS : %.6X\n", REG[3], REG[4]);
	printf("T : %.6X\n", REG[5]);

	//프로그램이 끝난 경우
	if (run_end_flag)
	{
		printf("          \tEnd Program\n");
		
		//PC, L register 초기화
		REG[8]=0;
		REG[2]=0;
		run_end_flag = 0;
	}
	//checkpoint에서 멈춘 경우
	else
		printf("          \tStop at checkpoint[%X]\n", REG[8]);
}

/*------------------------------------------*/
/* func_name : find_pc_bp        		    */
/* purpose : 현재 PC의 값과 breakpoint가 같은지 찾는다. 찾으면 1 return	    */
/* return  : 1 if current pc == bp, else 0  */
/*------------------------------------------*/
int find_pc_bp()
{
	//bp linked list가 존재하면, 현재 pc랑 bp랑 같은지 찾는다. 
	if (bp_head) {
		bp* cur = bp_head;
		while (cur != NULL)
		{
			//찾은 경우 return 1
			if (cur->breakpoint == REG[8])
				return 1;
			cur = cur->next;
		}
	}
	//못 찾은 경우, return 0
	return 0;
}

/*------------------------------------------------------------------*/
/* func_name : get_TA_3        										*/
/* purpose :  Format 3 또는 4일 때, 해당 nixbpe 값에 따라 Target Address를 계산한 후, */
/*메모리의 Target Address에 저장되어 있는 값 return. Format이 3이면 PC에 3, 4면 PC에 4를 더하여 준다. 	*/
/* return  : memory value which index is TA							*/
/*------------------------------------------------------------------*/
int get_TA_3()
{
	int disp = 0; //displacement 

	//n : 0, i: 0, SIC
	if (nixbpe_flag.ni == 0)
	{
		//SIC -> bpe disp/address 전부 주소이다. 
		disp = ((int)memory[REG[8] + 1] & 0x7F) * 256 + (int)memory[REG[8] + 2];

		TA = disp;

		//X flag가 set된 경우
		if ((nixbpe_flag.xbpe & 0x08) != 0)
			TA += REG[1];

		return memory[TA] * 256 * 256 + memory[TA + 1] * 256 + memory[TA + 2];

		//+3 to the PC
		REG[8] += 3;

	}
	//n is not 0, i is not 0, 따라서 format 4 가능
	else {
		//format 4
		if ( (nixbpe_flag.xbpe & 0x01) == 1)
			format4_flag = 1;

		//format 3 or 4에 의해 memory address를 얻는다. 
		if (format4_flag)
		{
			disp = ((int)memory[REG[8] + 1] % 16) * 256 * 256 + (int)memory[REG[8] + 2] * 256 + (int)memory[REG[8] + 3];

			//만일 음수인 경우, 2's complement 통해 양수로 변경. 
			if ((disp & 0x80000) != 0)
			{
				disp = ~disp;
				disp += 1;
				disp = disp & 0x7FFFF;
				disp *= -1;
			}

			//+4 to the PC
			REG[8] += 4;
		}
		else
		{
			disp = ((int)memory[REG[8] + 1] % 16) * 256 + (int)memory[REG[8] + 2];

			//만일 음수인 경우, 2's complement 통해 양수로 변경. 
			if ((disp & 0x800) != 0)
			{
				disp = ~disp;
				disp += 1;
				disp = disp & 0x7FF;
				disp *= -1;
			}

			//+3 to the PC
			REG[8] += 3;
		}
		TA = disp;
		//X flag is set
		if ((nixbpe_flag.xbpe & 0x08) != 0)
			TA += REG[1];


		//Base Relative
		if ((nixbpe_flag.xbpe & 0x04) != 0)
			TA += REG[3];

		//PC relative
		if ((nixbpe_flag.xbpe & 0x02) != 0)
			TA += REG[8];


		//n : 1, i: 0, indirect addressing
		if (nixbpe_flag.ni == 2)
		{
			TA = memory[TA] * 256 * 256 + memory[TA + 1] * 256 + memory[TA + 2];
			return  memory[TA] * 256 * 256 + memory[TA + 1] * 256 + memory[TA + 2];
		}
		//n : 1, i : 1, simple addressing
		else if (nixbpe_flag.ni == 3)
			return memory[TA] * 256 * 256 + memory[TA + 1] * 256 + memory[TA + 2];
		//n : 0, i : 1 immediate addressing
		else if (nixbpe_flag.ni == 1)
			return TA;
	}
}


/*---------------------------------------------------------------*/
/* func_name : get_TA_2        									 */
/* purpose :  Format 2인 경우, opcode 뒤에 들어온 r1, r2 정보를 REG 배열 r1, r2에 저장한다. 그리고 PC에는 2를 더해준다.  */
/* return  : none                        						 */
/*---------------------------------------------------------------*/
void get_TA_2()
{
	//format  2 : opcode 8, r1 :4, r2 : 4
	
	//register number를 r1, r2에 저장. 
	REG[10] = ((int)memory[REG[8] + 1] & 0xf0) >> 4;
	REG[11] = ((int)memory[REG[8] + 1] & 0x0f);


	//Add 2 to PC
	REG[8] += 2;
}

/*------------------------------------------*/
/* func_name : ST_opcode        			*/
/* purpose :  Opcode가 ST*인 경우, 메모리에 해당 레지스터 값을 저장시켜준다.  			*/
/* return  : none                        	*/
/*------------------------------------------*/
void ST_opcode(int ta, int register_num)
{
	memory[ta] = (unsigned char)((REG[register_num] & 0xFF0000) >> 16);
	memory[ta + 1] = (unsigned char)((REG[register_num] & 0x00FF00) >> 8);
	memory[ta + 2] = (unsigned char)(REG[register_num] & 0x0000FF);
}
