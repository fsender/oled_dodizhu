#include "wifi_funs.h"
/*
char t_ext[256] =" ";
char host[64] = "api.seniverse.com";
char APIKEY[64] = "Your ApiKey";        //API KEY
char city[CITY_LENGTH] = "Your City";
const char* language = "zh-Hans";//zh-Hans 简体中文  会显示乱码
const char* dayname[DAYS]={"今","明","后"};
*/
char ntpServerName[64] = "cn.ntp.org.cn";
const int timeZone = 8;//北京时间在东8区

const unsigned long BAUD_RATE = 115200;   // serial connection speed
const unsigned long HTTP_TIMEOUT = 5000;  // max respone time from server
//const size_t MAX_CONTENT_SIZE = 4000;   // max size of the HTTP response
 
  /*
WiFiClient client;
char response[MAX_CONTENT_SIZE];
char endOfHeaders[] = "\r\n\r\n";
*/
 WiFiUDP Udp;
 unsigned int localPort = 1337;

char c_a_time[10]="";
char c_a_date[16]="";
const char *c_a_weekname[7]={
  "SUN","MON","TUE","WED","THU","FRI","SAT"
};
uint8_t c_a_week=0;

/**
 * @return 0 :已连接
 * 1 :连接失败
 * 2 :ssid文件校验失败
 * 3 :psk文件校验失败
 * 4 :文件错误导致的连接失败
 * 5 :文件错误导致的连接失败并需要重试
 */


