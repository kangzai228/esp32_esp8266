#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>

//配网页面代码
const char *page_html = "<!DOCTYPE html><html lang='en'><head><meta charset='UTF-8'><meta name='viewport'content='width=device-width, initial-scale=1.0, maximum-scale=1.0, minimum-scale=1.0, user-scalable=no'><title>配置WIFI</title><style>*{margin:0;padding:0}body{font-size:16px;margin:0 15px;z-index:0}.flexcenter{display:flex;align-content:center;align-items:center}.button{width:40%;border-radius:5px;height:40px;background:none;border:1px solid green;font-size:20px;justify-content:space-evenly;color:green}.button:active{background:#ccc}.wifititle{display:flex;margin:0 15px;justify-content:space-between;font-size:22px;color:#333}.wifilist{display:flex;flex-direction:column;align-items:center;justify-content:space-between;font-size:25px;color:#ccc}divw{height:30px;border-bottom:1px solid#ccc;width:100%;background:none;margin-top:5px}divw:active{background:#50bfff}.title{justify-content:center;height:10%;font-size:35px;color:#725af3;letter-spacing:5px;text-shadow:0px 12px 10px#9d91bf}.content-input{height:20%;max-height:20%;display:flex;flex-direction:column}.input-text{display:flex;flex-direction:row;align-items:flex-end;justify-content:space-evenly;font-size:20px;color:#ccc}.input{width:76%;line-height:30px;font-size:25px;margin-top:15px;border-top:0;border-left:0;border-right:0;border-bottom:1px solid#ccc}.context-button{margin-top:15px;margin-bottom:15px;justify-content:space-around}.loading-svg-container{width:50px;height:50px}.path{stroke:#409eff;stroke-width:2;stroke-dasharray:95,126;stroke-dashoffset:0;animation:loading-dash 1.5s ease-in-out infinite}@keyframes loading-dash{0%{stroke-dasharray:1,126;stroke-dashoffset:0}50%{stroke-dasharray:95,126;stroke-dashoffset:-31px}to{stroke-dasharray:6,120;stroke-dashoffset:-120px}}.loading{width:40%;height:15%;position:absolute;background:#fff;display:flex;align-items:center;justify-content:space-around;top:30%;left:30%;flex-direction:column;color:#F4606C;border-radius:20px;overflow:hidden;font-size:14px;z-index:999}.box{width:100%;height:100%;position:fixed;background:#cccccc;opacity:0.8;margin-left:-15px;display:none}</style></head><body οnlοad='initData()'><div class='box'><div class='loading'><svg viewBox='0 0 50 50'class='loading-svg-container'><circle cx='25'cy='25'r='20'fill='none'class='path'></circle></svg><span id='tip'></span></div></div><div class='flexcenter title'>欢迎使用配置WIFI</div><div class='content-input'><div class='input-text'>SSID：<input class='input'id='ssid'type='text'value=''/></div><div class='input-text'>密码：<input class='input'id='pwd'type='password'value=''/></div></div><div class='flexcenter context-button'><button class='flexcenter button'οnclick='connwifi()'>提&nbsp;&nbsp;交</button><button class='flexcenter button'οnclick='reset()'>重&nbsp;&nbsp;置</button></div><div class='wifititle'><span>wifi列表</span><span style='color: blue'οnclick='getWifiList()'>刷新</span></div><div class='wifilist'id='wifilist'></div></body><script>function initData(){setTimeout(function(){getWifiList()},1000)}function showloading(msg){document.getElementsByClassName('box')[0].style.display='table';document.getElementById('tip').innerHTML=msg}function hideloading(){document.getElementsByClassName('box')[0].style.display='none';document.getElementById('tip').innerHTML=''}function getWifiList(){document.getElementById('tip').innerText='';showloading('搜索wifi中...');doget('/wifilist',function(result){if(result.indexOf('.nodata')!==-1){showloading('找不到有效wifi信息');setTimeout(function(){hideloading()},2000);return}hideloading();var arr=result.split(',');var html='';for(item of arr){if(item===''){continue}html+='<divw οnclick=\"setSsid(event)\">'+item+'</divw>'}document.getElementById('wifilist').innerHTML=html})}function setSsid(event){document.getElementById('tip').innerText='';document.getElementById('ssid').value=event.currentTarget.innerText}function reset(){document.getElementById('tip').innerText='';document.getElementById('ssid').value='';document.getElementById('pwd').value=''}function connwifi(){var ssid=document.getElementById('ssid').value;if(ssid===''||ssid===null||typeof(ssid)==='undefined'){showloading('SSID不能为空');setTimeout(function(){hideloading()},1000);return}var pwd=document.getElementById('pwd').value;var url='/configwifi?ssid='+ssid+'&pwd='+pwd;showloading('网络配置中...');doget(url,function(result){showloading(result);setTimeout(function(){hideloading()},1000)})}function doget(url,callback){var ajax=new XMLHttpRequest();ajax.open('get',url);ajax.send();ajax.onreadystatechange=function(){callback(ajax.response)}}</script></html>";

