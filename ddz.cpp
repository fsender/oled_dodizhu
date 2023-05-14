#include "ddz.h"

//--------斗地主服务器专用-------
//先匹配玩家,未匹配到时退出
//为新来的玩家分配ID并处理名字（昵称）信息
/*分配ID参考座位：___________________
  *玩家位置:    | 2号玩家    1号玩家  |
   *           |   0号玩家(服务器)   | 
               |-------------------|*/
uint8_t start_connect(plyr *player){ //开启匹配
  uint8_t join_cfg=0;
  Serial.println("开启斗地主匹配,IP地址为");
  Serial.println(WiFi.localIP());
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_wqy12_t_gb2312);
  u8g2.setCursor(0,13);
  u8g2.print("连接到我的IP地址 :");
  u8g2.setCursor(0,29);
  u8g2.print(WiFi.localIP());
  u8g2.sendBuffer();
  //u8g2.setCursor(0,45);
  //u8g2.setCursor(0,61);
  server.begin();
  server.setNoDelay(true);
  while(1){
    uint16_t i;
    yield();
  if (server.hasClient()) {
    for (i = 0; i < 2; i++) {
      //释放旧无效或者断开的client
      if (!playr[i] || !playr[i].connected()) {
        if(player[i+1].id){
          Serial.printf("玩家ID %d 断开连接",i+1);
          u8g2.setColorIndex(0);
          u8g2.drawBox(0,32+16*i,128,16);
          u8g2.setColorIndex(1);
          u8g2.sendBuffer();
          player[i+1].id=0;
          join_cfg--;
        }
        if (playr[i]) {
          playr[i].stop();
        }
        //分配最新的client
        playr[i] = server.available();
        Serial.print("New client: "); 
        Serial.print(i);
        break;
      }
    }
    //当达到最大连接数 无法释放无效的client，需要拒绝连接
    if (i == 2) {
      WiFiClient playr = server.available();
      playr.stop();
      Serial.println("Connection rejected ");
    }
  }
   for (i = 0; i < 2; i++) {
      if(playr[i].available()){
        Serial.print("接收到请求信息:");
        delay(1);//等待接收完成
        //读取接收来的client发送请求 
        if(playr[i].read()=='b'){
          for(int j=0;j<6;j++){
            player[i+1].name[j]=playr[i].read();
          }
          player[i+1].name[6]=0;
          player[i+1].id=i+1;
          Serial.print("玩家 ");
          Serial.print(player[i+1].name);
          Serial.print(" 进入游戏");
          u8g2.setCursor(0,45+16*i);
          u8g2.print("玩家 ");
          u8g2.print(player[i+1].name) ;
          u8g2.print(" 进入游戏");
          u8g2.sendBuffer();
          char willSend[4]="bx\n";
          willSend[1]=i+1;
          playr[i].print(willSend); //发送回复
          join_cfg++;//只有当接收完成之后才视为加入成功
          Serial.println(join_cfg);
          
          while(playr[i].available()) playr[i].read();//清除缓存
          Serial.printf("当前玩家数量: %d回复信息: b%d\n",join_cfg,i+1);
          delay(100);
        }
        else {
          Serial.println("加入信息校验失败");
          playr[i].stop();
        }
      }
   }
    if(join_cfg>=2) break;
    if(digitalRead(BtnL)==0){
      while(digitalRead(BtnL)==0) yield();
      delay(24);
      server.stop();
      return 1;  //自己退出
    }
  }
  char infosync[24]="u";
  strncat(infosync,player[0].name,6);
  strncat(infosync,player[1].name,6);
  strncat(infosync,player[2].name,6);
  infosync[19]='\n';
  infosync[20]='\n';
  Serial.print("发送信息：");
  Serial.println(infosync);
  playr[0]. print(infosync);
  playr[1]. print(infosync);
  return 0;
}
//若在client 模式返回0代表成功匹配到玩家
        //，若主机未就绪则返回1
 //若在server 此时如果接收到掉线请求 (b开头) 先检查人数再决定是否加入
      //  需要一个数据同步函数，此函数响应
uint8_t wait_ready_s(plyr * player){ //等待准备
  Serial.println("等待准备");
  u8g2.drawUTF8(51,37,"准备");
  while(playr[0].available()) playr[0].read();
  while(playr[1].available()) playr[1].read();
  disp_status("正在等待其他玩家准备");
  uint8_t prpflag=0;
  while(1){
   yield();
   for(int i=0;i<2;i++){
    if(playr[i].available()){
     char cmd=playr[i].read();
     if(cmd=='r'){
      prpflag++;
      playr[i].read();
      playr[i].read();
      u8g2.setFont(u8g2_font_wqy12_t_gb2312);
      u8g2.drawUTF8(i?0:103,26,"准备");
      u8g2.sendBuffer();
      playr[i].write('r');
     }
     else if(cmd=='t') {
      pcards nullpool;
      nullpool.num=3;
      nullpool.card[0]=120;
      nullpool.card[1]=120;
      nullpool.card[2]=120;
      serve_sync(player,&nullpool);//服务断线连接
     }
     while(playr[i].available()) playr[i].read();
    }
   }
   if(digitalRead(BtnL)==0 && digitalRead(BtnR)==0){
    server.stop();
    return 1;
   }
   if(prpflag==2) break;
  }
  return 0;
}
uint8_t serve_deal(plyr * player,pcards *To){ //发牌，To代表底牌
  Serial.println("正在发牌");
  yield();
  char willSend[64]="";
  
  randomCards(&player[0].cards,&player[1].cards,&player[2].cards,To);
  takeOrder(&player[0].cards);//排序
  takeOrder(&player[1].cards);//排序
  takeOrder(&player[2].cards);//排序
  //生成新牌
  willSend[0]='d';
  willSend[1]=2;
  //strncat(willSend,(char *)player[0].cards.card,17);
  for(uint8_t i=0;i<17;i++)
    willSend[i+2]=player[0].cards.card[i]+2;
  willSend[19]=3;
  //strncat(willSend,(char *)player[0].cards.card,17);
  for(uint8_t i=0;i<17;i++)
    willSend[i+20]=player[1].cards.card[i]+2;
  willSend[37]=4;
  //strncat(willSend,(char *)player[0].cards.card,17);
  for(uint8_t i=0;i<17;i++)
    willSend[i+38]=player[2].cards.card[i]+2;
  willSend[55]=To->card[0]+2;
  willSend[56]=To->card[1]+2;
  willSend[57]=To->card[2]+2;
  willSend[58]='\n';
  playr[0].print(willSend);
  playr[1].print(willSend);
  while(!playr[0].available() || !playr[1].available()) yield();
  if(playr[0].read()=='d' && playr[1].read()=='d') return 0;//success.
  while(playr[0].available()) playr[0].read();
  while(playr[1].available()) playr[1].read();
  delay(100);
  return 1;//校验失败
}

      //叫分，需要加入响应代码,并处理地主信息和基础分
      //除了第一次是房主叫分，以后每次都是上一把的赢家叫分
      //如果上一把地主赢了则由地主的下家叫分
    //需要确保所有client都访问过才能进入下一步
    //此函数需要写入base_score变量