uint8_t connect_to_WiFi(uint8_t useFile){
  File cfgFile;
  uint8_t f_exist=0;
  WiFi.disconnect();
  if(useFile==1){
    uint8_t retry=0;
    f_exist=SPIFFS.exists(wifi_config_file);
    if(f_exist){
      char c_SSID[64]="";
      char c_PSK[32]="";
      uint8_t ssidl=0;
      uint8_t pskl=0;
      uint8_t chksm=0;
      
      cfgFile = SPIFFS.open(wifi_config_file,"r");
      if(cfgFile.read()!='s') chksm=1;
      ssidl=(cfgFile.read()-48)*10;
      ssidl+=(cfgFile.read()-48);
      cfgFile.read();
      for(int i=0;i<ssidl;i++){
        c_SSID[i]=cfgFile.read();
      }
      c_SSID[ssidl]=0;
      if(cfgFile.read()!='p') chksm=2;
      pskl=(cfgFile.read()-48)*10;
      pskl+=(cfgFile.read()-48);
      cfgFile.read();
      for(int i=0;i<pskl;i++){
        c_PSK[i]=cfgFile.read();
      }
      c_PSK[pskl]=0;
      cfgFile.close();
      if(chksm>=1) return chksm+1;
      
      //Serial.printf("SSID readed: %s\n",c_SSID);
      //Serial.printf("PSK  readed: %s\n",c_PSK);
      
      WiFi.begin(c_SSID,c_PSK);
      uint8_t lp=0;
      while (WiFi.status() != WL_CONNECTED){
        u8g2.clearBuffer();
        u8g2.setFont(u8g2_font_wqy12_t_gb2312);
        u8g2.setCursor(0,32);
        u8g2.print(F("正在连接到已存的WiFi"));
        u8g2.setCursor(0,44);
        u8g2.print(c_SSID);
        for(uint8_t i=0;i<32;i++){
          u8g2.setColorIndex(0);
          u8g2.drawHLine(32,60,64);
          u8g2.setColorIndex(1);
          u8g2.drawHLine(i+32,60,32);
          u8g2.sendBuffer();
          delay(1);
        }
        for(uint8_t i=0;i<32;i++){
          u8g2.drawHLine(32,60,64);
          u8g2.setColorIndex(0);
          u8g2.drawHLine(i+32,60,32);
          u8g2.setColorIndex(1);
          u8g2.sendBuffer();
          delay(1);
        }
        lp++;
        if(lp>=WIFI_TIMEOUT) {
          drawList("未连接到WiFi:",c_SSID,"可能已存的WiFi无信号","退出    重试    扫描",1);
          while(digitalRead(D1) && digitalRead(D4) && digitalRead(D8)) yield();
          if(!digitalRead(D4)) retry=1;
          if(!digitalRead(D8)) retry=2;
          while(!digitalRead(D1) || !digitalRead(D4) || !digitalRead(D8)) yield();
          delay(30);
          if(retry==1) return 5;
          if(retry==2) break;
          return 4;
        }
      }
      if (WiFi.status() == WL_CONNECTED) {
        drawList("","已经连接到WiFi:",c_SSID,"请按确定键继续");
		return 0;
      }
      if(!retry) return 0;
    }
  }
  while(1){
    yield();
    drawList("","正在扫描有效WiFi...","","",1);
  WiFi.scanNetworks(true,true);
  delay(1000);
  // print out Wi-Fi network scan result uppon completion
  int n = WiFi.scanComplete();
  Serial.println(n);
  if(n >= 0){
  Serial.printf("%d network(s) found\n", n);
    yield();
  if(n>WIFINAME_MAX) n=WIFINAME_MAX;
  for (int i = 0; i < n; i++){
     Serial.printf("%d: %s, Ch:%d (%ddBm) %s\n", i+1, WiFi.SSID(i).c_str(), WiFi.channel(i), WiFi.RSSI(i), WiFi.encryptionType(i) == ENC_TYPE_NONE ? "open" : "");
    strncpy(wifi_names[i],WiFi.SSID(i).c_str(),WIFINAME_LENMAX-1);
    wifi_names[i][WIFINAME_LENMAX-1]=0;
    c_items[i]=wifi_names[i];
  }
  c_items[n]=C_REFRESH;
  uint8_t intts=listdraw(c_title,c_items,n+1);
  if(intts==n) continue;
  else if(intts==255){
    WiFi.scanDelete();
    if(useFile==2 && SPIFFS.exists(wifi_config_file)) connect_to_WiFi(1);
    return 1;
  }
  else{
    char info_v[32]={0};
    const char* w_enType[8]={
      "WEP","","WPA PSK","开放","","WPA2 PSK","WPA/WPA2","未知"
    };
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_wqy12_t_gb2312);
    u8g2.setCursor(0,14);
    u8g2.print(WiFi.SSID(intts));
    info_v[0]=0;
    sprintf(info_v,"信号强度: %d dbm",WiFi.RSSI(intts));
    u8g2.setCursor(0,27);
    u8g2.print(info_v);
    info_v[0]=0;
    uint8_t entype =WiFi.encryptionType(intts);
    if(entype<2 && entype>8 ) entype=7;
    else entype-=2;
    sprintf(info_v,"加密方式: %s",w_enType[entype]);
    u8g2.setCursor(0,39);
    u8g2.print(info_v);
    info_v[0]=0;
    sprintf(info_v,"网络通道号: %d",WiFi.channel(intts));
    u8g2.setCursor(0,51);
    u8g2.print(info_v);
    u8g2.setCursor(0,63);
    u8g2.print(F("确认输入密码并连接"));
    u8g2.sendBuffer();
    while(digitalRead(D4)!=0) {
      if(digitalRead(D1)==0||digitalRead(D1)==0) {
        entype=10;
        break;
      }
      yield();
    }
    if(entype==10) continue;
  }
  char wifi_pwd[32]={0};
  getchs(wifi_pwd,32);
  WiFi.begin(WiFi.SSID(intts),wifi_pwd);
  uint8_t lp=0;
  while (WiFi.status() != WL_CONNECTED){
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_wqy12_t_gb2312);
    u8g2.setCursor(0,32);
    u8g2.print(F("正在连接到WiFi"));
    u8g2.setCursor(0,44);
    u8g2.print(WiFi.SSID(intts));
    for(uint8_t i=0;i<32;i++){
      u8g2.setColorIndex(0);
      u8g2.drawHLine(32,60,64);
      u8g2.setColorIndex(1);
      u8g2.drawHLine(i+32,60,32);
      u8g2.sendBuffer();
      delay(1);
    }
    for(uint8_t i=0;i<32;i++){
      u8g2.drawHLine(32,60,64);
      u8g2.setColorIndex(0);
      u8g2.drawHLine(i+32,60,32);
      u8g2.setColorIndex(1);
      u8g2.sendBuffer();
      delay(1);
    }
    lp++;
    if(lp>=WIFI_TIMEOUT) {
      uint8_t retry=0;
      drawList("未连接到WiFi:",WiFi.SSID(intts).c_str(),"可能是密码错误","退出    扫描",1);
      while(digitalRead(D1) && digitalRead(D4)) yield();
      if(digitalRead(D1)) retry=1;
      while(!digitalRead(D1) || !digitalRead(D4)) yield();
      delay(30);
      if(retry) break;
      WiFi.scanDelete();
      if(useFile==2) connect_to_WiFi(1);
      return 1;
    }
  }
  if (WiFi.status() == WL_CONNECTED) {
    drawList("","已经连接到WiFi:",WiFi.SSID(intts).c_str(),"请按确定键继续");
    if(useFile){
      cfgFile=SPIFFS.open(wifi_config_file,"w");
      cfgFile.write('s');
      cfgFile.write(strlen(WiFi.SSID(intts).c_str())/10+48);
      cfgFile.write(strlen(WiFi.SSID(intts).c_str())%10+48);
      cfgFile.write(' ');
      cfgFile.print(WiFi.SSID(intts).c_str());
      
      cfgFile.write('p');
      cfgFile.write(strlen(wifi_pwd)/10+48);
      cfgFile.write(strlen(wifi_pwd)%10+48);
      cfgFile.write(' ');
      cfgFile.print(wifi_pwd);
      cfgFile.close();
    }
    WiFi.scanDelete();
    break;
  }
  }
  else
    drawList("","未扫描到有效WiFi","正在重试...","",500,0);
  WiFi.scanDelete();
  //打印一次结果之后把缓存中的数据清掉
}

  return 0;
}
/*
void TCPTest(){
  WiFiClient client;
  char host_ip[16]="";
  drawList("TCP客户端实验","请输入主机IP地址","若想退出实验,请不要输","入任何字符,然后确定。");
  getchs(host_ip,16);
  if(host_ip[0]==0) return;
  int ho_port=slider("请输入主机端口号",10000);
  drawList("TCP客户端实验","正在连接到",host_ip,"",1);
  if(ho_port==-32768) return;//退出tcp实验
  while (!client.connect(host_ip, ho_port)) {
    drawList("TCP客户端实验","未能连接到",host_ip,"可能是端口错误",2000);
    getchs(host_ip,16);
    if(host_ip[0]==0) return;//退出tcp实验
    ho_port=slider("请输入主机端口号",10000);
    if(ho_port==-32768) return;//退出tcp实验
  }
  client.setNoDelay(1);
  char chs[LINE_BUFLEN]="Hello World!";
  while(1){
    uint8_t cb_result=0; //标记是否退出TCP实验
    getchs(chs);
    if(chs[0]==0) break;
    Serial.println(chs);
    client.println(chs);
    while(client.available()<=0){
      drawList("信息已发送","等待接收",host_ip,"",50,0);
      if(digitalRead(D4)==0){
      const char *tcp_test_text[]={
        "退出TCP","实验吗?","否","是","修改IP.."
      };
      cb_result=controlBoard(tcp_test_text);
      if(cb_result==3 || cb_result==4) break;
      }
    }
    if(cb_result==3) break;
    if(cb_result==4){

  client.stop();
  drawList("TCP通讯实验","请输入修改后的","主机IP地址","留空以退出实验");
  getchs(host_ip,16);
  if(host_ip[0]==0) return;
  ho_port=slider("请输入主机端口号",10000);
  if(ho_port==-32768) return;//退出tcp实验
  drawList("TCP通讯实验","正在连接到",host_ip,"",1);
  if (!client.connect(host_ip, ho_port)) {
    drawList("TCP客户端实验","未能连接到",host_ip,"可能是端口错误",2000);
    getchs(host_ip,16);
    if(host_ip[0]==0) return;//退出tcp实验
    ho_port=slider("请输入主机端口号",10000);
    if(ho_port==-32768) return;//退出tcp实验
  }
  strcpy(chs,"Coming Back Again!");
      
      continue;
    }
    drawList("已接收到数据","等待接收完毕",host_ip,"",100,0);
    String line = client.readStringUntil('\r');
    strncpy(chs,line.c_str(),LINE_BUFLEN-1);
    chs[LINE_BUFLEN-1]=0;
    while(client.read()>0) yield();
  }
  client.stop();
}

void FileExplorer(){
  static char *fnames[64];
  static const char *opers[5]={
    "打开文件","重命名","返回","删除","!格式化"
  };
  Dir dir=SPIFFS.openDir("/");
  uint8_t i;
  
  //for(i=0;i<64;i++){
  //  fnames[i]=new char[strlen(dir.fileName().c_str())+1];
  //  strcpy(fnames[i],dir.fileName().c_str());
  //  if(!dir.next()) { i++; break; }
  //}

  for(i=0;dir.next() && i<64;i++){
    fnames[i]=new char[strlen(dir.fileName().c_str())+1];
    strcpy(fnames[i],dir.fileName().c_str());
  }

  while(1){
    uint8_t openn=listdraw("SPIFFS文件浏览器",(const char **)fnames,i);
    if(openn==255) break;
    uint8_t ctl=controlBoard(opers);
    uint8_t formatee=0;
    while(digitalRead(D4)==0) yield();
    delay(30);
    if(ctl==3){//删除
      uint8_t retry=0;
      drawList("确定要删除文件",fnames[openn],"吗？不可以恢复哦","确定    取消",1);
      while(digitalRead(D1) && digitalRead(D4)) yield();
      if(digitalRead(D1)==0) retry=1;
      while(!digitalRead(D1) || !digitalRead(D4)) yield();
      delay(30);
      if(retry){
        if(SPIFFS.remove(fnames[openn]))
          drawList("删除文件",fnames[openn],"删除成功","请按确定键继续");
        else drawList(fnames[openn],"删除失败","可能由于文件已不存在","请按确定键继续");
      }
    }
    if(ctl==1){//重命名
      uint8_t retry=0;
      char newname[32]="/";
      if(getchs(newname+1,31)==0) continue;
      while(!digitalRead(D1) || !digitalRead(D4)) yield();
      drawList("确定要重命名文件吗？",fnames[openn],newname,"取消    确定",1);
      while(digitalRead(D1) && digitalRead(D4)) yield();
      if(digitalRead(D4)==0) retry=1;
      while(!digitalRead(D1) || !digitalRead(D4)) yield();
      delay(30);
      if(retry){
        if(SPIFFS.rename(fnames[openn],newname))
          drawList("重命名文件",fnames[openn],"成功,请退出浏览器并重","新进入以刷新文件列表");
        else drawList(fnames[openn],"重命名失败","可能由于文件已不存在","请按确定键继续");
      }
    }
    
    if(ctl==4){//格式化
      uint8_t retry=0;
      drawList("确定要格式化SPIFFS吗?","您将丢失所有文件数据","包括WiFi等重要配置","确定    取消",1);
      while(digitalRead(D1) && digitalRead(D4)) yield();
      if(digitalRead(D1)==0) retry=1;
      while(!digitalRead(D1) || !digitalRead(D4)) yield();
      delay(30);
      if(retry){
        SPIFFS.format();
        drawList("格式化SPIFFS成功","即将退出浏览器","","请按确定键继续");
        formatee=1;
      }
    }
    if(ctl==0){//打开文件
      uint8_t retry=0;
      drawList("即将打开文件",fnames[openn],"","取消    确定",1);
      while(digitalRead(D1) && digitalRead(D4)) yield();
      if(digitalRead(D4)==0) retry=1;
      while(!digitalRead(D1) || !digitalRead(D4)) yield();
      delay(30);
      if(retry){
        File fp;
        if(fp = SPIFFS.open(fnames[openn],"r")){
          scroll_string(NULL,1,&fp);
          fp.close();
        }
        else drawList(fnames[openn],"打开失败","可能由于文件已不存在","请按确定键继续");
      }
    }
    if(formatee==1) break;
  }
  for (int j=0;j<i;j++)
    delete [] fnames[j];
}

//tcp&udp
bool sendRequest(const char* host, const char* cityid, const char* apiKey) {
  // We now create a URI for the request
  //心知天气  发送http请求
  String GetUrl = "/v3/weather/daily.json?key=";
  GetUrl += apiKey;
  GetUrl += "&location=";
  GetUrl += cityid;
  GetUrl += "&language=";
  GetUrl += language;
  // This will send the request to the server
  client.print(String("GET ") + GetUrl + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "Connection: close\r\n\r\n");
  Serial.println("create a request to:");
  Serial.println(host);
  Serial.println("Connection: close\r\n");
  delay(1000);
  return true;
}
  

// @Desc 跳过 HTTP 头，使我们在响应正文的开头

bool skipResponseHeaders() {
  // HTTP headers end with an empty line
  bool ok = client.find(endOfHeaders);
  if (!ok) {
    Serial.println("No response or invalid response!");
  }
  return ok;
}
  

// @Desc 从HTTP服务器响应中读取正文

void readReponseContent(char* content, size_t maxSize) {
  size_t length = client.readBytes(content, maxSize);
  delay(100);
  Serial.println("Get the data from Internet!");
  content[length] = 0;
  //Serial.println(content);
  Serial.println("Read data Over!");
  client.flush();//清除一下缓冲
}
  

bool parseUserData(char* content, struct WeatherData* weatherData) {
  DynamicJsonBuffer jsonBuffer;
   
  JsonObject& root = jsonBuffer.parseObject(content);
   
  if (!root.success()) {
    Serial.println("JSON parsing failed!");
    return false;
  }
    
  //复制我们感兴趣的字符串
  strcpy(weatherData->city, root["results"][0]["location"]["name"]);
  for(int i=0;i<DAYS;i++){
    strcpy(weatherData->weather[i], root["results"][0]["daily"][i]["text_day"]);
    strcpy(weatherData->weathern[i], root["results"][0]["daily"][i]["text_night"]);
    strcpy(weatherData->temp[i], root["results"][0]["daily"][i]["high"]);
    strcpy(weatherData->templ[i], root["results"][0]["daily"][i]["low"]);
    strcpy(weatherData->hu[i], root["results"][0]["daily"][i]["humidity"]);
  }
  strcpy(weatherData->udate, root["results"][0]["last_update"]);
  //  -- 这不是强制复制，你可以使用指针，因为他们是指向“内容”缓冲区内，所以你需要确保
  //   当你读取字符串时它仍在内存中
  
  return true;
}
   
// 打印从JSON中提取的数据
void printUserData(const struct WeatherData* weatherData) {
  char weather_ele[64]="";
  strcat(t_ext,"    ");
  strcat(t_ext,weatherData->city);
  for(int i=0;i<DAYS;i++){
    sprintf(weather_ele,"%s天白天天气: %s, 夜间: %s, 温度: %s~%s ℃,湿度: %s %%;",
    dayname[i],weatherData->weather[i],weatherData->weathern[i],
    weatherData->temp[i],weatherData->templ[i],weatherData->hu[i]);
    strcat(t_ext,weather_ele);
    weather_ele[0]=0;
  }
  Serial.println(t_ext);
}
 
// 关闭与HTTP服务器连接
void stopConnect() {
  Serial.println("Disconnect");
  client.stop();
}
  
void clrEsp8266ResponseBuffer(void){
    memset(response, 0, MAX_CONTENT_SIZE);      //清空
}
*/
void digitalClockDisplay(uint8_t drawSecond)
{
  // digital clock display of the time
  if(drawSecond) sprintf(c_a_time,"%02d:%02d:%02d",hour(),minute(),second());
  else sprintf(c_a_time,"%02d:%02d",hour(),minute());
  sprintf(c_a_date,"%d/%d/%d",year(),month(),day());
  c_a_week=weekday()-1;
}
 
  
/*-------- NTP code ----------*/
  
