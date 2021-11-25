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
}SYM;	// SYM : STMTAB 저장 위한 구조체
SYM* first;	//SYMTAB의 처음 가리킴
SYM* before;	//SYMTAB의 이전 노드 가리킴

typedef struct _Modi {
	int start_addr;
	struct _Modi* next;
}Modi;				//Modi : Modifcation 저장 위한 구조체
Modi* modi_head;	//modification linked list의 처음 가리킴
Modi* modi_prev;	//modification의 이전 노드 가리킴

char command[100];	//variable which get input command
int line_num;		//variable which get history's line index
int quit_flag;		//distinct if input gets 'quit', if 1 program end
int error_flag;			//distinct if error occurs
int memory_address;		//variable which stores last printed memory address+16
unsigned char memory[1048576];	//arr which stores memory 
int loc;	//현재 줄의 loc 값을 저장하는 변수이다.
int start_address; //.asm 파일의 시작 주소를 저장하는 변수이다. 	
int program_length; //.asm 파일의 총 길이를 저장하는 변수이다. 
int pc;	// pc의 값을 저장하는 변수이다. 
int symbol_flag;	// symbol이 들어왔는지 판별하는 flag이다. Symbol이 들어오면 1이 되고, 아니면 0이다. 
char file_name[31];	//파일의 이름을 저장하는 배열이다. 
unsigned char line_opcode[31];		//입력 받은 줄의 opcode를 저장하는 배열이다. 
unsigned char line_symbol[31];		//입력 받은 줄의 symbol을 저장하는 배열이다
unsigned char line_operand[31];	//입력 받은 줄의 operand를 저장하는 배열이다. 
int opcode_val;	//opcode의 value 값을 저장하는 변수이다.
int text_flag;		//text record 작성 시 필요한 flag이다. 같은 라인에 작성하면 1을 갖고 있다.
char text_record[61];	//Text record에 대한 정보를 갖고 있는 배열이다.
int text_index;		//text record의 index이다. 
char result_obj_code[31];	//해당 줄의 object code에 대한 정보를 갖는 배열이다
char BASE_str[31];	//BASE 지시어에 들어온 label에 대한 정보를 갖는 배열이다.
int obj_loc;		//Text record 작성 시 제일 처음에 들어가는 해당 줄의 시작 주소를 갖고 있는 변수이다. 
int asm_start_flag;	//.asm 파일에 start가 존재하는지 판별하기 위한 flag이다. 존재하면 1이 된다.
int lst_line_num = 5;	//해당 줄의 line 번호를 갖고 있는 변수이다. 
int pass_1_flag;		//pass1이 성공적으로 실행되었는지 알려주는 flag이다. 성공적으로 실행되면 1이 된다. 

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
	//만일 type이 들어온 경우,  call command_type()
	else if (!strncmp(arr, "type", 4))
	{
		//If got blank after type
		if (arr[4] == ' ')
		{
			error_flag--;
			command_type();
		}
	}
	//만일 assemble이 들어온 경우, call command_assemble()
	else if (!strncmp(arr, "assemble", 8))
	{
		//이전에 assemble한 파일들의 SYMTAB와 modificaiton linked list를 free해야 하므로 아래 함수 call
		free_symtab();

		//opcode, operand, symbol and symbol_flag등을 저장하는 배열 및 flag들 전부 초기화
		memset(line_opcode, 0, sizeof(line_opcode));
		memset(line_operand, 0, sizeof(line_operand));
		memset(line_symbol, 0, sizeof(line_symbol));
		memset(file_name, 0,sizeof(file_name));
		memset(BASE_str, 0, sizeof(BASE_str));
		symbol_flag = 0;
		asm_start_flag = 0;
		first = NULL;
		before = NULL;
		modi_head = NULL;
		modi_prev = NULL;
		pass_1_flag=0;

		//assemble 뒤에 빈칸 받은 경우.
		if (arr[8] == ' ')
		{
			error_flag--;
			command_assemble();
			//error_flag가 0이면 성공적으로 assemble 되었으므로 아래 내용 출력.
			if (error_flag == 0)
				printf("Successfully assemble %s.\n", file_name);
			else
			{		//pass1 실패시
					if(pass_1_flag==0)
					{
							//intermediate 파일 삭제
							remove("intermediate");
					}
			}
		}
	}
	//만일 symbol이 들어온 경우, call command_symbol()
	else if (!strncmp(arr, "symbol", 6))
	{
		error_flag--;
		command_symbol();
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
/* purpose : 파일을 읽어서    */
/*   	     파일 내용 출력*/
/* return  : none                      */
/*-------------------------------------*/
void command_type()
{
	//get_filename 함수로 파일 이름 얻는다. 
	get_filename(file_name, 5);
	
	char temp;	//파일의 한 단어를 저장하는 변수
	FILE *fp = fopen(file_name, "r");
	if(fp == NULL)	//존재하지 않으면 오류 메시지 출력
			printf("Error : No such file in the directory!\n");
	else{		
				//내용 출력
				while(fscanf(fp, "%c", &temp)!=EOF)
					printf("%c", temp);
		}
}

/*------------------------------------------*/
/* func_name : get_filename                 */
/* purpose : 파일 이름을 얻어서   */
/*   	     file_name 배열에 저장*/
/* return  : none                           */
/*------------------------------------------*/
void get_filename(char* arr, int i)
{
	int j = 0;		//file_name의 index

	//아래 while문으로 file_name 얻는다. 
	while (1)
	{
		if (command[i] == '\0')
			break;
		file_name[j++] = command[i++];
	}
}

/*------------------------------------------------------------------*/
/* func_name : divide_one_line             							*/
/* purpose : 읽은 한 라인을 나누어 opcode, operand, symbol 정보를 얻는다.    					*/
/* return  : 0 : opcode=start, 1 : opcode=end, -1 : comment line    */
/*------------------------------------------------------------------*/
int divide_one_line(char* arr, int pass_num)
{
	int i = 0, j = 0, k = 0, l = 0, cnt_blank = 0, comma_flag = 0;	//comma_flag : 1 -> ,가 있으므로, 빈칸이 있어도 다음 값 받아야함
	
	//만일 pass2에서 불린 경우
	if (pass_num == 2)
	{
		char line_loc[4] = { 0 };

		//라인 넘버와 빈칸은 skip
		while (arr[i++] != '\t');

		//이 라인의 loc 값 copy
		while (arr[i] != '\t')
			line_loc[j++] = arr[i++];

		//loc 값 hexa 로 바꾸어 저장
		loc = strtol(line_loc, NULL, 16);

		//loc 이후의 blank skip
		i++;
		
		j = 0;
	}

	//라인이 . 으로 시작하면 주석이다. 
	if (arr[i] == '.')
		return -1;

	//빈칸으로 시작하면 opcode와 operand만 존재
	else if (arr[i] == ' ' || arr[i]=='\t')
		symbol_flag = 0;

	//그게 아니면 ,symbol이다. line_symbol 에 저장. 
	else
	{
		symbol_flag = 1;
		line_symbol[j++] = arr[i++];
	}
	int opcode_cnt = 0;	//opcode인지 operand인지 판별 위한 변수.

	//라인을 빈칸에 의해 나누어 opcode와 symbol에 관한 정보 얻는다. 
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

		//if arr[i]가 symbol
		if (symbol_flag == 1 && cnt_blank == 0)
			line_symbol[j++] = arr[i];

		//if arr[i]가 opcode
		else if ((symbol_flag==1 &&cnt_blank == 1) || (symbol_flag == 0 && cnt_blank == 0))
		{
			line_opcode[k++] = arr[i];
			opcode_cnt++;
		}
		
		//if arr[i]가 operand
		else if ((symbol_flag == 1 && cnt_blank == 2) || (symbol_flag == 0 && cnt_blank == 1))
		{
			line_operand[l++] = arr[i];

			//만일 comma 있으면, 다른 operand 받아야함.
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
	
	//opcode == 'START'
	if (!asm_start_flag) {
		//START 있는 경우
		if (!strcmp(line_opcode, "START"))
		{
			asm_start_flag = 1;
			return 0;
		}
		//START 없으면 오류 메시지 출력
		else
		{
			printf("Assemble Error in line %d : No START opcode\n", lst_line_num);
			error_flag = 1;
			return -4;
		}
	}
	//opcode가 END인 경우
	else if (!strcmp(line_opcode, "END"))
		return 1;
	else return 2;
}


/*-----------------------------------------------*/
/* func_name : command_assemble             	 */
/* purpose : 파일을 pass1, pass2를 통해 assemble하고 */
/*   	     .obj, .lst file 만듦 	     	   	 */
/* return  : none                      			 */
/*-----------------------------------------------*/
void command_assemble()
{
	char obj_name[31] = { 0 };
	char lst_name[31] = { 0 };
	get_filename(file_name, 9);

	int file_index = 0, x=0;	//file_name, obj_name, lst_name의 index

	//.asm 없이 obj_name and lst_name에 file name copy
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
	char one_line[100] = { 0 };	//.asm 파일의 한 라인 정보 받는 배열
	
	//파일 없는 경우, 에러 메시지
	if(fp == NULL)
	{
			printf("File Open error : No such file in the directory!\n");
			error_flag=1;
			return;
	}

	//첫번째 줄 입력
	fgets(one_line, 100, fp);

	//If OPCODE = 'START'
	if (divide_one_line(one_line,1) == 0)
	{
		//#[OPERAND] 를 staring address로 저장
		start_address = strtol(line_operand, NULL, 16);
		//loc를 시작 주소로 초기화
		loc = start_address;
	}
	//Else, LOCCTR을 0으로 초기화
	else
	{
		loc = 0;
		error_flag = 1;
		return;
	}
	//intermediate file에 해당 line 작성
	fprintf(inter, "%d\t%.4X\t%s", lst_line_num, loc, one_line);
	lst_line_num += 5;		// 5를 line number에 더함

	while (1)
	{
		//line, opcode, operand, symbol and symbol_flag에 관한 정보 갖는 array 초기화
		memset(one_line, 0, sizeof(one_line));
		memset(line_opcode, 0, sizeof(line_opcode));
		memset(line_operand, 0, sizeof(line_operand));
		memset(line_symbol, 0, sizeof(line_symbol));
		symbol_flag = 0;

		//한 줄 입력
		if (fgets(one_line, 100, fp) == NULL)
		{
			//NULL이면 end 메시지 없는 것이므로 오류 메시지 출력. 
			printf("Assemble Error on line %d : No END opcode in the .asm file!\n", lst_line_num);
			error_flag = 1;
			return;
		}

		//symbol, opcode, operand에 대한 정보 divide_one_line function 함수 call 하여 얻는다. 
		int end_flag = divide_one_line(one_line,1);

		//OPCODE = END
		if (end_flag == 1)
		{
			//마지막 line을 intermediate file에 쓴다. 
			fprintf(inter, "%d\t    \t%s", lst_line_num, one_line);
			break;
		}
		//COMMENT STATEMENTS
		else if (end_flag == -1)
		{
			//현재 라인을 intermediate file에 쓴다.
			fprintf(inter, "%d\t    \t%s", lst_line_num, one_line);
			lst_line_num += 5;		//line number에 5 더함. 
			continue;
		}
		else if (end_flag == 2)
		{
			//LABEL field에 symbol 있는 경우
			if (symbol_flag)
			{
				//SYMTAB에서 label 찾음
				SYM* sym_cur = first;
				if (first)
				{
					while (sym_cur != NULL) {
						if (!strcmp(sym_cur->label, line_symbol))
						{
							//만일 찾은 경우, 중복 되므로 에러 메시지 출력
							printf("Assemble Error on line %d : same label has many LOC\n", lst_line_num);
							error_flag = 1;
							return;
						}
						sym_cur = sym_cur->next;
					}
				}

			//LABEL,LOCCTR)을 SYMTAB에 insert

				int insert_flag = 0;		//SYMTAB에 insert하면 1로 바뀜
				
				//insert하기 위해 새로운 node 생성
				SYM* node = (SYM*)malloc(sizeof(SYM));
				strcpy(node->label, line_symbol);
				node->locctr = loc;
				node->next = NULL;

				//처음 head가 없는 경우, 처음에다가 노드 insert
				if (first == NULL)
				{
					first = node;
					insert_flag = 1;
				}
					
				SYM* cur = first;	//SYMTAB에서 현재 노드 가리킴
				before = first;
				
				int cnt = 0;	//SYMTAB에서 얼마나 많은 노드를 지나갔는지 알기 위한 변수
				
				//SYMTAB에 insert하기 전까지 계속 반복
				while (!insert_flag) {

					//만일 새로운 label이 현재 노드 label 보다 작은 경우, insert(오름차순)
					int ret = strcmp(node->label, cur->label);
					if (ret < 0)
					{
						//SYMTAB의 처음에 대입
						if (before == first && cnt==0){
							node->next = cur;
							first = node;
						}
						//처음 아닌 곳에 대입
						else{
							before->next = node;
							node->next = cur;
						}
						insert_flag = 1;
						break;
					}

					//큰 경우, 오름차순이 안되므로 다음 노드로 이동
					
					//cur와 before가 다른 경우, before 이동
					if (cur != before)
						before = before->next;
						
					//cur를 다음 노드로 이동
					cur = cur->next;
					cnt++;

					//마지막에 삽입
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
			//숫자가 들어온게 아니면 오류 메시지 출력. 
			if ((int)line_operand[0] >= 58)
			{
				printf("Assemble Error in line %d : Only Decimal number is possible for WORD constant\n", lst_line_num);
				error_flag = 1;
				return;
			}

			//intermediate file에 작성
			fprintf(inter, "%d\t%.4X\t%s", lst_line_num, loc, one_line);
			lst_line_num += 5;		//line number에 5 더함
			
			loc += 3;			//LOCCTR에 3 더함.
		}
		else if (!strcmp(line_opcode, "RESW"))
		{
			//intermediate file에 작성
			fprintf(inter, "%d\t%.4X\t%s", lst_line_num, loc, one_line);
			lst_line_num += 5;		//line number에 5 더함

			loc += 3 * atoi(line_operand);	//loc에 operand 십진수 값 * 3 더함.
		}
		else if (!strcmp(line_opcode, "RESB"))
		{
			//intermediate file에 작성
			fprintf(inter, "%d\t%.4X\t%s", lst_line_num, loc, one_line);
			lst_line_num += 5;		//line number에 5 더함

			loc += atoi(line_operand);	//loc에 operand 십진수 값 더함
		}
		else if (!strcmp(line_opcode, "BYTE"))
		{
			//상수의 길이 찾음
			int constant_length = 0;
			//만일 operand가 C인 경우
			if (line_operand[0] == 'C')
			{
				int i = 2;		//line operand의 index
				//아래 while문으로 길이 찾음
				while (line_operand[i++] != '\'')
					constant_length++;
					
				//intermediate file에 작성
				fprintf(inter, "%d\t%.4X\t%s", lst_line_num, loc, one_line);
				lst_line_num += 5;		//line number에 5 더함

				//char는 1 byte, 따라서 loc + 1*constant_legnth
				loc += constant_length;
			}
			//operand가 X인 경우
			else if (line_operand[0] == 'X')
			{
				int i = 2;		//line operand의 index
				//아래 while문으로 길이 찾음
				while (line_operand[i++] != '\'')
					constant_length++;
				
				//intermediate file에 작성
				fprintf(inter, "%d\t%.4X\t%s", lst_line_num, loc, one_line);
				lst_line_num += 5;		//line number에 5 더함
			
				//constant는 하프바이트 단위로 쓰인다. 따라서 짝수가 아니면 1을 추가해줘야한다. 
				if (constant_length % 2 == 1)
					constant_length += 1;
				loc += ((constant_length) / 2);

			}
		}
		else if (!strcmp(line_opcode, "BASE"))
		{
			//BASE의 operand BASE_str에 copy
			strcpy(BASE_str, line_operand);

			//intermediate file에 작성
			fprintf(inter, "%d\t    \t%s", lst_line_num, one_line);
			lst_line_num += 5;		//line number에 5 더함
			continue;
		}
		else if (line_opcode[0] == '+') {
						
			//temp_opcode opcde table에서 찾음, 0이 되면 해당 opcode 없음
			if (find_opcode(line_opcode, 1) == 0)
			{
				printf("Assembly File Error :  No %s on the opcodelist\n", line_opcode);
				error_flag = 1;
				return;
			}

			//intermediate file에 작성
			fprintf(inter, "%d\t%.4X\t%s", lst_line_num, loc, one_line);
			lst_line_num += 5;		//line number에 5 더함

			//상수 immediate addressing이 아니면 format 4 modification 정보 저장.
			if (!(line_operand[0] == '#' && (line_operand[1] >= '0' && line_operand[1] <= '9'))) {
				//modification 위한 dynamic memory 할당
				Modi* modi_node = (Modi*)malloc(sizeof(Modi));
				modi_node->start_addr = loc+1;
				modi_node->next = NULL;

				//modification 위한 linked list 생성
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

			//opcode에서 찾았으면, loc에 4더함. 
			loc += 4;
		}
		//format 1, 2, 3 instruction
		else{
			//opcode opcode table에서 찾음
			int opcode_found_flag = find_opcode(line_opcode, 0);

			//없는 경우, 오류 메시지 출력
			if (opcode_found_flag == 0)
			{
				printf("Assembly Error in line %d :  No %s on the opcodelist\n",lst_line_num, line_opcode);
				error_flag = 1;
				return;
			}

			//찾은 경우. 
			
			//intermediate file에 작성
			fprintf(inter, "%d\t%.4X\t%s", lst_line_num, loc, one_line);
			lst_line_num += 5;		//line number에 5 더함
			
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

	//program의 총 길이 program_length에 저장
	program_length = loc - start_address;

	
	//에러가 없으면 pass2 실행
	if(!error_flag){
	pass_1_flag=1;
	FILE* interm = fopen("intermediate", "r");
	FILE* obj_fp = fopen(obj_name, "w");
	FILE* lst_fp = fopen(lst_name, "w");
	 
	//line, opcode, operand, symbol and symbol_flag에 관한 값 초기화
	memset(one_line, '\0', sizeof(one_line));
	memset(line_opcode, '\0', sizeof(line_opcode));
	memset(line_operand, '\0', sizeof(line_operand));
	memset(line_symbol, '\0', sizeof(line_symbol));
	symbol_flag = 0;
	int variable_flag = 0;	//만일 변수면, 해당 object line 적기 위해 쓰이는 flag 
	int printed_flag = 0;	//만일 아직 안쓰인 object line이 있는데 end가 들어온 경우, object line print하기 위해 쓰이는 flag
	asm_start_flag = 0;

	//첫번째 라인 입력
	fgets(one_line, 100, interm);

	//If OPCODE = 'START'
	if (divide_one_line(one_line,2) == 0)
	{
		//listing line 작성
		fprintf(lst_fp, "%s", one_line);

		//.obj header record 작성
		fprintf(obj_fp, "H%-6s%.6X%.6X\n", line_symbol, start_address, program_length);
	}
	lst_line_num = 5;
	//opcode가 end일 때까지 계속 반복
	while (!error_flag)
	{
		//line, opcode, operand, symbol, result_obj_code에 관한 값 초기화
		memset(one_line, 0, sizeof(one_line));
		memset(line_opcode, 0, sizeof(line_opcode));
		memset(line_operand, 0, sizeof(line_operand));
		memset(line_symbol, 0, sizeof(line_symbol));
		memset(result_obj_code, 0, sizeof(result_obj_code));
		symbol_flag = 0;
		lst_line_num += 5;

		//다음 줄 입력
		fgets(one_line, 100, fp);

		//ymbol, opcode, operand에 관한 정보 divide_one_line function 통해 얻음
		int end_flag = divide_one_line(one_line, 2);

		//OPCODE = END
		if (end_flag == 1)
		{
			//END line은 object code 생성 안함. 

			//아직 안 쓰인 text record가 있으면 해당 내용 적음
			if (printed_flag != 0) {
				fprintf(obj_fp, "T%.6X", obj_loc);
				text_record[0] = hex_char_return(((text_index - 2)/2) / 16);
				text_record[1] = hex_char_return(((text_index - 2)/2) % 16);
				fprintf(obj_fp, "%s\n", text_record);
			}			

			//Modification Record 작성
			Modi* modi_cur = modi_head;
			while(modi_cur != NULL)
			{
				fprintf(obj_fp, "M%.6X05\n", modi_cur->start_addr);
				modi_cur = modi_cur->next;
			}

			//해당 줄 .lst 파일에 작성
			fprintf(lst_fp, "%s", one_line);

			fprintf(obj_fp, "E%.6X\n", start_address);
			break;
		}
		//COMMENT STATEMENTS
		else if (end_flag == -1)
		{
			//주석은 object code 만들지 않음

			//해당 줄 .lst 파일에 작성
			fprintf(lst_fp, "%s", one_line);
		}
		else {
			//if opcode is BASE, object code 만들지 않음
			if (!(strcmp(line_opcode, "BASE")))
			{
				//BASE 뒤에 나오는 symbol이 SYMTAB에 없으면 오류 메시지 출력
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
			//if opcode is RESW, object code 만들지 않음
			else if (!(strcmp(line_opcode, "RESW")))
			{
				fprintf(lst_fp, "%s", one_line);

				if (!variable_flag) {
				//변수이면, 새로운 text record line 만듦. text_record는 .obj 파일에 적음
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
			//if opcode is RESB, object code 만들지 않음
			else if (!(strcmp(line_opcode, "RESB")))
			{
				fprintf(lst_fp, "%s", one_line);

				if (!variable_flag) {
					///변수이면, 새로운 text record line 만듦. text_record는 .obj 파일에 적음
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

			//obj 코드 계산, 아래 distinc_format 함수 통해서. 
			distinct_format(line_opcode, line_operand);

			//one line에 \n 제거 
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

			//.lst 파일에 현재 라인 적음
			fprintf(lst_fp, "%-35s\t%-35s\n", one_line, result_obj_code);
			printed_flag++;

			//변수가 나왔다면 이전까지 만든 text record 프린트 되었을 테니까 다시 0으로 만듦
			if (variable_flag)
				variable_flag = 0;
	
			int obj_index = 0, cnt=0;		//result_obj_code의 index, object code의 길이
			while(result_obj_code[obj_index++] != '\0')	//object code 길이 계산.
				cnt++;

			//만일 이 object code 적었을 때 max 넘어가면 이 object code 이전까지의 text record .obj에 작성
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

			//새로운 text record line 만듦
			if (!text_flag)
			{
				//text_record and text_index 초기화
				memset(text_record, 0, sizeof(text_record));
				text_index = 2;
				text_flag = 1;
				obj_loc = loc;
			}

			obj_index = 0;

			//object code를 text_record array에 copy
			while (result_obj_code[obj_index] != '\0')
				text_record[text_index++] = result_obj_code[obj_index++];

		}
	}
	//Close File Pointer
	fclose(interm);
	fclose(obj_fp);
	fclose(lst_fp);
	}
	//intermediate file 제거
	remove("intermediate");
	if(error_flag)
	{
		//pass2에서 error가 났으므로 .obj, .lst 파일 제거
			remove(obj_name);
			remove(lst_name);
	}
}

/*----------------------------------------------------------------------------------*/
/* func_name : find_opcode             												*/
/* purpose : 들어온 opcode를 opcode table에서 찾음    											*/
/* return  : 못 찾은 경우 0, 찾은 경우 format type 반환   */
/*----------------------------------------------------------------------------------*/
int  find_opcode(char *arr, int i) {

	char temp_mnemonic[7] = { 0 };	//들어온 mnemonic을 저장하는 string
	int mnemonic_num = 1;	//hash table의 index를 판별하기 위한 변수
	int mnemonic_format = 0;		// mnemonic의 format을 저장하는 변수
	int j = 0, mnemonic_length = 0;	//temp_mnemonic의 index, mnemonic 길이

	//들어온 mnemominc을 temp_mnemonic array에 copy
	while (arr[i] != '\0')
	{
		//잘못된 mnemonic이 들어온 경우
		if (j == 6)
			return 0;
		temp_mnemonic[j++] = arr[i];
		mnemonic_num += (int)arr[i++];
		mnemonic_length++;
	}

	//menmonic의 hash index 계산
	mnemonic_num += mnemonic_length * 19;
	mnemonic_num %= 20;

	Hash* cur = table[mnemonic_num];	//cur은 현재 hash table node 가리킴
	while (1)
	{
		//못 찾은 경우, 에러 메시지 출력
		if (cur == NULL)
		{
			return 0;
			break;
		}

		//찾은 경우
		if (!(strcmp(cur->mnemonic, temp_mnemonic)))
		{
			//format number와 value 값 저장
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
/* purpose : SYMTAB의 모든 label과 loc 오름차순 출력  */
/* return  : none                              */
/*---------------------------------------------*/
void command_symbol() {
	SYM* cur = first;
	//SYMTAB이 있는 경우, 출력
	if (first) {
		for (; cur != NULL; cur = cur->next)
			printf("\t%s\t%.4X\n", cur->label, cur->locctr);
	}
	else
	{
		//SYMTAB이 없는 경우
		error_flag = 1;
		printf("Error : No SYMTAB!\n");
	}
}

/*--------------------------------------------------------------------*/
/* func_name : free_symtab        									  */
/* purpose : SYMTAB과 modification linked list 전부 free      */
/* return  : none                         							  */
/*--------------------------------------------------------------------*/
void free_symtab()
{
	//SYMTAB이 존재하면 전부 free
	if (first)
	{
		SYM* cur = first;	//cur은 현재 노드 가리킴
		while (1)
		{
			if (cur == NULL)
				break;
			SYM* previous = cur;	//previous은 이전 노드 가리킴
			cur = cur->next;
			free(previous);
		}
	}

	//Modification의 linked list가 존재하면 전부 free
	if (modi_head)
	{
		Modi* modi_cur = modi_head;	//cur은 현재 노드 가리킴
		while (1)
		{
			if (modi_cur == NULL)
				break;
			modi_prev = modi_cur;	//prev은 이전 노드 가리킴
			modi_cur = modi_cur->next;
			free(modi_prev);
		}
	}
}


/*----------------------------------------*/
/* func_name : distinct_format        	  */
/* purpose : opcode와 operand를 받아서      */
/*			object code를 만듦		  */
/* return  : object code character array  */
/*----------------------------------------*/
void distinct_format(char* opcode, char*operand)
{
	char result_obj_before_bind[130] = { 0 };	//4개씩 묶기 전의 10101으로 된 object 코드
	int opcode_format = 0;	//opcode의 format number 저장
	int x_flag = 0;	//x register 사용 유무
	int b_flag = 0;	//BASE relative인지 아닌지
	int p_flag = 0;	//PC relative인지 아닌지
	int e_flag = 0; //Extension인지 아닌지
	
	//Format4
	if (opcode[0] == '+')
	{
		int addr = 0; 	//format4에서의 주소
		find_opcode(opcode, 1);

		//opcode value를 result_obj_before_bind array에 저장
		num_to_hexchar(result_obj_before_bind, opcode_val);

		//format 4, extension, 주소의 절댓값 사용
		e_flag = 1;
		
		//Indirect addresssing
		if (operand[0] == '@')
		{
			char pure_operand[31] = { 0 };	//만일 #이나 @가 붙은 경우, 이걸 제외한 순수한 operand 값 저장 배열

			//indirect addressing의 n, i value 저장
			result_obj_before_bind[6] = '1'; result_obj_before_bind[7] = '0';

			//x,b,p,e value 저장
			xbpe_setting(result_obj_before_bind, x_flag, b_flag, p_flag, e_flag);

			int operand_index = 1, pure_index = 0;
			//@를 제외하고 pure_operand에 operand 내용 저장
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
			char pure_operand[31] = { 0 };	//만일 #이나 @가 붙은 경우, 이걸 제외한 순수한 operand 값 저장 배열

			//immediate addressing의 n, i value 저장
			result_obj_before_bind[6] = '0'; result_obj_before_bind[7] = '1';

			int s_flag = 0;
			//symbol이 들어온 경우 s_flag = 1
			if ((int)operand[1] >= 65)
				s_flag = 1;
		

			int operand_index = 1, pure_index = 0;
			//#를 제외하고 pure_operand에 operand 내용 저장
			while (operand[operand_index] != '\0')
				pure_operand[pure_index++] = operand[operand_index++];

			//x,b,p,e value 저장
			xbpe_setting(result_obj_before_bind, x_flag, b_flag, p_flag, e_flag);

			//operand의 disp 저장
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
			//n, i 값 저장
			result_obj_before_bind[6] = '1'; result_obj_before_bind[7] = '1';

			//x,b,p,e value 저장
			xbpe_setting(result_obj_before_bind, x_flag, b_flag, p_flag, e_flag);

			//SYMTAB에서 symbol 찾음
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

		//binary number를 hexa number로 바꿈
		change_hex(result_obj_before_bind, result_obj_code, 3);
		
		//disp number 저장
		disp_addr_set(result_obj_code, addr,4);
	}
	//Byte 상수가 들어온 경우
	else if (!strcmp(opcode, "BYTE"))
	{
		int i = 2;		//operand의 index
		int j = 0;		//result_object_code의 index

		//C 상수인 경우
		if (operand[0] == 'C')
		{
			//아래 while문 통해서 16진수로 result_obj_code에 저장
			while (operand[i] != '\'')
			{
				result_obj_code[j++] = hex_char_return(((int)operand[i] / 16));
				result_obj_code[j++] = hex_char_return(((int)operand[i++] % 16));
			}
		}
		//16진수 상수인 경우
		else if (operand[0] == 'X')
		{
			int operand_length = 0;
			
			//operand 길이 계산
			while(operand[i++] != '\'')
				operand_length++;

			//홀수면 앞에 0붙임, half byte 단위이므로 짝수여야함. 
			if (operand_length % 2 == 1)
				result_obj_code[j++] = '0';

			i = 2;
			//16진수 값을 result_obj_code에 저장. 
			while (operand[i] != '\'')
				result_obj_code[j++] = operand[i++];
		}
	}
	//WORD 상수가 들어온 경우 
	else if (!strcmp(opcode, "WORD"))
	{
		//num에 operand 10진수 값 저장.
		int num = atoi(operand);

		int result_index = 0;	//result_obj_code의 index

		//num을 6 byte hex character array로 바꿈
		int hexx = 65536;	//6byte 16진수로 바꾸기 위해 필요한 변수
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
		//opcode의 format과 value 값을 받는다. 
		opcode_format = find_opcode(opcode, 0);

		//opcode의 value 값을 result_obj_before_bind array에 저장. 
		num_to_hexchar(result_obj_before_bind, opcode_val);
		//Format 1
		if (opcode_format == 1)
		{
			//opcode만 존재 1 byte
			if (operand[0] != '\0')
			{
				printf("Assemble Error in the line %d : Format 1 cannot have operand!\n", lst_line_num);
				error_flag = 1;
				return;
			}
			//pc 는 next instruction 가리킴
			pc = loc + 1;

			//binary number를 hexa number로 바꿈
			change_hex(result_obj_before_bind, result_obj_code, 1);
		}
		//Format2
		else if (opcode_format == 2)
		{
			//pc 는 next instruction 가리킴
			pc = loc + 2;

			//Register instruction
			int comma_flag = 0;		//1: 2개의 register가 operand에 있음을 의미

			int operand_index = 0;	//operand의 index
			char register_arr[3] = { 0 };	//register 타입 저장하는 arra
			int register_index = 0;	//register의 index 저장하는 변수
			while (operand[operand_index] != '\0')
			{
				if (operand[operand_index] == ',')
				{
					//register number를 result_obj_before_bind arr에 저장
					register_num(result_obj_before_bind, register_arr);
					register_index = 0;
					operand_index++;
					comma_flag++;
				}
				register_arr[register_index++] = operand[operand_index++];
			}
			
			//register number를 result_obj_before_bind arr에 저장
			if (comma_flag)
				register_num(result_obj_before_bind, register_arr);
			else
			{
				//하나의 register만 존재하는 경우
				register_num(result_obj_before_bind, register_arr);

				//register 2에는 전부 0을 넣음
				strcat(result_obj_before_bind, "0000");
			}

			//binary number 에서 hexa number로 바꿈
			change_hex(result_obj_before_bind, result_obj_code,2);
		}
		//Format3
		else
		{
			//pc는 다음 instruction 가리킴
			pc = loc + 3;
			p_flag = 1;
			int disp = 0;
			//operand가 없는 경우
			if(operand[0]=='\0')
			{
				//n, i value 저장
				result_obj_before_bind[6] = '1'; result_obj_before_bind[7] = '1';
				
				//pc relative가 아님
				p_flag = 0;

				//x,b,p,e value 저장
				xbpe_setting(result_obj_before_bind, x_flag, b_flag, p_flag, e_flag);

				//binary number를 hexa number로 바꾸어줌
				change_hex(result_obj_before_bind, result_obj_code, 3);

				//"000"을 result_obj_code 마지막에 붙여줌
				strcat(result_obj_code, "000");

				return;
			}
			//Indirect addresssing
			else if(operand[0]=='@')
			{
				char pure_operand[31] = { 0 };	//@ 제외한 operand를 저장하는 배열

				//n, i value 값 저장
				result_obj_before_bind[6] = '1'; result_obj_before_bind[7] = '0';

				//indirect addressing, pc relative
				p_flag = 1;
				
				//x,b,p,e value 저장
				xbpe_setting(result_obj_before_bind, x_flag, b_flag, p_flag, e_flag);

				int operand_index = 1, pure_index = 0;
				//operand를 pure_operand에 #, @ 제외하고 저장
				while (operand[operand_index] != '\0')
					pure_operand[pure_index++] = operand[operand_index++];
				
				//symbol을 SYMTAB에서 찾음
				int find = find_symbol(pure_operand);
				if (find == -1)
				{
					//symbol이 SYMTAB에 없는 경우
					printf("Assemble Error in line %d : No such symbol in SYMTAB!\n", lst_line_num);
					error_flag = 1;
					return;
				}
				else
					disp = find - pc;	//symbol의 loc에서 pc 값 빼서 disp 계산
			}
			//Immediate addressing
			else if(operand[0]=='#')
			{
				char pure_operand[31] = { 0 };	//# 제외한 operand를 저장하는 배열

				//n, i value 값 저장
				result_obj_before_bind[6] = '0'; result_obj_before_bind[7] = '1';

				//operand로 symbol 받으면, pc relative
				if ((int)operand[1] >= 65)
					p_flag = 1;
				
				//operand로 상수 받으면, pc relative 아님
				else
					p_flag = 0;

				int operand_index = 1, pure_index=0;
				
				//operand를 pure_operand에 #, @ 제외하고 저장
				while (operand[operand_index] != '\0')
					pure_operand[pure_index++] = operand[operand_index++];

				//x,b,p,e value 저장
				xbpe_setting(result_obj_before_bind, x_flag, b_flag, p_flag, e_flag);

				//operand의 disp 저장
				if (p_flag)
				{
					//symbol을 SYMTAB에서 찾음
					int find = find_symbol(pure_operand);
					if (find == -1)
					{
						//symbol이 SYMTAB에 없는 경우
						printf("Assemble Error in line %d : No such symbol in SYMTAB!\n",lst_line_num);
						error_flag = 1;
						return;
					}
					else
						disp = find_symbol(pure_operand) - pc;	//symbol의 loc에서 pc 값 빼서 disp 계산
				}
				else
					disp = atoi(pure_operand);	//pc relative 아니면 operand의 10진수 값 그대로 저장. 
			}
			//Simple addressing
			else
			{
				//n, i value 값 저장
				result_obj_before_bind[6] = '1'; result_obj_before_bind[7] = '1';

				char pure_operand[31] = { 0 };		//, 제외한 operand를 저장하는 배열 

				int operand_index = 0, pure_index = 0, comma_flag=0;	 //operand and pure_operand의 index,  comma가 존재할 때 comma_flag는 1이 됨.
				//operand를 pure_operand에 , 제외하고 저장
				while (operand[operand_index] != '\0')
				{
					if (operand[operand_index] == ',')
					{
						comma_flag = 1;
						operand_index++;
					}
					//comma와 X가 존재하면, then it x flag는 1이 됨
					if (comma_flag == 1 && operand[operand_index] == 'X')
						x_flag = 1;
					else if (comma_flag == 1 && operand[operand_index] != 'X' && operand[operand_index] != ' ')
					{
						//comma 이후 잘못된 instruction이 들어옴, 에러 메시지 출력
						printf("Assemble Error in the line %d : wrong register after , !\n",lst_line_num);
						error_flag = 1;
						return;
					}
					if(!comma_flag)
						pure_operand[pure_index++] = operand[operand_index];

					operand_index++;
				}
				//symbol을 SYMTAB에서 찾음
				int symbol_length = find_symbol(pure_operand);
			
				if (symbol_length == -1)
				{
					//symbol이 SYMTAB에 없는 경우
					printf("Assemble Error in line %d : No such symbol in SYMTAB!\n",lst_line_num);
					error_flag = 1;
					return;
				}
				
				//symbol loc 이용하여 disp 계산
				disp = symbol_length - pc;

				//만일 disp의 절댓값이 2047을 넘는 경우, format 3가 불가능하므로 BASE relative
				if (abs(disp)>2047)
				{	
					//disp가 음수 -2048인 경우는 format 3가능. 
					if (disp < 0 && disp == 0x8000)
					{
						b_flag = 0;
						p_flag = 0;
					}
					else{
					//아닌 경우, 불가능하므로 BASE relative
					b_flag = 1;
					p_flag = 0;}
				}

				//x,b,p,e value 저장
				xbpe_setting(result_obj_before_bind, x_flag, b_flag, p_flag, e_flag);

				//If BASE relative
				if (b_flag)
				{
					//BASE의 symbol을 SYMTAB에서 찾음. 
					int find = find_symbol(BASE_str);
					if (find == -1)
					{
						//symbol이 SYMTAB에 없는 경우
						printf("Assemble Error in line %d : No such symbol in SYMTAB!\n",lst_line_num);
						error_flag = 1;
						return;
					}
					else
					{
						//BASE relative로 disp 계산
						disp = symbol_length - find;

						//여전히 범위 넘기면 오류 메시지 출력
						if (abs(disp) > 2047)
						{
								if(disp > 0 || (disp <0 && abs(disp)>2048)){
								printf("Assemble Error in line %d : Address out of bound!\n", lst_line_num);
								error_flag = 1;
								return;}
						}
					}
					
				}

			}
			
			//binary number를 hexa number로 변환
			change_hex(result_obj_before_bind, result_obj_code, 3);
			
			//disp number를 result_obj_code에 저장. 
			disp_addr_set(result_obj_code, disp, 3);

		}
	}
}

/*----------------------------------------*/
/* func_name : num_to_hexchar        	  */
/* purpose : 받은 int 정수를 2진수로 바꾸어 value array에 저장. */
/* return  :	none             		  */
/*----------------------------------------*/
void num_to_hexchar(char* value, int num)
{
	int x = 0, i=0;	//x는 16으로 나눈 몫이나 나머지 저장, i는 value arr의 index
	for (int cnt = 0; cnt < 2; cnt++)
	{
		if (cnt == 0)
			x = num / 16;	//16으로 나눈 몫
		else
			x = num % 16;	//16으로 나눈 나머지
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

		i += 4;	//다음 4개의 index로 간다. 
	}
}

/*-------------------------------------------*/
/* func_name : xbpe_setting       		  	 */
/* purpose : x,b,p,e value에 따라 xbpe 값을 set    */
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
/* purpose : SYMTAB에서 symbol을 찾고    */
/*   	     symbol's LOCCTR을 리턴  	  */
/* return  : symbol's LOCCTR              */
/*----------------------------------------*/
int find_symbol(char* operand) {
	SYM* cur=first;
	while (cur != NULL)
	{
		//찾은 경우, loc 값 return
		if (!(strcmp(cur->label, operand)))
			return cur->locctr;
		cur = cur->next;
	}
	
	//찾지 못하면 return -1
	return -1;
}

/*-------------------------------------------------------*/
/* func_name : change_hex            					 */
/* purpose : 2진수로 된 array를 16진수로 바꾸어서 저장.      */
/* return  : none                         				 */
/*-------------------------------------------------------*/
void change_hex(char* bin, char* hex, int format_num)
{
	int k = 0;			//16진수 array의 index
	//만일 format_number가 1이면, 8 비트만 16진수로 바꾸면 됨. 
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
	//만일 format_number가 2이면, 16비트를 16진수로 바꾸어준다. 
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
	//format 1,2가 아니면 12bit를 16진수로 바꾸어준다. 
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
/* purpose : 입력 받은 정수에 대한 16진수 char 값을 return */
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
/* purpose : disp 또는 addr을 16진수로 바꾸어 result object code에 저장*/
/* return  : symbol's LOCCTR                    */
/*----------------------------------------------*/
void disp_addr_set(char *arr, int num, int format_type)
{
	int i = 3, quotient = 0; // arr의 index, 16진수로 나눈 몫을 저장하는 변수 
	int hexx = 0;	//16의 제곱들을 저장하는 변수
	
	//disp가 음수인 경우, 2's complement
	if (num < 0)
	{	
		//num에 -1을 곱한다. 
		num *= -1, hexx = 256;
		char temp_num[13] = { 0 };

		//-1곱한 것을 2진수로 바꾸어 저장. 
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
		//모든 2진수 값 XOR 연산
		for (int j = 0; j < 12; j++)
		{
			if (temp_num[j] == '0')
				temp_num[j] = '1';
			else
				temp_num[j] = '0';
		}
		//마지막에 1을 더한다. 
		int temp_index = 11;
		while (temp_index >= 0)
		{
			//temp_num[temp_index]가 0이면, 1로 바꾸어줌
			if (temp_num[temp_index] == '0')
			{
				temp_num[temp_index] = '1';
				break;
			}
			//temp_num[temp_index]가 1이면, carry 계산을 해준다. 
			else
				temp_num[temp_index] = '0';

			temp_index--;
		}

		//temp_number을 16진수로 바꾸어 temp_hex_arra에 저장 
		char temp_hex[3] = { 0 };
		change_hex(temp_num, temp_hex, 3);

		//16진수를 arr에 저장.
		for (int j = 0; j < 3; j++)
			arr[i++] = temp_hex[j];

	}
	//format 4, address 16진수로 변환
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
	//format 3, disp 16진수로 변환
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
/* purpose : register character를 입력 받아 해당 번호 저장*/
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
		//이외의 값이 들어온 경우. 에러 메시지 출력. 
		printf("Assemble Error on line %d : No such register!\n", lst_line_num);
		error_flag;
		return;
	}
}
