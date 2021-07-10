#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>

#define onboard 16
#include <SECRETS.h>

const char *SSID = SECRET_SSID;
const char *PASSWORD = SECRET_PASSWORD;
const char *HOST = SECRET_HOST; 
const char *PATH = SECRET_PATH;
const int HTTPSPORT = 443;
int refreshTime = 15; 

String dataRX;
long cronTimer;

void setup() {
  // put your setup code here, to run once:
  pinMode(onboard, OUTPUT);

  delay(1200);
  Serial.begin(9600);
  WiFi.mode(WIFI_OFF);
  delay(1500);
  WiFi.mode(WIFI_STA);
  WiFi.begin(SSID, PASSWORD);
  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    delay(699);
    Serial.print(".");
  }
  Serial.print("Connected: ");
  Serial.println(SSID);
}


void callHTTPS() {
  WiFiClientSecure secureClient;
  secureClient.setInsecure();
  secureClient.setTimeout(12000);
  delay(1500);
  int retry = 0;
  while ((!secureClient.connect(HOST, HTTPSPORT)) && (retry < 15)) {
    delay(90);
    Serial.print(".");
    retry++;
  }
  if (retry == 15) {
    Serial.println("Made no connection.");
  } else {
    Serial.println("Connected to server.");
  }
  secureClient.println(String("GET ") + PATH + " HTTP/1.1"); 
  secureClient.println(String("Host: ") + HOST); 
  secureClient.println("Connection: close");
  secureClient.println();
  Serial.println(String("GET ") + PATH + " HTTP/1.1\r\n" +
                  "Host: " + HOST + "\r\n" + 
                  "Connection: close\r\n\r\n");

  while (secureClient.connected()) {
    String line = secureClient.readStringUntil('\n');
    if (line == "\r") {
      break;
    }
  }
  while (secureClient.available()) {
    dataRX += secureClient.readStringUntil('\n');
  } 
  Serial.println(dataRX);
  dataRX = "";

}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(onboard, LOW);
  delay(1500);
  Serial.println("Go MCU!");
  digitalWrite(onboard, HIGH);
  delay(300);
  callHTTPS();
}
