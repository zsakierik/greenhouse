#include <dht.h>

#include <SPI.h>
#include <RH_NRF24.h>


dht DHT;

/*Time*/
#define sec 1000
#define minute 60000
#define hour 3600000

#define DHT11 7
#define Relay 10
#define Pump 6
#define Soil 5
#define RelayTime minute*10

int value;
int RelayStatus;

unsigned long start, finished, elapsed, startRelay;

void setup(){
  Serial.begin(9600);
  pinMode(Relay, OUTPUT);
  pinMode(DHT11, OUTPUT);
  pinMode(Pump, OUTPUT);
  pinMode(Soil, INPUT);
  RelayStatus = 1;
  digitalWrite(Relay, HIGH);
  startRelay=millis();
  start=millis();
  Serial.print("Temperature;Humidity;Soil;Relay;Time;");
}


void displayResult()
{
  float h,m,s,ms;
  unsigned long over;
  elapsed=finished-start;
  h=int(elapsed/3600000);
  over=elapsed%3600000;
  m=int(over/60000);
  over=over%60000;
  s=int(over/1000);
  ms=over%1000;
/*  Serial.print("Raw elapsed time: ");
  Serial.println(elapsed);
  Serial.print("Elapsed time: ");*/
  Serial.print(h,0);
  Serial.print("h ");
  Serial.print(m,0);
  Serial.print("m ");
  Serial.print(s,0);
  Serial.print("s ");
  Serial.print(ms,0);
  Serial.print("ms");
}


void loop()
{
  
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
      RelayStatus = 1;
     digitalWrite(Relay, HIGH);
    }
     startRelay=millis();
  }
  
  delay(sec);
  
  finished=millis();
  Serial.println();
  Serial.print(DHT.temperature);
  Serial.print(";");
  Serial.print(DHT.humidity);
  Serial.print(";");
  Serial.print(value);
  Serial.print(";");
  Serial.print(RelayStatus);
  Serial.print(";");
  displayResult();
  Serial.print(";");





  
    /*Pump*/
   /* delay(2000); 
    digitalWrite(Pump, HIGH);
    delay(2000); 
    digitalWrite(Pump, LOW);*/
  
  
}

