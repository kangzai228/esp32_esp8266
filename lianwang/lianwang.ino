/*
    This sketch demonstrates how to set up a simple HTTP-like server.
    The server will set a GPIO pin depending on the request
      http://server_ip/gpio/0 will set the GPIO2 low,
      http://server_ip/gpio/1 will set the GPIO2 high
    server_ip is the IP address of the ESP8266 module, will be
    printed to Serial when the module is connected.
*/

#include <ESP8266WiFi.h>

int ledPin = 16;

const char* ssid = "asus";
const char* password = "asus19850609";

// Create an instance of the server
// specify the port to listen on as an argument
WiFiServer server(80);

void setup() {
  Serial.begin(115200);
  delay(10);

  pinMode(ledPin,OUTPUT);
  digitalWrite(ledPin,LOW);

  // Connect to WiFi network
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");

  // Start the server
  server.begin();
  Serial.println("Server started");

  // Print the IP address
  Serial.println(WiFi.localIP());
}
void loop() {

  digitalWrite(ledPin,HIGH);
  delay(1000);
  digitalWrite(ledPin,LOW);
  delay(1000);
  
  // Check if a client has connected
  WiFiClient client = server.available();
  if (!client) {
    return;
  }

  // Wait until the client sends some data
  Serial.println("new client");
  while (!client.available()) {
    delay(1);
  }

  // Read the first line of the request
  String req = client.readStringUntil('\r');
  Serial.println(req);
  client.flush();

  // Match the request
  int val;
  Serial.printf("%d\n", req.indexOf("/gpio/0")); //查找是否包含/gpio/0，找不到返回-1
  Serial.printf("%d\n", req.indexOf("/gpio/1"));
  if (req.indexOf("/gpio/0") != -1)
    val = 0;
  else if (req.indexOf("/gpio/1") != -1)
    val = 1;
  else {
    Serial.println("invalid request");
    client.stop();
    return;
  }


  // Set GPIO2 according to the request
  digitalWrite(LED_BUILTIN, val);

  client.flush();

  // Prepare the response

  //  String s = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<!DOCTYPE HTML>\r\n<html>\r\nGPIO is now ";
  //  s += (val) ? "high" : "low";
  //  s += "</html>\n";
  //  // Send the response to the client
  //  client.print(s);
  client.print("<html><body><p>GPIO is now ");
  client.print((val) ? "high" : "low");
  client.println(" </p>");
  client.print("<a href=\"http://");
  client.print(WiFi.localIP());
  client.println("/gpio/1\">开</a>");//当遇到要输出"时，为了避免和print（""),可以写成print("\"")
  client.print("<a href=\"http://");
  client.print(WiFi.localIP());
  client.println("/gpio/0 \">关</a>");
  client.println("</body></html>");

  delay(1);
  Serial.println("Client disonnected");

  // The client will actually be disconnected
  // when the function returns and 'client' object is detroyed
}