const int NTP_PACKET_SIZE = 48; // NTP time is in the first 48 bytes of message
byte packetBuffer[NTP_PACKET_SIZE]; //buffer to hold incoming & outgoing packets
  
time_t getNtpTime()
{
  IPAddress ntpServerIP; // NTP server's ip address
  
  while (Udp.parsePacket() > 0) ; // discard any previously received packets
  Serial.println("Transmit NTP Request");
  // get a random server from the pool
  WiFi.hostByName(ntpServerName, ntpServerIP);
  Serial.print(ntpServerName);
  Serial.print(": ");
  Serial.println(ntpServerIP);
  sendNTPpacket(ntpServerIP);
  uint32_t beginWait = millis();
  while (millis() - beginWait < 5000) {
    int size = Udp.parsePacket();
    if (size >= NTP_PACKET_SIZE) {
      Serial.println("Receive NTP Response");
      Udp.read(packetBuffer, NTP_PACKET_SIZE);  // read packet into the buffer
      unsigned long secsSince1900;
      // convert four bytes starting at location 40 to a long integer
      secsSince1900 =  (unsigned long)packetBuffer[40] << 24;
      secsSince1900 |= (unsigned long)packetBuffer[41] << 16;
      secsSince1900 |= (unsigned long)packetBuffer[42] << 8;
      secsSince1900 |= (unsigned long)packetBuffer[43];
      return secsSince1900 - 2208988800UL + timeZone * SECS_PER_HOUR;
    }
  }
  Serial.println("No NTP Response :-(");
  return 0; // return 0 if unable to get the time
}
  
