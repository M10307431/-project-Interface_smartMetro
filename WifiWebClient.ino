#include <LTask.h>
#include <LWiFi.h>
#include <LWiFiClient.h>

#define WIFI_AP "icl-B03"
#define WIFI_PASSWORD "EE5040701"
#define WIFI_AUTH LWIFI_WEP  // choose from LWIFI_OPEN, LWIFI_WPA, or LWIFI_WEP.
#define SITE_URL "140.118.206.163"
//--------I/O----------------
#define LED  13

LWiFiClient c;

void setup()
{
  //----- define I/O------------
  pinMode(LED, OUTPUT);
  LTask.begin();
  LWiFi.begin();
  Serial.begin(9600);  // monitoring
  Serial1.begin(9600);  // uart -> pxa
  delay(10000);
  // keep retrying until connected to AP
  Serial.println("Connecting to AP");
  if (LWiFi.connectWPA(WIFI_AP, WIFI_PASSWORD) < 0)
  {
    Serial.println("Fail!");
    return;
  }
  Serial.println("Success!");
  // keep retrying until connected to website
  Serial.println("Connecting to Server...");
  while (!c.connect(SITE_URL, 12345)) {
    Serial.println("Fail!");
    delay(1000);
  }
  Serial.println("Success!");
}

boolean disconnectedMsg = false;

void loop()
{
  String input = "";
  String info = "";
  while (Serial1.available())  // waiting for pxa
  {
    char i = Serial1.read();  // recv input data form pxa
    if (i != -1)
    {
      input += i;
    }
    delay(5);
  }
  if(input != "") {
    Serial.println(input);
    c.print(input);      // send input data to server
    delay(1000);
    while (true) {
      int recv = c.read(); // recv info from server
      if(recv != -1) {
        info += (char)recv;
        Serial.print((char)recv);
      }
      else {
        if(info != "") {
          Serial.println("");
          Serial1.println(info); // send info to pxa
          info = "";
          input = "";
          break;
        }
        break;
        //Serial.println("Receiving data...");
        //delay(1000);
      }
      delay(5);
    }
  }
  Serial.println("Waiting data...");
  delay(1000);
  /*
  if (!disconnectedMsg)
  {
    Serial.println("disconnected by server");
    disconnectedMsg = true;
  }
  delay(500);
  */
}
