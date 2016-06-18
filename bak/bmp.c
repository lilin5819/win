#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define DEBUG   1
#define AMD64

#ifdef AMD64
//typedef     unsigned char           u8;
//typedef     unsigned short          u16;
//typedef     unsigned int            u32;
//typedef     unsigned long           u64;
#define     u8      unsigned char
#define     u16     unsigned short
#define     u32     unsigned int
#define     u64     unsigned long
#define     s8      signed char
#define     s16     signed short
#define     s32     signed int
#define     s64     signed long long

#else

#define     u8      unsigned char
#define     u16     unsigned short
#define     u32     unsigned int
#define     u64     unsigned long
#define     s8      signed char
#define     s16     signed short
#define     s32     signed int
#define     s64     signed long long

#endif

#pragma pack(1)         //取消4字节对齐，否则BH占16字节,使用1,2字节对齐都可以
typedef struct tagBMPHEADER{
    u16     type;
    u32     size;
    u32     reserved;
    u32     offBits;
}BH_T,*PBH;

typedef struct tagBMPINFO{
    u32     size;       //INFO结构体大小
    u32     W;
    u32     H;
    u16     planes;     //数据平面数,bmp图总为1
    u16     bitCount;   //像素位数
    u32     compression;
    u32     sizeImage;  //4bytes对齐的图像数据大小
    u32     xPelsPerMeter;
    u32     yPelsPerMeter;
    u32     clrUsed;
    u32     clrImportant;
}BI_T,*PBI;

typedef struct PIXEL {
    u8      b;
    u8      g;
    u8      r;
}PIX_T,*PPIX;

char *pBmpName,*pOpenArgs;
FILE *fp;
BH_T *bh;
BI_T *bi;
PIX_T pix;

FILE *readBmp(char const* bmp,char const* args);

int main(s32 argc, char const* argv[])
{
    if(argc>2)
    {
        pBmpName=argv[1];       //指向第二个参数
        pOpenArgs=argv[2];      //指向第三个参数
    }
    else
    {
        char bmpName[20],openArgs[20];
        printf("请输入测试bmp图\n");
        gets(bmpName);
        printf("请输入阅读参数\n");
        gets(openArgs);
        pBmpName=bmpName;
        pOpenArgs=openArgs;
    }

#if DEBUG
    printf("参数个数:%d\n\n",argc);
    for(s32 i=0;i<argc;i++)
        printf("参数%d:%s\n",i,argv[i]);
    putchar('\n');
    printf("图片名:%s\n读取参数:%s\n",pBmpName,pOpenArgs);
#endif

    if((fp=readBmp(pBmpName,pOpenArgs))==NULL)
    {
        printf("读取信息失败\n");
        exit(1);
    }
    else
        printf("读取信息完成!\n");

    fclose(fp);
    getchar();
    return 0;
}

