#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#include<signal.h>

#define MAXLINE 80
#define MAXARGS 20
#define MAXCMD 1024
void handle_SIGINT()
{
	
}
void process_input(int argc, char **argv) {
  if(argc > 1)
  {
    int gPosition = 0, lPosition = 0, i, pPos = 0;
	int pipeFlag = 0;
	for(i = 0; i < argc; i++)
	{
		if(strcmp(argv[i], "|") == 0)
		{
			pPos = i;
			pipeFlag = 1;
		}
	}
	if(pipeFlag)
	{
		if(argc < 3)
		{
			printf("csc60shell: pipe syntax error\n");
			_exit(EXIT_FAILURE);
		}
		int fd[2];
		pid_t childpid;
		pipe(fd);
		if((childpid = fork()) == 0)
		{
			dup2(fd[1], STDOUT_FILENO);
			close(fd[0]);
			close(fd[1]);
			argv[pPos] = NULL;
			execvp(argv[0], argv);
		}
		else
		{
			dup2(fd[0], STDIN_FILENO);
			close(fd[0]);
			close(fd[1]);
			execvp(argv[pPos + 1], argv + pPos + 1);
		}
	}
    for(i = 0; i < argc; i++)
    {
       if(strcmp(argv[i], ">") == 0)
       {
            gPosition = i;
       }
       else if(strcmp(argv[i], "<") == 0)
       {
            lPosition = i;
       }
    }
    if(strcmp(argv[gPosition], ">") == 0)
    {
         if(argc < 3)
         {
            printf("Output redirection error\n");
            _exit(-1);
         }
        int fileID = open(argv[gPosition + 1], O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
        close(1);
        if(fileID < 0)
        {
            printf("Create file error\n");
            _exit(-1);
        }
        dup2(fileID, 1);
        close(fileID);
        argv[gPosition] = NULL;      
     }
     else if(strcmp(argv[1], "<") == 0)
     {
        if(argc < 3)
        {
            printf("Input Redirection error\n");
            _exit(-1);
        }
        
        int fileID = open(argv[2], O_RDONLY);
        if(fileID < 0)
        {
            printf("No such file %s\n", argv[2]);
            _exit(-1);
        }
        dup2(fileID, 0);
        argv[1] = argv[2];
        argv[2] = NULL;   
     }
  } 
  if(execvp(argv[0], argv) == -1)
  {
	 if(strcmp(argv[0], NULL) == 0)
		_exit(0);
     printf("funix: %s: command not found\n", argv[0] );
     _exit(-1);
  }
}

/* ----------------------------------------------------------------- */
/*                  parse input line into argc/argv format           */
/* ----------------------------------------------------------------- */
int parseline(char *cmdline, char **argv)
{
  int count = 0;
  char *separator = " \n\t";
  argv[count] = strtok(cmdline, separator);
  while ((argv[count] != NULL) && (count+1 < MAXARGS)) {
   argv[++count] = strtok((char *) 0, separator);
  }
  return count;
}
/* ----------------------------------------------------------------- */
/*                  The main program starts here                     */
/* ----------------------------------------------------------------- */
int main(void)
{
 char cmdline[MAXLINE];
 char *argv[MAXARGS];
 int argc;
 int status;
 int maxcmd = MAXCMD;
 pid_t pid;
 char tempbuf[1056];
 char *temp = "";
 struct sigaction sig_action;
 sigemptyset( &sig_action.sa_mask );
 sig_action.sa_handler = SIG_IGN;
 sig_action.sa_flags = 0;
 sigaction(SIGINT,&sig_action, NULL);
 
 while (1) {
  printf("csc60msh> ");
  fgets(cmdline, MAXLINE, stdin);
  int argc = parseline(cmdline, argv);
  if(strcmp(cmdline, "\n") == 0)
  {
     continue;
  }
  else if(strcmp(argv[0], "exit") == 0)
  {
     return 0;
  }
  else if(strcmp(argv[0], "pwd") == 0)
  {
     printf("current directory: %s\n", getenv("PWD"));
     continue;
  }
  else if(strcmp(argv[0], "cd") == 0)
  {
     if(argv[1] == NULL)
     {
        temp = getenv("HOME");
        chdir(temp);
        setenv("PWD", temp, 1);
     }
     else
     {
        if(chdir(argv[1]) != 0)
        {
            printf("No such directory: %s\n", argv[1]);
            continue;
        }
        getcwd(tempbuf, 1056);
        setenv("PWD", tempbuf, 1);
     }
     continue;
  }

  pid = fork();
  if (pid == -1) 
    perror("Shell Program fork error");
  else if (pid == 0) 		//child
  {
	 sigaction( SIGINT, &sig_action, NULL );
     sig_action.sa_handler = SIG_DFL;
    process_input(argc, argv);
  }
  else 						//parent process
    if (wait(&status) == -1)
      perror("Shell Program error");
 }
}