const char *AP_NAME = "esp8266-config"; // wifi名字
IPAddress apIP(192, 168, 4, 1);        // esp8266-AP-IP地址
DNSServer dnsServer;                   //创建dnsServer实例
ESP8266WebServer server(80);           //创建WebServer

/**
   @brief 访问主页回调函数

*/
void handleRoot()
{
  server.send(200, "text/html", page_html);
}

/**
   @brief Post回调函数

*/
void handleConfigWifi()
{
  WiFi.setAutoConnect(true);                                         // 设置自动连接
  WiFi.begin(server.arg("ssid").c_str(), server.arg("pwd").c_str()); // 使用配网获取的wifi信息
  int count = 0;
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    count++;
    if (count > 50)
    { //如果5秒内没有连上，就开启Web配网 可适当调整这个时间
      server.send(200, "text/html", "wifi连接失败,请检查密码后重试。"); //返回保存成功页面
      break;                                                            //跳出 防止无限初始化
    }
    Serial.print(".");
  }
  if (WiFi.status() == WL_CONNECTED) //如果连接上 就输出IP信息 防止未连接上break后会误输出
  {
    Serial.print("WIFI Connected:");                               //打印esp8266的IP地址
    Serial.println(WiFi.localIP());                                //打印esp8266的IP地址
    server.send(200, "text/html", "wifi连接成功，即将重启设备。"); //返回保存成功页面
    delay(3000);
    server.stop();
  }
}

/**
   @brief 获取wifi列表回电函数

*/
void handleWifiList()
{

  int n = WiFi.scanNetworks(); //开始同步扫描，将返回值存放在变量n中
  if (n > 0)
  {
    char wifilist[640] = {0}; // 返回给网页的数据
    Serial.println("sacn wifi.");
    for (int i = 0; i < n; i++) //开始逐个打印扫描到的
    {
      sprintf(wifilist, "%s%s%s", wifilist, WiFi.SSID(i).c_str(), ","); // 组装信息返回给接口
    }
    Serial.print(wifilist);                  // 打印一下日志
    server.send(200, "text/html", wifilist); //返回保存成功页面
    return;                                  // 结束这里的操作
  }
  Serial.println("no any wifi.");           // 打印没有任何wifi日志
  server.send(200, "text/html", ".nodata"); //返回保存成功页面
}

/**
   @brief 初始化WebServer

*/
void initWebServer(void)
{
  WiFi.mode(WIFI_AP);                                         //初始化AP模式
  WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0)); //初始化AP模式
  WiFi.softAP(AP_NAME);                                   //初始化AP模式
  server.on("/", HTTP_GET, handleRoot);                 //设置主页回调函数
  server.onNotFound(handleRoot);                        //设置无法响应的http请求的回调函数
  server.on("/configwifi", HTTP_GET, handleConfigWifi); //设置Post请求回调函数
  server.on("/wifilist", HTTP_GET, handleWifiList);     // 设置获取wifi列表回调函数
  server.begin();                                       //启动WebServer
  Serial.println("WebServer started!");
  dnsServer.start(53, "*", apIP);
}

void connectNewWifi(void)
{
  WiFi.mode(WIFI_STA);       //切换为STA模式
  WiFi.setAutoConnect(true); //设置自动连接
  WiFi.begin();              //连接上一次连接成功的wifi
  Serial.println("");
  Serial.print("Connect to wifi");
  int count = 0;
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    count++;
    if (count > 100)
    {
      initWebServer(); //如果5秒内没有连上，就开启Web配网 可适当调整这个时间
      break;           //跳出 防止无限初始化
    }
    Serial.print(".");
  }
  Serial.println("");
  if (WiFi.status() == WL_CONNECTED) //如果连接上 就输出IP信息 防止未连接上break后会误输出
  {
    Serial.print("WIFI Connected!"); //打印esp8266的IP地址
    Serial.println(WiFi.localIP());  //打印esp8266的IP地址
  }
}

void setup()
{
  Serial.begin(115200);           //初始化基础
  WiFi.hostname("Smart-ESP8266"); //设置ESP8266设备名
  connectNewWifi();
}

void loop()
{
  server.handleClient();
  dnsServer.processNextRequest();
}
