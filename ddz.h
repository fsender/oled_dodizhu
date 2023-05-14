#ifndef _CTG_DDZ_H
#define _CTG_DDZ_H

//#define DBG_VERSION

#include "U8g2lib.h"
#include "ctg_stack.h"
#include "Xbitmap.h"

#define SERVER_PORT 193
#define REQTIMEOUT 1000

extern U8G2_SSD1306_128X64_NONAME_F_4W_HW_SPI u8g2;

typedef struct {
  uint8_t player;
  uint8_t num;
  uint8_t type;
  uint8_t card[21];
} pcards;

typedef struct {
  uint8_t id;
  char name[7];
  uint8_t identity;//身份，地主为1，农民为0
  int32_t score;
  pcards cards;
} plyr;

static const char *ddz_modes[4]={
  "加入房间","开新房间","查看说明","返回系统"
};
static const char *sco[4]={
    "黑桃","红桃","梅花","方块"
  };
static const uint16_t scob[4]={
    20468,17514,20454,17492
  };
static const char *spt[]={
    "3","4","5","6","7","8","9","10","J","Q","K","A","2","小王","大王"
  };
static const char *ch_calling[4]={
  "不叫","一分","二分","三分"
};
static const uint8_t zx1[20]={
  124,124,124,124,124,123,122,121,121,122,123,124,125,126,127,127,126,125,124,124
  };
static const uint8_t zy1[20]={
  58,59,60,61,62,63,63,62,61,60,60,60,60,60,59,58,57,57,58,59
  };
extern char ddznote_str[];
extern char c_a_time[];
static WiFiServer server(SERVER_PORT);
static WiFiClient playr[2];
static uint8_t base_score=0;//基础叫分
static uint16_t plex=1;    //倍数
static uint8_t p_holder=0;  //出牌权最大的玩家
static uint8_t p_master=0;  //地主
static uint8_t yourID=0; //标记玩家的ID
static uint8_t winner=0;  //赢家，赢家的ID
#ifdef DBG_VERSION
static uint8_t DBG_MODE =0;
#endif

