#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/stat.h> 
int main(int argc, char *argv[])
{
	while (1)
	{
		
		char Input[100] ;
		
		printf(">");
		
		fgets(Input,100,stdin);
		
		char *tokens[10] ;
		
		int i = 0;
		
		tokens[0] = strtok(Input," "); 
		
		int strlength = strlen(tokens[0]);
		
		if(tokens[0][strlength-1]=='\n')tokens[0][strlength-1]='\0';

		while(i < 9 && tokens[i] != NULL) 
		{
			i++;

			tokens[i] = strtok(NULL," ");

			if (tokens[i] != NULL)
			{
				strlength = strlen(tokens[i]);

				if(tokens[i][strlength-1]=='\n') tokens[i][strlength-1]='\0';		
			
			}
		}


		switch ((i-1))
		{
			case 0: 

				/***
				command with no arguments
				***/
				/**
				exit Command 
				**/
				if (strcmp(tokens[0],"exit") == 0)
				{
					printf("Shell is terminating\n");
					/*Kill any Child Process Created by the Shell */

					printf("\n[Process Completed]\n");

					exit(0);
				}

				else if (strcmp(tokens[0],"pwd") == 0)
				{
					char cwd[1000];
					 if (getcwd(cwd, sizeof(cwd)) != NULL)
					 {
						printf("%s\n",cwd);
					 } 
					
				}
			break;
			case 1: 
				/***
				command with 1 argument 
				***/
				if (strcmp(tokens[0],"mkdir") == 0)
				{
					int check = mkdir(tokens[1],0777);
					if (check)
					{
						printf("Folder Not Created Successfull! \n");
					}
					else 
					{
						printf("Folder Created \n");
					}
				}				

			break;
			case 2:
				/***
				command with 2 arguments (Background execcution using &) 
				***/
				
			break ;
			case 3: 
			break;
		}	
	}
}
