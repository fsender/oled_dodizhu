# oled_dodizhu

Play card game on ESP8266 with oled12864.

在 ESP8266 上用 OLED12864 玩斗地主游戏

## 局域网可连接性

三台设备在一个wifi热点里面可以联机.

可以输入用户名和IP地址

## 需要的材料

1. ESP-12F (NodeMCU 开发板 或者 Wemos D1 Mini开发板)

2. OLED12864(SPI接口或I2C接口都行, 但是I2C的显示速度慢, 动画卡顿, 建议SPI的)

3. 三个按键

4. 以上这些材料, 准备三份, 做三台设备(一台怎么玩斗地主)

5. 编辑 ctg_stack.h 文件, 把引脚设定改成你的引脚连接方法()

## 问题

掉线不能重连, 所以玩的时候千万别动复位键