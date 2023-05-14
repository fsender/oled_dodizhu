#include "hadChars.h"
/** @def from hadChars.h
#define FONT_before_0x4dff_chars_COUNT 584
//#define FONT_0xFFxx_chars_COUNT 98
ff01-ff5e, ffe0 ffe1 ffe3 ffe5
#define FONT_WIDTHS_IMPL_HADCHARS_H 34 */

uint8_t hasChar(uint16_t unicode){
  if(unicode<0x80) return u8g2_GetGlyphWidth(u8g2.getU8g2(),unicode); //ascii字符
  else if(unicode< 0x4e00) return hasCharBefore4dff(unicode);
  else if(unicode<=0x9fff || (unicode > 0xff00 && unicode < 0xff5f) || unicode == 0xffe0 
  || unicode == 0xffe1 || unicode == 0xffe3 || unicode == 0xffe5) return 12; //中文和全角符号
  else return 0;
}
uint8_t hasCharBefore4dff(uint16_t unicode){ //二分法查询宽度
  int binlow=-1,binhigh=FONT_before_0x4dff_chars_COUNT;
  //if(unicode == pgm_read_word(font_before_0x4dff_chars)) return 12;
  while(binhigh-binlow>1){
    int bincentre = (binlow+binhigh)>>1;
    uint16_t searchRes=pgm_read_word(font_before_0x4dff_chars+bincentre);
    if(unicode==searchRes) return bincentre<FONT_WIDTHS_IMPL_HADCHARS_H?pgm_read_byte(font_widths+bincentre):12;
    else if(unicode<searchRes) binhigh=bincentre;
    else binlow=bincentre;
  }
  return 0;
}
void unicode_to_utf_8(uint16_t unicode_v,char *a){
  if(unicode_v<128){
    a[0]=unicode_v;
    a[1]=0;
  }
  else if(unicode_v<0x800){
    a[0]=192+(unicode_v>>6);
    a[1]=128+(unicode_v&63);
    a[2]=0;
  }
  else if(unicode_v<0x10000){
    a[0]=224+(unicode_v>>12);
    a[1]=128+((unicode_v>>6)&63);
    a[2]=128+(unicode_v&63);
    a[3]=0;
  }
}

uint16_t utf8_to_unicode(const char *a){
  uint16_t al=uint8_t(a[0]);
  if(al<=0x7f) return al;
  else if(al>=0xc0 && al<=0xdf){
    al&=31;
    al<<=6;
    al|=(a[1]&63);
    return al;
  }
  else if(al>=0xe0 && al<=0xef){
    //if(al == 0xe3 && a[1] == 0x80 && a[2] == 0x80) 
    //  return 32; //U+3000 replaced to space char
    al&=15;
    al<<=6;
    al|=(a[1]&63);
    al<<=6;
    al|=(a[2]&63);
    return al;
  }
  return 0xffff;
}
uint16_t getUTF8WidthQuick(const char *s){
  int p=0; //指向字符串的不同位置来迭代计算宽度
  int w=0; //宽度
  while(s[p]){
    uint16_t uni=utf8_to_unicode(s+p); //Unicode值
    int wc=hasChar(uni); //一个utf8字符的宽度
    if(!wc) wc=6;//问号'?'的宽度
    else if(uni == 0x3000) wc=6;//一个空格' '的宽度为6
    w+=wc; //多出来的是一像素空白
    p++;
    if(uni>=0x80) p++;  //双字节utf8
    if(uni>=0x800) p++; //三字节utf8
  }
  return w-1; //去掉最后一像素的空白
}