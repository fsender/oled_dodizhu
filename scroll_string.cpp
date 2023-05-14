//-------------- Createen Studios --------------//
//2021.1.3
//A library to make characters alive!
//Remade from U8G2 offical example: Shennong.ino

//Includings:
#include "scroll_string.h"

#if USE_NEW_PAGEDECODER
#include "hadChars.h"
#endif

//constants
  const char c[4][3][11]={
    {"qwertyuiop",
    "asdfghjkl\xab",
    "\xb6zxcvbnm \xbb"},
    {"QWERTYUIOP",
    "ASDFGHJKL\xab",
    "\xb6ZXCVBNM \xbb"},
    {"1234567890",
    ",./;\'[]\\`\xab",
    "\xb6-=      \xbb"},
    {"!@#$%^&*()",
    "<>?:\"{}|~\xab",
    "\xb6_+      \xbb"}
  };
static char testwidth[LINE_BUFLEN]={0};

void drawframe(uint8_t x,uint8_t y,uint8_t w,uint8_t h){
    u8g2.drawHLine(x+2,y,w-4);
    u8g2.drawHLine(x+2,y+h-1,w-4);
    u8g2.drawVLine(x,y+2,h-4);
    u8g2.drawVLine(x+w-1,y+2,h-4);
    u8g2.drawPixel(x+1,y+1);
    u8g2.drawPixel(x+w-2,y+1);
    u8g2.drawPixel(x+1,y+h-2);
    u8g2.drawPixel(x+w-2,y+h-2);
}

uint16_t procline(const char *st,uint8_t *lfeed,int perline){
  u8g2.clearBuffer();
  
  u8g2.setFont(_FONT);
  u8g2.drawUTF8(30,37, "正在加载文本...");
  u8g2.sendBuffer();
  uint16_t chs=0, proc=0, rtns=0, wid=0, overco=0;
  testwidth[0]=0;
  //proc 处理的字符数
  //rtns 处理的行数
  //chs 当前行总字符
  //wid 当前行总宽度数
  //  abcdefgh
  while(st[proc]){
    testwidth[chs]=st[proc];
    testwidth[chs+1]=0;
    char i_proc=st[proc];
    if(i_proc<128 || i_proc>=192){
      if(i_proc>=192){
        chs++;proc++;
    testwidth[chs]=st[proc];
#ifdef _PROCLINE_SAFE_MODE
    if(testwidth[chs]>=192)      testwidth[chs] &= 191; // BIN 1011 1111
    else if(testwidth[chs]<=127) testwidth[chs] |= 64 ; // BIN 0100 0000
#endif
    //testwidth[chs+1]=0;
    overco=1;
        if(i_proc>=224){
          chs++;proc++;
    testwidth[chs]=st[proc];
#ifdef _PROCLINE_SAFE_MODE
    if(testwidth[chs]>=192)      testwidth[chs] &= 191; // BIN 1011 1111
    else if(testwidth[chs]<=127) testwidth[chs] |= 64 ; // BIN 0100 0000
#endif
    //testwidth[chs+1]=0;
    overco=2;
          if(i_proc>=240){
            chs++;proc++;
    testwidth[chs]=st[proc];
#ifdef _PROCLINE_SAFE_MODE
    if(testwidth[chs]>=192)      testwidth[chs] &= 191; // BIN 1011 1111
    else if(testwidth[chs]<=127) testwidth[chs] |= 64 ; // BIN 0100 0000
#endif
    //testwidth[chs+1]=0;
    overco=3;
          }
        }
      }
      else overco=0;
      testwidth[chs+1]=0;
  #if USE_NEW_PAGEDECODER
      wid=getUTF8WidthQuick(testwidth);
  #else
      wid=u8g2.getUTF8Width(testwidth);
  #endif
      if(wid>perline || i_proc=='\n'){
        if(i_proc=='\n') {
          proc++; chs++;
        }
        while(overco){
          proc--; chs--;
          overco--;
        }
        lfeed[rtns]=chs;
        if(rtns >= BUFLEN - 2) break;
        else rtns++;
        chs=0;
        continue;
      }
      else{
        chs++;
      }
    }
    proc++;
  }
  lfeed[rtns]=chs;
  lfeed[rtns+1]=0;
  return rtns+1;
}
uint16_t procfile(File *fp,uint8_t *lfeed,int perline){
  u8g2.clearBuffer();
  
  u8g2.setFont(_FONT);
  u8g2.drawUTF8(0,32, "正在加载文件...");
  u8g2.drawUTF8(0,46, fp->name());
  u8g2.sendBuffer();
  uint16_t chs=0, proc=0, rtns=0, wid=0, overco=0;
  uint8_t  proced=0;
  testwidth[0]=0;
  //proc 处理的字符数
  //rtns 处理的行数
  //chs 当前行总字符
  //wid 当前行总宽度数
  //  abcdefgh
  while(fp->position()<fp->size()){
    testwidth[chs]=fp->peek();
    testwidth[chs+1]=0;
    char i_proc=testwidth[chs];
    if(i_proc<128 || i_proc>=192){
      if(i_proc>=192){
        chs++;proc++;
        fp->seek(1,SeekCur);
    testwidth[chs]=fp->peek();
#ifdef _PROCLINE_SAFE_MODE
    if(testwidth[chs]>=192)      testwidth[chs] &= 191; // BIN 1011 1111
    else if(testwidth[chs]<=127) testwidth[chs] |= 64 ; // BIN 0100 0000
#endif
    //testwidth[chs+1]=0;
    overco=1;
        if(i_proc>=224){
          chs++;proc++;
          fp->seek(1,SeekCur);
    testwidth[chs]=fp->peek();
#ifdef _PROCLINE_SAFE_MODE
    if(testwidth[chs]>=192)      testwidth[chs] &= 191; // BIN 1011 1111
    else if(testwidth[chs]<=127) testwidth[chs] |= 64 ; // BIN 0100 0000
#endif
    //testwidth[chs+1]=0;
    overco=2;
          if(i_proc>=240){
            chs++;proc++;
            fp->seek(1,SeekCur);
    testwidth[chs]=fp->peek();
#ifdef _PROCLINE_SAFE_MODE
    if(testwidth[chs]>=192)      testwidth[chs] &= 191; // BIN 1011 1111
    else if(testwidth[chs]<=127) testwidth[chs] |= 64 ; // BIN 0100 0000
#endif
    //testwidth[chs+1]=0;
    overco=3;
          }
        }
      }
      else overco=0;
      testwidth[chs+1]=0;
  #if USE_NEW_PAGEDECODER
      wid=getUTF8WidthQuick(testwidth);
  #else
      wid=u8g2.getUTF8Width(testwidth);
  #endif
      if(wid>perline || i_proc=='\n'){
        if(i_proc=='\n') {
          proc++; chs++;
          fp->seek(1,SeekCur);
        }
        while(overco){
          proc--; chs--;
          overco--;
          fp->seek(-1,SeekCur);
        }
        lfeed[rtns]=chs;
        if(rtns >= BUFLEN - 2) break;
        else rtns++;
        chs=0;
        continue;
      }
      else{
        chs++;
      }
    }
    /* else if(chs == 0){
      int16_t fpread = fp->peek();
      while(fpread>127 && fpread<192) fp->seek(-1,SeekCur);
    }*/
    proc++;
    fp->seek(1,SeekCur);
    yield();
    if(fp->position()*100/fp->size()>proced){
      proced=fp->position()*100/fp->size();
      u8g2.setCursor(58,63);
      u8g2.print(proced);
      u8g2.print('%');
      u8g2.sendBuffer();
    }
  }
  lfeed[rtns]=chs;
  lfeed[rtns+1]=0;
  return rtns+1;
}

