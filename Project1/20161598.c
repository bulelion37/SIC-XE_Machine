#include "20161598.h"

int main()
{
	//making hash table about opcode
	make_hashtb();

	while (!quit_flag) {
		//get input until q or quit  
		
		int i = 0;
		
		printf("sicsim> ");
		if(fgets(command, 100, stdin)!=NULL)
		{
		//putting '\0' in the end of string instead of '\n' 
		while (command[i]!='\n')
			i++;
		command[i] = '\0';
		
		//to distinct command, call command_distinct()
		command_distinct(command);
		}
	}
	return 0;
}
