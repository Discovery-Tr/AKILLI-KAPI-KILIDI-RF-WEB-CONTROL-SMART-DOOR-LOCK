/*
  Name:    Discovery Door Lock.ino
  Created: 22/02/2021 23:51:11 AM
  Author:  Adem KIRMIZIYÜZ
*/

// Bütün Kütüphaneler ekleniyor
#include <DNSServer.h>
#include <ESP8266WiFi.h>         //https://github.com/esp8266/Arduino
#include <ESP8266WebServer.h>
#include <WiFiManager.h>         // https://github.com/tzapu/WiFiManager
#include <Servo.h>
#include <RCSwitch.h>
#include <EEPROM.h>
#define EEPROM_SIZE 1
RCSwitch mySwitch = RCSwitch();
WiFiServer server(80);
Servo myservo;
int buttonState;// EEPROM a kaydedeceğimiz buton durumu.


// Pinler
const int openButton = 5;  // D1
const int closeButton  = 14; // D5
const int closeLed = 12;    // D6
const int resetButton = 13; // D7
const int openLed = 15;     // D8
int buzzerPin = 16; //D0

void setup() {
  Serial.begin(115200);
  pinMode(buzzerPin, OUTPUT);
  //Pin modlarını ayarlayan method çağrılıyor
  initPinModes();

  // Wireless bağlantı ayarlarını yapan method
  initWireless();
  EEPROM.begin(EEPROM_SIZE);
  buttonState = EEPROM.read(0);// Buton durumunu EEPROM a kaydediyoruz.
  Serial.println("İlk Açılış");
  Serial.println(EEPROM.read(0));
  mySwitch.enableReceive(0);  // D3
  delay(2500);
}

void loop()
{
  buttonControl();
  htmlControl();
  rfControl();
}

void initWireless()
{
  // WiFiManager
  WiFiManager wifiManager;
  wifiManager.setSTAStaticIPConfig(IPAddress(192, 168, 1, 200), IPAddress(192, 168, 1, 1), IPAddress(255, 255, 255, 0));
  wifiManager.autoConnect("Discovery Door Lock");
  Serial.println("Connected.");
  server.begin();
}
// Pin modlarını ayarlayan method
void initPinModes()
{
  Serial.begin(115200);
  pinMode(openLed, OUTPUT);
  pinMode(closeLed, OUTPUT);
  pinMode(openButton, INPUT_PULLUP);
  pinMode(closeButton, INPUT_PULLUP);
  pinMode(resetButton, INPUT_PULLUP);
}

void htmlControl() {
  // bir clien istemci baglı olup olmadığını kontrol ediyoruz
  WiFiClient client = server.available();

  if (!client) {
    return;
  }

  // client ın bir data gondermesini bekliyoruz
  Serial.println("new client");
  while (!client.available()) {
    delay(1);
  }

  // gelen istekleri okuyoruz
  String request = client.readStringUntil('\r');
  Serial.println(request);
  client.flush();

  if ((request.indexOf("/KAPI-KAPALI") != -1 ) && (buttonState == 0))
  {
    Serial.println("Web Kapanış");
    Kapi_Kapat();

  }
  if ((request.indexOf("/KAPI-ACIK") != -1 ) && (buttonState == 1))
  {
    Serial.println("Web Açılış");
    Kapi_Ac();

  }


  // bu kısımda html kodlarını internet arayüzüne yazıdırıyoruz.
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");
  client.println("");
  client.println("<!DOCTYPE html><html lang='tr'><head><meta charset='utf-8'><meta name='viewport' content='width=device-width, initial-scale=1,shrink-to-fit=no'>");
  client.println("<!-- Bootstrap CSS -->");
  client.println("<link rel='stylesheet' href='https://maxcdn.bootstrapcdn.com/bootstrap/4.0.0-alpha.6/css/bootstrap.min.css' integrity='sha384-rwoIResjU2yc3z8GV/NPeZWAv56rSmLldC3R/AZzGRnGxQQKnKkoFVhFQhNUwEyJ' crossorigin='anonymous'>");
  client.println("<meta http-equiv='refresh' content='3; URL=http://192.168.1.200/'></head>");
  client.println("<body>");
  client.println("<div class='container mt-2'>");
  client.println("<div class='row'>");
  client.println("<div class='col-sm-12'>");
  client.println("<div class='card'>");
  client.println("<div class='card-header'>");
  client.println("<h4>NodeMCU Door Controller</h4>");
  client.println("</div>");
  client.println("<div class='card-body'>");
  client.println("<div class='table-responsive'>");
  client.println("<thead>");
  client.println("</thead>");
  client.println("<tbody>");
  client.println("<table width='10%' height='10%' border='0' cellspacing='1' cellpadding='1'align='center'><tr>");
  client.println("<td class='align-center'>");
  if (buttonState == 0)
  {
    client.println("<a href=\"/KAPI-KAPALI\"\"><div class='d-flex align-items-center rounded float-center p-3 py-2 mb-1 bg-light rounded' style='font-size: 10em'><svg x='10' class='bi bi-unlock-fill' width='1em' height='1em' viewBox='0 0 16 16' fill='currentColor' xmlns='http://www.w3.org/2000/svg'><path d='M11 1a2 2 0 0 0-2 2v4a2 2 0 0 1 2 2v5a2 2 0 0 1-2 2H3a2 2 0 0 1-2-2V9a2 2 0 0 1 2-2h5V3a3 3 0 0 1 6 0v4a.5.5 0 0 1-1 0V3a2 2 0 0 0-2-2z'></path></svg></div></a>");

    buttonState == 1;
  }
  else if (buttonState == 1)
  {
    client.println("<a href=\"/KAPI-ACIK\"\"><div class='d-flex align-items-center rounded float-center p-3 py-2 mb-1 bg-light rounded' style='font-size: 10em'><svg class='bi bi-lock-fill' width='1em' height='1em' viewBox='0 0 16 16' fill='currentColor' xmlns='http://www.w3.org/2000/svg'><path d='M8 1a2 2 0 0 1 2 2v4H6V3a2 2 0 0 1 2-2zm3 6V3a3 3 0 0 0-6 0v4a2 2 0 0 0-2 2v5a2 2 0 0 0 2 2h6a2 2 0 0 0 2-2V9a2 2 0 0 0-2-2z'></path></svg></div></a>");
    buttonState == 0;
  }
  client.println("</td>");
  client.println("</tr>");
  client.println("</table>");
  client.println("</tbody>");
  client.println("</div>");
  client.println("</div>");
  client.println("<div class='card-footer'>");
  client.println("<div class='form-group float-left'>Adem KIRMIZIYÜZ 2021</div>");
  client.println("</div>");
  client.println("</div>");
  client.println("</body>");
  client.println("</html>");
  delay(1);
  Serial.println("Client disonnected");
  Serial.println("");

}