/*ServerCilent 通讯暗号：
 ----------------------client发送的有：---------------------
 * b: 请求匹配（包括断线重连）后面为昵称 不足6位的用空格补充（Ascii为32）
 如昵称为abcdef 则发送bbcdefg
 * r: 准备 只有1字节
 * t: 同步数据请求 只有1字节
 * d: 响应发牌请求（接收发牌） d后面是发到几张牌，发送为uint8_t，
 * 再后面是牌，发送为uint8_t数组,所有数据+2
 * c: 发送叫分 c后面是叫几分，发送为字符
 * o: 出牌 o后面是出几张牌，发送为uint8_t,后面是出的牌，发送为uint8_t[],所有数据+2
 * e: 接收到游戏结束信号后回应 只有1字节
 * 
 -----------------------server响应的有：--------------------
 * b: 回复请求匹配（包括断线重连）后面为分配的ID,
 若为3或者4则需要同步数据，因为游戏正在进行中
 * r: 回应准备 只有1字节
 * u: 所有玩家均就绪,之后是玩家0、玩家1、玩家2的名称，每个玩家6字节
 * t: 回应同步数据请求,重复3次,格式如下
 * ['t'][name(7 Bytes)][identity][score(6 Bytes string)][cards(24 Bytes)]
 * [1][name(7 Bytes)][identity][score(6 Bytes string)][cards(24 Bytes)]
 * [2][name(7 Bytes)][identity][score(6 Bytes string)][cards(24 Bytes)]
 * ['t'][name(7 Bytes)][identity][score(6 Bytes string)][cards(24 Bytes)]
 * [1][name(7 Bytes)][identity][score(6 Bytes string)][cards(24 Bytes)]
 * [2][name(7 Bytes)][identity][score(6 Bytes string)][cards(24 Bytes)]
 * ['t'][name(7 Bytes)][identity][score(6 Bytes string)][cards(24 Bytes)]
 * [1][name(7 Bytes)][identity][score(6 Bytes string)][cards(24 Bytes)]
 * [2][name(7 Bytes)][identity][score(6 Bytes string)][cards(24 Bytes)]
 * ['f'][cards(24 Bytes)] //桌子上的牌，写入pool[0]，前3字节为card结构体的变量，所有数据+2
 * ['q'][cards(24 Bytes)]//桌子上的牌，写入pool[1]，所有数据+2
 * ['x'][cards(4 Bytes)]//桌子上的牌，写入_reserved，所有数据+2
 * cards为手牌，首字节为张数，次字节为type，默认0，最后22字节为牌
 * 除了name和score为字符，其他数据均+2
 * 
 * d: 发送发牌请求（接收发牌） d后面是玩家ID，然后是发几张牌，发送为uint8_t
 *  再后面是牌，发送为uint8_t数组，最后是底牌，所有数据全部+2
 *  
 * l: 发送叫分请求 l后面是玩家ID、当前叫的最高分、上家叫的分
 * c: 回复叫分 c后面是玩家，然后是叫几分，发送为uint8_t
 * g: 发送出牌信息广播 然后是玩家ID，再是是否有权限出牌
 * 
 * e: 发送游戏结束信号,然后是分数加减，解析为拆开的16位整数,解析为
 * 示例：若三个人的得分为 -6，3，3
 * ['e'][255][250][0][3][0][3](2字节后的补码)
*/
//--------斗地主服务器专用-------
uint8_t start_connect(plyr *player); //开启匹配
uint8_t wait_ready_s(plyr * player); //等待准备
uint8_t serve_deal(plyr * player,pcards *To); //发牌,To为底牌
uint8_t serve_call_score(plyr * player);//处理叫分
//uint8_t set_order();  //设置出牌权
uint8_t serve_card(plyr * player,pcards *pool,uint8_t sendOnly=0); //处理打来的牌
uint8_t end_game(plyr * player);  //结束并计算得分
uint8_t end_connect();//结束连接
String serve_sync(plyr * player,pcards *pool);

//--------斗地主客户端专用-------
uint8_t start_connect_to(const char *ip,uint32_t h_port,plyr *player);//连接到房主
uint8_t send_ready();  //准备
uint8_t wait_ready(); //等待准备
uint8_t sync_data(plyr *player,pcards* pool);  //处理断线重连
uint8_t recv_deal(plyr *player,pcards* pool); //接收发牌
uint8_t send_call_score(int16_t yourScore);//叫分
uint8_t wait_call_score(); //等待叫分
uint8_t get_order(plyr *player,pcards* pool); //获得出牌权 返回1代表获得成功
uint8_t send_card(pcards *To,pcards *rest); //打出去牌
uint8_t game_ended(plyr *player);  //结束并计算得分
uint8_t end_connect_to();//与房主断开

//----------GUI部分代码----------
void draw_gui(plyr * playing,pcards * pool);
//uint8_t call_score();//叫分
void draw_card(pcards *To, int x, int y, uint8_t xpage=0, 
uint8_t n=0, uint8_t selecting=255, const uint8_t *selected=NULL );
pcards get_card(plyr * playing,const pcards * pool);//选择要出的牌的函数
void randomCards(pcards *pc1,pcards *pc2,pcards *pc3,pcards *rsv);//随机生成牌
void takeOrder(pcards *pc);//排序函数
void disp_status(const char* st,uint8_t ry=63);
void shiftOne(pcards *zfrom,pcards *zto,int where);//移位寄存
uint8_t check_legal(pcards *c,const pcards *po,uint8_t *selsp);
uint8_t isslide(uint8_t* card,uint8_t num);
uint8_t strToInt(int *,char *);//字符到整数,成功返回1,有非数字字符则返回0
void disScores(plyr *player,int32_t *scdelta);
void print_buchu(uint8_t dtx,uint8_t dty);

//-----------调试用代码-----------
void print_card(const pcards * ccard);
//void test_gui();

void dodizhu();
#endif
