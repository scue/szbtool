#include    <stdio.h>
#define u8 unsigned char
#define u32 unsigned int
#define u16 unsigned short
#define INFOSIZE 256
#define BUFFER_SIZE 1024

typedef struct {
    u8 magic[8];
    u32 checksum;
}szb_header_t;

typedef struct {
    u32 filesize;
    u8 author[32];
    u8 version[32];
    u32 timestamp;
    u32 imagecount;
    u8 tools[32];
    u8 reserve[INFOSIZE-120];
}szb_head_t;

typedef struct {
    u8 magic[8];
    u32 checksum;
    u32 filesize;
    u8 author[32];
    u8 version[32];
    u32 timestamp;
    u32 imagecount;
    u8 tools[32];
    u8 reserve[INFOSIZE-120];
}szb_info;

typedef struct{
    u8 filename[64];
    u8 partname[32];
    u32 checksum;
    u32 timestamp;
    u32 imageoffset;
    u32 imagesize;
    u32 eraseflag;
    u32 writeflag;
    u8 reserve[INFOSIZE-120];
}szb_images_t;

