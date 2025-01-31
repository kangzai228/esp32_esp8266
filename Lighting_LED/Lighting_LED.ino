// 定义蓝色连接的引脚
const int Led1Pin = 16; // 蓝色连接到GPIO16引脚
void setup() {
  // put your setup code here, to run once:
pinMode(Led1Pin, OUTPUT); // 初始化蓝色LED引脚为输出模式
}

void loop() {
  // put your main code here, to run repeatedly:
digitalWrite(Led1Pin, LOW); // 打开蓝色LED
delay(2000); // 延时2秒
digitalWrite(Led1Pin, HIGH); // 关闭蓝色LED
delay(1000); // 延时1秒

}
