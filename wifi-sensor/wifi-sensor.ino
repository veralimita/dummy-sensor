
/*
  Blink
  Turns on an LED on for one second, then off for one second, repeatedly.

  Most Arduinos have an on-board LED you can control. On the UNO, MEGA and ZERO 
  it is attached to digital pin 13, on MKR1000 on pin 6. LED_BUILTIN takes care 
  of use the correct LED pin whatever is the board used.
  If you want to know what pin the on-board LED is connected to on your Arduino model, check
  the Technical Specs of your board  at https://www.arduino.cc/en/Main/Products
  
  This example code is in the public domain.

  modified 8 May 2014
  by Scott Fitzgerald
  
  modified 2 Sep 2016
  by Arturo Guadalupi
*/

#include "ESP8266WiFi.h"
#include "DHT.h"

#define DHTPIN 0     // what digital pin we're connected to

// Uncomment whatever type you're using!
//#define DHTTYPE DHT11   // DHT 11
#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321
//#define DHTTYPE DHT21   // DHT 21 (AM2301)

// Connect pin 1 (on the left) of the sensor to +5V
// NOTE: If using a board with 3.3V logic like an Arduino Due connect pin 1
// to 3.3V instead of 5V!
// Connect pin 2 of the sensor to whatever your DHTPIN is
// Connect pin 4 (on the right) of the sensor to GROUND
// Connect a 10K resistor from pin 2 (data) to pin 1 (power) of the sensor

// Initialize DHT sensor.
// Note that older versions of this library took an optional third parameter to
// tweak the timings for faster processors.  This parameter is no longer needed
// as the current DHT reading algorithm adjusts itself to work on faster procs.
DHT dht(DHTPIN, DHTTYPE);
const char* ssid     = "MOVISTAR_BAF6";
const char* password = "3DA2C8917D4296AAEE8D";

const char* host     = "iot.veralimita.com";
String path          = "/" ;

byte mac[6]; 
String s_mac;     

// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  Serial.begin(115200);

  WiFi.begin(ssid, password);

  s_mac = connectToWifi();

  dht.begin();
}
// the loop function runs over and over again forever
void loop() {

 // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  float f = dht.readTemperature(true);

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t) || isnan(f)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  // Compute heat index in Fahrenheit (the default)
  float hif = dht.computeHeatIndex(f, h);
  // Compute heat index in Celsius (isFahreheit = false)
  float hic = dht.computeHeatIndex(t, h, false);

  Serial.print("Humidity: ");
  Serial.print(h);
  Serial.print(" %\t");
  Serial.print("Temperature: ");
  Serial.print(t);
  Serial.print(" *C ");
  Serial.print(f);
  Serial.print(" *F\t");
  Serial.print("Heat index: ");
  Serial.print(hic);
  Serial.print(" *C ");
  Serial.print(hif);
  Serial.println(" *F");

  if (WiFi.status() != WL_CONNECTED) {
     s_mac = connectToWifi();
  }
  

  WiFiClient client;
  const int httpPort = 80;
  if (!client.connect(host, httpPort)) {
    return;
  };

  int sensorValue = analogRead(2);
  Serial.println();
  Serial.print("voltage: ");
  Serial.println(sensorValue);

  // Convert the analog reading (which goes from 0 - 1023) to a voltage (0 - 5V):
  float voltage = sensorValue * (3.3 / 1023.0);

  String data = "{\"temperature\": " + String(t) + ", \"humidity\": " + String(h) +  ", \"heatIndex\": " + String(hic) + ", \"voltage\": " + String(voltage) + ", \"mac\": \"" + s_mac + "\" }";
 

  client.print(String("POST ") + "/meteo" + " HTTP/1.1\r\n" +
                 "Host: " + host + "\r\n" +
                 //"Connection: close\r\n" +
                 "Content-Type: application/json\r\n" +
                 "Content-Length: " + data.length() + "\r\n" +
                 "\r\n" + // This is the extra CR+LF pair to signify the start of a body
                 data + "\n");


  delay(1000 * 60 * 5);

return;
  //on-of code     
  client.print(String("GET ") + path + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" + 
               "Connection: keep-alive\r\n\r\n");

  delay(1000); // wait for server to respond

  // read response
  int dir = 0; 
    String section="header";
  while(client.available()){
    String line = client.readStringUntil('\r');
    if (line == "\n") {
      Serial.println("RESPONSE:");
    }
  /////////////////////////////////////////////////////////////////////
  if (line.substring(1,10) == "response:") {
    Serial.println(line.substring(10));
    if (line.substring(10) == "ON") {
      digitalWrite(2, HIGH);
    }
    else {
         digitalWrite(2, LOW);
    }
  } 
 }
}

String connectToWifi(){    

  Serial.println("\n");
  Serial.print("Connecting to ");
  Serial.println(ssid);   
  Serial.println("");
  while (WiFi.status() != WL_CONNECTED) {
      
      delay(1000);
      Serial.print(".");
    
  }
  Serial.println("");
  Serial.println("Connected!");

  WiFi.macAddress(mac);
  return String(mac[5],HEX)+ ":" + String(mac[4],HEX) + ":" + String(mac[3],HEX) + ":" + String(mac[2],HEX) + ":" + String(mac[1],HEX) + ":" + String(mac[0],HEX);
}
