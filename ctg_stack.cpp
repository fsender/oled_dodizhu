#include "ctg_stack.h"
uint8_t ctg_init(uint8_t initSD){
	pinMode(BtnL,INPUT_PULLUP);
	pinMode(BtnM,INPUT_PULLUP);
	//pinMode(BtnR,INPUT_PULLUP);
	// 当BtnR为D8时就必须加上下面语句
	//if(BtnR == D8) analogWrite(BtnR,1023);
#if (BtnR == D0)
  pinMode(BtnR,INPUT_PULLDOWN_16);
#else
  pinMode(BtnR,INPUT_PULLUP);
#endif
  u8g2.begin();
  u8g2.enableUTF8Print();
  u8g2.sendBuffer();
  u8g2.clearBuffer();
  u8g2.setFont(FONT_cn);
  SPIFFS.begin();
  if(initSD){
    while(!SD.begin(CTG_SD_CS)){
	  drawList("","    未检测到SD卡","   插入后自动重试","按右键退出",500,0);
	  if(getBtn(BtnR)==0) return 1;
    }
  }
  return 0;
}
void disptm(size_t cnt,size_t total){
  static size_t lastp=0;
  if(cnt*10/total>lastp){
    u8g2.clearBuffer();
	u8g2.drawBox(0,0,128,16);
	u8g2.setColorIndex(0);
	u8g2.drawStr(48,13,"Loading...");
	u8g2.setColorIndex(1);
	u8g2.drawVLine(0,16,47);
	u8g2.drawVLine(127,16,47);
	u8g2.drawHLine(0,63,128);
	u8g2.drawStr(32,32,"Status: ");
    u8g2.setCursor(54,46);
    u8g2.print(lastp=cnt*10/total);
    u8g2.print("0%");
    u8g2.setCursor(2,60);
    u8g2.print(cnt);
    u8g2.print('/');
    u8g2.print(total);
    u8g2.print(" Bytes");
    u8g2.sendBuffer();
  }
}
void ctg_update(){
  while(!SD.begin(CTG_SD_CS)){
	drawList("","    未检测到SD卡","   插入后自动重试","按右键退出",500,0);
	if(getBtn(BtnR)==0) return;
  }
  SDUpdater ud;
  u8g2.setFont(FONT_med);
  Update.onProgress(disptm);
  disptm(0,1);
	ud.updateFromSD();
    ESP.restart();
}

uint8_t getBtn(uint8_t btn){
  //old style
  //return (btn==16)?(!digitalRead(btn)):(digitalRead(btn));
  
#if ((BtnR == D0) || (BtnL == D0) || (BtnM == D0))
  if(btn == 16) {
    pinMode(btn,INPUT_PULLDOWN_16);
    return (!digitalRead(btn));
  }
#endif
  pinMode(btn,INPUT_PULLUP);
  uint8_t readb = digitalRead(btn);
  if(btn == D6) pinMode(D6,SPECIAL); //针对MISO引脚进行专门的优化
  else {
    pinMode(btn,OUTPUT);
    digitalWrite(btn,HIGH);    //这些引脚的默认电平都是高电平
  }
  return readb;
}