uint8_t serve_call_score(plyr * player){//处理叫分
  uint8_t last_call=4; //初始设为无效值
  uint8_t amax=0; //即将成为地主的玩家ID
  char willSend[6]="l";
  Serial.println("等待叫分");
  for(int ci=0;ci<3;ci++){
    if(winner==0){
      int16_t s=0;
      pcards tpool[3];
      tpool[0].num=0; tpool[1].num=0;
      tpool[2].num=3; tpool[2].card[0]=120; tpool[2].card[1]=120; tpool[2].card[2]=120;
      //叫分前先让大家知道上一个玩家的叫分情况
      willSend[1]=winner+48; willSend[2]=base_score+48; 
      willSend[3]=last_call+48; willSend[4]='\n'; willSend[5]=0;
      playr[0].print(willSend);
      playr[1].print(willSend);
      do{
        //draw_gui(player,tpool);
        disp_status("按确定键叫分",37);
        while(digitalRead(BtnM)==1) {
            u8g2.setColorIndex(0);
            u8g2.drawBox(0,48,128,16);
            u8g2.setColorIndex(1);
          draw_card(&player[yourID].cards,0,49,15*(digitalRead(BtnR)==0 ||digitalRead(BtnL)==0));
          yield();
        }
        while(digitalRead(BtnM)==0) yield();
        delay(24);
        
        s=slider("选择想要叫的分",3);
        draw_gui(player,tpool);
      }while(s==-32768);
      
      last_call=s;
      if(s>base_score) {
        base_score=s;
        u8g2.drawUTF8(51,26,ch_calling[s]);
        amax=0;
      }
      else u8g2.drawUTF8(51,26,ch_calling[0]);
      u8g2.sendBuffer();
    }
    else {
      disp_status("等待别人叫分",37);
      willSend[1]=winner+48; willSend[2]=base_score+48; 
      willSend[3]=last_call+48; willSend[4]='\n'; willSend[5]=0;
      playr[0].print(willSend);
      playr[1].print(willSend);
      uint32_t asTimeOut=millis();//防止丢包
      uint8_t asRecv=0;//防止丢包
      while(1){
        if(playr[winner-1].available()){
          char preread=playr[winner-1].read();
          if(preread=='v') {
            Serial.println("接收到校验信息");
            asRecv=1;
            while(playr[winner-1].available()) playr[winner-1].read();
          }
          else if(preread='c') {
            last_call=playr[winner-1].read()-48;
            while(playr[winner-1].available()) playr[winner-1].read();
            break;
          }
          else { //丢包重新发送
            while(playr[winner-1].available()) playr[winner-1].read();
            delay(76);
            playr[winner-1].print(willSend);
          }
        }
        else if(millis()-asTimeOut>REQTIMEOUT && 0==asRecv){ //丢包重新发送
            Serial.println("叫分环节丢包");
            delay(76);
            playr[winner-1].print(willSend);
            asTimeOut=millis();
        }
        yield();
      }
      //0,26      103,26      51,37
      if(last_call>base_score) {
        base_score=last_call;
        u8g2.drawUTF8(winner==2?0:103,26,ch_calling[base_score]);
        amax=winner;
      }
      else u8g2.drawUTF8(winner==2?0:103,26,ch_calling[0]);
      u8g2.sendBuffer();
    }
    if(base_score==3) break;
    winner=(winner+1)%3;
  }
  willSend[0]='c'; willSend[1]=amax+48; willSend[2]=base_score+48;
  willSend[3]='\n'; willSend[4]=0;
  playr[0].print(willSend);
  playr[1].print(willSend);
  if(base_score) player[amax].identity=1;
  delay(1200);
  return amax;
}
/*
uint8_t set_order(){  //设置出牌权，仅发送，紧接着调用serve_card（）
  char willSend[4]="g0\n"
  if(p_holder){
    willSend[1]+=p_holder;
    playr[p_holder-1].print(willSend);
  }
  return 0;
}*/
      //出牌不符合要求需要重新出牌
      //serve_card函数返回1代表符合要求
      //serve_card函数返回0代表出完牌了
      //serve_card函数返回2代表玩家不出牌
uint8_t serve_card(plyr * player,pcards *pool,uint8_t sendOnly){ //处理打来的牌
  //收到出牌信息后再次发送其他玩家出牌信息
  Serial.print("服务出牌函数，");
  
  //广播式发送同步信息
  String willSend = "g";
  if(sendOnly) willSend += (char)(winner+51);
  else willSend += (char)(p_holder+48);
  willSend += serve_sync(player,pool);
  playr[0].print(willSend);
  playr[1].print(willSend);
  if(p_holder==0 && !sendOnly){//自己出牌
    pcards got =get_card(player,pool);
    if(got.num==0) return 2;
    else {
      pool[1]=pool[0];
      pool[0]=got;
      if(pool[0].type>=15 && p_holder==0) plex<<=1;//炸弹翻倍
      if(player[0].cards.num==0) return 0;//出完了
      return 1;
    }
  }
  else {
    uint8_t poolnum=0;
    uint32_t timeOut=millis();
    while(!sendOnly){
      yield();
      if(playr[p_holder-1].available()){
        if(playr[p_holder-1].read()=='v'){
          break;//读取到v时校验成功
        }
      }
      if(millis()-timeOut>REQTIMEOUT){
        Serial.println("处理出牌环节丢包");
        playr[p_holder-1].print(willSend);//重新发送
        timeOut=millis();
      }
    }
    uint8_t breakfg[2]={0};
    while(sendOnly){
      yield();
      for(uint8_t j=0;j<2;j++){
      if(playr[j].available()){
        if(playr[j].read()=='v') breakfg[j]++;
      }
      if(millis()-timeOut>REQTIMEOUT){
        //结算时丢包
        Serial.printf("结算得分时丢包，玩家1接收到: %d，玩家2接收到: %d\n",breakfg[0],breakfg[1]);
        if(!breakfg[1]) playr[1].print(willSend);//重新发送
        if(!breakfg[0]) playr[0].print(willSend);//重新发送
        timeOut=millis();
      }
      }
      if(breakfg[0] && breakfg[1]) return 0;
    }
    while(playr[p_holder-1].available()) playr[p_holder-1].read();
    delay(50);
    while(!playr[p_holder-1].available()){
        u8g2.setColorIndex(0);
        u8g2.drawBox(0,48,128,16);
        u8g2.setColorIndex(1);
        uint8_t motionS=(millis()/50)%18;
          u8g2.drawPixel(zx1[motionS],zy1[motionS]);
          u8g2.drawPixel(zx1[motionS+1],zy1[motionS+1]);
          u8g2.drawPixel(zx1[motionS+2],zy1[motionS+2]);
        if(player[0].cards.num>15){
        if(digitalRead(BtnR)==0 ||digitalRead(BtnL)==0){
          u8g2.drawVLine(126,51,5);
          u8g2.drawVLine(125,52,3);
          u8g2.drawPixel(124,53);
        }
        else{
          u8g2.drawVLine(124,51,5);
          u8g2.drawVLine(125,52,3);
          u8g2.drawPixel(126,53);
        }
        draw_card(&player[0].cards,0,49,15*(digitalRead(BtnR)==0 ||digitalRead(BtnL)==0));
        }
        else draw_card(&player[0].cards,0,49);
      yield();
    }
    delay(10);//等待接收完成
    if ( playr[p_holder-1].read()=='o' ){
      char send_to_pool[48]="";
      for(int i=0;i<47;i++){
        send_to_pool[i] = playr[p_holder-1].read();
    Serial.write(send_to_pool[i]);
    Serial.print((uint8_t)send_to_pool[i]);
    Serial.print(' ');
      }
      //偏移量为24
      if((poolnum=send_to_pool[0]-2)>0){//有出的牌才写入pool[0]
      pool[1]=pool[0];
      pool[0].num=poolnum;
      pool[0].player=send_to_pool[1]-2;
      pool[0].type=send_to_pool[2]-2;
      for(int i=0;i<21;i++) {
        if(send_to_pool[i+3]!=1)
          pool[0].card[i] =send_to_pool[i+3]-2;
      }
      player[p_holder].cards.num=send_to_pool[24]-2;
      for(int i=0;i<21;i++) {
        if(send_to_pool[i+25]!=1)
          player[p_holder].cards.card[i] =send_to_pool[i+25]-2;
      }//同步剩余的牌
      }
    }
    while(playr[p_holder-1].available()) playr[p_holder-1].read();
    //读取完多余缓存
    if(poolnum==0) return 2;//没出牌
    if(pool[0].type>=15 && p_holder==pool[0].player) plex<<=1;//炸弹翻倍
    if(player[p_holder].cards.num==0) return 0;//出完了
  }
  return 1;
}
uint8_t end_game(plyr * player){  //结束并计算得分,返回值为赢家的id+3
  //绘制对话框
  return winner+3;
}
uint8_t end_connect(){//结束连接
  Serial.println("游戏结束");
  return 0;
}
/* 回应同步数据请求，限服务器,把数据做成可以被发送的类型
   返回值：无*/
String serve_sync(plyr * player,pcards *pool){
  String willSend="t";
  char pname[7] = "";
  char idty = '0';
  char pscore[7] = "";
  char xcards[25]= "";
  for(int i=0;i<3;i++){
    strcpy(pname,player[i].name);
    idty += player[i].identity;
    sprintf(pscore,"%6d",player[i].score);
    xcards[0]=player[i].cards.num+2;
    for(int j=0;j<23;j++){
      if(j<player[i].cards.num)
      xcards[j+1]=player[i].cards.card[j] +2;
      else xcards[j+1]=1;
    }
    willSend +=pname;
    willSend +=idty;
    willSend +=pscore;
    willSend +=xcards;
    willSend +='t';
    idty='0';
  }
  for(int i=0;i<3;i++){
    xcards[0]=pool[i].num+2;
    xcards[1]=pool[i].player+2;
    xcards[2]=pool[i].type+2;
    for(int j=0;j<21;j++){
      if(j<pool[i].num)
      xcards[j+3]=pool[i].card[j] +2;
      else xcards[j+3]=1;
    }
    willSend +=xcards;
    willSend +='t';
  }
  return willSend;
}

