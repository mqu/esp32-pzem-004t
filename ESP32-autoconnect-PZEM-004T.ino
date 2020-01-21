/*
  Simple.ino, Example for the AutoConnect library.
  Copyright (c) 2018, Hieromon Ikasamo
  https://github.com/Hieromon/AutoConnect

  This software is released under the MIT License.
  https://opensource.org/licenses/MIT
*/

String PROJECT_URL="https://github.com/mqu/esp32-pzem-004t";

// HardwareSerial PzemSerial2(2);     // Use hwserial UART2 at pins IO-16 (RX2) and IO-17 (TX2)
#include <HardwareSerial.h>
#include <PZEM004T.h>

PZEM004T pzem(&Serial2);
IPAddress ip(192,168,1,1);

// serial2
#define RXD2 16
#define TXD2 17


#include <esp_int_wdt.h>
#include <esp_task_wdt.h>

#if defined(ARDUINO_ARCH_ESP8266)
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#elif defined(ARDUINO_ARCH_ESP32)
#include <WiFi.h>
#include <WebServer.h>
#endif
#include <time.h>
#include <AutoConnect.h>

#if defined(ARDUINO_ARCH_ESP8266)
ESP8266WebServer Server;
#elif defined(ARDUINO_ARCH_ESP32)
WebServer Server;
#endif
AutoConnect      Portal(Server);
AutoConnectConfig   Config;       // Enable autoReconnect supported on v0.9.4

#define TIMEZONE    (3600 * 1)    // Paris
#define NTPServer1  "ntp.nict.jp" // NICT japan.
#define NTPServer2  "time1.google.com"


/**
 *  An actual reset function dependent on the architecture
 */
#if defined(ARDUINO_ARCH_ESP8266)
#define SOFT_RESET()  ESP.reset()
#elif defined(ARDUINO_ARCH_ESP32)
#define SOFT_RESET()  ESP.restart()
#endif

// from ESP/wifi/autoconnect ; https://github.com/Hieromon/AutoConnect/blob/master/src/AutoConnect.cpp#L18
// invoque watchdog + infinite loop
void hard_restart() {
  esp_task_wdt_init(1,true);
  esp_task_wdt_add(NULL);
  while(true);
}

void restart() {
  Serial.println("going to reset");
  sleep(1000);
  // ESP.restart(); not working
  hard_restart();
}

extern "C" int rom_phy_get_vdd33();

void vcc() {
  char buffer[20];
  sprintf(buffer, "%d\n", rom_phy_get_vdd33());
  Server.send(200, "text/txt", buffer);
}

void uptimems() {
  char buffer[20];
  sprintf(buffer, "%d\n", millis());
  Server.send(200, "text/txt", buffer);
}

void uptime() {
  char buffer[20];
  sprintf(buffer, "%d\n", millis() / 1000);
  Server.send(200, "text/txt", buffer);
}

void power_all() {
  char buffer[40];

  float v = pzem.voltage(ip);
  if (v < 0.0) v = 0.0;
  float i = pzem.current(ip);
  float p = pzem.power(ip);
  float e = pzem.energy(ip);
  sprintf(buffer, "%0.2fV;%0.3fA;%0.1fW;%0.1fWh\n", v, i, p, e);
  Serial.println(buffer);
  Server.send(200, "text/txt", buffer);
}

void voltage() {
  char buffer[30];
  float v = pzem.voltage(ip);
  if (v < 0.0) v = 0.0;
  sprintf(buffer, "%0.2fV\n", v);
  Serial.println(buffer);
  Server.send(200, "text/txt", buffer);
}

void current() {
  char buffer[30];
  float v = pzem.current(ip);
  if (v < 0.0) v = 0.0;
  sprintf(buffer, "%0.3fA\n", v);
  Serial.println(buffer);
  Server.send(200, "text/txt", buffer);
}

void _power() {
  char buffer[30];
  float v = pzem.power(ip);
  if (v < 0.0) v = 0.0;
  sprintf(buffer, "%0.2fW\n", v);
  Serial.println(buffer);
  Server.send(200, "text/txt", buffer);
}

void energy() {
  char buffer[30];
  float v = pzem.energy(ip);
  if (v < 0.0) v = 0.0;
  sprintf(buffer, "%0.1fWh\n", v);
  Serial.println(buffer);
  Server.send(200, "text/txt", buffer);
}

void rootPage() {
  String  content = 
    "<html>"
    "<head>"
    "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">"
    "</head>"
    "<body>"
    "<h2 align=\"center\" style=\"color:blue;margin:20px;\">Hello, world</h2>"
    "<h3 align=\"center\" style=\"color:gray;margin:10px;\">{{DateTime}}</h3>"
    "<p style=\"padding-top:10px;text-align:center\">" AUTOCONNECT_LINK(COG_32) "</p>"
    "</body>"
    "</html>";
  static const char *wd[7] = { "Sun","Mon","Tue","Wed","Thr","Fri","Sat" };
  struct tm *tm;
  time_t  t;
  char    dateTime[26];

  t = time(NULL);
  tm = localtime(&t);
  sprintf(dateTime, "%04d/%02d/%02d(%s) %02d:%02d:%02d.",
    tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday,
    wd[tm->tm_wday],
    tm->tm_hour, tm->tm_min, tm->tm_sec);
  content.replace("{{DateTime}}", String(dateTime));
  Server.send(200, "text/html", content);
}

void blink(int count, int _delay=180){
    for(int i=0;i<count;i++){
       digitalWrite(LED_BUILTIN, HIGH);
       delay(_delay);
       digitalWrite(LED_BUILTIN, LOW);
       delay(_delay);
    }
}

int count=0;

void setup() {
  delay(1000);
  Serial.begin(115200);
  Serial.println();

  Serial.println("Energy monitoring system using ESP32 and PZEM-004T");
  Serial.print("- project URL");
  Serial.println(PROJECT_URL);
  
  // to read VCC
  // ADC_MODE(ADC_VCC);  // not available for ESP32

  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);

  // PZEM
  Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2);

   while (true) {
      Serial.println("Connecting to PZEM...");
      blink(2,110);
      if(pzem.setAddress(ip))
         break;
         delay(1000);
   }
   Serial.println("connected to PZEM");

  // Behavior a root path of ESP8266WebServer.
  Server.on("/", rootPage);

  // restart ESP32 when needed
  Server.on("/restart", restart);

  // what it uptime (in seconds)
  Server.on("/uptime", uptime);
  Server.on("/uptimems", uptimems);
  Server.on("/vcc", vcc);
  Server.on("/power/all", power_all);
  Server.on("/power", _power);
  Server.on("/voltage", voltage);
  Server.on("/current", current);
  Server.on("/energy", energy);


  Portal.config(Config);

  // Establish a connection with an autoReconnect option.
  if (Portal.begin()) {
    Serial.println("WiFi connected: " + WiFi.localIP().toString());
    configTime(TIMEZONE, 0, NTPServer1, NTPServer2);
  }

  // avoid ping slow RTT - https://github.com/espressif/arduino-esp32/issues/1484
  // https://arduino-esp8266.readthedocs.io/en/latest/esp8266wifi/generic-class.html#wifi-power-management-dtim
  WiFi.setSleep(false);
}

void loop() {
  Portal.handleClient();
  yield();

  count++;
  if(count%500000==0)
    blink(2,30);

  if(count%600000==0) {
    Serial.print(rom_phy_get_vdd33());
    Serial.println("VCC");
  }
  yield();
}
