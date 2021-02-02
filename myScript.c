#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/stat.h> 
#include <dirent.h>
#include <sys/wait.h>

int is_hidden(const char *name);

int main(void)
{
	FILE *HISTFILEP = NULL;

	char HISTFILE[100]= ".bash_history";

	HISTFILEP = fopen(HISTFILE, "a+");

	char cwd[1000];

	int commands = 0;
	while (1)
	{
		char Input[100] ;
		char *tokens[10] ;
		int i = 0;
		int strlength = 0;
		char s[100] ;
		getcwd(s, 100);
		printf("%s>",s);
		
		fgets(Input,100,stdin);

		commands++;

		fprintf(HISTFILEP," %d  %s",commands,Input);

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
				else if (strcmp(tokens[0],"history") == 0)
				{

					FILE * HISTFILER = fopen(HISTFILE,"r");
					int c = getc(HISTFILER);
					while (c != EOF)
					{
   						printf("%c",c);
      					c = getc(HISTFILER);
					}

				   fclose(HISTFILER);


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
					}
				}
				/*History Command "*/
				else if (strcmp(tokens[0],"history") == 0)
				{
					if (strcmp(tokens[1],"-c") == 0)
					{
						fclose(HISTFILEP);
						HISTFILEP = fopen(HISTFILE,"w");
						fprintf(HISTFILEP,"%c",EOF);
						fclose(HISTFILEP);
						HISTFILEP = fopen(HISTFILE,"a+");
						commands = 0;
						
					}
					else
					{
						FILE * HISTFILER = fopen(HISTFILE,"r");
						int c = getc(HISTFILER);
						int Line = 0;
						int LineOffest[1000];
						int CharacterCount = 0;
						int StartRequestLine = 0;
   						while (c != EOF)
   						{
   							CharacterCount ++;
      						if(c == '\n') 
							{
								LineOffest[Line] = CharacterCount;
      							Line ++;
							}
      						c = getc(HISTFILER);
					   }

					   StartRequestLine = Line - atoi(tokens[1]) - 1 ;

					   if(StartRequestLine > 0)
					   {
						   	fseek(HISTFILER, LineOffest[StartRequestLine],SEEK_SET);

							c = getc(HISTFILER);

	   						while (c != EOF)
	   						{
	   							printf("%c",c);
	      						c = getc(HISTFILER);
						   }
						}
						else 
						{
							printf ("The File Don't Contain Much Commands");
						}

					   fclose(HISTFILER);

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
				/*A Command with Arguments redirected to File "pwd > foo" */
				else if (strcmp(tokens[0],"pwd") == 0)
				{
					if (strcmp(tokens[1],">") == 0)
					{
						FILE *fp;

						fp = fopen(tokens[2], "a");

						char cwd[1000];

						if (fp != NULL)
						{					 
					 		if (getcwd(cwd, sizeof(cwd)) != NULL)
					 		{
								fprintf(fp,"%s\n",cwd);
					 		} 

					 		fclose(fp);
					 		
						}

					}
				}

			/*A Command with Arguments Read from File "cat < testfile"*/	
				else if (strcmp(tokens[0],"cat") == 0)
				{
					if (strcmp(tokens[1],"<") == 0)
					{
						FILE *fp;

						fp = fopen(tokens[2], "r");

						char cwd[1000];

						if (fp != NULL)
						{

							char C;
							do
							{
								C = fgetc(fp) ;
								if (C != EOF) printf("%c",C);
							}while (C != EOF);


					 		fclose(fp);
					 		
						}
						else 
						{
							printf("File Not Found Please Create One \n");
						}

					}
				}
				
			break ;
			case 3:
			/*A Command without Arguments Piped to Another Command "locate file | cat*/
			/*if (strcmp(tokens[2],"|") == 0)
			{

			}*/



			break;
		}	
	}
	fclose(HISTFILEP);
}


int is_hidden(const char *name)
{
  return (name[0] == '.' &&
         strcmp(name, ".") != 0 &&
         strcmp(name, "..") != 0);
}