//--------斗地主客户端专用-------
//自己退出和连接失败均视为返回值0
  //成功则返回由服务器获取的玩家ID（1或者2）,同时设置你的昵称。
  //若为断线重连，返回3或4，将在之后接收到所有游戏数据
  //注意默认输入的昵称在 player[0]->name 中，需要先转存到新字符串后再转到需要的位置
uint8_t start_connect_to(const char *ip,uint32_t h_port,plyr *player){//连接到房主
  Serial.println("尝试连接到房主");
  char playername[7]="";
  uint8_t gotid=0;
  strcpy(playername,player[0].name);
  drawList("正在连接到房主",playername,"房主IP地址",ip,1);
  if(!playr[0].connect(ip,h_port)) return 0;//连接失败
  playr[0].setNoDelay(1);
  Serial.println("连接成功\n接收校验数据");
  delay(10);
  char willSend[8]="b";
  strcat(willSend,playername);
  playr[0].print(willSend);
  while(!playr[0].available()) {
    if(digitalRead(BtnL)==0){
      playr[0].stop();
      delay(200);
      return 0;
    }
    yield();
  }
  delay(1);
  Serial.print("接收数据成功,");
  if(playr[0].read()=='b') Serial.printf("获得ID: %d\n",gotid=playr[0].read());
  while(playr[0].available()) playr[0].read();
  if(gotid==1 || gotid==2) {
    strcpy(player[gotid].name,playername);
  
    delay(40);
    char idn[2]={0};
    idn[0]=gotid+48;
    drawList("获得玩家ID成功",playername,idn,"等待其他玩家进入游戏",1,0);
  //等待其他玩家加入游戏，当所有玩家加入且主机已准备后，主机将发送所有玩家昵称信息，此时不用回复
    while(!playr[0].available() && (digitalRead(BtnM) || digitalRead(BtnR))) {
    if(digitalRead(BtnL)==0){
      playr[0].stop();
      delay(200);
      return 0;
    }
    yield();
    }
    String pl_names = playr[0].readStringUntil('\n');
    if(pl_names[0]=='u'){
      strncpy(player[0].name,(pl_names.c_str())+1,6);
      player[0].name[6]=0;
      strncpy(player[1].name,(pl_names.c_str())+7,6);
      player[1].name[6]=0;
      strncpy(player[2].name,(pl_names.c_str())+13,6);
      player[2].name[6]=0;
    }
    while(playr[0].available()) playr[0].read();
  }
  return gotid;
}
uint8_t send_ready(){  //准备
  Serial.println("发送准备请求");
  playr[0].print("r\n");
  return 0;
}
//若在client 模式返回0代表成功匹配到玩家
        //，若主机未就绪则返回1
uint8_t wait_ready(){ //等待准备
  u8g2.setFont(u8g2_font_wqy12_t_gb2312);
  Serial.println("等待准备");
  if(yourID==1)  u8g2.drawUTF8(0,26,"准备");
  else if(yourID==2)  u8g2.drawUTF8(103,26,"准备");
    u8g2.sendBuffer();
  uint32_t prepareTime=millis();
  uint8_t rtnFlag=1;
  while(!playr[0].available()) {
    yield();
    if(millis()-prepareTime>REQTIMEOUT*2) return 1;
  }
  delay(10);
  if(playr[0].read()=='r') {
    rtnFlag=0; //成功回应
    u8g2.drawUTF8(51,37,"准备");
    disp_status("等待其他玩家准备");
  }
  while(playr[0].available()) playr[0].read();
  return rtnFlag;//成功
}
/** sync_data
 * 需要同步的信息有：1.三个玩家的信息（昵称、得分、身份是否为地主、手牌）
 * 2.当前牌桌上的牌  3.底牌  4.叫分情况与倍数
 * 同步信息，若失败(返回0)则退出游戏 
     @return : 0.同步失败
        //1.等待准备，此时可以加入游戏 //2.等待叫分 //3.等待出牌
 */
uint8_t sync_data(plyr *player,pcards* pool){
  Serial.println("同步游戏信息");
  char abf[192]="";
  delay(10);//等待接收完成
  for(int i=0;i<192;i++){
    abf[i]=playr[0].read();
  }
  if(abf[0]=='t'){
    for(int i=0;i<3;i++){//处理玩家信息
      for(int j=0;j<6;j++){
        player[i].name[j]=abf[j+i*38+1];
      }
      player[i].name[6]=0;//昵称同步完成
      player[i].identity=abf[i*38+7]-48;//同步身份信息
      strToInt(&player[i].score,&abf[i*38+8]);//同步得分
      player[i].cards.num=abf[i*38+14]-2;
      for(int j=0;j<21;j++){//同步手牌
        if(abf[j+i*38+15]!=1) player[i].cards.card[j]=abf[j+i*38+15]-2;
      }
    }
    for(int i=0;i<3;i++){
      //同步牌池
      if(abf[25*i+114]=='t'){
        pool[i].num=abf[25*i+115]-2;
        pool[i].player=abf[25*i+116]-2;
        pool[i].type=abf[25*i+117]-2;
        for(int j=0;j<21;j++){
          if(abf[25*i+118+j]!=1) pool[i].card[j]=abf[25*i+118+j]-2;
        }
      }
    }
  }
  while(playr[0].available()) playr[0].read();
  return 0;
}
uint8_t recv_deal(plyr *player,pcards* pool){ //接收发牌
  Serial.println("接收到发的牌");
  while(!playr[0].available()) {
    yield();
    if(digitalRead(BtnL)==0 && digitalRead(BtnR)==0) return 1;
  }
  delay(10);//等待发送完
  char gotcard[64]="";
  for(int i=0;i<59;i++){
    gotcard[i]=playr[0].read()-2;
  }
  if(gotcard[0]=='b' && gotcard[1]==0){
    player[0].cards.num=17;
    for(int j=0;j<17;j++)
      player[0].cards.card[j]=gotcard[j+2];
    player[1].cards.num=17;
    for(int j=0;j<17;j++)
      player[1].cards.card[j]=gotcard[j+20];
    player[2].cards.num=17;
    for(int j=0;j<17;j++)
      player[2].cards.card[j]=gotcard[j+38];

     pool[1].num=3;
     pool[1].card[0]=gotcard[55];
     pool[1].card[1]=gotcard[56];
     pool[1].card[2]=gotcard[57];
     playr[0].write('d');
  }
  return 0;
}
uint8_t send_call_score(int16_t yourScore){//叫分
  Serial.println("叫分");
  if(yourScore==3) playr[0].print("c3");
  else if(yourScore==2) playr[0].print("c2");
  else if(yourScore==1) playr[0].print("c1");
  else if(yourScore==0) playr[0].print("c0");
  delay(200);
  return 0;
}
//wait_call_score是轮询函数，负责接收叫分信号,不需要发送任何内容
//返回值 0：不需要叫分，1：需要叫分，2~4：叫分完成，5：无人叫分重新发牌
uint8_t wait_call_score(){
  //Serial.println("等待叫分");
  char buff[4]="";
  if(playr[0].available()){
   delay(5);
   char preread=playr[0].read();
   if(preread=='l'){
    for(int i=0;i<3;i++) buff[i]=playr[0].read();
    Serial.print("上一个玩家叫分情况: ");
    Serial.println(buff);//
    base_score=buff[1]-48;
    if(buff[2]<buff[1]){
      if(buff[0]-48==yourID) u8g2.drawUTF8(0,26,ch_calling[0]);
      else if(buff[0]-48==(yourID+1)%3) u8g2.drawUTF8(51,26,ch_calling[0]);
      else if(buff[0]-48==(yourID+2)%3) u8g2.drawUTF8(103,26,ch_calling[0]);
    }
    else if(buff[2]==buff[1]){
      if(buff[0]-48==yourID) u8g2.drawUTF8(0,26,ch_calling[base_score]);
      else if(buff[0]-48==(yourID+1)%3) u8g2.drawUTF8(51,26,ch_calling[base_score]);
      else if(buff[0]-48==(yourID+2)%3) u8g2.drawUTF8(103,26,ch_calling[base_score]);
    }
    u8g2.sendBuffer();
    while(playr[0].available()) playr[0].read();
    if(buff[0]-48==yourID) {
#ifdef DBG_VERSION
if (DBG_MODE) delay(2000);
#endif
      playr[0].write('v');//防止误判为丢包
      delay(120);
      while(playr[0].available()) playr[0].read();//清除多余数据
      return 1;
    }
   }
   else if(preread=='c'){
    Serial.print("获得叫分结束指令");
    p_master=playr[0].read()-48;
    uint8_t base_verify=playr[0].read()-48;
    while(playr[0].available()) playr[0].read();
    Serial.printf("%d %d %d\n",p_master,base_verify,base_score);
    base_score=base_verify;
     if(p_master==(yourID+2)%3) u8g2.drawUTF8(0,26,ch_calling[base_score]);
     else if(p_master==yourID) u8g2.drawUTF8(51,26,ch_calling[base_score]);
     else if(p_master==(yourID+1)%3) u8g2.drawUTF8(103,26,ch_calling[base_score]);
     u8g2.sendBuffer();
     delay(800);
     if (!base_score) {
      Serial.println(5);
      return 5;
     }
     else {
      Serial.println(2+p_master);
      return 2+p_master;
     }
   }
  }
  return 0;
}
uint8_t get_order(plyr *player,pcards* pool){ //获得出牌权 返回1代表获得成功
  //是轮询函数,其他人出牌的话需要改写player和pool
  //若available为0直接跳过
  //同时同步游戏数据,也要能修改五边形的颜色（"20地主"后面的）
  if(playr[0].available()){
    delay(10);
    if(playr[0].read()=='g'){
      p_holder = playr[0].read()-48;
      sync_data(player,pool);
      if(pool[0].type>=15 && (p_holder==(pool[0].player+1)%3 || p_holder>=3)) plex<<=1;//炸弹翻倍
      draw_gui(player,pool);
    }
    while(playr[0].available()) playr[0].read();
    if(p_holder == yourID || p_holder>=3) {//发送校验指令
#ifdef DBG_VERSION
if(DBG_MODE) delay(2000);
#endif
      playr[0].write('v');
      while(playr[0].available()) playr[0].read();//清除多余数据
      return p_holder == yourID;
    }
  }
  return 0;
}
uint8_t send_card(pcards *To,pcards *rest){ //打出去牌
  Serial.println("成功打出牌");
  String willSend="o";
  char sendt[25]="";
  sendt[0]=To->num+2;
  sendt[1]=To->player+2;
  sendt[2]=To->type+2;
  for(int i=0;i<21;i++){
    if(i<To->num) sendt[i+3]=To->card[i]+2;
    else sendt[i+3]=1;
  }
  willSend+=sendt;
  sendt[0]=rest->num+2;
  for(int i=0;i<23;i++){
    if(i<rest->num) sendt[i+1]=rest->card[i]+2;
    else sendt[i+1]=1;
  }
  willSend+=sendt;
  playr[0].print(willSend);
  delay(10);
  return 0;
}
uint8_t game_ended(plyr *player){  //结束并计算得分
  return 0;
}
uint8_t end_connect_to(){//与房主断开
  Serial.println("与主机断开");
  return 0;
}

