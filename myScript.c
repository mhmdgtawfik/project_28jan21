#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/stat.h> 
#include <dirent.h>
#include <sys/wait.h>

int is_hidden(const char *name);

int main(int argc, char *argv[])
{
	while (1)
	{
		
		char Input[100] ;
		char *tokens[10] ;
		int i = 0;
		int strlength = 0;
		
		printf(">");
		fgets(Input,100,stdin);
		int PIDFlag = 0;
		int PIDs[100];
		
		tokens[0] = strtok(Input," "); 
		
		strlength = strlen(tokens[0]);
		
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
				command with no arguments "exit"
				***/
				/**
				exit Command 
				**/
				if (strcmp(tokens[0],"exit") == 0)
				{
					printf("Shell is terminating\n");

					/*Kill any Child Process Created by the Shell */
					if(PIDFlag > 0)
					{
						for (PIDFlag ; PIDFlag > -1 ; PIDFlag --)
						{
							if(kill(PIDs[PIDFlag],SIGKILL))
							{
								printf("Process with ID %d is not killed ",PIDs[PIDFlag]);
							}
						}

					}	

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
				command with 1 argument  {"ls &"}
				***/

				if (strcmp(tokens[0],"ls") == 0)
				{
					if (strcmp(tokens[1],"&") == 0)
					{
					    

						PIDs[PIDFlag] = fork();

						if(PIDs[PIDFlag]< 0)
						{
							printf("Forking Failed");
						}
						
						else if (PIDs[PIDFlag] == 0)
						{
						    DIR *d;

						    struct dirent *dir;

						    d = opendir(".");

						    if (d)

						    {

						        while ((dir = readdir(d)) != NULL)

						        {
						        	if(!is_hidden(dir->d_name))printf("%s \n", dir->d_name);

						        }

						        closedir(d);

						    }
							else {
								printf("Issue with Directory");
								exit(1);
							}
							exit(0);
						}
						
						else {

							int Eror = 0;
							waitpid(PIDs[PIDFlag],&Eror,0);
							if(Eror != 0)printf("Child Ended Unsuccessfully");
						}

						PIDFlag++;

					}

				}

				/*A Command with Arguments "cd "Directory"*/
				else if (strcmp(tokens[0],"cd") == 0)
				{
					if(chdir(tokens[1]))printf("Directory is not found");
					else 
					{
						char s[100] ;
						getcwd(s, 100);
						printf("Directory Changed to %s\n",s);
					}
				}
			

			break;
			case 2:
				/***
				command with 2 arguments (Background execcution using &) "ls -a &"
				***/
				if (strcmp(tokens[2],"&") == 0)
				{
					if (strcmp(tokens[0],"ls") == 0)
					{
						if (strcmp(tokens[1],"-a") == 0)
					    {

							PIDs[PIDFlag] = fork();
							
							if(PIDs[PIDFlag] < 0)
							{
								printf("Forking Failed");
							}
							
							else if (PIDs[PIDFlag] == 0)
							{
							    DIR *d;

							    struct dirent *dir;

							    d = opendir(".");

							    if (d)

							    {

							        while ((dir = readdir(d)) != NULL)

							        {
							        	printf("%s \n", dir->d_name);

							        }

							        closedir(d);

							    }
								else {
									printf("Issue with Directory");
									exit(1);
								}
								exit(0);
							}
							
							else 
							{

								int Eror = 0;
								waitpid(PIDs[PIDFlag],&Eror,0);
								if(Eror != 0)printf("Child Ended Unsuccessfully");
							}

							PIDFlag++;
						}

					}					
				}

			/*A Command with Arguments Read from File "Sort < testfile"*/	
			
				
			break ;
			case 3:

			/*A Command with Arguments redirected to File "ls -l > foo" */




			/*A Command without Arguments Piped to Another Command "ls -l | more"*/


			break;
		}	
	}
}


int is_hidden(const char *name)
{
  return (name[0] == '.' &&
         strcmp(name, ".") != 0 &&
         strcmp(name, "..") != 0);
}