void scroll_string(const char *c_str0,uint8_t readFile,File *fp) {
 static uint8_t c_feed[BUFLEN]={0};
 uint8_t autoScroll=1;
 char buf[LINE_BUFLEN];     // there are at most 8 chinese glyphs per line, max buf size is 8*3 = 24 
 uint32_t c_processed=0;
 uint16_t total_lines;   // the total number of lines in the story
 uint16_t i;       // loop variable for the lines
 uint16_t line_cnt;    // number of lines to draw, usually equal to lines_per_draw
 uint16_t start_line;    // topmost visible line, derived from top_window_pos
 uint16_t lines_per_draw;  // how many lines to draw on the screen, derived from font and display height
 uint16_t glyph_height;  // height of the glyphs

 uint32_t top_window_pos=0;    // defines the display position in pixel within the text
// uint16_t total_height;      // total height in pixel, derived from font height and total_lines
 uint32_t long_press=0;   //长按识别
 uint16_t top_offset;     // offset between the first visible line and the display
 String historyPath = "";
 u8g2.setFont(_FONT);
  if(readFile){
    historyPath = fp->fullName();
    int fl=historyPath.lastIndexOf('.');
    String fnext = historyPath.substring(fl+1);
    if(readFile>1 &&(fnext == "txt" || fnext == "TXT")){
      historyPath += "cfg"; //  *.txtcfg
      if(SD.exists(historyPath)){
        File history = SD.open(historyPath, "r");
        total_lines = history.read(); //把读取记录写入到文件
        total_lines |= (history.read()<<8);
        history.read(c_feed,total_lines);
        top_window_pos =  history.read();
        top_window_pos |= (history.read()<<8);
        top_window_pos |= (history.read()<<16);
        uint8_t rd_in_buff = history.read(); //临时变量
        if(rd_in_buff & 128) rd_in_buff &= 127;
        else autoScroll = 0;
        top_window_pos |= (rd_in_buff<<24);
        history.close();
        total_lines += SCROLL_BEGIN_LINES;
      }
      else{
        total_lines = procfile(fp,c_feed,u8g2.getDisplayWidth());
        File history = SD.open(historyPath, "w");
        history.write(total_lines & 255); //把读取记录写入到文件
        history.write((total_lines>>8) & 255);
        history.write(c_feed,total_lines);
        history.write(0); //写入当前位置, 数据值: top_window_pos, 32bit, LSB
        history.write(0);
        history.write(0);
        history.write(128);
        history.close();
        total_lines+=SCROLL_BEGIN_LINES;
      }
    }
    else total_lines = procfile(fp,c_feed,u8g2.getDisplayWidth())+SCROLL_BEGIN_LINES;
  }
  else total_lines = procline(c_str0,c_feed,u8g2.getDisplayWidth())+SCROLL_BEGIN_LINES;
  /* calculate the length of the text in lines */
 //total_lines = u8x8_GetStringLineCnt(c_str);
  
  /* get the height of the glyphs */
 glyph_height = u8g2.getMaxCharHeight()-1;
  /* calculate the height of the text in pixel */
// total_height = (uint16_t)total_lines * (uint16_t)glyph_height;
  

  /* calculate how many lines must be drawn on the screen */
 lines_per_draw = u8g2.getDisplayHeight() / glyph_height;
 lines_per_draw += 2;

  /* start at the top of the text */
  //top_window_pos = 0;
 
 while(1){
  start_line = top_window_pos / glyph_height;
  top_offset = top_window_pos % glyph_height;
  
  line_cnt = total_lines - start_line;
  if ( line_cnt > lines_per_draw )
    line_cnt = lines_per_draw;
/*
    for(i=0;c_feed[i];i++){
      strncpy(buf,c_str0+c_processed,c_feed[i]);
      if(buf[c_feed[i]-1]=='\n') buf[c_feed[i]-1]=0;
      else buf[c_feed[i]]=0;
      c_processed+=c_feed[i];
      Serial.print(i);
      Serial.print(' ');
      Serial.print(strlen(buf));
      Serial.print(' ');
      Serial.println(c_feed[i]);
    }
    break;
    */
    c_processed=0;
    for(int j=0;j<start_line-SCROLL_BEGIN_LINES;j++)
      c_processed+=c_feed[j];
    if(readFile) fp->seek(c_processed);
  u8g2.setFont(_FONT);
  u8g2.clearBuffer();
      for( i = 0; i < line_cnt; i++ )
      {
  // copy a line of the text to the local buffer *
  //u8x8_CopyStringLine(buf, i+start_line, c_feed);
      int16_t cntline = i+start_line-SCROLL_BEGIN_LINES;
    if(i+start_line<total_lines && cntline>=0){
      if(readFile) {
        //fp->seek(c_processed);
        for(uint8_t j=0;j<c_feed[cntline];j++){
          if((buf[j]=fp->read())==0xe3){
            buf[j+1]=fp->read();
            buf[j+2]=fp->read();
            if(buf[j+1] == 0x80 && buf[j+2] == 0x80){
              buf[j] = ' ';
              buf[j+1] = '\x7f';
              buf[j+2] = '\x7f';
            }
            j+=2;
          }
        }
      }
      else strncpy(buf,c_str0+c_processed,c_feed[cntline]);
      if(buf[c_feed[cntline]-1]=='\n') buf[c_feed[cntline]-1]=0;
      /*
      else if(buf[c_feed[cntline]-1]>=224){ //再读2字节
        buf[c_feed[cntline]]=fp->read();
        buf[c_feed[cntline]+1]=fp->read();
        buf[c_feed[cntline]+2]=0;
      }
      else if(buf[c_feed[cntline]-1]>=192 || buf[c_feed[cntline]-2]>=224){ //再读1字节
        buf[c_feed[cntline]]=fp->read();
        buf[c_feed[cntline]+1]=0;
      }*/
      else buf[c_feed[cntline]]=0;
      c_processed+=c_feed[cntline];
    }
    else buf[0]=0;
  // draw the content of the local buffer *
  u8g2.drawUTF8(0, i*glyph_height-top_offset +glyph_height, buf);
      }
  u8g2.setColorIndex(0);
  u8g2.drawBox(0,58,24,6);
  u8g2.setColorIndex(1);
  u8g2.setFont(FONT_small);
  u8g2.setCursor(0,64);
  
  u8g2.print(100.0*top_window_pos/(total_lines-SCROLL_BEGIN_LINES)/glyph_height);
  u8g2.print('%');
  u8g2.sendBuffer();
  //按下右键加速
  if(getBtn(BtnR)==0){
    if(top_window_pos<(total_lines-SCROLL_BEGIN_LINES)*glyph_height-4*SCROLL_DELTA)
      top_window_pos += 4*SCROLL_DELTA;
    if(!long_press) long_press=millis();
    else if(millis()-long_press>2500 &&
    top_window_pos<(total_lines-SCROLL_BEGIN_LINES)*glyph_height-12*SCROLL_DELTA) top_window_pos += 8*SCROLL_DELTA;
    yield();
  }
  //按下左键回退
  else if(getBtn(BtnL)==0){
    if(top_window_pos>4*SCROLL_DELTA)
      top_window_pos -= 4*SCROLL_DELTA;
    if(!long_press) long_press=millis();
    else if(millis()-long_press>2500 &&
    top_window_pos>12*SCROLL_DELTA) top_window_pos -= 8*SCROLL_DELTA;
    yield();
  }
  else if(top_window_pos<(total_lines-SCROLL_BEGIN_LINES)*glyph_height && autoScroll){
    top_window_pos += SCROLL_DELTA;
    long_press=0;
    delay(SCROLL_DELAY);
  }
  else {
    long_press=0;
    yield();
  }
  //if(top_window_pos>=total_lines*glyph_height) top_window_pos=0;
  
  //按下中键打开菜单
  if(getBtn(BtnM)==0){
    delay(300);
//    double skipTo;
    const char *reader_text[]={
      "自动滚动","跳转到..","返回","退出","文档信息"
    };
    uint8_t stte=controlBoard(reader_text);
    if(stte==0) autoScroll=!autoScroll;
    if(stte==1) {
      int32_t slideTo= slider("拖动进度条(0.1%)",1000);
      if(slideTo!=-32768)
      // 显示 = 1000*top_window_pos/(total_lines-SCROLL_BEGIN_LINES)/glyph_height
      // 显示/1000 = top_window_pos/(total_lines-SCROLL_BEGIN_LINES)/glyph_height
      // 显示*(total_lines-SCROLL_BEGIN_LINES)*glyph_height/1000 = top_window_pos
      //求出它的逆运算
      top_window_pos =uint32_t(total_lines-SCROLL_BEGIN_LINES)*glyph_height *slideTo /1000;
      if(top_window_pos>(total_lines-SCROLL_BEGIN_LINES)*glyph_height) 
      top_window_pos=(total_lines-SCROLL_BEGIN_LINES)*glyph_height;
    }
    if(stte==4) {
      char b[32]="",c[32]="",d[32]="";
      if(readFile) sprintf(c,"大小: %d 字节",fp->size());
      else {
        sprintf(b,"地址: %p",c_str0);
        sprintf(c,"长度: %d 字节",strlen(c_str0));
      }
      sprintf(d,"行数: %d 行",total_lines-SCROLL_BEGIN_LINES);
      if(readFile) drawList(fp->name(),c,d,"请按确定键继续");
      else drawList(b,c,d,"请按确定键继续");
    }
    if(stte==3) {
      if(readFile>1 && SD.exists(historyPath)){
        File history = SD.open(historyPath, "a");
        history.seek(history.size()-4);
        history.write(top_window_pos);
        history.write(top_window_pos>>8);
        history.write(top_window_pos>>16);
        history.write((top_window_pos>>24)|(autoScroll<<7));
        history.close();
      }
      break;
    }
    delay(300);
  }
 }
}

