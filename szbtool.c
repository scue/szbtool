/*
 * =====================================================================================
 *
 *       Filename:  calSum.c
 *
 *    Description:  联想手机szb格式验证码计算；
 *
 *        Version:  1.0
 *        Created:  2013年03月25日 01时46分16秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  linkscue (scue), 
 *   Organization:  
 *
 * =====================================================================================
 */
#include	"stdlib.h"
#include	"stdio.h"
#include	"string.h"
#include	"unistd.h"
#include	"fcntl.h"
#include    "time.h"

#include    "szb.h"
#include    "libfile.h"
#include    "libget.h"

extern char *optarg;    //选项的参数指针
extern int optind;      //下一次调用getopt的时，从optind存储的位置处重新开始检查选项;
extern int opterr;      //当opterr=0时，getopt不向stderr输出错误信息;
extern int optopt;      //当命令行选项字符不包括在optstring中或者选项缺少必要的参数时，该选项存储在optopt中，getopt返回?


/*主函数*/
int main ( int argc, char *argv[] )
{
    FILE *fp,*fp1,*fp2,*fd1,*fd2,*sub_image;
    char *file=NULL;
    u8 magic[8];
    u8 imagename[64];
    u8 partname[32];
    u8 version[32];
    u8 author[32];
    u32 filesize;
    u32 offset=8192;                            /* 初始的偏移量 */
    char opt;                                   /* 获取输入的选项 */
    int index=0;                                /* 判断szb中的分区索引数量 */
    int DO_NOTHING=0;                           /* 依据此变量判断是否执行 */
    int ERASE_DATA=0;                           /* 依据此变量判断是否擦除数据 */
    
    //清空变量
    memset(version,0x00,sizeof(version));
    memset(imagename,0x00,sizeof(imagename));
    memset(partname,0x00,sizeof(partname));
    strncpy(author,"scue@Link",sizeof(author));

    //显示作者信息
    author_info();

    //检测输入参数
    if(argc == 1)
    {
        usage();                                /* 显示使用手册 */
        exit(0);
    }

    //判断选项中是否有版本
    int i;
    char tmp1[32]="";
    char tmp2[32]="";
    strncpy(tmp2,"-v",sizeof(tmp2));
    for (i = 0; i < argc; i++) {
        strncpy(tmp1,argv[i],sizeof(tmp1));
        if ( strcmp(tmp1,tmp2) == 0 ) {
            strncpy(version,argv[i+1],sizeof(version));
        }
        if ( strcmp(tmp1,"-x") == 0 || strcmp(tmp1,"-i")==0 ) { 
            DO_NOTHING = 1;
        }
    }
    if ( strlen(version) == 0 && DO_NOTHING !=1 ){
        printf ( "\n" );
        printf ( "Please use -v to specify a version name!\n" );
        usage();
        exit(1);
    }
//    printf("version: %s \n",version);

    if (DO_NOTHING != 1) {                      /* 当发现有事可做的时候... */
        //创建文件
        remove(version);                            /* delete the old szb file */
        if ( (fp=fopen(version,"wb+")) == NULL ) {
            printf ( "Create version file %s failure!\n",version);
            exit(1);
        }   
        u8 header_buffer[INFOSIZE*32];
        strncpy(header_buffer, "LmSzBfMt", sizeof(header_buffer));
        fwrite(header_buffer,  sizeof(header_buffer) , 1, fp); /* 向其中写8192字节后退出 */
        index++;
        fclose(fp);
    }

    //开始获取选项信息
    opterr=0;                                   /* 不显示错误的选项信息 */
    while ((opt = getopt(argc, argv, "b:k:r:s:c:p:d:x:i:v:a:e")) != -1)
      switch (opt) {
      case 'v':
            strncpy(version,optarg,sizeof(version));
            break;
      case 'b':
            file=optarg;
            strncpy(partname,"bootloader",sizeof(partname));
            strncpy(imagename,"uboot.bin",sizeof(imagename));
            offset=appendImage(version, file, imagename, partname, offset, index);
            index++;
            break;
      case 'k':
            file=optarg;
            strncpy(partname,"boot",sizeof(partname));
            strncpy(imagename,"boot.img",sizeof(imagename));
            offset=appendImage(version, file, imagename, partname, offset, index);
            index++;
            break;
      case 'r':
            file=optarg;
            strncpy(partname,"recovery",sizeof(partname));
            strncpy(imagename,"recovery.img",sizeof(imagename));
            offset=appendImage(version, file, imagename, partname, offset, index);
            index++;
            break;
      case 's':
            file=optarg;
            strncpy(partname,"system",sizeof(partname));
            strncpy(imagename,"system.img",sizeof(imagename));
            offset=appendImage(version, file, imagename, partname, offset, index);
            index++;
            break;
      case 'c':
            file=optarg;
            strncpy(partname,"cpimage",sizeof(partname));
            strncpy(imagename,"cpimage.img",sizeof(imagename));
            offset=appendImage(version, file, imagename, partname, offset, index);
            index++;
            break;
      case 'p':
            file=optarg;
            strncpy(partname,"preload",sizeof(partname));
            strncpy(imagename,"preload.img",sizeof(imagename));
            offset=appendImage(version, file, imagename, partname, offset, index);
            index++;
            break;
      case 'd':
            file=optarg;
            strncpy(partname,"userdata",sizeof(partname));
            strncpy(imagename,"userdata.img",sizeof(imagename));
            offset=appendImage(version, file, imagename, partname, offset, index);
            index++;
            break;
      case 'e':
            ERASE_DATA=1;
            break;
      case 'a':
            strncpy(author,optarg,sizeof(author));
            break;
      case 'x':
            DO_NOTHING=1;
            file=optarg; 
            splitFile(file);
            break;
      case 'i':
            DO_NOTHING=1;
            file=optarg;
            get_szb_info(file);
            break;
      default:
            DO_NOTHING=1;
            usage();
      }
    argv += optind; 


    //开始执行制作szb文件
    if ( DO_NOTHING != 1 ){
    //检查是否输入格式名    
    if ( strlen(version) == 0 ){
        DO_NOTHING = 1;
        printf ( "\n" );
        printf ( "Please use -v to specify a version name!\n" );
        usage();
        exit(1);
    }
    // 双清功能
    if ( ERASE_DATA == 1) {                     /* 判断是否有“双清功能” */
        u8 erase_data[32];
        u8 erase_cache[32];
        strncpy(erase_data, "userdata", sizeof(erase_data));
        strncpy(erase_cache, "cache", sizeof(erase_data));
        if ( (fp=fopen(version, "rb+")) == NULL ) {
            printf("Add erase features, open file %s failure!\n", version);
            exit(1);
        }
        fseek( fp, (INFOSIZE * index + 64 ), SEEK_SET ); /* erase userdata */
        fwrite(erase_data, sizeof(erase_data), 1, fp);
        index++;
        fseek( fp, (INFOSIZE * index + 64 ), SEEK_SET ); /* erase cache */
        fwrite(erase_cache, sizeof(erase_cache), 1, fp);
        index++;
        fclose(fp);

    }

    //制作SZB格式文件头部(256-12)部分
    szb_head_t head;
    if ( (fp=fopen(version,"rb+")) ==NULL ){
        printf("Calculate file size, open file %s failure!\n", version);
        exit(1);
    }
    head.filesize=getSize(fp);                          /* get szb file size */
    strncpy(head.author,author,sizeof(head.author));    /* 写入作者信息 */
    strncpy(head.version,version,sizeof(head.version)); /* 写入版本信息 */
    time(&head.timestamp);                              /* 写入时间信息 */
    head.imagecount=(index-1);                              /* 写入镜像数目 */
    strncpy(head.tools,"",sizeof(head.tools));          /* 工具信息为空 */
    strncpy(head.reserve,"",sizeof(head.reserve));      /* 尾部空闲部分 */
    fseek( fp, 12, SEEK_SET );
    fwrite(&head,sizeof(szb_head_t),1,fp);              /* 写入256-12字节部分 */
    fclose(fp);

    u32 szb_checksum;
    szb_checksum=getSzbSum(version);
    if ( (fp=fopen(version,"rb+")) ==NULL ){
        printf("Calculate file size, open file %s failure!\n", version);
        exit(1);
    }
    fseek(fp, 8, SEEK_SET);
    fwrite(&szb_checksum,sizeof(szb_checksum),1,fp);              /* 写入总的验证码字节部分 */
    fclose(fp);
    get_szb_info(version);
    printf ( ">> Congratulations! The Official ROM %s done!\n",version);
    printf ( "\n" );
    }
    return 0;
}
