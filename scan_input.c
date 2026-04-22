#include"header.h"

int status;
int count = 0;
char *ext_cmds[152]; // 2d array to store external commands
extern char prompt[25];
extern char input_string[25];
int pid;
Slist *head= NULL;
int child_flag = 1;

int insert_at_first(Slist **head, int pid, char *input_string)
{
	Slist *node = malloc(sizeof(Slist));
	if(node == NULL) return FAILURE;
	node->pid = pid; 
    strcpy(node->string, input_string); 
	node->link = *head;
	*head = node;;
	return SUCCESS;
}

void signal_handler(int sig_num)
{
    if(sig_num == SIGINT )
    {
       if(child_flag == 1) // no child just print prompt
       {
         write(1, "\n", 1);
         write(1, prompt, strlen(prompt));
       }
       else write(1, "\n", 1); // child is there, default behaviour will handle it
    }
    if(sig_num == SIGTSTP)
    {
       if(child_flag == 1)  // no process running
       {
         write(1, "\n", 1);
         write(1, prompt, strlen(prompt));
       }
        else if(pid > 0)
       {
            count++;
            write(1, "\n", 1);
            char buffer[50];
            sprintf(buffer, "[%d+] stopped %s", count, input_string);
            write(1, buffer, strlen(buffer));
            write(1, "\n", 1);
            insert_at_first(&head, pid, input_string);
       }
    }
    if(sig_num == SIGCHLD)
    {
        waitpid(-1, &status, WNOHANG);
    }
}

void scan_input(char *prompt, char *input_string)
{
    extract_external_commands(ext_cmds);


    signal(SIGINT, signal_handler); //^C
    signal(SIGTSTP, signal_handler); //^Z

    while(1)
    {
        printf("%s", prompt);
        scanf("%[^\n]", input_string);
        getchar();
        int flag = 0;
        if(strncmp(input_string, "PS1=", 4) == 0)
        {
            int i;
            for(i = 4; input_string[i]!='\0';i++)
            {
                if(input_string[i] == ' ')
                {
                    printf("Error : PS1 command not found\n");
                    flag = 1;
                    break;
                }
            }
            if(flag == 0) strcpy(prompt, input_string+4);
        }
        else
        {
            char *cmd = get_command(input_string);
            int type = check_command_type(cmd);
            if(type == BUILTIN) //bulitin - internal
            {
                execute_internal_commands(input_string);
            }
            else if(type == EXTERNAL)
            {
                pid = fork();
               if(pid == 0)
               {
                    //registering signal for default behaviour
                    signal(SIGINT, SIG_DFL); // ^C
                    signal(SIGTSTP, SIG_DFL); // ^Z
                    execute_external_commands(input_string);
                    exit(0);
               }
               else if(pid > 0)
               {
                    child_flag = 0; 
                    waitpid(pid, &status, WUNTRACED);
                    child_flag = 1; 
               }
            }
            else
            {
                printf("not a command\n");
            }
        }
    }
}