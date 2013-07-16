/*
 * =====================================================================================
 *
 *       Filename:  getinfo.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  Tuesday, July 16, 2013 02:41:18 HKT
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

/*获取文件长度*/
long getSize( FILE *fp )
{
    long int save_pos;                          /* 保存当前文件指针 */
    long size_of_file;                          /* 保存文件的大小 */
    save_pos = ftell( fp );                     /* 暂存当前文件指针位置 */
    fseek( fp, 0L, SEEK_END );                  /* 从文件开始位置移动至文件结尾 */
    size_of_file = ftell( fp );                 /* 获取文件指针位置 */
    fseek( fp, save_pos, SEEK_SET );            /* 还原之前的文件指针位置 */
    return( size_of_file );                     /* 返回文件大小数值 */
}

/*获取文件CRC校验*/
//unsigned int calcChecksum(unsigned int *data,unsigned int size)
//{
//    unsigned int i,checksum=0;
//    for (i = 0; i < size/sizeof(unsigned int); i++)
//    {
//    checksum+=*(data+i);
//    }
//    return checksum;
//}

unsigned int getSzbSum(char *file){
    FILE *fp=NULL;
    if ( (fp=fopen(file, "rb")) == NULL ) {
        printf("Get szb checksum, open file %s failure !\n", file);
        exit(1);
    }
    int size,fp_local;
    unsigned int tmp,sum=0;
    fseek(fp, 12, SEEK_SET);
    while ( fp_local != 8192 ) {
        fscanf(fp,"%4c",&tmp);
        sum+=tmp;
        fp_local=ftell(fp);
    }
    fclose(fp);
    return sum;                    

}

/*计算一个文件的CRC校验*/
unsigned int getSum(FILE *fp)
{
    int size;
    unsigned int tmp,sum=0;
    fseek(fp, 0L, SEEK_SET);
    while (!feof(fp)) {
        fscanf(fp,"%4c",&tmp);
        sum+=tmp;
    }
    return sum;                                 /* 这里的检验和的值是正确的 */
}

void get_szb_info(char *file){
    FILE *fp;
    int i,count;
    szb_info info;
    if ( (fp=fopen(file, "rb"))  == NULL ) {
        /* code */
    }
    fscanf(fp,"%8c",&info.magic);
    fscanf(fp,"%4c",&info.checksum);
    fscanf(fp,"%4c",&info.filesize);
    fscanf(fp,"%32c",&info.author);
    fscanf(fp,"%32c",&info.version);
    fscanf(fp,"%4c",&info.timestamp);
    fscanf(fp,"%4c",&info.imagecount);
    fscanf(fp,"%32c",&info.tools);
    fscanf(fp,"%136c",&info.reserve);
    u8 magic[]="LmSzBfMt";
    if ( strstr(info.magic,magic) == NULL ){
        printf ( "This file not in szb format, abort!\n" );
        exit(1);
    }
    struct tm *ptr;
    time_t lt;
    unsigned int times=info.timestamp;
    char str[80];
    lt=times;
    ptr=localtime(&lt);
    strftime(str,100,"%F %X",ptr);
    printf ( "\n" );
    printf ( "Header:\n" );
    printf ( "Checksum:\t0x%08x\n",info.checksum );
    printf ( "Size:\t\t0x%08x\n",info.filesize );
    printf ( "Author:\t\t%s\n",info.author );
    printf ( "Version:\t%s\n",info.version );
    printf ( "Times:\t\t%s\n",str );
    printf ( "Counts:\t\t0x%x\n",info.imagecount );

    count=info.imagecount;
    szb_images_t images[10];
    printf ( "\n" );
    printf ( "Images:\n" );
    for ( i=0; i < info.imagecount ; i++ ){
    fscanf(fp,"%64c",&images[i].filename);
    fscanf(fp,"%32c",&images[i].partname);
    fscanf(fp,"%4c",&images[i].checksum);
    fscanf(fp,"%4c",&images[i].timestamp);
    fscanf(fp,"%4c",&images[i].imageoffset);
    fscanf(fp,"%4c",&images[i].imagesize);
    fscanf(fp,"%4c",&images[i].eraseflag);
    fscanf(fp,"%4c",&images[i].writeflag);
    fscanf(fp,"%136c",&images[i].reserve);
    if ( strlen(images[i].filename) != 0 ){
        printf ( "Position: %-10sOffset: 0x%08x  Checksum: 0x%08x\n",images[i].partname,  images[i].imageoffset, images[i].checksum );}
    else{
        printf ( "Earse:    %-10sCaution!\n",images[i].partname);
    }
    }
    printf ( "\n" );
    fclose(fp);
}

void author_info(){  
    printf ( "\n" );
    printf ( "===============================================================\n" );
    printf ( "          Welcome to use Lenovo K860/K860i szbtool!    \n" );
    printf ( "\n" );
    printf ( "                                    -- version: 0.18       \n" );
    printf ( "                                    -- author:  linkscue   \n" );
    printf ( "===============================================================\n" );
}
void usage(){  
    printf ( "\n" );
    printf ( "---------------------------------------------------------------\n" );
    printf ( "usage:\n" );
    printf ( "szbtool -b uboot      # add uboot.bin\n" );
    printf ( "szbtool -k boot       # add boot.img\n" );
    printf ( "szbtool -r recovery   # add recovery.img\n" );
    printf ( "szbtool -s system     # add system.img\n" );
    printf ( "szbtool -c cpimage    # add cpimage.img\n" );
    printf ( "szbtool -p preload    # add preload.img\n" );
    printf ( "szbtool -d userdata   # add userdata.img\n" );
    printf ( "szbtool -e erase      # erase data & cache space\n" );
    printf ( "szbtool -a author     # specify author\n" );
    printf ( "szbtool -v version    # specify version\n" );
    printf ( "szbtool -i szb        # szb information\n" );
    printf ( "szbtool -x szb        # extract szb \n" );
    printf ( "szbtool -h            # help\n" );
    printf ( "---------------------------------------------------------------\n" );
    printf ( "e.g. szbtool -k boot.img -s system.img -a scue@Link -v Link.szb\n" );
    printf ( "\n" );
}
