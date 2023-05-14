#ifndef _CTG_STACK_H
#define _CTG_STACK_H

#include <SPI.h>
#include <SD.h>
#include <FS.h>
#include <U8g2lib.h>
#include <ESP8266SDUpdater.h>
#include "scroll_string.h"
#include "wifi_funs.h"
/*
#define BtnL 5
#define BtnM 12
#define BtnR 2
extern U8G2_SSD1306_128X64_NONAME_F_4W_HW_SPI u8g2;
#define CTG_OLED_DC 5
#define CTG_OLED_CS 15
#define CTG_OLED_RST U8X8_PIN_NONE
#define CTG_SD_CS 0

#define FONT_cn    _FONT
#define FONT_small u8g2_font_4x6_tr
#define FONT_med   u8g2_font_6x10_tf

*/
#define BtnL D1
#define BtnM D4
#define BtnR D0
extern U8G2_SSD1306_128X64_NONAME_F_4W_HW_SPI u8g2;
#define CTG_OLED_DC D2
#define CTG_OLED_CS D8
#define CTG_OLED_RST U8X8_PIN_NONE
#define CTG_SD_CS D3

#define FONT_cn    _FONT
#define FONT_small u8g2_font_4x6_tr
#define FONT_med   u8g2_font_6x10_tf

uint8_t ctg_init(uint8_t initSD=0);
void ctg_update();
void disptm(size_t cnt,size_t total);

uint8_t getBtn(uint8_t btn);

#endif
