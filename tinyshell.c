
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/wait.h>

#define LINESZ 1024 // Maximum number of characters in an input string

//========== MARCO for Printing error messages ================
// Simply prints an error message
#define PRINT_ERROR                    \
  do                                   \
  {                                    \
    printf("An error has occurred\n"); \
  } while (0)

// To be used to handle failures of system calls
// such as fork, execvp, waitpid, getcwd, etc.
#define PRINT_ERROR_SYSCALL(x) \
  do                           \
  {                            \
    perror(x);                 \
    PRINT_ERROR;               \
  } while (0)

// To be used to simply free up any dynamically allocated memory in the
// command data structure
#define FREE_ALLOCATED_MEMORY(x) \
  do                             \
  {                              \
    free_mem(&x);                \
  } while (0)
//---------------------------------------------


// Command data structure: 
// After parsing a given input string, this data structure
// is used to hold/store the parsed data
typedef struct tcommand
{
  // The actual command to be exectued 
  char cmd[LINESZ + 2];       
  
  // The number of arguments including the command itself
  int arg_count;

  // The arguments along with the command (at args[0])
  char *args[LINESZ + 2];

  // A flag to denote whether command has output redirection
  // 1 = present 0 = absent
  unsigned char redirection;

  // In case of output redirection, the number of the output file ...
  char ofile[LINESZ + 2];

  // A flag to denote whether the command ends with "&" 
  // (to be executed in the background)
  // 1 = present , 0 = absent
  unsigned char detach;

} cmd_string;


// Initialize a given command structure
void initialize(cmd_string *cstring)
{
  // making argument count 0
  cstring->arg_count = 0;

  // ensuring redirection flag to be absent
  cstring->redirection = (unsigned char)0;

  // ensuring detach flag to be absent
  cstring->detach = (unsigned char)0;

  // Filling up the "ofile" buffer with 0
  memset(cstring->ofile, 0, LINESZ + 2);

  // Filling up the "cmd" buffer with 0
  memset(cstring->cmd, 0, LINESZ + 2);
}


/* 
 * This function frees up dynamically allocated
 * memory that are currently being pointed to by 
 * cstring->args[i]
 */
void free_mem(cmd_string *cstring)
{
  for (int i = 0; i < cstring->arg_count; i++)
  {
    if (cstring->args[i])
    {
      free(cstring->args[i]);
#ifdef DEBUG
      printf("DEBUG: freed memory\n");
#endif
      cstring->args[i] = NULL;
    }
  }
}



/* The function that populates/loads cmd_string after parsing a line
 * 
 * @param:
 *    line    - the input string read from stdin
 *    cstring - the command structure to be loaded with the parsed values from line
 */
void parse_cmd_string(char *line, cmd_string *cstring)
{
  initialize(cstring);
  char *p = strtok(line, " \t\n");
  //this while loop just tokenizes the string
  while (p)
  {
    if (cstring->arg_count == 0)
    {
      strcpy(cstring->cmd, p);
    }
    cstring->args[cstring->arg_count] = strdup(p);
    ++cstring->arg_count;
    p = strtok(NULL, " \t\n");
  }
 #ifdef DEBUG
  printf("DEBUG: arg_count = %d\n", cstring->arg_count);
 #endif

  if (cstring->arg_count == 0)
    return; //empty line
  // if there is & then just remove it from the string and then decrement the arg count while making detach as 1
  if (strcmp(cstring->args[cstring->arg_count - 1], "&") == 0)
  {
    free(cstring->args[cstring->arg_count - 1]); // free
    cstring->args[cstring->arg_count - 1] = NULL;
    cstring->detach = 1;
    --cstring->arg_count;
  }

  //here checking the if there is an output file and then finding the file  name and it is put in ofile and redirection is made 1
  
  if (cstring->arg_count >= 3)
  {
    if (strcmp(cstring->args[cstring->arg_count - 2], ">") == 0)
    {
 #ifdef DEBUG
      printf("DEBUG: Processing redirection\n");
 #endif

      strcpy(cstring->ofile, cstring->args[cstring->arg_count - 1]);

      free(cstring->args[cstring->arg_count - 1]); // free
      free(cstring->args[cstring->arg_count - 2]); // free
      cstring->arg_count -= 2;
      cstring->args[cstring->arg_count] = NULL;
      cstring->redirection = (unsigned char)1;
      return;
    }
    else
    {
      cstring->args[cstring->arg_count] = NULL;
      return;
    }
  }
  cstring->args[cstring->arg_count] = NULL;
  return;
}

