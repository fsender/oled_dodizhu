#ifndef _CTG_Xbitmap
#define _CTG_Xbitmap

// "分" width: 8, height: 8
const unsigned char BM_SCORE[] U8X8_PROGMEM = { 0x10,0x24,0x42,0x81,0x3e,0x48,0x44,0x62 };
// "倍"width: 8, height: 8
const unsigned char BM_MULTP[] U8X8_PROGMEM = { 0x24,0xfa,0x53,0xfa,0x02,0x7a,0x4a,0x7a };
// width: 62, height: 8
const unsigned char BM_PUTOUT[] U8X8_PROGMEM = { 
  0xfe,0xff,0x7f,0xf8,0xff,0xff,0xe1,0x1f,0xb7,0xad,0xfb,0x0c,0x60,0xdb,0xf3,0x3e,
  0xb7,0x8d,0xc0,0xfc,0x7e,0xdb,0xf3,0x3f,0x07,0xec,0xea,0x7c,0x7a,0xc0,0x73,0x3e,
  0xbb,0x8b,0xe0,0xbc,0xb6,0xbb,0xf3,0x3e,0xbb,0x2b,0xc0,0xcc,0xae,0xbb,0xf3,0x3e,
  0x03,0xa8,0xfb,0xfc,0x3e,0x80,0x73,0x3c,0xfe,0xff,0x7f,0xf8,0xff,0xff,0xe1,0x1f };
// "地主正在出牌"width: 16, height: 7
const unsigned char BM_DIZHU[] U8X8_PROGMEM = { 0x8a,0x20,0x1a,0x71,0xbb,0xdb,0x3e,0x89,0xaa,0x8b,0x0a,0x89,0xb9,0xfb };
// "农民正在出牌"width: 16, height: 7
const unsigned char BM_NONGMIN[] U8X8_PROGMEM = { 0xc4,0x23,0x5f,0x72,0xd5,0xdb,0x42,0x89,0xd7,0x8b,0x4a,0x89,0xd6,0xfa };
/*
const unsigned char ddz_logo[] U8X8_PROGMEM = { 
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x07,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x3c,0x00,0x00,0x00,0x00,0x00,0x00,0xfc,0x03,0x00,
  0xe0,0x7f,0x70,0x00,0x00,0x00,0x00,0x00,0xc0,0xff,0x1f,0x00,0xff,0xff,0xe3,0x00,
  0x00,0x00,0x00,0x00,0xfc,0xc1,0xff,0xe1,0xff,0xff,0xcf,0x01,0x00,0x00,0x00,0x80,
  0x03,0x00,0xff,0xff,0xff,0xff,0xdf,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0xfc,0xff,
  0xff,0xff,0xff,0x03,0x00,0x00,0x00,0x00,0x00,0x00,0xe0,0xff,0xff,0xff,0xff,0x03,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xfc,0xff,0x1f,0xf8,0x07,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0xfe,0xff,0x07,0xc0,0x07,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xff,
  0xff,0x01,0x00,0x07,0x00,0x00,0x00,0x00,0x00,0x00,0xc0,0xff,0xff,0x00,0x00,0x06,
  0x00,0xc0,0x07,0x00,0x00,0x00,0xf8,0xff,0x7f,0x00,0x00,0x0c,0x00,0xf8,0x7f,0x00,
  0x00,0xc0,0xff,0xff,0x7f,0x00,0x00,0x08,0x00,0xff,0xff,0xff,0x01,0xfe,0xff,0xff,
  0x3f,0x00,0x00,0x00,0xc0,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x3f,0x00,0x00,0x00,
  0xf0,0x03,0xf8,0xff,0xff,0xff,0xff,0xff,0x1f,0x00,0x00,0x00,0x3c,0x00,0x00,0xff,
  0xff,0xff,0xff,0xff,0x1f,0x00,0x00,0x00,0x03,0x00,0x00,0xf8,0xff,0xff,0xff,0xff,
  0x1f,0x00,0x00,0x00,0x00,0x00,0x00,0xe0,0xff,0xff,0xff,0xff,0x1f,0x00,0x00,0x00,
  0x00,0x00,0x00,0xc0,0xff,0xff,0xff,0xff,0x1f,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0xfe,0xff,0xff,0xff,0x1f,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xf8,0xff,0xff,0xff,
  0x1f,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x1f,0xe0,0xff,0x3f,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x80,0xff,0x3f,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0xff,0x7f,0x00,0x00,0x08,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xfe,
  0xff,0x00,0x00,0x04,0x00,0x00,0x00,0x00,0x00,0x80,0x7f,0xfc,0xff,0x00,0x00,0x06,
  0x00,0x0c,0x00,0x00,0x00,0xf8,0xff,0xff,0xff,0x01,0x80,0x03,0x00,0x70,0x00,0x00,
  0x00,0xff,0xff,0xff,0xff,0x07,0xf0,0x01,0x00,0xc0,0x07,0x00,0xc0,0xff,0xff,0xff,
  0xff,0x3f,0xfc,0x01,0x00,0x00,0x7f,0x00,0xfc,0xff,0xff,0xff,0xff,0xff,0xff,0x00,
  0x00,0x00,0xfc,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x7f,0x00,0x00,0x00,0xf0,0xff,
  0xff,0xff,0xff,0xff,0xff,0xff,0x3f,0x00,0x00,0x00,0xc0,0xff,0xff,0xff,0xff,0xff,
  0xff,0xff,0x1f,0x00,0x00,0x00,0x00,0xff,0xff,0x1f,0xe0,0xff,0xff,0xff,0x0f,0x00,
  0x00,0x00,0x00,0xf0,0x7f,0x00,0x00,0xfc,0xff,0xff,0x03,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0xfc,0x7f,0x00,0x00 };
*/
#endif /** _CTG_Xbitmap **/