// send an NTP request to the time server at the given address
void sendNTPpacket(IPAddress &address)
{
  // set all bytes in the buffer to 0
  memset(packetBuffer, 0, NTP_PACKET_SIZE);
  // Initialize values needed to form NTP request
  // (see URL above for details on the packets)
  packetBuffer[0] = 0b11100011;   // LI, Version, Mode
  packetBuffer[1] = 0;     // Stratum, or type of clock
  packetBuffer[2] = 6;     // Polling Interval
  packetBuffer[3] = 0xEC;  // Peer Clock Precision
  // 8 bytes of zero for Root Delay & Root Dispersion
  packetBuffer[12] = 49;
  packetBuffer[13] = 0x4E;
  packetBuffer[14] = 49;
  packetBuffer[15] = 52;
  // all NTP fields have been given values, now
  // you can send a packet requesting a timestamp:
  Udp.beginPacket(address, 123); //NTP requests are to port 123
  Udp.write(packetBuffer, NTP_PACKET_SIZE);
  Udp.endPacket();
}
//-----------------------------------end--------------------------------------
int GenshinResinRest(int conse){
  static int Rrest=1600;
  static unsigned long millup=millis();
  if(millis()>4294937296) millup=0;
  else while(millis()-millup>48000 && millis()-millup<4200000000 && Rrest<1600){  //4294487296
    millup+=48000;
    Rrest++;
  }
  if(conse){
    if(Rrest>=1600) millup=millis();
    if(Rrest>=conse) Rrest-=conse;
    if(conse==10) {
      Rrest-=Rrest%10;
      millup=millis();
    }
  }
  return Rrest;
}
/*
uint8_t getCityApikey(char *cityname,char *apikey,uint8_t gmode){
  File cfgFile;
  uint8_t chksm=0;
  if(gmode){//写入
      cfgFile=SPIFFS.open(weather_config_file,"w");
      cfgFile.write('n');
      cfgFile.write(strlen(cityname)/10+48);
      cfgFile.write(strlen(cityname)%10+48);
      cfgFile.write(' ');
      cfgFile.print(cityname);
      
      cfgFile.write('k');
      cfgFile.write(strlen(apikey)/10+48);
      cfgFile.write(strlen(apikey)%10+48);
      cfgFile.write(' ');
      cfgFile.print(apikey);
      cfgFile.close();
  }
  else if(SPIFFS.exists(weather_config_file)){
    uint8_t ssidl=0,pskl=0;
    cfgFile = SPIFFS.open(weather_config_file,"r");
      if(cfgFile.read()!='n') chksm=2;
      ssidl=(cfgFile.read()-48)*10;
      ssidl+=(cfgFile.read()-48);
      cfgFile.read();
      for(int i=0;i<ssidl;i++){
        cityname[i]=cfgFile.read();
      }
      cityname[ssidl]=0;
      if(cfgFile.read()!='k') chksm=3;
      pskl=(cfgFile.read()-48)*10;
      pskl+=(cfgFile.read()-48);
      cfgFile.read();
      for(int i=0;i<pskl;i++){
        apikey[i]=cfgFile.read();
      }
      apikey[pskl]=0;
      cfgFile.close();
  }
  else chksm=1;
  return chksm;
}
void TCPServerTest(){
  drawList("TCP服务器实验","","    敬请期待","");
}
void Serial_monitor(){
  drawList("串口监视器","","    敬请期待","");
}
void doudizhu(){
  drawList("斗地主OL - 局域网版","","    敬请期待","");
}
*/
