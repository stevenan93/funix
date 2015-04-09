#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>

#define MAXLINE 80
#define MAXARGS 20

void process_input(int argc, char **argv) {
  /* Problem 1: perform system call execvp to execute command     */ 
  /*            No special operator(s) detected                   */
  /* Hint: Please be sure to review execvp.c sample program       */
  /* if (........ == -1) {                                        */  
  /*  perror("Shell Program");                                    */
  /*  _exit(-1);                                                  */
  /* }                                                            */
  /* Problem 2: Handle redirection operators: < , or  >, or both  */
  if(argc > 1)
  {
    printf("tryna do somthing bud amirite\n");
    _exit(0);
  } 
  if(execvp(argv[0], argv) == -1)
  {
    perror("Shell program error\n");
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
 pid_t pid;
 char tempbuf[1056];
 char *temp = "";

 /* Loop forever to wait and process commands */
 while (1) {
  /* Step 1: Name your shell: csc60mshell - m for mini shell */ 
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
  /* Step 1: If user hits enter key without a command, continue to loop again at the beginning */
  /*         Hint: look up for they keyword "continue" in C */
  /* Step 1: Call parseline to build argc/argv: argc/argv parameters declared above */ 
  /* Step 1: Handle build-in command: exit, pwd, or cd - if detect one              */
  /* Step 1: Else, fork off a process */ 
  pid = fork();
  if (pid == -1) 
    perror("Shell Program fork error");
  else if (pid == 0) 
    /* I am child process. I will execute the command, call: execvp */
    process_input(argc, argv);
  else 
    /* I am parent process */
    if (wait(&status) == -1)
      perror("Shell Program error");
//    else
  //    printf("Child returned status: %d\n",status);
 }
}