void buttonControl() {
  if ((digitalRead(closeButton) == HIGH) && (buttonState == 0))
  {
    Serial.println("Buton Kapanış");
    Kapi_Kapat();
  }

  if ((digitalRead(openButton) == HIGH) && (buttonState == 1))
  {
    Serial.println("Buton Açılış");
    Kapi_Ac();

  }

  if (digitalRead(resetButton) == HIGH)
  {
    Serial.println("Ayarlar Silindi");
    digitalWrite(closeLed, HIGH);
    digitalWrite(openLed, HIGH);
    ESP.eraseConfig();
    delay(3500);
    digitalWrite(closeLed, LOW);
    digitalWrite(openLed, LOW);
    EEPROM.write(0, 0);
    EEPROM.commit();
    ESP.restart();
  }
}
void rfControl()
{
  int value = mySwitch.getReceivedValue();
  if (mySwitch.available()) {

    if ((value == 14244580) && (buttonState == 1))
    {
      Serial.println("rf Açılış");

      Kapi_Ac();
      delay(1500);
    }
    if ((value == 14244577) && (buttonState == 0))
    {
      Serial.println("rf Kapanış");

      Kapi_Kapat();
      delay(1500);
    }
    else
    {
      Serial.print("İşlem Tekrarı. ");
      Serial.println(mySwitch.getReceivedValue());
    };
    mySwitch.resetAvailable();
  }
}


void Kapi_Kapat()
{
  WiFiClient client = server.available();
  digitalWrite(openLed, HIGH);
  for (int i = 0; i < 2; i++) // Buzzer için 2 defa dönülüyor
  {
    digitalWrite(buzzerPin, HIGH); // Buzzer açılıyor
    delay(200); // 0.2 sn bekleniyor
    digitalWrite(buzzerPin, LOW); // Buzzer kapatılıyor
    delay(200); // 0.2 sn bekleniyor
  }
  Serial.println(EEPROM.read(0));
  myservo.attach(4);//D2
  myservo.write(100);
  client.print("POST /KAPI-KAPALI/ HTTP/1.1");
  delay(3500);
  client.print("POST / HTTP/1.1");
  myservo.detach();
  buttonState = 1;
  EEPROM.write(0, buttonState);
  EEPROM.commit();
  digitalWrite(openLed, LOW);
}

void Kapi_Ac()
{
  digitalWrite(closeLed, HIGH);
  WiFiClient client = server.available();
  digitalWrite(buzzerPin, HIGH);
  delay(200);
  digitalWrite(buzzerPin, LOW);
  Serial.println(EEPROM.read(0));
  myservo.attach(4);//D2
  myservo.write(80);
  client.print("POST /KAPI-ACIK/ HTTP/1.1");
  delay(5000);
  myservo.write(91 );
  delay(2000);

  myservo.attach(4);//D2
  myservo.write(100);
  delay(60);
  myservo.detach();

  client.print("POST / HTTP/1.1");
  buttonState = 0;
  EEPROM.write(0, buttonState);
  EEPROM.commit();
  digitalWrite(closeLed, LOW);
}