uint8_t controlBoard(const char * txt[]){
  uint8_t cnt=2;
  static const uint8_t poses[5][4]={
  {10,20,51,15},
  {62,20,51,15},
  {10,36,27,15},
  {38,36,27,15},
  {66,36,51,15}};
  while(getBtn(BtnM)==0) yield();
  delay(50);
#ifdef MOTION_ON
      for(uint8_t i=2;i<16;i++){
    u8g2.setColorIndex(0);
    u8g2.drawBox(64-7*i/2,36-(5*i>>2),7*i,5*i/2);
    u8g2.setColorIndex(1);
    drawframe(64-7*i/2,36-(5*i>>2),7*i,5*i/2);
        u8g2.sendBuffer();
        delay(8);
        if(i>10) delay(4);
        if(i>13) delay(6);
      }
      delay(20);
#endif
  while(1){
    yield();
    if(getBtn(BtnL)==0){
      if(cnt>0) cnt--;
      else cnt=4;
#ifdef MOTION_ON
      for(uint8_t i=0;i<16;i++){
        uint8_t tx=map(i,0,16,poses[(cnt+1)%5][0],poses[cnt][0]);
        uint8_t ty=map(i,0,16,poses[(cnt+1)%5][1],poses[cnt][1]);
        uint8_t tr=map(i,0,16,poses[(cnt+1)%5][2],poses[cnt][2]);
    u8g2.setColorIndex(0);
    u8g2.drawBox(8,16,112,40);
    u8g2.setColorIndex(1);
    drawframe(8,16,112,40);
    u8g2.setFont(_FONT);
    u8g2.drawUTF8(12,32,txt[0]);
    u8g2.drawUTF8(64,32,txt[1]);
    u8g2.drawUTF8(12,48,txt[2]);
    u8g2.drawUTF8(40,48,txt[3]);
    u8g2.drawUTF8(68,48,txt[4]);
        drawframe(tx,ty,tr,15);
        u8g2.sendBuffer();
        if(i>=3 && i<11) i++;
        delay(1);
      }
#else
      delay(200);
#endif
    }
    if(getBtn(BtnR)==0){
      if(cnt<4) cnt++;
      else cnt=0;
#ifdef MOTION_ON
      for(uint8_t i=0;i<16;i++){
        uint8_t tx=map(i,0,16,poses[(cnt+4)%5][0],poses[cnt][0]);
        uint8_t ty=map(i,0,16,poses[(cnt+4)%5][1],poses[cnt][1]);
        uint8_t tr=map(i,0,16,poses[(cnt+4)%5][2],poses[cnt][2]);
    u8g2.setColorIndex(0);
    u8g2.drawBox(8,16,112,40);
    u8g2.setColorIndex(1);
    drawframe(8,16,112,40);
    u8g2.setFont(_FONT);
    u8g2.drawUTF8(12,32,txt[0]);
    u8g2.drawUTF8(64,32,txt[1]);
    u8g2.drawUTF8(12,48,txt[2]);
    u8g2.drawUTF8(40,48,txt[3]);
    u8g2.drawUTF8(68,48,txt[4]);
        drawframe(tx,ty,tr,15);
        u8g2.sendBuffer();
        if(i>=3 && i<11) i++;
        delay(1);
      }
#else
      delay(200);
#endif
    }
    u8g2.setColorIndex(0);
    u8g2.drawBox(8,16,112,40);
    u8g2.setColorIndex(1);
    drawframe(8,16,112,40);
    u8g2.setFont(_FONT);
    u8g2.drawUTF8(12,32,txt[0]);
    u8g2.drawUTF8(64,32,txt[1]);
    u8g2.drawUTF8(12,48,txt[2]);
    u8g2.drawUTF8(40,48,txt[3]);
    u8g2.drawUTF8(68,48,txt[4]);
    drawframe(poses[cnt][0],poses[cnt][1],poses[cnt][2],poses[cnt][3]);
        
    u8g2.sendBuffer();
    if(getBtn(BtnM)==0){
      while(getBtn(BtnM)==0) yield();
      delay(24);
      return cnt;
    }
  }
  return 255;
}
int16_t slider(const char *title,int16_t maxval,int16_t minval){
  int16_t n=(maxval+minval)/2;
  uint32_t lastPress=0;
  while(getBtn(BtnM)==0) yield();
      for(uint8_t i=2;i<16;i++){
    u8g2.setColorIndex(0);
    u8g2.drawBox(64-7*i/2,36-(5*i>>2),7*i,5*i/2);
    u8g2.setColorIndex(1);
    drawframe(64-7*i/2,36-(5*i>>2),7*i,5*i/2);
        u8g2.sendBuffer();
        delay(8);
        if(i>10) delay(4);
        if(i>13) delay(6);
      }
      delay(20);
  while(1){
    u8g2.setColorIndex(0);
    u8g2.drawBox(8,16,112,40);
    u8g2.setColorIndex(1);
    drawframe(8,16,112,40);
    u8g2.setCursor(12,30);
    u8g2.print(title);
    
    u8g2.drawHLine(12,35,100);
    u8g2.drawFrame(map(n,minval,maxval,0,100)+11,32,3,7);
    u8g2.setFont(FONT_med);
    u8g2.setCursor(12,50);
    u8g2.print(n);
    u8g2.setFont(_FONT);
    u8g2.setCursor(44,50);
    u8g2.print("确定长按取消");
    u8g2.sendBuffer();
    yield();
    if(getBtn(BtnL)==0){
      if(n>minval)
      n--;
      if(millis()-lastPress<900)
      delay(200);
      else if(millis()-lastPress<2100)
      delay(10);
      else if(n>=minval+19) n-=19;
    }
    else if(getBtn(BtnR)==0){
      if(n<maxval)
      n++;
      if(millis()-lastPress<900)
      delay(200);
      else if(millis()-lastPress<2100)
      delay(10);
      else if(n<=maxval-19) n+=19;
    }
    else if(getBtn(BtnM)==0){
      delay(400);
      if(getBtn(BtnM)==0){
        u8g2.setCursor(44,50);
        u8g2.print("松开取消输入");
        u8g2.sendBuffer();
        while(getBtn(BtnM)==0) yield();
        break;
      }
      else return n;
    }
    else lastPress=millis();
  }
    return -32768;
}
    
