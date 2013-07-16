#include	"stdlib.h"
#include	"stdio.h"
#include	"string.h"
#include	"unistd.h"
#include	"fcntl.h"
#include    "time.h"
#include    "szb.h"

//分解文件函数；
void splitFile(char *file){

    FILE *fd = NULL;
    FILE *ft = NULL;
    int i=0,count=0;
    get_szb_info(file);                         /* 显示szb文件信息 */
    if ( (fd=fopen(file,"rb")) == NULL ) {      /* 打开文件进行操作 */
        printf ( "Extract szb file, open %s failure!\n", file );
        exit(1);
    }
    fseek( fd, 0, SEEK_SET );                   /* 重新定向文件指针位置 */
    u32 imagecount = 0;
    fseek( fd, 84, SEEK_SET );                  /* 略过前边的84字节 */
    fscanf( fd,"%4c", &imagecount );            /* 获取镜像文件个数 */
    fseek( fd, 168, SEEK_CUR );                 /* 总共略过前边的256字节的Header信息 */
    szb_images_t images[10];
    memset(images,0x00,sizeof(images));
    for ( i=0; i < imagecount ; i++ ){          /* 把镜像信息写入一个数组； */
    fscanf(fd,"%64c",&images[i].filename);      /* 获取得文件名称 */
    fscanf(fd,"%32c",&images[i].partname);
    fscanf(fd,"%4c",&images[i].checksum);
    fscanf(fd,"%4c",&images[i].timestamp);
    fscanf(fd,"%4c",&images[i].imageoffset);    /* 获取偏移位置 */
    fscanf(fd,"%4c",&images[i].imagesize);      /* 获取镜像文件的大小 */
    fscanf(fd,"%4c",&images[i].eraseflag);
    fscanf(fd,"%4c",&images[i].writeflag);
    fscanf(fd,"%136c",&images[i].reserve);
    }
    //开始解压数据；
    int size,n,offset,fp_local,end;
    unsigned char imagename[32]="";
    unsigned char buffer[4];              /* 创建缓冲区 */
    strncpy(buffer,"",sizeof(buffer));              /* 清空缓冲区内容 */
    for( i=0; i < imagecount ; i++ ){
        strncpy(imagename, images[i].filename, sizeof(imagename));
        offset=images[i].imageoffset;
        size=images[i].imagesize;
        end=(offset+size);
        if ( size != 0 ) {
            if ( ( ft=fopen(imagename,"wb") ) == NULL ){
                printf("Extract szb file, open %s failure!\n",imagename);
            }
            fseek( fd, offset, SEEK_SET);                /* 跳转至数据段 */
            printf("Extract %s..\n",imagename);
            n=0;count=0;
            while ( count < size )  {
                n  = fread(buffer,1, sizeof(buffer), fd); /* 一次只读取一个1字节，循环读取sizeof(buffer)次 */
                fwrite(buffer, n, 1, ft);
                count+=n;
                fp_local=ftell(fd);
            }
        }
    }
    fclose(fd);
    printf("Extract szb file done!\n");
}

////在尾部追加二进制文件(for Windows)
//void appendFile(char *fp, char *body) { 
//    int n=0;
//    FILE *in,*out;
//    int size=0;
//    if ( (in = fopen(fp, "rb")) == NULL){
//        printf ( "Open in file failure!\n" );
//        exit(1);
//    }
//    size=getSize(in);
//    u8 buffer[size];
//    sleep(0.2);
//    if ( (out=fopen(body ,"ab")) == NULL ){
//        printf ( "Open out file failure!\n" );
//        exit(1);
//    }
//    while (!feof(in)) {
//        n = fread(buffer, 1, size, in);
//        fwrite(buffer, 1, n, out);
//    }
//    fclose(in);
//    fclose(out);
//}

//在尾部追加二进制文件
void appendFile(char *fp, char *body) { 
    int n=0;
    FILE *in,*out;
    u8 buffer[BUFFER_SIZE];
    if ( (in = fopen(fp, "rb")) == NULL){
        printf ( "Open in file failure!\n" );
        exit(1);
    }
    if ( (out=fopen( body , "ab")) == NULL ){
        printf ( "Open out file failure!\n" );
        exit(1);
    }
    while (!feof(in)) {
        n = fread(buffer, 1, BUFFER_SIZE, in);
        fwrite(buffer, 1, n, out);
    }
    fclose(in);
    fclose(out);
}

