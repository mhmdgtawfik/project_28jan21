#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/stat.h> 
#include <dirent.h>
#include <sys/wait.h>

int is_hidden(const char *name);

int CommandExecute (int Size ,char * Command);

FILE * HISTFILEP = NULL;

FILE * Profile = NULL;

char cwd[100];

int commands = 0;

char Enviroment[3][10] = {"PATH","HOME","HISTFILE"};

char EnivromentValue[3][50] = {"/bin",".",".bash_history"};

int PIDFlag = 0;

int PIDs[50];

int main(void)
{
	HISTFILEP = fopen(EnivromentValue[2], "a+");	

	/*Open File .bash_profile and Run the Commands from it */
	Profile = fopen(".CIS3110_profile","a+");

	if (Profile == NULL ) 
	{
		printf("Error Init Shell Terminating");

		exit(1);

	}

	else {
		/* Get the first line of the file. */

		char * line_buf  =NULL;

		unsigned long int line_buf_size = 0;

  	    int line_size = 0;

		line_size = getline(&line_buf, &line_buf_size, Profile);

  		while (line_size >= 0 && line_buf != NULL)
  		{
  			/*Get the Rest of the File */
  			CommandExecute(line_buf_size,line_buf);

  			line_size = getline(&line_buf, &line_buf_size, Profile);
  		}

	}

	while (1)
	{
		char s[100] ;
		char Input[100] ;
		int CommandReturn = 0;
		int Y = 0;
		char C ;
		
		getcwd(s, 100);
		
		printf("%s>",s);
		
		fgets(Input,100,stdin);
		
		fprintf(HISTFILEP," %d  %s",commands,Input);
		
		fclose(HISTFILEP);
		
		HISTFILEP = fopen(EnivromentValue[2], "a+");
		
		C = Input[0];

		while (C != '\n')
		{
			Y++;
			C = Input[Y];

		}

		Input[Y] = '\0' ;

		CommandReturn = CommandExecute(100,Input);
	}
}


int is_hidden(const char *name)
{
  return (name[0] == '.' &&
         strcmp(name, ".") != 0 &&
         strcmp(name, "..") != 0);
}


int CommandExecute (int Size ,char * Command )
{
		char *tokens[10] ;

		int strlength = 0;

		int i = 0;

		commands++;
		
		tokens[0] = strtok(Command," "); 
		
		strlength = strlen(tokens[0]);

		while(i < 9 && tokens[i] != NULL) 
		{
			i++;

			tokens[i] = strtok(NULL," ");

			if (tokens[i] != NULL)
			{
				strlength = strlen(tokens[i]);
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

					FILE * HISTFILER = fopen(EnivromentValue[2],"r");
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
						HISTFILEP = fopen(EnivromentValue[2],"w");
						fprintf(HISTFILEP,"%c",EOF);
						fclose(HISTFILEP);
						HISTFILEP = fopen(EnivromentValue[2],"a+");
						commands = 0;
						
					}
					else
					{
						FILE * HISTFILER = fopen(EnivromentValue[2],"r");
						int c = getc(HISTFILER);
						int Line = 0;
						static int LineOffest[1000];
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
				else if (strcmp(tokens[0],"echo") == 0)
				{

					if(tokens[1][0] == '$')
					{
						char C = tokens[1][1];
						int i  = 0;
						char Env[10] ;
						while (C != '\0')
						{
							Env[i] = C ;
							i++;
							C = tokens[1][i+1];
						}
						Env[i] = '\0';

						if ((strcmp(Env,"HOME") == 0))
						{
							printf("%s\n",EnivromentValue[1]);
						}
						else if ((strcmp(Env,"PATH") == 0))
						{
							printf("%s\n",EnivromentValue[0]);
						}
						else if ((strcmp(Env,"HISTFILE") == 0))
						{
							printf("%s\n",EnivromentValue[2]);
						}
						else 
						{
							printf("No Enviroment Variable with These Name \n");

						}

					}
				}
				else if (strcmp(tokens[0],"export") == 0)
				{
					char C ;
					int i = 0;
					int equalIndex = 0;
					int DollarSignIndices[10] ;
					int DollarSignNumber = 0;
					char Env[10] ;
					int EnviromentIndex = 0;
					C = tokens[1][0];
					printf("%d",EnviromentIndex);
					while (C != '\0')
					{
						if (C == '=')
						{
							equalIndex = i;
						}
						else if (C == '$')
						{
							DollarSignIndices[DollarSignNumber]= i;
							DollarSignNumber ++;
						}
						i++;

					}


					C = tokens[1][1];
					i  = 0;
	
					while (C != '\0')
					{
						Env[i] = C ;
						i++;
						C = tokens[1][i+1];
					}
					Env[i] = '\0';

					if ((strcmp(Env,"HOME") == 0))
					{
						EnviromentIndex = 1;

					}
					else if ((strcmp(Env,"PATH") == 0))
					{
						EnviromentIndex = 0;
					}
					else if ((strcmp(Env,"HISTFILE") == 0))
					{
						EnviromentIndex = 2;
					}
					else 
					{
						printf("No Enviroment Variable with These Name \n");
					}


					int EniromentVariableSize = sizeof(EnivromentValue[EnviromentIndex]);
					
					int R = 1;
					int Q = 0;

					for (int Q = EniromentVariableSize; Q < (sizeof(tokens[1]) - equalIndex) ; Q++,R++)
					{
						EnivromentValue[EnviromentIndex][Q] = tokens[1][equalIndex+R];

					}
					EnivromentValue[EnviromentIndex][Q] = '\0';

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
