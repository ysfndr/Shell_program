// The MIT License (MIT)
// 
// Copyright (c) 2016 Trevor Bakker 
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
// 
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

/*

Name: Yusuf Nadir Cavus
ID: 1001941135

*/

#define _GNU_SOURCE

#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <signal.h>

#define WHITESPACE " \t\n"      // We want to split our command line up into tokens
                                // so we need to define what delimits our tokens.
                                // In this case  white space
                                // will separate the tokens on our command line

#define MAX_COMMAND_SIZE 255    // The maximum command-line size

#define MAX_NUM_ARGUMENTS 12    // Mav shell only supports eleven arguments (command + 10 parameters)

int main()
{

  char * command_string = (char*) malloc( MAX_COMMAND_SIZE);

  long pidlist[20]; //to store the pids of the last 20 processes for listpids
  int i = 0; //keep track of the position in pidlist

  char **history = malloc(15 * (sizeof(char *))); //to store the last 15 commands entered
  int j = 0; //keep track of the position in history

  while( 1 )
  {
    // Print out the msh prompt
    printf ("msh> ");

    // Read the command from the commandline.  The
    // maximum command that will be read is MAX_COMMAND_SIZE
    // This while command will wait here until the user
    // inputs something since fgets returns NULL when there
    // is no input
    while( !fgets (command_string, MAX_COMMAND_SIZE, stdin) );

    /*Insert command into history so we can run history command*/
    if (j < 15)
    {
      history[j] = (char*) malloc( sizeof(command_string) + 1); // allocating memory for history[j]
      strcpy(history[j], command_string);
      j++; //incremeting j so we can insert the next command into next position on history
    }

    /*Check if the command is !# before parsing and tokenizing so we can update command_string*/
    if (command_string[0] == '!')
    { 
      int index = atoi (&command_string[1]);
      if (history[index] != NULL)
      {
        strcpy(command_string, history[index]);
      }
      else
      {
        printf("Command not in history.\n");
      }
    }

    /* Parse input */
    char *token[MAX_NUM_ARGUMENTS];

    int token_count = 0;                                 
                                                           
    // Pointer to point to the token
    // parsed by strsep
    char *argument_ptr;                                         
                                                           
    char *working_string  = strdup( command_string );                

    // we are going to move the working_string pointer so
    // keep track of its original value so we can deallocate
    // the correct amount at the end
    char *head_ptr = working_string;

    // Tokenize the input strings with whitespace used as the delimiter
    while ( ( (argument_ptr = strsep(&working_string, WHITESPACE ) ) != NULL) && 
              (token_count<MAX_NUM_ARGUMENTS))
    {
      token[token_count] = strndup( argument_ptr, MAX_COMMAND_SIZE );
      if( strlen( token[token_count] ) == 0 )
      {
        token[token_count] = NULL;
      }
        token_count++;
    }

    if ( command_string[0] == '\n' ) 
    {
      continue;
    }

    if (!strcmp(token[0], "exit") || !strcmp(token[0], "quit"))    //If the command is 'exit' or 'quit'
    {
      exit(0);
    }
    else if (!strcmp(token[0], "cd"))
    {
      chdir(token[1]); //token[0] = cd, token[1] = path/directory
    }
    else if (!strcmp(token[0], "listpids"))
    {
      for (int i=0; i<20; i++)
      {
        if (pidlist[i])
        {
          printf("%d: %ld\n", i, pidlist[i]);
        }
        else
        {
          break;
        }
      }
    }
    else if (!strcmp(token[0], "history"))
    {
      for (int i=0; i<15; i++)
      {
        if (history[i] != NULL)
        {
          printf("[%d]: %s", i, history[i]);
        }
        else
        {
          break;
        }
      }
    }

    /*>>>>>>>>>>>>>>>>>>>>>><<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*/

    else
    {
      pid_t pid = fork( );
      if( pid == 0 )
      {
        int ret = execvp( token[0], &token[0] );  
        if( ret == -1 )
        {
          printf("%s: Command not found.\n", token[0]);
        }
        exit(0);    //exit the child process. Note: before adding this line exit or quit calls 
                    //on the next command did not work because it only exited the child process.
      }
      else 
      {
        /*Parent returns child's pid. We store that pid so we can list it when listpids is run*/
        if (i < 20) //to avoid segFault when more than 20 processes entered
        {
          pidlist[i] = pid; //putting child's pid into pidlist
          i++; //incremeting i so we can insert the next proccess's pid into next position on pidlist
        }

        int status;
        wait( & status );
      }
    }
    

    free(head_ptr);
  }

  /*Free history*/
  for (int i=0; i<15; i++)
  {
    free(history[i]);
  }
  free(history);

  return 0;
  // e2520ca2-76f3-90d6-0242ac120003
}
