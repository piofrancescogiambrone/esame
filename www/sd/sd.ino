#include <SPI.h>
#include <Ethernet.h>
#include <SD.h>
#include <SFE_BMP180.h>
#include <Wire.h>
#include <dht11.h>
int pinLED = 9;
boolean acceso = false; 
byte mac[] = { 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 };
IPAddress ip(192, 168, 1, 2); 
EthernetServer server(80);  
File webFile;
dht11 DHT;
#define DHT11_PIN 2
SFE_BMP180 pressure;
#define ALTITUDE 200.0
char status;
double T,P,p0,a;
int lum,lux;
void setup()
{
    pinMode(pinLED,OUTPUT);
    digitalWrite(pinLED,LOW);
    Ethernet.begin(mac, ip);  
    server.begin();           
    Serial.begin(9600); 
    if (pressure.begin())
      Serial.println("BMP180 init success");
    else
    {
      while(1); 
    }       
    if (!SD.begin(4)) {
        return;
    }
    if (!SD.exists("index.htm")) {
        return;
    }
}
void loop()
{
  int lum = analogRead(2); 
  int Vout = (lum * 0.0048828125);
  int lux = (10000.0 * (5 - Vout))/Vout;
  status = pressure.getPressure(P,T);
  if (status != 0)
  {
    Serial.println(P);
    p0 = pressure.sealevel(P,ALTITUDE); 
    a = pressure.altitude(P,p0);
  }
  int chk = DHT.read(DHT11_PIN);    // READ DATA
  switch (chk){
    case DHTLIB_OK:  
                Serial.print("OK,\t"); 
                break;
    case DHTLIB_ERROR_CHECKSUM: 
                Serial.print("Checksum error,\t"); 
                break;
    case DHTLIB_ERROR_TIMEOUT: 
                Serial.print("Time out error,\t"); 
                break;
    default: 
                Serial.print("Unknown error,\t"); 
                break;
  }
  int temp=DHT.temperature;
  int umi=DHT.humidity;
  EthernetClient client = server.available();
  String postText ="";
  if (client) {  
        boolean currentLineIsBlank = true;
        while (client.connected())
        {
            if (client.available())
            {
                char c = client.read();
                        if(postText.length()<10){
                          postText +=c;
                        }
                if (c == '\n' && currentLineIsBlank)
                {
                    client.println(F("HTTP/1.1 200 OK"));
                    client.println(F("Content-Type: text/html"));
                    client.println(F("Connection: close"));
                    client.println();
                   /* webFile = SD.open("index.htm");       
                    if (webFile) 
                    {
                        while(webFile.available())
                        {
                            client.write(webFile.read()); 
                        }
                        webFile.close();
                    }
                        if(postText.indexOf("?on") >0){
                              digitalWrite(pinLED,HIGH);
                              acceso = true;
                            }
                         else if(postText.indexOf("?off") >0 ){
                            digitalWrite(pinLED,LOW);
                            acceso = false;
                           }
                          if(acceso){
                              String ciao=(String)temp+","+(String)umi;*/
                             // client.println(F("<a href=\"./?off\"> <button>Spengi</button></a>"));
                             //  client.println(ciao);
                             /* ciao=(String)lum;
                              client.println(ciao);*/
                              client.print(temp);
                              client.print(F(":"));
                              client.print(umi);
                              client.print(F(":"));
                              client.print(lux);
                              client.print(F(":"));
                              client.print(P);
                           /* }else{
                              /*client.println(F("<a href=\"./?on\"> <button style=''>Accendi</button></a>"));

                              client.println(F("<h3> Il LED è spento </h3>"));
                              }   */
                        delay(1);
                    break;
                }
                if (c == '\n') {
                    currentLineIsBlank = true;
                } 
                else if (c != '\r') {
                    currentLineIsBlank = false;
                }
            }

        } 
        delay(1);    
        client.stop(); 
    } 
}

