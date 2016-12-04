/*  
 Név:        TransparentSerial
 Leírás:     Transzparens USB-soros-SWsoros átjáró ESP-8266 WiFi modulhoz
 Link:       http://www.tavir.hu/iot-0
 Fordító:    Arduino-1.6.5.
 Lapka/chip: Arduino Diecimila/UNO
 Szerző:     superp (nick@ forum.tavir.hu)
 Módosítás:  Cseh Róbert / avr(kukac)tavir(pont)hu
 Licensz:    Public Domain
 
 Forrás:     https://www.arduino.cc/en/Tutorial/SoftwareSerialExample
*/
 
#include <SoftwareSerial.h>
#include <MyProperty.h>
#define DEBUG false

MyProperty property = MyProperty();
String apiKey = (String) property.getApiKey();
String ssid = (String) property.getSsid();
String pass = (String) property.getPass();

SoftwareSerial ESPSerial(8, 9); // RX, TX

String sendData(String command, const int timeout, boolean debug)
{
    String response = "";
    ESPSerial.print(command); // send the read character to the esp8266
    long int time = millis();
    while( (time+timeout) > millis())
    {
      while(ESPSerial.available())
      {
        
        // The esp has data so display its output to the serial window 
        char c = ESPSerial.read(); // read the next character.
        response+=c;
      }  
    }
    
    if(debug)
    {
      Serial.println("");
      Serial.println("response: ");
      Serial.print(response);
      Serial.println("");
    }
    return response;
}

String sendSensore(String strTemp){
  
  // TCP connection
  String cmd = "AT+CIPSTART=\"TCP\",\"";
  cmd += "184.106.153.149"; // api.thingspeak.com
  cmd += "\",80\r\n";
  sendData(cmd,5000,DEBUG);

  if(ESPSerial.find("ERROR")){
    Serial.println("AT+CIPSTART error");
    return "Erorr";
  }else{
    Serial.println("AT+CIPSTART OK");
  }
  delay(1000);
  // prepare GET string
  String getStr = "GET /update?api_key=";
  getStr += apiKey;
  getStr +="&field1=";
  getStr += String(strTemp);
  getStr += "\r\n";
  Serial.println(getStr);
  
  // send data length
  cmd = "AT+CIPSEND=";
  cmd += String(getStr.length());
  cmd += "\r\n";
  sendData(cmd,2000,DEBUG);
delay(2000);
ESPSerial.print(getStr); 
delay(2000);
ESPSerial.println("AT+CIPCLOSE");
return "OK";
}



void setup()
{
  Serial.begin(115200); // A belső port megnyitása azonos sebességű legyen az ESPSeriel-al
  ESPSerial.begin(115200); // ESP8266 port 115200 kapcsolati sebesség
  sendData("AT+CIOBAUD=9600\r\n",2000,DEBUG);
  Serial.begin(9600); // A belső port megnyitása azonos sebességű legyen az ESPSeriel-al
  ESPSerial.begin(9600); // ESP8266 port 9600 kapcsolati sebesség
  Serial.println("ESP8266 WIFI PORT START");
  sendData("AT+CWMODE=3\r\n",1000,DEBUG);
  sendData("AT+CWJAP=\""+ssid+"\",\""+pass+"\"\r\n",5000,DEBUG);
}



void loop()
{
  String strTemp = "30";
  sendSensore(strTemp);
  delay(3000);
}


