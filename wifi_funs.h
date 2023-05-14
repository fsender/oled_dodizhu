//-------------- Createen Studios --------------//
//2021.1.3
//A library to make characters alive!
//Remade from U8G2 offical example: Shennong.ino

#ifndef _CTG_WIFI_FUNS
#define _CTG_WIFI_FUNS

//Includings:
#include "Arduino.h"
#include "U8g2lib.h"
#include "ESP8266WiFi.h"
#include "WiFiUdp.h"
//#include "ArduinoJson.h"
#include "TimeLib.h"
#include "scroll_string.h"
#include "FS.h"

#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif

extern U8G2_SSD1306_128X64_NONAME_F_4W_HW_SPI u8g2;

//defines:
#define WIFINAME_LENMAX 32
#define WIFINAME_MAX 32
#define WIFI_TIMEOUT 30  //1=320ms


static const char *c_title="选择你想连接的WiFi";
static char wifi_names[WIFINAME_MAX][WIFINAME_LENMAX]={"测试1","测试2","ceshi3"};
static const char *c_items[WIFINAME_MAX+1];
static const char *C_REFRESH="刷新WiFi列表";
static const char *weather_config_file ="/weather.dat";
static const char *wifi_config_file ="/SysCfg.dat";
#define DAYS 3
//以下三个定义为调试定义
#define  CITY_LENGTH 30
//const char* ssid     = "510快乐源泉";         // XXXXXX -- 使用时请修改为当前你的 wifi ssid
//const char* password = "Aa123123";         // XXXXXX -- 使用时请修改为当前你的 wifi 密码


/*
// 我们要从此网页中提取的数据的类型
struct WeatherData {
  char city[16];//城市名称
  char weather[DAYS][32];//天气介绍（多云...）
  char weathern[DAYS][32];//天气介绍（多云...）
  char temp[DAYS][16];//温度
  char templ[DAYS][16];//温度
  char hu[DAYS][16];//湿度
  char udate[32];//更新时间
};
extern char t_ext[256];
extern char host[];
extern char APIKEY[];        //API KEY
extern char city[CITY_LENGTH] ;
extern const char* language ;//zh-Hans 简体中文  会显示乱码
extern const char* dayname[DAYS];
*/
extern char ntpServerName[] ;
extern const int timeZone ;//北京时间在东8区

extern const unsigned long BAUD_RATE ;    // serial connection speed
extern const unsigned long HTTP_TIMEOUT ; // max respone time from server
#define MAX_CONTENT_SIZE 4000             // max size of the HTTP response
 
  /*
extern WiFiClient client;
extern char response[MAX_CONTENT_SIZE];
extern char endOfHeaders[];
*/

extern  WiFiUDP Udp;
extern  unsigned int localPort ;
extern char c_a_time[];
extern char c_a_date[];
extern const char *c_a_weekname[];
extern uint8_t c_a_week;
//functions
uint8_t connect_to_WiFi(uint8_t useFile=0);
//void TCPTest();
//void FileExplorer();

time_t getNtpTime();
void digitalClockDisplay(uint8_t drawSecond=0);
void sendNTPpacket(IPAddress &address);
//bool sendRequest(const char* host, const char* cityid, const char* apiKey);
//bool skipResponseHeaders();
//void readReponseContent(char* content, size_t maxSize);
//bool parseUserData(char* content, struct WeatherData* weatherData);
//void printUserData(const struct WeatherData* weatherData);
//void stopConnect();
//void clrEsp8266ResponseBuffer(void);
int GenshinResinRest(int conse=0);
//uint8_t getCityApikey(char *cityname,char *apikey,uint8_t gmode=0);

//void TCPServerTest();
//void Serial_monitor();
//void doudizhu();
#endif
