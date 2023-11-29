/*********
  Rui Santos
  Complete project details at https://randomnerdtutorials.com  
*********/

// Load Wi-Fi library
#include <ESP8266WiFi.h>
#include "DHT.h"
#include <SoftwareSerial.h>
#include <PubSubClient.h>
SoftwareSerial NeoComPort(12,13);  ////d6-tx   ,d7-rx


#define DHTPIN 5
#define DHTTYPE DHT11

WiFiClient espClient;
PubSubClient client(espClient);
#define TEMP_TOPIC    "smarthome/room1/temp"

DHT dht(DHTPIN, DHTTYPE);


// Replace with your network credentials
const char* ssid     = "PtichkoLan";
const char* password = "89608117122";
bool sended=false;
String str = "Hz";


  // Set web server port number to 80
WiFiServer server(85);

// Variable to store the HTTP request
String header;

void setup() {
   Serial.begin(115200);
   NeoComPort.begin(9600);
  // NeoComPort.print("testsms");
  // NeoComPort.end;
  // Initialize the output variables as outputs

  dht.begin();
   // Connect to Wi-Fi network with SSID and password
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  // Print local IP address and start web server
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  server.begin();
}

void loop(){

  float h = dht.readHumidity();
  float t = dht.readTemperature();
 

  uint32_t sec = millis() / 1000ul;
  int timeHours = (sec / 3600ul);
  int timeMins = (sec % 3600ul) / 60ul;
  int timeSecs = (sec % 3600ul) % 60ul;

  if (str == "Svyaz OK") {
  
  if (sended == false) {
    Serial.println("staaaaaaaart"+String(str));
   sendsms("+79370662487","Boot OK");
  }
  }
  WiFiClient client = server.available();   // Listen for incoming clients

  if (client) {                             // If a new client connects,
    Serial.println("New Client.");          // print a message out in the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected()) {            // loop while the client's connected
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        header += c;
        if (c == '\n') {                    // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-Type: text/html; charset=utf-8");
            client.println("Connection: close");
            client.println();
            
            if (header.indexOf("GET /5/on") >= 0) {
              Serial.println("GPIO 5 on");
              //sended=true;
             sendsms("+79608117122","testONsms");
             //creg();
            }
            creg();
            // Display the HTML web page
            client.println(F("<!DOCTYPE html><html>"));
             // Web Page Heading
             client.println(F("<HEAD>"));
             client.println(F("<meta http-equiv=\"Refresh\" content=\"5\" >"));///обновление страницы раз в 15 сек.
             client.println(F("/<HEAD>"));
             client.println(F("<body><h1>Панова 50-44</h1>"));
             client.println(F("<p>Привет Саня</p>"));
             client.println(F("<p>Температура="));
             client.println(t);
             client.println(F(" Влажность="));
             client.println(h); client.println("</p>");
             client.println(F("<p> Время работы:")); client.println(timeHours); client.println("ч."); client.println(timeMins); client.println("м."); client.println(timeSecs); client.println(F("сек.</p>"));
             Serial.println(F("Время работы:")); Serial.print(timeHours); Serial.print("ч."); Serial.print(timeMins); Serial.print("м."); Serial.print(timeSecs); Serial.print(F("сек."));
             client.println(F("<p>CREG=")); client.println(str); client.println("</p>");
             Serial.println(str);
            client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
            client.println(".button { background-color: #195B6A; border: none; color: white; padding: 16px 40px;");
            client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
            client.println(".button2 {background-color: #77878A;}</style></head>");
            client.println("<p><a href=\"/5/on\"><button class=\"button\">ON</button></a></p>");

             
              client.println(F("</body></html>"));
           // NeoComPort.begin(9600);
            // NeoComPort.print("testsms");
            // The HTTP response ends with another blank line
            client.println();
            // Break out of the while loop
            break;
          } else { // if you got a newline, then clear currentLine
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
      }
    }
    // Clear the header variable
    header = "";
    // Close the connection
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  }
}

void sendsms(String phone, String text) {
  float t = dht.readTemperature();
  NeoComPort.begin(9600);
Serial.println("SMS send started");
//String phone = "+79608117122";
//String text = "Blyat";    //String(t);

NeoComPort.println("AT+CLIP=1");  //включаем АОН
delay(500);
NeoComPort.println("AT+CMGF=1");  //режим кодировки СМС - обычный (для англ.)
delay(500);
NeoComPort.println("AT+CSCS=\"GSM\"");  //режим кодировки текста
delay(500);
NeoComPort.println("AT+CMGS=\"" + phone + "\"");
delay(500);
NeoComPort.print(text);
delay(500);
NeoComPort.print((char)26);
  delay(500);
  Serial.println("SMS send complete");
  delay(2000);
  sended=true; 
}

void creg(){
NeoComPort.println("AT+CREG?");
delay(500);
    // проверяем результат
    if (NeoComPort.find("+CREG: 0,1")) {
      Serial.println("Svyaz 0,1");
      str="Svyaz OK";
    }
    else { 
      str="Svyaz HZ";
      Serial.println("Svyaz' hz");
    }
}

void mqttconnect() {
  /* Loop until reconnected */
  while (!client.connected()) {
    Serial.print("MQTT connecting ...");
    /* client ID */
    String clientId = "ESP32Client";
    /* connect now */
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      /* subscribe topic with default QoS 0*/
      client.subscribe(LED_TOPIC);
    } else {
      Serial.print("failed, status code =");
      Serial.print(client.state());
      Serial.println("try again in 5 seconds");
      /* Wait 5 seconds before retrying */
      delay(5000);
    }
  }
}