void drawKey(const char c,uint8_t x,uint8_t y,uint8_t st){
  if(!st)
    u8g2.drawFrame(x,y,9,14);
  else{
    u8g2.drawBox(x,y,9,14);
    u8g2.setColorIndex(0);
  }
  u8g2.drawGlyph(x+2,y+10,c);
  u8g2.setColorIndex(1);
}
//171 <<      176 °     187 >>    215 x   247 除号
//163 shift
int16_t getchs (char *tbuff,uint16_t maxlen){
  while(getBtn(BtnM)==0) yield();
  delay(50);
  if(maxlen<2) return 0;
  int16_t n=strlen(tbuff),pi=0,pj=0,tpad=0,btn=0,
  sPressed=-1,end_input=0,tlp=0,bliink=0;
  if(maxlen==16) tpad=2;//斗地主专用代码
#ifdef MOTION_ON
  int16_t ins_st=1;
#endif
  uint32_t tmillis=0;
  u8g2.setFont(FONT_med);
  while(1){
  if(sPressed!=-1 && getBtn(BtnM)==1){
    delay(50); sPressed=-1;
  }
  int16_t lth=n,lst=0;
  if (lth>20) {
    lst=lth-20;
    lth=20;
  }
  u8g2.clearBuffer();
  u8g2.drawHLine(1,1,126*n/(maxlen-1));
  for(int i=0;i<3;i++){
    for(int j=0;j<10;j++){
      drawKey(c[tpad][i][j],j*12+1+i*2,i*16+17,sPressed == i*10+j);
#ifdef MOTION_ON        
        if(pi==i && pj==j && !tlp) 
#else
        if(pi==i && pj==j)
#endif
        u8g2.drawFrame(j*12+i*2,i*16+16,11,16);
    }
  }
#ifdef MOTION_ON
  if (ins_st==1){
    if(lst==0){
      u8g2.drawFrame(0,0,128,15);
    for(int i=0;i<6;i++){
      u8g2.setColorIndex(0);
      u8g2.drawBox(2,2,6*lth,11);
      u8g2.setColorIndex(1);
      u8g2.setCursor(2,11);
      u8g2.print(tbuff+lst);
      if(maxlen-n<6)
      u8g2.drawBox(6*lth-3+i,2,4,11);
      else
      u8g2.drawBox(6*lth-3+i,2,2,11);
      u8g2.sendBuffer();
      delay(10);
    }
    }
    else{
    for(int i=0;i<6;i++){
      u8g2.drawFrame(0,0,128,15);
      u8g2.setColorIndex(0);
      u8g2.drawBox(1,2,6*lth,11);
      u8g2.setColorIndex(1);
      u8g2.setCursor(2-i,11);
      u8g2.print(tbuff+lst-1);
      if(maxlen-n<6)
      u8g2.drawBox(6*lth+2,2,4,11);
      else
      u8g2.drawBox(6*lth+2,2,2,11);
      u8g2.sendBuffer();
      delay(10);
    }
    }
  ins_st=0;
  }
  
  if (ins_st==-1){
    if(lth<20){
      u8g2.drawFrame(0,0,128,15);
    for(int i=0;i<6;i++){
      u8g2.setColorIndex(0);
      u8g2.drawBox(2,2,6*lth,11);
      u8g2.setColorIndex(1);
      u8g2.setCursor(2,11);
      u8g2.print(tbuff+lst);
      if(maxlen-n<6)
        u8g2.drawBox(6*lth+8-i,2,4,11);
      else
        u8g2.drawBox(6*lth+8-i,2,2,11);
      u8g2.setColorIndex(0);
      u8g2.drawBox(6*lth+10-i,2,2,11);
      u8g2.setColorIndex(1);
      u8g2.sendBuffer();
      delay(10);
    }
    }
    else{
    for(int i=0;i<6;i++){
      u8g2.drawFrame(0,0,128,15);
      u8g2.setColorIndex(0);
      u8g2.drawBox(1,2,6*lth,11);
      u8g2.setColorIndex(1);
      u8g2.setCursor(-4+i,11);
      u8g2.print(tbuff+lst);
      if(maxlen-n<6)
        u8g2.drawBox(6*lth+2,2,4,11);
      else
        u8g2.drawBox(6*lth+2,2,2,11);
      u8g2.sendBuffer();
      delay(10);
    }
    }
  ins_st=0;
  }
#endif
  u8g2.drawFrame(0,0,128,15);
  u8g2.setColorIndex(0);
  u8g2.drawBox(1,2,6*lth+1,11);
  u8g2.setColorIndex(1);
  u8g2.setCursor(2,11);
  u8g2.print(tbuff+lst);
  if(maxlen-n<6)
  u8g2.drawBox(6*lth+2,2,4,11);
  else
  u8g2.drawBox(6*lth+2,2,2,11);

  u8g2.sendBuffer();
  if(tlp >=1) {
#ifdef MOTION_ON
    for(int i=0;i<12;i++){
      drawKey(c[tpad][pi][pj],pj*12+1+pi*2,pi*16+17,sPressed == pi*10+pj);
      u8g2.drawFrame(pj*12+pi*2+i-12,pi*16+16,11,16);
      u8g2.sendBuffer();
      delay(5);
      u8g2.setColorIndex(0);
      u8g2.drawFrame(pj*12+pi*2+i-12,pi*16+16,11,16);
      u8g2.setColorIndex(1);
      if(i>2 && i<8) i++;
    }      
      if(pj>0) drawKey(c[tpad][pi][pj-1],pj*12-11+pi*2,pi*16+17,sPressed == pi*10+pj);
      drawKey(c[tpad][pi][pj],pj*12+1+pi*2,pi*16+17,sPressed == pi*10+pj);
      u8g2.drawFrame(pj*12+pi*2,pi*16+16,11,16);
      u8g2.sendBuffer();
#else
    delay(80);
#endif
  }
  //向下
  if(tlp <=-1) {
#ifdef MOTION_ON
    if(pi>0){
    for(int i=0;i<16;i++){
      drawKey(c[tpad][pi][pj],pj*12+1+pi*2,pi*16+17,sPressed == pi*10+pj);
      u8g2.drawFrame(pj*12+pi*2,pi*16+i,11,16);
      u8g2.sendBuffer();
      delay(5);
      u8g2.setColorIndex(0);
      u8g2.drawFrame(pj*12+pi*2,pi*16+i,11,16);
      u8g2.setColorIndex(1);
      if(i>3 && i<12) i++;
    }
    }
    else{
    for(int i=30;i>0;i-=2){
      drawKey(c[tpad][0][pj],pj*12+1,17,sPressed == pj);
      u8g2.drawFrame(pj*12+(i>>3),pi*16+17+i,11,16);
      u8g2.sendBuffer();
      delay(5);
      u8g2.setColorIndex(0);
      u8g2.drawFrame(pj*12+(i>>3),pi*16+17+i,11,16);
      u8g2.setColorIndex(1);
      if(i>4 && i<26) i-=2;
    }
    }
      if(pi>0) drawKey(c[tpad][pi-1][pj],pj*12-1+pi*2,pi*16+1,sPressed == pi*10+pj);
      else{
        drawKey(c[tpad][1][pj],pj*12+3,33,sPressed == 10+pj);
        drawKey(c[tpad][2][pj],pj*12+5,49,sPressed == 20+pj);
      }
      drawKey(c[tpad][pi][pj],pj*12+1+pi*2,pi*16+17,sPressed == pi*10+pj);
      u8g2.drawFrame(pj*12+pi*2,pi*16+16,11,16);
      u8g2.sendBuffer();
#else
    delay(100);
#endif
  }
  if(tlp ==1 ||tlp ==-1) delay(220);
  while((btn=(!getBtn(BtnL))+2*(!getBtn(BtnM))+4*
  (!getBtn(BtnR)))==0 && sPressed==-1) {
    tlp=0; 
    bliink++;
    if(bliink==100) {
      u8g2.setColorIndex(0);
      if(maxlen-n<6)
      u8g2.drawBox(6*lth+2,2,4,11);
      else
      u8g2.drawBox(6*lth+2,2,2,11);
      u8g2.setColorIndex(1);
      u8g2.sendBuffer();
    }
    if(bliink==200) {
      bliink=0;
      if(maxlen-n<6)
      u8g2.drawBox(6*lth+2,2,4,11);
      else
      u8g2.drawBox(6*lth+2,2,2,11);
      u8g2.sendBuffer();
    }
    delay(2);
  }
  if(btn==1){
      if(pi>=2) pi=0; else pi++;
      delay(24);
      tlp--;
  }
  if(btn==2){
#ifdef MOTION_ON
        tlp=0;
#endif
    if(sPressed==-1){
    sPressed=pi*10+pj;
    switch(sPressed) {
      case 19:
      if(n){
        tbuff[n-1]=0; 
        n--;
#ifdef MOTION_ON
        ins_st=-1;
#endif
      }
      break;
      case 20:
        delay(24); if(getBtn(BtnM)) break;
        if(tpad>=3) tpad=0; else tpad++;
      break;
      case 29:
        end_input=1;
      break;
      default:
      if(n<maxlen-1){
        tbuff[n]=c[tpad][pi][pj]; 
        n++;
#ifdef MOTION_ON
        ins_st=1;
#endif
      }
    }
    tmillis=millis();
    }
    else{
      uint32_t gms=millis()-tmillis;
      if(gms>230 && gms%160<19){
        if(n<maxlen-1 && sPressed!=19&& sPressed!=20&& sPressed!=29){
          tbuff[n]=c[tpad][pi][pj]; 
          n++;
#ifdef MOTION_ON
          ins_st=1;
#endif
        }
        if(n && sPressed==19){
          tbuff[n-1]=0; 
          n--;
#ifdef MOTION_ON
          ins_st=-1;
#endif
        }
        delay(20);
      }
    }
  }
  if(btn==4){
    if(pj>=9) pj=0; else pj++;
      delay(24);
    tlp++;
  }
  if(end_input){
    while(getBtn(BtnM)==0) yield();
      delay(24);
    break;
  }
  }
  //斗地主专用代码开始
  while(n<6 && n>0 && maxlen==7){
    tbuff[n]=' '; n++;
  }
  //斗地主专用代码结束
  return n;
}




