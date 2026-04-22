#include"header.h"

extern char *ext_cmds[152];
extern int status;
extern char prompt[25];
extern char path[100];
char *builtins[] = {"echo", "printf", "read", "cd", "pwd", "pushd", "popd", "dirs", "let", "eval",
"set", "unset", "export", "declare", "typeset", "readonly", "getopts", "source","exit", "exec",
 "shopt", "caller", "true", "type", "hash", "bind", "help","jobs","bg", "fg", NULL};
 
 extern int count;
 extern Slist *head ;

void print_list(Slist *head)
{
	if (head == NULL)
	{
		//printf("INFO : List is empty\n");
        //prints nothing
	}
    else
    {
	    while (head)		
	    {
		    printf("%s\n", head -> string);
		    head = head -> link;
	    }
    }
}

int sl_delete_first(Slist **head)
{
    if(*head == NULL) return FAILURE;
    Slist *temp = (*head)->link;
    free(*head);
    *head = temp;
    return SUCCESS;
} 

//create a 2d array for external commands
void extract_external_commands(char **external_commands)
{
    FILE *fp;
    fp = fopen("ext_cmds.txt", "r");
    if(fp == NULL)
    {
        printf("File not opened\n");
        return;
    }
    int ch;
    int j = 0;
    while((ch = fgetc(fp)) != EOF)
    {
        char command[50];
        int i =0;
        command[i++] = ch;
        while((ch = fgetc(fp)) != '\n' && ch != EOF)
        {
            command[i] = ch;
            i++;
        }
        command[i] = '\0';
        external_commands[j] = malloc((strlen(command)+1) * sizeof(char));
        strcpy(external_commands[j], command);
        j++;
    }
    external_commands[j] = NULL;
    fclose(fp);
}

char cmd[10];
char *get_command(char *input_string)
{
    int i = 0;
    while(input_string[i]!=' ' && input_string[i]!='\0')
    {
        cmd[i] = input_string[i];
        i++;
    }
    cmd[i] = '\0';
    return cmd;
}

int check_command_type(char *command)
{
    int i;
    //check for internal commands
    for(i = 0; builtins[i]!=NULL; i++)
    {
        if(strcmp(command, builtins[i]) == 0)
            return BUILTIN;
    }
    /*else check for external commands*/
    for(i = 0; i < 152 ; i++)
    {
        if(strcmp(command, ext_cmds[i]) == 0)
            return EXTERNAL;
    }
    //else not a command
    return NO_COMMAND;
}

int execute_internal_commands(char *input_string)
{
    char buff[100];
    //check if command is exit
    if(strcmp(cmd, "exit") == 0)
        exit(0);
    else if(strcmp(cmd,"pwd") == 0)
    {
        getcwd(buff, 100);
        printf("%s\n", buff);
    }
    else if(strncmp(input_string, "cd", 2) == 0)
    {
        if(strcmp(input_string, "cd") == 0)
        {
            if(chdir(path) ==-1)
            {
                perror(" ");
                return 0;
            }
        }
        else 
        {
            if(chdir(input_string + 3) == -1)
            {
                perror(" ");
                return 0;
            }
            getcwd(buff, sizeof(buff));
            printf("%s\n", buff);
         }
    }
    else if(strcmp(input_string, "echo $$") == 0)
    {
        printf("%d\n", getpid());
    }
    else if(strcmp(input_string, "echo $?") == 0)
    {
        if(WIFEXITED(status))
            printf("%d\n", WEXITSTATUS(status));
        else if(WIFSIGNALED(status))
            printf("%d\n", WTERMSIG(status));
    }
    else if(strcmp(input_string, "echo $SHELL") == 0)
    {
        printf("%s\n", getenv("SHELL"));
    }
    else if(strcmp(input_string, "jobs") == 0)
    {
        print_list(head);
    }
    else if(strcmp(input_string, "fg") == 0)
    {
        if(head == NULL)
        {
            //list empty, no process is stopped , prints error message and returns
            printf("-bash: fg: current: no such job\n");
            return 0;
        } 
        kill(head->pid, SIGCONT);
        printf("%s\n", head -> string);
        waitpid(head->pid , &status, WUNTRACED);
        sl_delete_first(&head);
    }
    else if(strcmp(input_string, "bg") == 0)
    {
        if(head == NULL)
        {
            //list empty, no process is stopped , prints error message and returns
            printf("-bash: bg: current: no such job\n");
            return 0;
        }
        //register signal
        signal(SIGCHLD, signal_handler);
        kill(head->pid, SIGCONT);
        printf("[%d]+ %s &\n", count, head->string);
        count --;
        sl_delete_first(&head);
    }
}

int execute_external_commands(char *input_string)
{
    int cmd[10];
    cmd[0] = 0;
    int ind = 1;
    int i = 0;
    char arr[10][100];
    int k = 0;
    char buffer[100];
    while(input_string[i] != '\0')
    {
        int j = 0;
        while(input_string[i] != ' ' && input_string[i] != '|' && input_string[i] != '\0')
        {
            buffer[j++] = input_string[i++];
        }
        buffer[j] = '\0';
        if(j > 0) strcpy(arr[k++], buffer);
        if(input_string[i] == '|')
        {
            arr[k][0] = '|';
            arr[k][1] = '\0';
            k++;
        }
        if(input_string[i] != '\0') i++;  
    }
    char *args[10];
    int j = 0;
    for(i = 0; i < k; i++)
    {
        if(strcmp(arr[i], "|") == 0)
        {
            args[j++] = NULL;
            cmd[ind++] = i + 1;
        }
        else args[j++] = arr[i];
    }
    args[j] = NULL;
    if(ind == 1) //PIPE not present - Single Command
    {
        //pipe not present
        execvp(args[0], args);
        perror("execvp");
        exit(1);
    }
    else //pipe present
    {
        //n pipe logic
        int fd[2];
        int i;
        for(i = 0; i < ind; i++)
        {
            if(i != ind-1)
            {
                pipe(fd);
            }
            int pid = fork();
            switch(pid)
            {
                case -1 :
                     perror(" ");
                     return -1;
                case 0 : //child process
                    if(i != ind-1)
                    {
                        close(fd[0]);
                        dup2(fd[1],1);
                        close(fd[1]);
                    }
                    execvp(args[cmd[i]], args + cmd[i]) ;
                    perror("execvp");
                    exit(1);
                default : //parent process
                    if(ind - 1 != i)
                    {
                        dup2(fd[0],0);
                        close(fd[0]);
                        close(fd[1]);
                    }
            }
        }
        for(i = 0; i < ind;i++)
        {
            wait(NULL);
        }
        input_string[0] = '\0';
    }
    return SUCCESS;
}