FILE *readBmp(char const* bmp,char const* args)
{
    //fp=fopen(bmp,args);
    if((fp=fopen(bmp,args))==NULL)
    {
        printf("打开失败\n");
        exit(1);
    }
    else
        printf("打开成功!\n");
#if DEBUG

    printf("BH大小:%d BI大小:%d\n",sizeof(BH_T),sizeof(BI_T));
#endif

    bh=(BH_T *)malloc(sizeof(BH_T));
    bi=(BI_T *)malloc(sizeof(BI_T));

    printf("偏移指针:%p\n",ftell(fp));
    fread(bh,sizeof(*bh),1,fp);
    fread(bi,sizeof(*bi),1,fp);
    u32 lineBytesFull=(bi->W*bi->bitCount+31)/32*4;
    u32 lineBytesReal=(bi->W*bi->bitCount)/8;
    u32 lineBytesPatch=lineBytesFull-lineBytesReal;

    printf("偏移指针:%p\n",ftell(fp));
    printf("文件头:%x 文件大小:%ld 像素偏移地址:%ld\n",bh->type,bh->size,bh->offBits);
    printf("像素宽:%d 像素高:%d 像素位宽:%d 对齐像素数据大小:%d\n",bi->W,bi->H,bi->bitCount,bi->sizeImage);
    printf("对齐后每行字节数:%d 每行真实像素字节:%d 每行额外字节:%d\n",lineBytesFull,lineBytesReal,lineBytesPatch);

    if((bh->type)!=0x4d42)  printf("格式不正确\n"),exit(2);
    if((lineBytesFull*bi->H)!=bi->sizeImage) printf("图片像素字节数不正确"),exit(3);

    printf("偏移指针:%p\n",ftell(fp));
    printf("单像素大小:%d\n",sizeof(PIX_T));

    u8 *pLineBuf=(u8 *)malloc(lineBytesFull);
    u8 *pPixMap=(u8 *)malloc((bi->H)*lineBytesReal);
    printf("单行字节:%d\n",sizeof(*pPixMap));
    memset(pLineBuf,0,lineBytesFull);
    memset(pPixMap,0,bi->H*lineBytesReal);
    printf("总像素字节:%d\n",bi->H*lineBytesReal);
    if(pPixMap && pLineBuf)
    {
        for(u32 i=0;i<bi->H;i++)
        {
            fread(pLineBuf,lineBytesFull,1,fp);
            memcpy(pPixMap+(bi->H-i-1)*lineBytesReal,pLineBuf,lineBytesReal);
            printf("偏移指针:%p\n",ftell(fp));
        }
    }

    printf("全色\n");
    for(u32 i=0;i<bi->H;i++)
    {
        for(u32 j=0;j<bi->W;j++)
        {
        //    printf("pPixMap+%d*lineBytesReal+%d=%x\n",i,j,*(pPixMap+i*lineBytesReal+j));
            for(u32 k=0;k<3;k++)
            {
                //printf("pPixMap+%d*lineBytesReal+3*%d+%d=%p\n",i,j,k,pPixMap+i*lineBytesReal+3*j+k);
                printf("%02x",*(pPixMap+i*lineBytesReal+3*j+k));
            }
        }
        putchar('\n');
    }

    printf("蓝色\n");
    for(u32 i=0;i<bi->H;i++)
    {
        for(u32 j=0;j<bi->W;j++)
        {
            printf("%02x",*(pPixMap+i*lineBytesReal+3*j));
        }
        putchar('\n');
    }

    printf("绿色\n");
    for(u32 i=0;i<bi->H;i++)
    {
        for(u32 j=0;j<bi->W;j++)
        {
            printf("%02x",*(pPixMap+i*lineBytesReal+3*j+1));
        }
        putchar('\n');
    }

    printf("红色\n");
    for(u32 i=0;i<bi->H;i++)
    {
        for(u32 j=0;j<bi->W;j++)
        {
            printf("%02x",*(pPixMap+i*lineBytesReal+3*j+2));
        }
        putchar('\n');
    }

    PIX_T **pPix;
    pPix=(PIX_T **)malloc(sizeof(PIX_T**)*bi->W*bi->H);
    for(u32 i=0;i<bi->H;i++)
    {
        //*(pPix+i)=(PIX_T *)(pPixMap+i*lineBytesReal);
        pPix[i]=(PIX_T *)(pPixMap+i*lineBytesReal);
        //for(u32 j=0;j<bi->W;j++)
        //{
        //    *(pPix+i)+j=(PIX_T **)(pPixMap+i*lineBytesReal);
        //    //pPix[i]=(PIX_T *)(pPixMap+i*lineBytesReal);
        //    //printf("1 pix=%p\n",pix);
        //    //printf("2 pix+i*bi->H+j=%p\n",pix+i*bi->H+j);
        //    //printf("3 *(pix+i*bi->H+j)=%p\n",*(pix+i*bi->H+j));
        //}
    }

    putchar('\n');
    for(u32 i=0;i<bi->H;i++)
    {
        for(u32 j=0;j<bi->W;j++)
        {
            printf("%02x",pPix[i][j].b);
            printf("%02x",pPix[i][j].g);
            printf("%02x",pPix[i][j].r);
        }
        putchar('\n');
    }


    return fp;
}