uint8_t listdraw(const char *title,const char *items[],uint8_t numitem,uint8_t dure){
  uint8_t long_pressed=0;
  if(dure >= numitem) dure = numitem-1;
  while(getBtn(BtnM)==0) yield();
  for(int i=22;i>1;i--){ //动画
    const char *str1=NULL, *str2=NULL, *str3=NULL;
    if(numitem == 1 ) str2 = items[0];
    else if(dure==0) { str2 = items[0]; str3 = items[1]; }
    else if(dure==numitem-1) { str1 = items[numitem-2]; str2 = items[numitem-1]; }
    else if(numitem){
      str1 = items[dure-1]; str2 = items[dure]; str3 = items[dure+1];
    }
    /*else if(numitem == 0 ) "(无项目,按确定键返回)" */
    drawLists(title,str1,str2,str3,0,1,numitem,i*i/4);
    delay(1);
  }
  if(numitem == 1 ){
    drawLists(title,NULL,items[0],NULL,0,1,numitem);
  while(getBtn(BtnM)==1) yield();
  for(int i=0;i<200;i++){
    if(getBtn(BtnM)==0) long_pressed++;
    delay(2);
  }
  if(long_pressed==200){
    u8g2.setColorIndex(0);
    u8g2.drawBox(44,28,40,24);
    u8g2.setColorIndex(1);
    drawframe(44,28,40,24);
    u8g2.setFont(_FONT);
    u8g2.drawUTF8(52,47,"返回");
    u8g2.sendBuffer();
    while(getBtn(BtnM)==0) yield();
    delay(24);
    return 255;
  }
    delay(24);
    return 0;
  }
  if(numitem == 0 ){
    drawLists(title,NULL,NULL,NULL,0,1,numitem);
    while(getBtn(BtnM)==1) yield();
    while(getBtn(BtnM)==0) yield();
    delay(24);
    return 255;
  }
  //uint8_t dure=numfresh;
  while(switchMode(BtnM,10)){
    uint8_t way=0; //这两行代码来自远古的Arduino Due时代,
    //好像是2018年写的, 原来用的byte 作为类型关键字
    //46上,48下  (Due引脚)
    if(switchMode(BtnR,20)==0){
      way=1;
      if(dure>=numitem-1)
      dure=0;
      else
      dure++;
    }
    if(switchMode(BtnL,20)==0){
      way=2;
      if(dure<=0)
      dure=numitem-1;
      else
      dure--;
    }
    if(dure==0)
    drawLists(title,"",items[0],items[1],way,dure+1,numitem);
    else if(dure==numitem-1)
    drawLists(title,items[numitem-2],items[numitem-1],"",way,dure+1,numitem);
    else
    drawLists(title,items[dure-1],items[dure],items[dure+1],way,dure+1,numitem);
  } 
  for(int i=0;i<200;i++){
    if(getBtn(BtnM)==0) long_pressed++;
    delay(2);
  }
  if(long_pressed==200){
    u8g2.setColorIndex(0);
    u8g2.drawBox(44,28,40,24);
    u8g2.setColorIndex(1);
    drawframe(44,28,40,24);
    u8g2.setFont(_FONT);
    u8g2.drawUTF8(52,47,"返回");
    u8g2.sendBuffer();
    while(getBtn(BtnM)==0) yield();
    delay(24);
    return 255;
  }
  delay(24);
  return dure;
}
void drawList(const char*s1,const char*s2,const char*s3,const char*s4,uint32_t delayms,uint8_t hmotion){
  u8g2.setFont(_FONT);
  #ifdef MOTION_ON
  if(hmotion){
    for(int i=0;i<16;i++){
      u8g2.drawHLine(64-4*i,31,8*i);
      u8g2.drawHLine(64-4*i,32,8*i);
      u8g2.sendBuffer();
      delay(5);
    }
    for(int i=1;i<16;i++){
      u8g2.setColorIndex(0);
      u8g2.drawBox(0,32-2*i,128,4*i);
      u8g2.setColorIndex(1);
      drawframe(0,32-2*i,128,4*i);
      if(i>=4){
        //u8g2.setCursor(2,45-2*i);u8g2.print(s1);
        u8g2.setCursor(2,25+2*i);
        u8g2.print(s4);
      }
      if(i>=8){
        //u8g2.setCursor(2,61-2*i);u8g2.print(s2);
        u8g2.setCursor(2,11+2*i);
        u8g2.print(s3);
      }
      if(i>=12){
        //u8g2.setCursor(2,75-2*i);u8g2.print(s3);
        u8g2.setCursor(2,2*i-3);
        u8g2.print(s2);
      }
      u8g2.sendBuffer();
      delay(5);
    }
  }
  #endif
  u8g2.clearBuffer();
    drawframe(0,0,128,64);
    u8g2.setCursor(2,13);
    u8g2.print(s1);
    u8g2.setCursor(2,29);
    u8g2.print(s2);
    u8g2.setCursor(2,43);
    u8g2.print(s3);
    u8g2.setCursor(2,57);
    u8g2.print(s4);
  u8g2.sendBuffer();
  if(delayms) delay(delayms);
  else{
    while(getBtn(BtnM)!=0) yield();
    while(getBtn(BtnM)==0) yield();
    delay(24);
  }
}

