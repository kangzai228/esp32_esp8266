#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>

const char* AP_NAME = "配网WIFI";//wifi名字
//暂时存储wifi账号密码
char sta_ssid[32] = {0};
char sta_password[64] = {0};
//配网页面代码
const char* page_html = "\
<!DOCTYPE html>\r\n\
<html lang='en'>\r\n\
<head>\r\n\
  <meta charset='UTF-8'>\r\n\
  <meta name='viewport' content='width=device-width, initial-scale=1.0'>\r\n\
  <title>Document</title>\r\n\
</head>\r\n\
<body>\r\n\
  <form name='input' action='/' method='POST'>\r\n\
        wifi名称: <br>\r\n\
        <input type='text' name='ssid'><br>\r\n\
        wifi密码:<br>\r\n\
        <input type='text' name='password'><br>\r\n\
        <input type='submit' value='保存'>\r\n\
    </form>\r\n\
</body>\r\n\
</html>\r\n\
";

const byte DNS_PORT = 53;//DNS端口号
IPAddress apIP(192, 168, 4, 1);//esp8266-AP-IP地址
DNSServer dnsServer;//创建dnsServer实例
ESP8266WebServer server(80);//创建WebServer

void handleRoot() {//访问主页回调函数
  server.send(200, "text/html", page_html);
}

void handleRootPost() {//Post回调函数
  Serial.println("handleRootPost");
  if (server.hasArg("ssid")) {//判断是否有账号参数
    Serial.print("got ssid:");
    strcpy(sta_ssid, server.arg("ssid").c_str());//将账号参数拷贝到sta_ssid中
    Serial.println(sta_ssid);
  } else {//没有参数
    Serial.println("error, not found ssid");
    server.send(200, "text/html", "<meta charset='UTF-8'>error, not found ssid");//返回错误页面
    return;
  }
  //密码与账号同理
  if (server.hasArg("password")) {
    Serial.print("got password:");
    strcpy(sta_password, server.arg("password").c_str());
    Serial.println(sta_password);
  } else {
    Serial.println("error, not found password");
    server.send(200, "text/html", "<meta charset='UTF-8'>error, not found password");
    return;
  }

  server.send(200, "text/html", "<meta charset='UTF-8'>保存成功");//返回保存成功页面
  delay(2000);
  //连接wifi
  connectNewWifi();
}

void initBasic(void){//初始化基础
  Serial.begin(115200);
  WiFi.hostname("Smart-ESP8266");//设置ESP8266设备名
}

void initSoftAP(void){//初始化AP模式
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
  if(WiFi.softAP(AP_NAME)){
    Serial.println("ESP8266 SoftAP is right");
  }
}

void initWebServer(void){//初始化WebServer
  //server.on("/",handleRoot);
  //上面那行必须以下面这种格式去写否则无法强制门户
  server.on("/", HTTP_GET, handleRoot);//设置主页回调函数
  server.onNotFound(handleRoot);//设置无法响应的http请求的回调函数
  server.on("/", HTTP_POST, handleRootPost);//设置Post请求回调函数
  server.begin();//启动WebServer
  Serial.println("WebServer started!");
}

void initDNS(void){//初始化DNS服务器
  if(dnsServer.start(DNS_PORT, "*", apIP)){//判断将所有地址映射到esp8266的ip上是否成功
    Serial.println("start dnsserver success.");
  }
  else Serial.println("start dnsserver failed.");
}

void connectNewWifi(void){
  WiFi.mode(WIFI_STA);//切换为STA模式
  WiFi.setAutoConnect(true);//设置自动连接
  WiFi.begin();//连接上一次连接成功的wifi
  Serial.println("");
  Serial.print("Connect to wifi");
  int count = 0;
   while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    count++;
    if(count > 10){//如果5秒内没有连上，就开启Web配网 可适当调整这个时间
      initSoftAP();
      initWebServer();
      initDNS();
      break;//跳出 防止无限初始化
    }
    Serial.print(".");
  }
  Serial.println("");
  if(WiFi.status() == WL_CONNECTED){//如果连接上 就输出IP信息 防止未连接上break后会误输出
    Serial.println("WIFI Connected!");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());//打印esp8266的IP地址
    server.stop();
  }
}

void setup() {
  initBasic();
  connectNewWifi();
}

void loop() {
  server.handleClient();
  dnsServer.processNextRequest();
}

