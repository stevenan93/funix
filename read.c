#include<stdio.h>

int main(int argc, char* argv[])
{
    if(argc < 2)
    {
        printf("argc: %d\n", argc);
        perror("Feed me something\n");
        return -1;
    }
    if(argc > 2)
    {
        printf("argc: %d\n", argc);
        perror("ITS TOO MUCH!!!\n");
        return -1;
    }
    printf("WHATS UP MANG %s\n", argv[1]);
    return 0;

}