void drawList(const __FlashStringHelper*s1,const __FlashStringHelper*s2,const __FlashStringHelper*s3,const __FlashStringHelper*s4,uint32_t delayms,uint8_t hmotion){
  u8g2.setFont(_FONT);
  #ifdef MOTION_ON
  if(hmotion){
    for(int i=0;i<16;i++){
      u8g2.drawHLine(64-4*i,31,8*i);
      u8g2.drawHLine(64-4*i,32,8*i);
      u8g2.sendBuffer();
      delay(5);
    }
    for(int i=1;i<16;i++){
      u8g2.setColorIndex(0);
      u8g2.drawBox(0,32-2*i,128,4*i);
      u8g2.setColorIndex(1);
      drawframe(0,32-2*i,128,4*i);
      if(i>=4){
        //u8g2.setCursor(2,45-2*i);u8g2.print(s1);
        u8g2.setCursor(2,25+2*i);
        if(s4) u8g2.print(s4);
      }
      if(i>=8){
        //u8g2.setCursor(2,61-2*i);u8g2.print(s2);
        u8g2.setCursor(2,11+2*i);
        if(s3) u8g2.print(s3);
      }
      if(i>=12){
        //u8g2.setCursor(2,75-2*i);u8g2.print(s3);
        u8g2.setCursor(2,2*i-3);
        if(s2) u8g2.print(s2);
      }
      u8g2.sendBuffer();
      delay(5);
    }
  }
  #endif
  u8g2.clearBuffer();
    drawframe(0,0,128,64);
    u8g2.setCursor(2,13);
    if(s1) u8g2.print(s1);
    u8g2.setCursor(2,29);
    if(s2) u8g2.print(s2);
    u8g2.setCursor(2,43);
    if(s3) u8g2.print(s3);
    u8g2.setCursor(2,57);
    if(s4) u8g2.print(s4);
  u8g2.sendBuffer();
  if(delayms) delay(delayms);
  else{
    while(getBtn(BtnM)!=0) yield();
    while(getBtn(BtnM)==0) yield();
    delay(24);
  }
}
void drawLists(const char*s1,const char*s2,const char*s3,
const char*s4,const uint8_t way,const uint8_t cn,const uint8_t al,uint8_t delta){
  const char *noItem = "";
  const char *noItem2 = "(无项目,按确定键返回)";
  if(s2 == NULL) s2 = noItem;
  if(s3 == NULL) s3 = noItem2;
  if(s4 == NULL) s4 = noItem;
  //way=1,按上键，2下键
  u8g2.setFont(_FONT);
  if(way==1){
  //  int dure=-16;
    for(uint8_t i=0;i<16;i++){
  u8g2.clearBuffer();
    u8g2.setCursor(1+delta,44-i);
    u8g2.print(s2);
    drawframe(0,32,128,16);
    u8g2.setCursor(1+delta,60-i);
    u8g2.print(s3);
    u8g2.setCursor(1+delta,76-i);
    u8g2.print(s4);
    drawframe(delta,0,128,16);
    u8g2.setCursor(2+delta,13);
    u8g2.print(s1);
  u8g2.setFont(FONT_small);
    u8g2.setCursor(115+delta,7);
    u8g2.print(cn);
    u8g2.setCursor(115+delta,13);
    u8g2.print(al);
  u8g2.setFont(_FONT);
  u8g2.sendBuffer();
    if(i>2&&i<13) i++;
    delay(1);
    }
  }
  if(way==2){
  //  int dure=-16;
    for(uint8_t i=0;i<16;i++){
  u8g2.clearBuffer();
    u8g2.setCursor(1+delta,14+i);
    u8g2.print(s2);
    drawframe(delta,32,128,16);
    u8g2.setCursor(1+delta,30+i);
    u8g2.print(s3);
    u8g2.setCursor(1+delta,46+i);
    u8g2.print(s4);
    drawframe(delta,0,128,16);
    u8g2.setCursor(2+delta,13);
    u8g2.print(s1);
  u8g2.setFont(FONT_small);
    u8g2.setCursor(115+delta,7);
    u8g2.print(cn);
    u8g2.setCursor(115+delta,13);
    u8g2.print(al);
  u8g2.setFont(_FONT);
  u8g2.sendBuffer();
    if(i>2&&i<13) i++;
    delay(1);
    }
  }
  u8g2.clearBuffer();
    drawframe(delta,0,128,16);
    u8g2.setCursor(2+delta,13);
    u8g2.print(s1);
  u8g2.setFont(FONT_small);
    u8g2.setCursor(115+delta,7);
    u8g2.print(cn);
    u8g2.setCursor(115+delta,13);
    u8g2.print(al);
  u8g2.setFont(_FONT);
    u8g2.setCursor(1+delta,29);
    u8g2.print(s2);
    u8g2.drawBox(delta,32,128,16);
    u8g2.setColorIndex(0);

    u8g2.drawPixel(delta,32);
    u8g2.drawPixel(delta,33);
    u8g2.drawPixel(delta+1,32);
    u8g2.drawPixel(126,32);
    u8g2.drawPixel(127,32);
    u8g2.drawPixel(127,33);
    u8g2.drawPixel(delta,47);
    u8g2.drawPixel(delta,46);
    u8g2.drawPixel(delta+1,47);
    u8g2.drawPixel(126,47);
    u8g2.drawPixel(127,46);
    u8g2.drawPixel(127,47);
    
    u8g2.setCursor(1+delta,45);
    u8g2.print(s3);
    u8g2.setColorIndex(1);
    u8g2.setCursor(1+delta,61);
    u8g2.print(s4);
  u8g2.sendBuffer();
}

uint8_t switchMode(const uint8_t spin,const uint8_t dtime){
  uint8_t oldState=getBtn(spin);
  for(uint8_t i=0;i<dtime;i++){
    if(getBtn(spin)!=oldState){
      return 2;
    }
    delay(1);
  }
  return oldState;
}
