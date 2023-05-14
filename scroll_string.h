//-------------- Createen Studios --------------//
//2021.1.3
//A library to make characters alive!
//Remade from U8G2 offical example: Shennong.ino

#ifndef _CTG_SCROLL_STRING
#define _CTG_SCROLL_STRING

#define FULL_BUFFERED

// 允许程序检查 utf-8 的兼容性, 并纠正 utf-8 的编码错误
//这会使得utf-8 的中文部分加载更慢
#define _PROCLINE_SAFE_MODE

/** @brief 允许使用更快的utf-8文本加载
 * 基于查找表实现, 会消耗相当大的flash(32kb左右, 视字体的字符个数,
 * 可能占用不同的空间大小) 和几百字节的静态RAM
 * 用于替代原生的u8g2.getStrWidth()函数, 原生函数基于字体包实现, 有些字符会非常慢 */
#define USE_NEW_PAGEDECODER 1

//Includings:
#include "Arduino.h"
#include "U8g2lib.h"
#include "ctg_stack.h"
#include <FS.h>

//constants
#define BUFLEN 16384
#define LINE_BUFLEN 64 //每一行包含的字符数上限
#define _FONT u8g2_font_wqy12_t_gb2312
#define getchs_max 60
#define SCROLL_DELTA 1
#define SCROLL_DELAY 100
#define SCROLL_BEGIN_LINES 3
#define MOTION_ON

extern U8G2_SSD1306_128X64_NONAME_F_4W_HW_SPI u8g2;

void drawframe(uint8_t x,uint8_t y,uint8_t w,uint8_t h);
uint16_t procline(const char *st,uint8_t *lfeed,int perline);
uint16_t procfile(File *fp,uint8_t *lfeed,int perline);
void scroll_string(const char *c_str0,uint8_t readFile=0,File *fp=NULL);
int16_t getchs (char *tbuff,uint16_t maxlen=LINE_BUFLEN);
void drawKey(const char c,uint8_t x,uint8_t y,uint8_t st=0);
uint8_t switchMode(const uint8_t spin,const uint8_t dtime=25);
uint8_t controlBoard(const char * txt[]);
int16_t slider(const char *title,int16_t maxval=100,int16_t minval=0);

uint8_t listdraw(const char *title,const char *items[],uint8_t numitem,uint8_t dure=0);
void drawList(const char*s1,const char*s2,const char*s3,const char*s4,uint32_t delayms=0,uint8_t hmotion=1);
void drawList(const __FlashStringHelper*s1,const __FlashStringHelper*s2,const __FlashStringHelper*s3,const __FlashStringHelper*s4,uint32_t delayms=0,uint8_t hmotion=1);

void drawLists(const char*s1,const char*s2,const char*s3,const char*s4,
const uint8_t way,const uint8_t cn,const uint8_t al,uint8_t delta=0);


#endif
