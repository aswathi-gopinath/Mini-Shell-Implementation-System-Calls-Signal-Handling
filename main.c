#include"header.h"

char prompt[25] = "minishell$: ";
char input_string[25];
char path[100];

int main()
{
    getcwd(path, 100);
    system("clear");
    scan_input(prompt, input_string);
}