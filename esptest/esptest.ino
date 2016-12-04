#include <SoftwareSerial.h>
#include <MyProperty.h>
#include <dht.h>
#include <SPI.h>
#include <RH_NRF24.h>


/*Time*/
#define sec 1000
#define minute 60000
#define hour 3600000

#define DHT11 7
#define Relay 10
#define Pump 6
#define Soil 5
#define RelayTime minute*10
#define DEBUG true

MyProperty property = MyProperty();
dht DHT;
SoftwareSerial ESPSerial(8, 9); // RX, TX

String apiKey = (String) property.getApiKey();
String ssid = (String) property.getSsid();
String pass = (String) property.getPass();

int value;
int RelayStatus;
unsigned long start, finished, elapsed, startRelay;



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

void sendSensore(String str){
  
  // TCP connection
  String cmd = "AT+CIPSTART=\"TCP\",\"";
  cmd += "184.106.153.149"; // api.thingspeak.com
  cmd += "\",80\r\n";
  sendData(cmd,5000,DEBUG);

  if(ESPSerial.find("ERROR")){
    Serial.println("AT+CIPSTART error");
    return;
  }else{
    Serial.println("AT+CIPSTART OK");
  }
  delay(1000);
  // prepare GET string
  String getStr = "GET /update?api_key=";
  getStr += apiKey;
  getStr += String(str);
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
}



void setup()
{
  Serial.begin(115200); // A belső port megnyitása azonos sebességű legyen az ESPSeriel-al
  ESPSerial.begin(115200); // ESP8266 port 115200 kapcsolati sebesség
  sendData("AT+CIOBAUD=9600\r\n",2000,DEBUG);
  Serial.begin(9600); // A belső port megnyitása azonos sebességű legyen az ESPSeriel-al
  ESPSerial.begin(9600); // ESP8266 port 9600 kapcsolati sebesség

  pinMode(Relay, OUTPUT);
  pinMode(DHT11, OUTPUT);
  pinMode(Pump, OUTPUT);
  pinMode(Soil, INPUT);
  RelayStatus = 1;
  digitalWrite(Relay, HIGH);
  startRelay=millis();
  start=millis();
  
  Serial.println("ESP8266 WIFI PORT START");
  sendData("AT+CWMODE=3\r\n",1000,DEBUG);
  sendData("AT+CWJAP=\""+ssid+"\",\""+pass+"\"\r\n",5000,DEBUG);
}



void loop()
{
  String strTemp, strHum, strSoil, strLight, strPump, str;

  /* Temp and Hum */
   int chk = DHT.read11(DHT11);
  /* Soil*/
  value= analogRead(Soil);
  
  /*Relay*/
  if(millis()-startRelay > RelayTime){
    if(RelayStatus){
      RelayStatus = 0;
     digitalWrite(Relay, LOW);
    }else{
      RelayStatus = 100;
     digitalWrite(Relay, HIGH);
    }
     startRelay=millis();
  }

  
  strTemp = String(DHT.temperature);
  strHum = String (DHT.humidity);
  strSoil = String(value);
  strLight = String(RelayStatus);
  strPump = "0";
  str  = "&field1=" + strTemp;
  str += "&field2=" + strHum;
  str += "&field3=" + strSoil;
  str += "&field4=" + strLight;
  str += "&field5=" + strPump;
  sendSensore(str);
  delay(1000);
}