/** 
 * Function
 * @name: process_builtin_commands
 *
 * @description:
 * Check if the given input is one of the supported built-in commands.
 * If so, process it here. 
 *
 * @param: C -- a pointer to the cmd_string struct
 *
 * @return: 
 *    1 if the input is a built-in command
 *    0 otherwise
 **/
unsigned char process_builtin_commands(cmd_string *C)
{
  if (strcmp(C->cmd, "cd") == 0)
  {
 
    if(C->arg_count>1)
    {
      int abc=chdir(C->args[1]);
      
    }
    else{
      char *home=getenv("HOME");
      int abc=chdir(home);
    }
    return (unsigned char)1;
  }
  else if (strcmp(C->cmd, "pwd") == 0)
  {
    
     char cwd[1024] = "";
     fprintf(stdout,"%s",getcwd(cwd, sizeof(cwd)));
     return (unsigned char)1;
    
  }
  else
    return 0;
}



int main(int argc, char *argv[])
{
  char line[LINESZ + 2];
  cmd_string C;
  initialize(&C);

  // Loop forever
  while (1)
  {
    // Free up allocated memory, if any
    FREE_ALLOCATED_MEMORY(C);

    // Let's print the shell prompt
    printf("tinyshell> ");

    // read a line (i.e., input string) from stdin
    if (fgets(line, LINESZ, stdin) == NULL)
    {
      return 0; // reached EOF (end-of-file), so we can safely terminate
    }

    #ifdef DEBUG
        printf("DEBUG: Input line: %s\n", line);
    #endif

    // Parsing the input string and storing info in C
    parse_cmd_string(line, &C);

    if (C.arg_count == 0)
    {
      #ifdef DEBUG
            printf("DEBUG: Ignoring empty line ...\n");
      #endif
      continue; //Ignore empty line ....
    }

    /* Handling "exit" command. It'sw already implemented for you. */
    if (strcmp(C.cmd, "exit") == 0)
    {
      /* check for arguments to `exit` commands.
       * Any argument to `exit` command is an error.
       */
      if (C.arg_count > 1)
      {
        PRINT_ERROR; // Print an error message
        continue;
      }
      else
      {
        return 0; // we can safely terminate now
      }
    }

    /* Check if the input command is a built-in command */
    unsigned char r = process_builtin_commands(&C);
    if (r == 1)
      continue; 

    // If we are here, that means the input is not a 
    // built-in command. So we must handle it using fork and execvp

    int fc = fork();

    if (fc < 0)
    {
      // Shows how to use PRINT_ERROR_SYSCALL() 
      PRINT_ERROR_SYSCALL("fork");
      return 1;
    }
    else if (fc == 0)
    {
      //child's code


      if (C.redirection == 1)
      {
        #ifdef DEBUG
                printf("DEBUG: Redirection enabled\n");
        #endif

        FILE* fp = fopen(C.ofile, "w" );
  
        int fd = fileno(fp);

        dup2(fd, STDOUT_FILENO);
        fclose(fp);
        
       

          
        


      }

       if (execvp(*C.args, C.args) < 0) {     /* execute the command  */
               PRINT_ERROR_SYSCALL("execvp");
               exit(1);
          }



      

      // if execvp() succeeds, child never executes the following
      FREE_ALLOCATED_MEMORY(C);
      return 1; 
    }
    else
    {
      

      

      if (C.detach == 0)
      {
         waitpid(-1, NULL, 0);
       
        

      }
    }
  }

  return 0; // done
}