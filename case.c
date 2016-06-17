#include <stdio.h>

void test_char(char c)
{
    switch(c)
    {
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
            printf("it\' a digit \n");
            break;
        case ' ':
        case '\n':
        case '\t':
            printf("it\'s a white\n");
            break;
        default:
            printf("it\'s a char\n");
            break;
    }
}

int main(int argc, char const* argv[])
{
    int ch;
    printf("请输入一个字符\n");
    scanf("%c",&ch);
    printf("char ch=%d\n",ch);
    test_char(ch);
    return 0;
}