//u32 repartImage(char *file, char *filename,char *partname,u32 offset)
u32 appendImage(char *szb, char *file, char *filename, char *partname, u32 offset, int index){
    FILE *fp,*fb;
    if((fp=fopen(file,"rb")) == NULL){
        printf ( "Open %s failure!\n",filename );
        exit(1);
    }
    szb_images_t image;
    strncpy(image.filename, filename, sizeof(image.filename));
    strncpy(image.partname, partname, sizeof(image.partname));
    strncpy(image.reserve, "", sizeof(image.reserve));
    time(&image.timestamp);
    image.imageoffset=offset;
    image.imagesize=getSize(fp);
    image.checksum=getSum(fp);
    image.eraseflag=1;
    image.writeflag=1;
    fclose(fp);
//    u8 buffer[INFOSIZE];
//    strncpy(buffer, &image, sizeof(buffer));
    printf("Adding: %-12sOffset: 0x%08x Checksum: 0x%08x\n",image.partname, image.imageoffset, image.checksum);
    if ( (fb=fopen(szb, "rb+")) == NULL) {
        printf(" append %s to szb file, open szb file failure!\n", file);
        exit(1);
    }
    fseek( fb, ( index * INFOSIZE ), SEEK_SET );     /* 移动文件指针的位置 */
//    fscanf(fd,"%64c",&images[i].filename);      /* 获取得文件名称 */
//    fscanf(fd,"%32c",&images[i].partname);
//    fscanf(fd,"%4c",&images[i].checksum);
//    fscanf(fd,"%4c",&images[i].timestamp);
//    fscanf(fd,"%4c",&images[i].imageoffset);    /* 获取偏移位置 */
//    fscanf(fd,"%4c",&images[i].imagesize);      /* 获取镜像文件的大小 */
//    fscanf(fd,"%4c",&images[i].eraseflag);
//    fscanf(fd,"%4c",&images[i].writeflag);
//    fscanf(fd,"%136c",&images[i].reserve);
    fwrite(&image.filename, sizeof(image.filename), 1, fb);           /* 向szb文件写入Image的相关信息 */
    fwrite(&image.partname, sizeof(image.partname), 1, fb);           /* 向szb文件写入Image的相关信息 */
    fwrite(&image.checksum, sizeof(image.checksum), 1, fb);           /* 向szb文件写入Image的相关信息 */
    fwrite(&image.timestamp, sizeof(image.timestamp), 1, fb);           /* 向szb文件写入Image的相关信息 */
    fwrite(&image.imageoffset, sizeof(image.imageoffset), 1, fb);           /* 向szb文件写入Image的相关信息 */
    fwrite(&image.imagesize, sizeof(image.imagesize), 1, fb);           /* 向szb文件写入Image的相关信息 */
    fwrite(&image.eraseflag, sizeof(image.eraseflag), 1, fb);           /* 向szb文件写入Image的相关信息 */
    fwrite(&image.writeflag, sizeof(image.writeflag), 1, fb);           /* 向szb文件写入Image的相关信息 */
    fwrite(&image.reserve, sizeof(image.reserve), 1, fb);           /* 向szb文件写入Image的相关信息 */
    fclose(fb);                                   /* 向szb文件写完Image信息后关闭 */
//    printf("appending %s to %s\n",file, szb);
    appendFile(file, szb);                        /* 向szb文件的尾部添加image数据 */
    return (image.imagesize+image.imageoffset);   /* 返回一个偏移数据，方便之后的判断 */
}



//擦除分区
//void erasePart(char *file, u8 *part){
//    FILE *fd;
//    int size=192;
//    appendZeros(file,64);
//    unsigned int buffer[size];
//    strncpy(buffer,part,sizeof(buffer));
//    if ( ( fd=fopen(file,"a+") ) == NULL ){
//        printf ( "Add erase space feature, can't open target file!\n" );
//        exit(1);
//    }
//
//    if ( size != 0 ){
//        fwrite(buffer, 1 , size , fd);
//    }
//    fclose(fd);
//}

//在尾部补零
//void appendZeros(char *file, int size){
//    FILE *fd;
//    unsigned int buffer[size];
//    strncpy(buffer,"",sizeof(buffer));
//    if ( ( fd=fopen(file,"ab+") ) == NULL ){
//        printf ( "Append zeros failure, can't open target file!\n" );
//        exit(1);
//    }
//
//    if ( size != 0 ){
//        fwrite(buffer, 1 , size , fd);
////        printf ( "补零数目：\t%d\n", size);
//    }
//    fclose(fd);
//}



////复制文件copyFile (FILE *form, FILE *to)
//void copyFile(char *from, char *to){
//    FILE * outfile, *infile;
//    infile = fopen(from, "rb");
//    outfile = fopen(to, "wb" );
//    unsigned char buf[BUFFER_SIZE];
//    if( outfile == NULL || infile == NULL )
//    {
//        printf("Copy file failure!");
//        exit(1);
//    }   
//    int rc;
//    while( (rc = fread(buf,sizeof(unsigned char), BUFFER_SIZE ,infile)) != 0 )
//    {
//        fwrite( buf, sizeof( unsigned char ), rc, outfile );
//    } 
////    sleep(0.1);
//    fclose(infile);
//    fclose(outfile);
//}
//
//合并文件mergeFile (infile1, infile2, filenmae)
//void mergeFile(char *fp1,char *fp2,char *name){
//    FILE *fd1,*fd2,*fp3;
//    unsigned char buf[BUFFER_SIZE];
//    char filename[100];
//    strncpy(filename,name,sizeof(filename));
//    int rc1,rc2;
//    fd1 = fopen(fp1,"rb");
//    fd2 = fopen(fp2,"rb");
//    fp3 = fopen(filename, "wb" );
//    while( (rc1 = fread(buf,sizeof(unsigned char), BUFFER_SIZE,fd1)) != 0 )
//    {
//        fwrite( buf, sizeof( unsigned char ), rc1, fp3 );
//    } 
//    while( (rc2 = fread(buf,sizeof(unsigned char), BUFFER_SIZE,fd2)) != 0 )
//    {
//        fwrite( buf, sizeof( unsigned char ), rc2, fp3 );
//    } 
//    sleep(0.1);
//    fclose(fd1);
//    fclose(fd2);
//    fclose(fp3);
//}
