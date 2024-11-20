//Blynk IoT based Plant Watering System using Soil Sensor
//Additionally DHT Temperature is used for monitoring temperature
//We can control in using Web dasboard as well as  blynk Android App

#define BLYNK_PRINT Serial


#define BLYNK_TEMPLATE_ID "TMPL38un2cZYR"
#define BLYNK_TEMPLATE_NAME "Plant Watering System"
#define BLYNK_AUTH_TOKEN "iyEMcG5DzfHfN46bdnksdxvrtH28qBFm"

#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <DHT.h>

int motorRelayPin = 0;
int indicationLight = 2;
int soilSensorPin = 34;
int dhtPin = 32;
int dhtType = 11;
int pinValue = 0;
int thresholdMoistureValue = 20;
int maxMoistureValue = 100;
int relayPinStatus = 0;

char ssid[] = "Shiva";
char pass[] = "Jai_Sharabeshwara_Swami";


//Declaring object of BlykTimer and DHT
BlynkTimer timer;
DHT dht(dhtPin,dhtType);

BLYNK_WRITE(V0)
{
  pinValue = param.asInt();
  relayPinStatus = !pinValue;
  digitalWrite(motorRelayPin,relayPinStatus);
  digitalWrite(indicationLight,pinValue);

  Serial.print("Relay pin value : ");
  Serial.println(relayPinStatus);
  
  Serial.println(pinValue);
}

void sendSoilMoisture()
{
  int value = analogRead(soilSensorPin);
  int measuredMoisture = map(value,0,4095,0,100);
  int moisture = maxMoistureValue - measuredMoisture;
  Blynk.virtualWrite(V1,moisture);

  // if(moisture < thresholdMoistureValue)
  // {
  //   digitalWrite(motorRelayPin,HIGH);
  //   Blynk.virtualWrite(V0,HIGH);
  // }

  if(moisture >= maxMoistureValue)
  {
    digitalWrite(motorRelayPin,HIGH);
    Blynk.virtualWrite(V0,LOW);
  }

  else
  {
    digitalWrite(motorRelayPin,relayPinStatus);
    Blynk.virtualWrite(V0,pinValue);
  }
}

void sendTemperature()
{
  int celsius = dht.readTemperature();
  Blynk.virtualWrite(V2,celsius);
}

void sendHumidity()
{
  int humidity = dht.readHumidity();
  Blynk.virtualWrite(V3,humidity);
}

void setup()
{
  //Initializing the Blink cloud server
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);

  //Initialising the Serial Communication
  Serial.begin(115200);

  //Initializing the Pins
  pinMode(motorRelayPin, OUTPUT);
  pinMode(soilSensorPin, INPUT);
  pinMode(dhtPin, INPUT);
  pinMode(indicationLight, OUTPUT);

  //Initialising the DHT sensor
  dht.begin();

  //Turning the relay OFF during setup
  digitalWrite(motorRelayPin, HIGH);

  //Setting interval for the function
  timer.setInterval(1000L,sendSoilMoisture);
  timer.setInterval(1000L,sendTemperature);
  timer.setInterval(1000L,sendHumidity);
}

void loop()
{
  
  Blynk.run();
  timer.run();
}


