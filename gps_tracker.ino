#include <TinyGPS++.h>
#include <SoftwareSerial.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h> 
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>
#include <BlynkSimpleEsp8266.h>

static const int RXPin = 4, TXPin = 5;   // GPIO 4=D2(conneect Tx of GPS) and GPIO 5=D1(Connect Rx of GPS

TinyGPSPlus gps; // The TinyGPS++ object

SoftwareSerial ss(RXPin, TXPin);  // The serial connection to the GPS device

/* Set these to your desired credentials. */
const char *ssid = "Zhone_C8B5";  //ENTER YOUR WIFI SETTINGS
const char *password = "znid314207157";

const char *host = "http://gps-info.plantcare.cloud";

BlynkTimer timer;
float latitude;
float longitude;
float spd;       //Variable  to store the speed
float sats;      //Variable to store no. of satellites response
String bearing;  //Variable to store orientation or direction of GPS

void setup()
{
  delay(1000);
  Serial.begin(115200);
  WiFi.mode(WIFI_OFF);        //Prevents reconnection issue (taking too long to connect)
  delay(1000);
  WiFi.mode(WIFI_STA);        //This line hides the viewing of ESP as wifi hotspot
  
  WiFi.begin(ssid, password);     //Connect to your WiFi router
  Serial.println("");

  Serial.print("Connecting");
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  //If connection successful show IP address in serial monitor
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());  //IP address assigned to your ESP
  
  Serial.println();
  ss.begin(9600);
  
  timer.setInterval(5000L, checkGPS); // every 5s check if GPS is connected, only really needs to be done once
}

void checkGPS(){
  if (gps.charsProcessed() < 10)
  {
    Serial.println(F("No GPS detected: check wiring."));
  }
}

void loop()
{
    HTTPClient http;    //Declare object of class HTTPClient

    while (ss.available() > 0) 
    {
      // sketch displays information every time a new sentence is correctly encoded.
      if (gps.encode(ss.read()))
      {
        if (gps.location.isValid() ) 
        {    
          latitude = (gps.location.lat());     //Storing the Lat. and Lon. 
          longitude = (gps.location.lng()); 
    
          Serial.print("LAT:  ");
          Serial.println(latitude, 6);  // float to x decimal places
          Serial.print("LONG: ");
          Serial.println(longitude, 6);
          
          spd = gps.speed.kmph();               //get speed
       
          sats = gps.satellites.value();    //get number of satellites

          bearing = TinyGPSPlus::cardinal(gps.course.value()); // get the direction
        }
        Serial.println();

        http.begin("http://gps-info.plantcare.cloud/sendValues?latitude="+String(latitude*10000)+"&longitude="+String(longitude*10000)+"&speed="+String(spd)+"&direction="+String(bearing)+"&satellites="+String(sats)+"");     //Specify request destination
        int httpCode = http.GET();            //Send the request
        String payload = http.getString();    //Get the response payload

        Serial.println(httpCode);   //Print HTTP return code
        Serial.println(payload);    //Print request response payload
      
        http.end();  //Close connection
        delay(5000);
      }      
  }
  timer.run();
}