//----------GUI部分代码----------
void draw_gui(plyr * playing,pcards * pool){
  //输出两个玩家的信息
  uint8_t itv;
  uint8_t cntID=(yourID+2)%3;
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_4x6_tr);
  digitalClockDisplay();
  u8g2.drawStr(54,21,c_a_time);
  u8g2.drawStr(0,5,playing[cntID].name);
  u8g2.setCursor(0,13);
  u8g2.print(playing[cntID].cards.num);
  if(playing[cntID].identity==1) u8g2.drawXBMP(8,7,16,7,BM_DIZHU);
  else u8g2.drawXBMP(8,7,16,7,BM_NONGMIN);
  
  cntID=(yourID+1)%3;
  u8g2.drawStr(104,5,playing[cntID].name);
  u8g2.setCursor(104,13);
  u8g2.print(playing[cntID].cards.num);
  if(playing[cntID].identity==1) u8g2.drawXBMP(112,7,16,7,BM_DIZHU);
  else u8g2.drawXBMP(112,7,16,7,BM_NONGMIN);
  
  u8g2.setCursor(103,45);
  u8g2.print(playing[yourID].cards.num);
  if(playing[yourID].identity==1) u8g2.drawXBMP(111,39,16,7,BM_DIZHU);
  else u8g2.drawXBMP(111,39,16,7,BM_NONGMIN);
  draw_card(pool+2,29,0);

  drawframe(55,0,45,14);
  u8g2.setFont(u8g2_font_6x10_tf);
  u8g2.setCursor(57,11);
  u8g2.print(base_score);
  
  if(pool[1].num>15) itv=6;
  else itv=8;
  if(pool[1].num && playing[0].identity+playing[1].identity+playing[2].identity){
    uint8_t dtx = 0,dty = 0;
    if(pool[1].player==yourID) { 
      dtx=63-(itv>>1)*pool[1].num; 
      dty=27; 
    }
    else if(pool[1].player==(yourID+1)%3) { 
      dtx=127-itv*pool[1].num; 
      dty=16; 
    }
    else if(pool[1].player==(yourID+2)%3) { 
      dtx=0; 
      dty=16; 
    }
    draw_card(pool+1,dtx,dty);
  }
  char plexn[8]="";
  sprintf(plexn,"%3d",plex);
  u8g2.setCursor(72,11);
 u8g2.print(plexn);
 u8g2.drawXBMP(63,3,8,8,BM_SCORE);
  u8g2.drawXBMP(90,3,8,8,BM_MULTP);
  
  //绘制自己的相关信息
  u8g2.drawXBMP(38,39,62,8,BM_PUTOUT);
 u8g2.setCursor(0,46);
  u8g2.print(playing[yourID].name);/*
  if(p_holder!=yourID || pool[2].card[0]==120){
    u8g2.setColorIndex(0);
    for(int i=38;i<100;i++){
      for(int j=39;j<47;j++){
        if((i+j)%2) u8g2.drawPixel(i,j);
      }
    }
    u8g2.setColorIndex(1);
  }
  if(p_holder==yourID && pool[0].num==0){
    u8g2.setColorIndex(0);
    for(int i=64;i<88;i++){
      for(int j=39;j<47;j++){
        if((i+j)%2) u8g2.drawPixel(i,j);
      }
    }
    u8g2.setColorIndex(1);
  }*/
  //绘制玩家是否正在出牌
  if(p_holder==yourID) u8g2.drawBox(123,41,3,4);
  else if(p_holder==(yourID+1)%3) u8g2.drawBox(124,9,3,4);
  else if(p_holder==(yourID+2)%3) u8g2.drawBox(20,9,3,4);
  //绘制牌池
  /**if(p_master==(yourID+2)%3) u8g2.drawUTF8(0,26,ch_calling[base_score]);
     else if(p_master==yourID) u8g2.drawUTF8(51,37,ch_calling[base_score]);
     else if(p_master==(yourID+1)%3) u8g2.drawUTF8(103,26,ch_calling[base_score]);
  绘制位置参考上文 */
  draw_card(&playing[yourID].cards,0,49);
  u8g2.setFont(u8g2_font_wqy12_t_gb2312);
  if(pool[0].num>15) itv=6;
  else itv=8;
  if(pool[0].num){
    int dtx = 0,dty = 0;
    if(pool[0].player==yourID) { 
      dtx=63-(itv>>1)*pool[0].num; 
      dty=27; 
      u8g2.drawHLine(123,43,3);
      if(p_holder==(pool[0].player+2)%3)
      //  u8g2.drawUTF8(103,26,"不出");
      {draw_card(pool,dtx,dty);
      print_buchu(103,26);}
    }
    else if(pool[0].player==(yourID+1)%3) { 
      dtx=127-itv*pool[0].num; 
      dty=16; 
      u8g2.drawHLine(124,11,3);
      if(p_holder==(pool[0].player+2)%3)
      //  u8g2.drawUTF8(0,26,"不出");
      {draw_card(pool,dtx,dty);
      print_buchu(0,26);}
    }
    else if(pool[0].player==(yourID+2)%3) { 
      dtx=0; 
      dty=16; 
      u8g2.drawHLine(20,11,3);
      if(p_holder==(pool[0].player+2)%3)
      //  u8g2.drawUTF8(51,37,"不出");
      {draw_card(pool,dtx,dty);
      print_buchu(51,37);}
    }
    if(p_holder!=(pool[0].player+2)%3){
      if(pool[0].player==yourID)
      for(int j=32;j>=0;j--){
        u8g2.setColorIndex(0);
        u8g2.drawBox(dtx-1,dty+j/2,itv*pool[0].num+3,19);
        u8g2.setColorIndex(1);
        draw_card(pool,dtx,dty+j/2);
        delay(10);
        j-=j/8;
      }
      else
      for(int j=pool[0].num*8-1;j>=0;j--){
        u8g2.setColorIndex(0);
        u8g2.drawBox(dtx-1,dty,itv*pool[0].num+3,16);
        u8g2.setColorIndex(1);
        if(pool[0].player==(yourID+2)%3) draw_card(pool,dtx-j,dty);
        else draw_card(pool,dtx+j,dty);
        if(pool[0].num>1) delay(40/pool[0].num);
        else delay(20);
        j-=j/8;
      }
    }
    else draw_card(pool,dtx,dty);
  }
  //绘制自己的相关信息
  u8g2.setColorIndex(0);
  u8g2.drawBox(0,39,128,25);
  u8g2.setColorIndex(1);
  u8g2.setCursor(103,45);
  u8g2.setFont(u8g2_font_4x6_tr);
  u8g2.print(playing[yourID].cards.num);
  if(playing[yourID].identity==1) u8g2.drawXBMP(111,39,16,7,BM_DIZHU);
  else u8g2.drawXBMP(111,39,16,7,BM_NONGMIN);
  if(p_holder==yourID) u8g2.drawBox(123,41,3,4);
  if(pool[0].player==yourID) u8g2.drawHLine(123,43,3);
  u8g2.drawXBMP(38,39,62,8,BM_PUTOUT);
  u8g2.setFont(u8g2_font_6x10_tf);
 u8g2.setCursor(0,46);
  u8g2.print(playing[yourID].name);
  if(p_holder!=yourID || pool[2].card[0]==120){
    u8g2.setColorIndex(0);
    for(int i=38;i<100;i++){
      for(int j=39;j<47;j++){
        if((i+j)%2) u8g2.drawPixel(i,j);
      }
    }
    u8g2.setColorIndex(1);
  }
  if(p_holder==yourID && pool[0].num==0){
    u8g2.setColorIndex(0);
    for(int i=64;i<88;i++){
      for(int j=39;j<47;j++){
        if((i+j)%2) u8g2.drawPixel(i,j);
      }
    }
    u8g2.setColorIndex(1);
  }
  draw_card(&playing[yourID].cards,0,49);
}
//uint8_t call_score(){//叫分
//  return 0;
//}
void draw_card(pcards *To, int x, int y, uint8_t xpage, uint8_t n, uint8_t selecting, const uint8_t *selected ){
  //xpage代表显示偏移量，如果为2则从第二张牌开始绘制 ,n代表有无选定的牌，
  //selecting代表光标位置(0代表第0张牌)，selected数组代表已选的牌的位置数组
  if(To->num==0) return;
  uint8_t toDraw=To->num-xpage;
  uint8_t itv=8;
  if(toDraw>15) {
    if(y==49) toDraw=15;
    else itv=6;
  }
  u8g2.setFont(u8g2_font_6x10_tf);
  for(int i=0;i<toDraw;i++){
    uint8_t cardID=To->card[i+xpage]/4;
    uint8_t tl=14;
    if(n) {
      if(selected[i+xpage]) {
        y-=1;
        tl++;
      }
    }
    u8g2.drawHLine(x+1,y,itv-1);
    u8g2.drawVLine(x,y+1,tl);
    if(i+xpage==selecting) {
      u8g2.drawBox(x+1,y+1,7,tl);
      u8g2.setColorIndex(0);
    }
    
    if(cardID==7){//点数为10
      u8g2.drawVLine(x+2,y+2,7); u8g2.drawVLine(x+4,y+3,5); u8g2.drawVLine(x+6,y+3,5);
      u8g2.drawPixel(x+5,y+2); u8g2.drawPixel(x+5,y+8);
    }
    else if(cardID==13){
      //小
      u8g2.drawVLine(x+4,y+2,5); u8g2.drawPixel(x+2,y+4);
      u8g2.drawPixel(x+6,y+4); u8g2.drawPixel(x+3,y+6);
      //王
      u8g2.drawHLine(x+2,y+8,5); u8g2.drawPixel(x+4,y+9); u8g2.drawHLine(x+3,y+10,3);
      u8g2.drawPixel(x+4,y+11); u8g2.drawHLine(x+2,y+12,5);
    }
    else if(cardID==14){
      //大
      u8g2.drawHLine(x+2,y+3,5); u8g2.drawPixel(x+4,y+2); u8g2.drawPixel(x+4,y+4);
      u8g2.drawPixel(x+3,y+5); u8g2.drawPixel(x+5,y+5); u8g2.drawPixel(x+2,y+6);
      u8g2.drawPixel(x+6,y+6);
      //王
      u8g2.drawHLine(x+2,y+8,5); u8g2.drawPixel(x+4,y+9); u8g2.drawHLine(x+3,y+10,3);
      u8g2.drawPixel(x+4,y+11); u8g2.drawHLine(x+2,y+12,5);
    }
    else if(cardID>14){
      u8g2.drawGlyph(x+2,y+9,'?');
    }
    else 
      u8g2.drawStr(x+2,y+9,spt[cardID]);
    if(cardID<=12)  {
      for(int gi=0;gi<15;gi++){//绘制花色信息
        if(scob[To->card[i+xpage]&3]&(1<<gi)) {
          u8g2.drawPixel(x+2+gi%3,y+10+gi/3);
          u8g2.drawPixel(x+6-gi%3,y+10+gi/3);
        }
      }
    }
    if(i+xpage==selecting) {
      u8g2.setColorIndex(1);
    }
    
    if(n && selected[i+xpage]) y+=1;
    x+=itv;
  }
  if(n && (selected[To->num-1] || selected[14])) u8g2.drawVLine(x,y,15);
  else u8g2.drawVLine(x,y+1,14);
  u8g2.sendBuffer();
}
pcards get_card(plyr * playing,const pcards * pool){
  //返回值：出去的牌，如果没选择“不出”调用之后你的牌将会减少。
  pcards tar={0};//tar初始为空白牌堆
  tar.num=0;
  uint8_t cur=0,decType=0;
  uint8_t selcardsp[21]={0};
  for(;;){
    yield();
    u8g2.setColorIndex(0);
   u8g2.drawBox(0,48,128,16);
    u8g2.setColorIndex(1);
    if(playing[yourID].cards.num>15){
        u8g2.drawFrame(122,52,6,9);
        if(cur>14 && cur<240){
          u8g2.drawVLine(126,54,5);
         u8g2.drawVLine(125,55,3);
          u8g2.drawPixel(124,56);
        }
        else{
          u8g2.drawVLine(123,54,5);
         u8g2.drawVLine(124,55,3);
          u8g2.drawPixel(125,56);
        }
    }
    if(cur!=241) u8g2.setColorIndex(0);
      u8g2.drawVLine(37,39,8); u8g2.drawVLine(62,39,8);
    if(cur!=241) u8g2.setColorIndex(1);
    if(cur!=242) u8g2.setColorIndex(0);
      u8g2.drawVLine(88,39,8); u8g2.drawVLine(63,39,8);
    if(cur!=242) u8g2.setColorIndex(1);
    if(cur!=243) u8g2.setColorIndex(0);
      u8g2.drawVLine(89,39,8); u8g2.drawVLine(100,39,8);
    if(cur!=243) u8g2.setColorIndex(1);
    draw_card(&playing[yourID].cards,0,49,(cur>14)&&(cur<240)?15:0,1,cur,selcardsp);
    if(digitalRead(BtnM)==0){//选择
      if(cur<240){
        selcardsp[cur]=!selcardsp[cur];
      }
      else if(cur==241){//出牌
        //若符合要求则退出并发送
          decType=check_legal(&playing[yourID].cards,pool,selcardsp);
          if(decType==0){//检验出的牌是否合法,若不合法则重新开始
            for(int ki=0;ki<21;ki++) selcardsp[ki]=0;
            continue;
          }
        break;
      }
      else if(cur==243){//查看比分
        while(digitalRead(BtnM)==0) yield();
        for(int j=15;j>=1;j--){
          u8g2.setColorIndex(0);
         u8g2.drawBox(0,48,128,16);
        u8g2.setColorIndex(1);
       u8g2.setFont(u8g2_font_4x6_tr);
      u8g2.setCursor(0,54+j); u8g2.print("Current Scores:");
     u8g2.setCursor(64,54+j); u8g2.print(playing[0].name);
    u8g2.print(": "); u8g2.print(playing[0].score);
     u8g2.setCursor(0,62+j); u8g2.print(playing[1].name);
      u8g2.print(": "); u8g2.print(playing[1].score);
       u8g2.setCursor(64,62+j); u8g2.print(playing[2].name);
        u8g2.print(": "); u8g2.print(playing[2].score);
         u8g2.sendBuffer();
          if(j>5) j--;
           delay(10);
        }
        while(digitalRead(BtnM)==0) yield();
        while(digitalRead(BtnM)==1) yield();
        delay(30);
      }
      
      while(digitalRead(BtnM)==0) yield();
      delay(50);
      if(cur==242) return tar;
    }
    if(digitalRead(BtnL)==0){//左移
      if(cur==241) cur=playing[yourID].cards.num-1;
      else if(cur>0) cur--;
      else cur=243;
      if(cur==242 && pool[0].num==0) cur--;
      delay(150);
    }
    if(digitalRead(BtnR)==0){//右移
      if(cur==playing[yourID].cards.num-1) cur=241;
      else if(cur==243) cur=0;
      else cur++;
      if(cur==242 && pool[0].num==0) cur++;
      delay(150);
    }
  }
  //在自己的牌中扣除相关牌
  uint8_t ji=0;
  for(uint8_t i=0;i<21;i++){
    if(selcardsp[i]){
      shiftOne(&playing[yourID].cards,&tar,i-ji);
      ji++;
      //向左移动一位，参数为需要被移走的位置,并写入到新的cards变量中
      Serial.printf("第 %d 张已出\n",i);
    }
  }
  tar.player=yourID;
  tar.type=decType;
  //同样操作也需要在主机中进行，发送时也要发送自己剩余的牌以供参考
  while(digitalRead(BtnM)==0) yield();
  return tar;
}
void randomCards(pcards *pc1,pcards *pc2,pcards *pc3,pcards *rsv){
  randomSeed(millis());
  delayMicroseconds(random(3,248));
  int restCard =54;
  int skipFlag=0;
  uint8_t target_card=0;
  uint8_t gccards[54]={0};
  pc1->num=17;
  pc2->num=17;
  pc3->num=17;
  rsv->num=3;
  for(int i=0;i<restCard;i++){
    int get_rand = (micros()%31287+random(0,238)) % (restCard-i) ;
    for(int j=0;j<restCard-i;j++){
      while(gccards[j+skipFlag]==1) skipFlag++;
      if(j==get_rand){
        target_card=j+skipFlag;
        gccards[j+skipFlag]=1;
        break;
      }
    }
    if(target_card==53) target_card=57;
    if(i<51){
      if(i%3==0) pc1->card[i/3]=target_card;
      if(i%3==1) pc2->card[i/3]=target_card;
      if(i%3==2) pc3->card[i/3]=target_card;
    }
    else rsv->card[i-51]=target_card;
    skipFlag=0;
  }
}
void takeOrder(pcards *pc){
  uint8_t len=pc->num;
    uint8_t temp;
    uint8_t i, j;
    for (i=0; i<len-1; i++){ /* 外循环为排序趟数，len个数进行len-1趟 */
        for (j=0; j<len-1-i; j++) { /* 内循环为每趟比较的次数，第i趟比较len-i次 */
            if (pc->card[j] < pc->card[j+1]) { /* 相邻元素比较，若逆序则交换（升序为左大于右，降序反之） */
                temp = pc->card[j];
                pc->card[j] = pc->card[j+1];
                pc->card[j+1] = temp;
            }
        }
    }
}
void disp_status(const char* st,uint8_t ry){
  u8g2.setFont(u8g2_font_wqy12_t_gb2312);
  if(ry==37){
    for(int i=15;i>0;i--){
      u8g2.setColorIndex(0);
      u8g2.drawBox(28,ry-10,100,11);
      u8g2.setColorIndex(1);
      u8g2.drawUTF8(28+i*i/2,ry,st);
      u8g2.sendBuffer();
      delay(10);
    }
  }
  else{
    for(int i=0;i<12;i++){
      u8g2.setColorIndex(0);
      u8g2.drawBox(0,ry-15,128,16);
      u8g2.setColorIndex(1);
      u8g2.drawUTF8(0,ry+9-i,st);
      u8g2.sendBuffer();
      delay(10);
    }
  }
  u8g2.sendBuffer();
}
void shiftOne(pcards *zfrom,pcards *zto,int where){//移位寄存
  zto->card[zto->num]=zfrom->card[where];
  for(int i=where;i<zfrom->num;i++) zfrom->card[i]=zfrom->card[i+1];
  zto->num++; zfrom->num--;
}
//让你不再不讲武德的代码，非   常   难   写 
uint8_t check_legal(pcards *c,const pcards *po,uint8_t *selsp){//检验合法性
#ifdef DBG_VERSION
  return 1;
#endif
  // c代表你的牌,po代表牌桌上的牌,selsp代表选定的牌
  uint8_t wcard[21]={0};
  uint8_t selnum=0,rtype=0;//rtype为当前牌型
  print_card(po);
  for(int i=0;i<c->num;i++){//处理牌的张数
    if(selsp[i]) {
      wcard[selnum] = c->card[i]/4;
      selnum++;
    }
  }
    //在这里处理牌型
    /*type:指牌桌上面的牌是什么牌型 
     * 0：非法牌型  1：单张、顺子  2：一对 、联队
     * 3：三张、飞机无翅膀 
     * 4：3带1
     * 5：3带2
     * 6：4带2、7.4带2对  
     8.飞机（每3张带1张） 9.飞机（每3张带1对）
     15：炸弹 16：王炸 */
  if(selnum==2 && wcard[0]>=13 && wcard[1]>=13) return 16;//是王炸
  if(selnum==1) rtype=1;//单张
  else if(selnum==2 && wcard[0]==wcard[1]) rtype=2;//顺子
  else if(selnum==3 && wcard[0]==wcard[1]&& wcard[0]==wcard[2]) rtype=3;//s三张
  else if(selnum==4){
    if(wcard[0]==wcard[1] && wcard[0]==wcard[2] && wcard[0]==wcard[3]) rtype=15;
    //检测到3带1
    else if(wcard[0]==wcard[1] && wcard[0]==wcard[2] && wcard[0]!=wcard[3]) rtype=4;
    else if(wcard[0]!=wcard[1] && wcard[1]==wcard[2] && wcard[1]==wcard[3]) rtype=4;
  }
  else if(selnum==5){
    //检测到3带2
    if(wcard[0]==wcard[1] && wcard[0]==wcard[2] && wcard[4]==wcard[3]) rtype=4;
    else if(wcard[0]==wcard[1] && wcard[4]==wcard[2] && wcard[4]==wcard[3]) rtype=4;
    else rtype=isslide(wcard,selnum);
  }
  else if(selnum>5) rtype=isslide(wcard,selnum);//是否为顺子或连对或飞机
 if(po->num>0){
  if(selnum==4 && wcard[0]==wcard[1] && wcard[0]==wcard[2] && wcard[0]==wcard[3]){
    if(po->num==4 && po->card[0]/4==po->card[1]/4 && po->card[0]/4==po->card[2]/4 
    && po->card[0]/4==po->card[3]/4 && wcard[0]<=po->card[0]/4) return 0;//是炸弹
    else if(po->num==2 && po->card[0]>51 && po->card[1]>51) return 0;//是王炸
    else return 15;
  }
  if(po->num!=selnum || rtype!=po->type) return 0;//张数和形式与牌池不符
  else {//比大小
    if(rtype<=3 && wcard[0]>po->card[0]/4) return rtype;//单张或对子或3张
    else if((rtype==4 || rtype==5) && wcard[2]>po->card[2]/4) return rtype;//3带n
    else if(rtype==6 && wcard[3]>po->card[3]/4) return rtype;//4带2
    else if(rtype==7){
      uint8_t tpo=0,tpo2=0;//4带2对
      if(wcard[0]==wcard[2]) tpo=wcard[0];
      else if(wcard[4]==wcard[2]) tpo=wcard[2];
      else tpo=wcard[4];
      if(po->card[0]/4==po->card[2]/4) tpo2=po->card[0]/4;
      else if(po->card[2]/4==po->card[4]/4) tpo2=po->card[2]/4;
      else tpo2=po->card[4]/4;
      if(tpo>tpo2) return rtype;//4带2对检验完成
    }
    else if(rtype==8 || rtype==9){
      //比较中间一张牌
      if(wcard[selnum/2] > po->card[selnum/2]/4) return rtype;
    }
    return 0;
  }
 }
 //else if(selnum==0 && rtype==0) return 0;
 else return rtype;//如果牌池为空且不成形
}
uint8_t isslide(uint8_t* card,uint8_t num){
  //3 0 4 1 5 2 6 3 7 4 8 5 9 6 10 7 j 8 q 9 k 10 a 11 超过范围不考虑
  uint8_t cardsent[12]={0};//存储12种牌每种有几张
//如777888为2组3张，即cardsnum[3]==2；如55667788为4组2张，即cardsnum[2]==4
  uint8_t ttotal=0,tkind=0,tnew=255,tstable=0 /*,breakfg=0*/ ;
  for(uint8_t i=0;i<num;i++)
    if(card[i]<12) cardsent[card[i]]++;
  for(uint8_t i=0;i<12;i++){
    if(cardsent[i]){
      ttotal+=cardsent[i];
      tkind+=1;
      if(tnew==255 || i-tnew==1){
        tnew=i;
        tstable++;
      }
    }
  }
  if(tstable==tkind && num==tkind) return 1;
  else if(tstable==tkind && num==tkind*2) return 2;
  else if(tstable==tkind && num==tkind*3) return 3;
/*for(int k=1;k<4;k++){j=i;if(cardsent[j]==k){while(cardsent[j]==k){j++;ttotal++;}if(ttotal*k==num) {breakfg=k;}}}*/
  if(num==8){
    for(int i=0;i<3;i++){
      if(isslide(card+i,6)==3) return 8;
    }
  }
  else if(num==6){//4带2 rtn 6
    for(int i=0;i<12;i++){
      if(cardsent[i]==4) return 6;
    }
  }
  else if(num==8){//4带2对 rtn 7
    for(int i=0;i<12;i++){
      if(cardsent[i]==4 && card[0]==card[1] && card[2]==card[3] && card[4]==card[5] && card[6]==card[7]) return 7;
    }
  }
  else if(num==12){//345666777888
    for(int i=0;i<4;i++){
      if(isslide(card+i,9)==3) return 8;
    }
  }
  else if(num==16){
    for(int i=0;i<5;i++){
      if(isslide(card+i,12)==3) return 8;
    }
  }
  else if(num==10){
    for(int i=0;i<3;i++){
      if(isslide(card+2*i,6)==3) return 9;
    }
  }
  else if(num==15){
    for(int i=0;i<4;i++){
      if(isslide(card+2*i,9)==3) return 9;
    }
  }
  else if(num==20){
    for(int i=0;i<6;i++){//34567888999101010jjjqqq
      if(isslide(card+i,15)==3) return 8;
    }
    for(int i=0;i<5;i++){//33445566777888999101010
      if(isslide(card+2*i,12)==3) return 9;
    }
  }
  return 0;
}
uint8_t strToInt(int32_t *regint,char *src){//src有6个字符，结尾无\0
  *regint=0;
  uint8_t sig=0;
  for(int i=0;i<6;i++){
    if(src[i]>=48 && src[i]<=57){
      (*regint)*=10;
      (*regint)+=src[i]-48;
    }
    else if(src[i]=='-') sig=1;
  }
  if(sig) (*regint)*=-1;
  return 1;
}
void disScores(plyr *player,int32_t *scdelta){
  char su[3][32]={"","",""};
    for(int i=0;i<3;i++)
      Serial.printf("玩家 %s 得分: %6d %3d\n",player[i].name,player[i].score,scdelta[i]);
  //绘制相关内容
  sprintf(su[0],"玩家%s:%d %d分",player[0].name,scdelta[0],player[0].score);
  sprintf(su[1],"玩家%s:%d %d分",player[1].name,scdelta[1],player[1].score);
  sprintf(su[2],"玩家%s:%d %d分",player[2].name,scdelta[2],player[2].score);
  drawList("点击准备 当前得分:",su[0],su[1],su[2],1);
  scdelta[0]=0; scdelta[1]=0; scdelta[2]=0; 
}
/* 0代表黑桃3，1代表红桃3，2代表梅花3，3代表方块3
 * 点数为牌的uint8_t值/4+3
 * 52、57代表小、大王
 */
