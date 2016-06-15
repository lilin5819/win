#include <stdio.h>
#include <string.h>

int main(int argc, char const* argv[])
{
    FILE *f;
    f=fopen("/home/16573/txt","w");
    char name[20]="yanglilin";
    //for(char i=1;i<20;i++)
        fwrite(name,20,1,f);

    printf("当前指针:%p\n", ftell(f));
    fclose(f);
    return 0;
}
