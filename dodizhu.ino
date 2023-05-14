#include <U8g2lib.h>
#include <ESP8266WiFi.h>
#include "ctg_stack.h"
U8G2_SSD1306_128X64_NONAME_F_4W_HW_SPI u8g2(U8G2_R0, /* cs=*/ CTG_OLED_CS, /* dc=*/ CTG_OLED_DC, /* reset=*/ U8X8_PIN_NONE);
#include "Xbitmap.h"
#include "ddz.h"

char ddznote_str[1024]=
  "斗地主说明文档\nCopyright (c) Createen Studios 2021\n版本 Beta v0.1.5\n"
  "新功能: 增加了部分场景的非线性动画，增加了“春天”（3倍）的判定，防止因为丢包造成的游戏无法继续 修复BUG: 叫分界面可能出问题(比如底牌突然出现),丢包检测未响应,出牌会有极小概率失败\n"
  "本项目属于开源项目，任何人不可将此游戏代码用于商业用途\n操作说明：\n"
  "左键、右键为选择方向键，中键为确定键。\n叫分环节和非自己出牌环节可以按住左、右键来查看自己的手牌。\n"
  "玩家身份图标为“地主”或者“农民”，左边的数字代表玩家剩余牌数，右边的五边形图标代表玩家的出牌状态。\n"
  "若为实心则说明此玩家正在出牌，若中间有一横线，则说明最近的一次出牌由此玩家所出。\n"
  "Build date: "
;

void setup() {
  // put your setup code here, to run once:
  strcat(ddznote_str,__TIME__);
  strcat(ddznote_str,__DATE__);
  Serial.begin(115200);
  ctg_init();
  WiFi.mode(WIFI_STA);
  while(connect_to_WiFi(1)==5) yield();
}

void loop() {
  drawList("斗地主OL 局域网版","","    点击进入","");
  dodizhu();

  //test_gui();
  // put your main code here, to run repeatedly:
}