void print_card(const pcards * ccard){
  for(int ti=0;ti<(ccard->num);ti++){
    Serial.print(sco[ccard->card[ti]%4]);
    Serial.print(spt[ccard->card[ti]/4]);
    Serial.print(' ');
  }
}
void print_buchu(uint8_t dtx,uint8_t dty){
  u8g2.setFont(u8g2_font_wqy12_t_gb2312);
      for(int j=24;j>=0;j--){//dty 20
        u8g2.setColorIndex(0);
        if(dtx<4 && dty<30) u8g2.drawBox(dtx-j-1,dty-11,26,16);
        else if(dty<30) u8g2.drawBox(dtx+j-1,dty-11,26,16);
        else u8g2.drawBox(dtx-1,dty+j/2-11,26,16);
        u8g2.setColorIndex(1);
        if(dtx<4 && dty<30) u8g2.drawUTF8(dtx-j,dty,"不出");
        else if(dty<30) u8g2.drawUTF8(dtx+j,dty,"不出");
        else u8g2.drawUTF8(dtx,dty+j/2,"不出");
        u8g2.sendBuffer();
        delay(18);
        j-=j/6;
      }
}
void dodizhu(){
  //斗地主游戏
  //首先决定游戏模式（谁是房主）
  /* u8g2.clearBuffer();
  u8g2.drawXBMP(18,17,92,38,ddz_logo);
  u8g2.sendBuffer();
  #ifndef DBG_VERSION
  delay(1000);
  #endif  */
#ifdef DBG_VERSION
  DBG_MODE =(!digitalRead(BtnR));
#endif
  uint8_t ddzmode=listdraw("斗地主OL 局域网版",ddz_modes,4);
  if(ddzmode==255) return;
  uint8_t quit_flag=0;
  char play_ip[16]="192.168.10.10";
  
  plyr players[3]={0};
  players[0].score=100;
  players[1].score=100;
  players[2].score=100;
  pcards pool[3]={0};//牌池
  int32_t lastdelta[3]={0};//得分变化
if(ddzmode==1){//服务器模式
  yourID=0;
  //服务器本身为玩家0，首先连接的为玩家1，最后连接的为玩家2
  drawList("","请输入你的昵称","最多6个字母","留空将自动退出");
  #ifdef DBG_VERSION
  strcpy(players[0].name,"server");
  #else
  getchs(players[0].name,7);
  #endif
  if(players[0].name[0]==0 || start_connect(players)==1) return;//先匹配玩家,未匹配到时退出
  /*玩家位置：    2号玩家    1号玩家
   *                  0号玩家(服务器)*/
  disScores(players,lastdelta);
  while(!quit_flag){
    base_score=0; plex=1; p_master=0; p_holder=0;
    players[0].identity=0; players[1].identity=0; players[2].identity=0;
    pool[0].num=0;  pool[1].num=0;  pool[2].num=3; 
    pool[2].card[0]=120;  pool[2].card[1]=120;  pool[2].card[2]=120;
    while(digitalRead(BtnM)==1) {
        if(digitalRead(BtnL)==0) {
          disp_status("长按左键将会退出游戏!");
          delay(1800);
          if(digitalRead(BtnL)==0) {
            quit_flag=1;
            break;
          }
          }
        yield();
    }
    if(quit_flag) break;
    while(digitalRead(BtnM)==0) yield();
    delay(24);
    draw_gui(players,pool);
    if(wait_ready_s(players)) return;//此时收到重连信号时为此信号同步
    disp_status("全员准备就绪！");
    delay(700);
    winner=(winner+1)%3;
    for(;;){
      disp_status("正在发牌...");
      serve_deal(players,pool+1);//发牌，需要加入响应代码
      yield();
      draw_gui(players,pool);
      p_master = serve_call_score(players);
      //叫分，需要加入响应代码,并处理地主信息和基础分
      //除了第一次是房主叫分，以后每次都是上一把的赢家叫分
      //如果上一把地主赢了则由地主的下家叫分
      //此函数需要写入base_score变量
      p_holder=p_master;
      if(base_score) break;//所有玩家不叫分时重新运行循环,有人叫分则继续进行
      delay(500);
    }
    pool[2]=pool[1];
    pool[1].num=0;
    players[p_master].cards.num=20;
    players[p_master].cards.card[17]=pool[1].card[0];
    players[p_master].cards.card[18]=pool[1].card[1];
    players[p_master].cards.card[19]=pool[1].card[2];
    takeOrder(&players[p_master].cards);//排序，为接下来的工作做准备
    uint8_t no_card_recv=0;
    for(;;){
      draw_gui(players,pool);
      uint8_t thisgame = serve_card(players,pool);
      //出牌不符合要求需要重新出牌
      //serve_card函数返回1代表符合要求
      //serve_card函数返回0代表出完牌了
      //serve_card函数返回2代表玩家不出牌
      if(thisgame==0) {
        //读写分数
        Serial.println("本局结束，");
        if(players[(p_master+1)%3].cards.num==17 && players[(p_master+2)%3].cards.num==17) plex*=3;
        draw_gui(players,pool);
        for(int ti=0;ti<3;ti++){
         if(players[ti].cards.num==0){
          winner=ti;
          if(players[ti].identity==1) {
           players[ti].score+=(lastdelta[ti]=2*base_score*plex);
           players[(ti+1)%3].score+=(lastdelta[(ti+1)%3]=-base_score*plex);
           players[(ti+2)%3].score+=(lastdelta[(ti+2)%3]=-base_score*plex);
          }
          else{
           players[p_master].score+=(lastdelta[p_master]=-2*base_score*plex);
           players[(p_master+1)%3].score+=(lastdelta[(p_master+1)%3]=base_score*plex);
           players[(p_master+2)%3].score+=(lastdelta[(p_master+2)%3]=base_score*plex);
          }
         }
        }
        serve_card(players,pool,1);
        delay(3000);
        disScores(players,lastdelta);
        break;
      }
      
      //如果连续两家不出牌则清空牌池
      if(thisgame==2 ) no_card_recv++;
      if(thisgame==1 ) no_card_recv=0;
      if(no_card_recv==2) {
        pool[0].num=0; pool[0].type=0;
        pool[1].num=0; pool[1].type=0;
        no_card_recv=0;
      }
      //轮到下家出牌
      p_holder=(p_holder+1)%3;
      delay(50);
    }
    //在此处理玩家赢了之后的事情
  }
  end_connect();//结束连接
}
else if(ddzmode==0){
  uint8_t eventing=0;
  drawList("","请输入房主的IP","连接到目标服务器","");
  getchs(play_ip,16);
  drawList("","请输入你的昵称","最多6个字母","留空将自动退出");
  #ifdef DBG_VERSION
  strcpy(players[0].name,"client");
  #else
  getchs(players[0].name,7);
  #endif
  if(players[0].name[0]==0 || play_ip[0]==0 ||
    (yourID=start_connect_to(play_ip,SERVER_PORT,players))==0) return;//先匹配玩家,未匹配到时退出
  //自己退出和连接失败均视为返回值0
  //成功则返回由服务器获取的玩家ID（1或者2）,同时设置你的昵称。
  //若为断线重连，返回3或4，将在之后接收到所有游戏数据
  
  disScores(players,lastdelta);
  while(!quit_flag){
    base_score=0; plex=1; p_master=0; p_holder=0;
    players[0].identity=0; players[1].identity=0; players[2].identity=0;
    pool[0].num=0;  pool[1].num=0;  pool[2].num=3; 
    pool[2].card[0]=120; pool[2].card[1]=120; pool[2].card[2]=120;
    if(yourID>=3){//返回>3代表游戏进行中但是你掉线了
      yourID-=2;
      if((eventing=sync_data(players,pool))==0) {//同步信息，若失败(返回0)则退出游戏
        drawList("同步消息失败","服务器地址:",play_ip,"可以尝试重新连接",0,0);
        break;
        //eventing : 0.同步失败
        //1.等待准备，此时可以加入游戏
        //2.等待叫分
        //3.等待出牌
      }
    }
    if(eventing==1 || eventing==0){ //表示游戏未开始，正常流程
      //此时eventing为零代表正常进入游戏
      while(digitalRead(BtnM)==1) {
        if(digitalRead(BtnL)==0) {
          disp_status("长按左键将会退出游戏!");
          delay(1800);
          if(digitalRead(BtnL)==0) {
            quit_flag=1;
            break;
          }
          }
        yield();
      }
      if(quit_flag) break;
      while(digitalRead(BtnM)==0) yield();
      delay(24);
      eventing=0;
      send_ready(); //开环函数只发送不接收
      while(1){
        draw_gui(players,pool);
        yield();
        disp_status("等待主机响应...");
        if(!wait_ready()) break; //返回0代表成功匹配到玩家
        //若主机未就绪则返回1
        disp_status("主机未响应,重试or退出?");
        while(digitalRead(BtnM)==1 &&digitalRead(BtnL)==1) yield();
        if(digitalRead(BtnL)==0) return;
        while(digitalRead(BtnM)==0) yield();
        delay(24);
        send_ready();
      }
    }
    yield();
    
    //发牌并叫分
    while(eventing==2 || eventing==0){
      uint8_t scoreState=0;
      disp_status("正在等待其他玩家准备");
      if(recv_deal(players,pool)) break;
      //此时eventing为零代表正常进入游戏
      draw_gui(players,pool);
      disp_status("等待别人叫分",37);
      while(1){
        scoreState=wait_call_score();
        //wait_call_score是轮询函数，负责接收叫分信号
        //wait_call_score可以在屏幕上显示叫分状态
        //返回值 0：不需要叫分，1：需要叫分，2：叫分完成，3：无人叫分重新发牌
        delay(20);
        
        if(scoreState>=2) break;
        if(scoreState==1){
        int16_t s=0;
        do{
          //draw_gui(players,pool);
          disp_status("按确定键叫分",37);
          while(digitalRead(BtnM)==1) {
            u8g2.setColorIndex(0);
            u8g2.drawBox(0,48,128,16);
            u8g2.setColorIndex(1);
            draw_card(&players[yourID].cards,0,49,15*(digitalRead(BtnR)==0 ||digitalRead(BtnL)==0));
            yield();
          }
          while(digitalRead(BtnM)==0) yield();
          delay(24);
        
          s=slider("选择想要叫的分",3);
          draw_gui(players,pool);
          disp_status("等待别人叫分",37);
        }while(s==-32768);
        send_call_score(s);
        }
      }
      if(scoreState!=5){
        players[p_master].identity=1;
        players[p_master].cards.num=20;
        if(yourID==p_master) {
          players[p_master].cards.card[17]=pool[1].card[0];
          players[p_master].cards.card[18]=pool[1].card[1];
          players[p_master].cards.card[19]=pool[1].card[2];
          takeOrder(&players[p_master].cards);
        }
        pool[2]=pool[1];//显示底牌
        pool[1].num=0;
        break;
      }
      //while(playr[0].available()) playr[0].read();
      draw_gui(players,pool);
      eventing=0;
    }
    yield();
    uint32_t send_flag=0;
    pcards willSend;
    //正式出牌
    while(eventing==3 || eventing==0){
      //此时eventing为零代表正常进入游戏

      if(get_order(players,pool)){//1代表成功获得出牌权
        willSend=get_card(players,pool);
        send_card(&willSend,&players[yourID].cards);
        send_flag=millis();//上次发送时间，若检测到时间超过可等待时间就重新发送
      }
      else if(p_holder>=3) {//游戏结束
        winner=p_holder-3;//分离出真正的赢家
        if(players[(p_master+1)%3].cards.num==17 && players[(p_master+2)%3].cards.num==17) plex*=3;
        draw_gui(players,pool);
        //显示得分板(待补充)
        for(int ti=0;ti<3;ti++){
         if(players[ti].cards.num==0){
          winner=ti;
          if(players[ti].identity==1) {
           lastdelta[ti]=2*base_score*plex;
           lastdelta[(ti+1)%3]=-base_score*plex;
           lastdelta[(ti+2)%3]=-base_score*plex;
          }
          else{
           lastdelta[p_master]=-2*base_score*plex;
           lastdelta[(p_master+1)%3]=base_score*plex;
           lastdelta[(p_master+2)%3]=base_score*plex;
          }
         }
        }
        delay(2990);
        disScores(players,lastdelta);
        break;
      }
      else {//在不是你的出牌阶段中显示你的牌并在右侧显示状态
        //画一些别的东西,有动画
        u8g2.setColorIndex(0);
        u8g2.drawBox(0,48,128,16);
        u8g2.setColorIndex(1);
        uint8_t motionS=(millis()/50)%18;
          u8g2.drawPixel(zx1[motionS],zy1[motionS]);
          u8g2.drawPixel(zx1[motionS+1],zy1[motionS+1]);
          u8g2.drawPixel(zx1[motionS+2],zy1[motionS+2]);
        if(players[yourID].cards.num>15){
        if(digitalRead(BtnR)==0 ||digitalRead(BtnL)==0){
          u8g2.drawVLine(126,51,5);
          u8g2.drawVLine(125,52,3);
          u8g2.drawPixel(124,53);
        }
        else{
          u8g2.drawVLine(124,51,5);
          u8g2.drawVLine(125,52,3);
          u8g2.drawPixel(126,53);
        }
        draw_card(&players[yourID].cards,0,49,15*(digitalRead(BtnR)==0 ||digitalRead(BtnL)==0));
        }
        else draw_card(&players[yourID].cards,0,49); 
        if(digitalRead(BtnM)==0 && digitalRead(BtnR)==0) continue;
      }
        //检验是否发送完成
        if(send_flag){
          if(p_holder==(yourID+1)%3 || p_holder>=3){//发送成功
            willSend.num=0;
            send_flag=0;
          }
          else if(millis()-send_flag>REQTIMEOUT){//超时了
            //重新发送
            send_card(&willSend,&players[yourID].cards);
            send_flag=millis();
          }
        }
      //yield();
        delay(10);
    }
    /* //在此处理玩家赢了之后的事情
    if((eventing=sync_data(players,pool))==1){ //每一局结束后需要校验数据
      //返回1代表本局已结束
      //draw_gui(players,pool);
      Serial.println("每局结束后的校验完成！");
    }*/
    
  }
  end_connect_to();
}
else if(ddzmode==2)
  scroll_string(ddznote_str);
else if(ddzmode==3)
  ctg_update();
}
