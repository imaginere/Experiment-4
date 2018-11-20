/* Code for the Night Stand Book
This code is for the C&C class 4 project.
Tabitha, JingPo & Frank Ferrao*/

/*
    Base Code supplied by:
   Creation & Computation - Digital Futures, OCAD University
   Kate Hartman / Nick Puckett


   PubNub Test
   Connect device to the embedded network - tested with Huzzah32 ESP32
   Read analog values and send them to PubNub

 * ***NOTE***
   When Wifi is enabled you can only use these pins as analog inputs
   A2
   A3
   A4
   A9 - 33
   A7 - 32

*/

#include <ArduinoJson.h>
#include "esp_wpa2.h"
#include <WiFi.h>
#define PubNub_BASE_CLIENT WiFiClient
#define PUBNUB_DEBUG
#include <PubNub.h>
#include <wifiDetails.h>

// The code to connec tto Pub nub
const static char pubkey[] = "pub-c-04f6b653-d826-4506-a887-4781a79392b1";  //get this from your PUbNub account
const static char subkey[] = "sub-c-4fe380e2-ebb6-11e8-9716-4ad2abb50219";  //get this from your PubNub account

const static char pubChannel[] = "sensorValues"; //choose a name for the channel to publish messages to


unsigned long lastRefresh = 0;    //used to make the timer work
int publishRate = 2000;           //how often to update the value on pubnub in milliseconds


int readPin2 = A3;

int sensorVal2;                       //variables to hold values to send

int counter;

String whoAmI = "Frank";

int lightOn = false;
String lightStat = "";


// Enable Wifi on th Feather
void setup()
{
  Serial.begin(9600);

  // Connect to Wifi - You must have wifiDetails.h in your library folder for this to work.
  // Add your wifi location in the () - "0" for OCAD or "1" for home.
  // Note: you will need to update the wifiDetails.h file with your home wifi credentials.
  connectStandardWifi(0);                              

  PubNub.begin(pubkey, subkey);                      //connect to the PubNub Servers
  Serial.println("PubNub Connected");

}


// Send Sensor Data from the Fether to PubNub and then send an SMS
// Stil have to code this

void loop()
{

  sensorVal2 =  map(analogRead(readPin2), 0, 300, 0, 100);

  if (sensorVal2 > 800 && lightOn == false) {
    lightStat = "Light came On";
    //lightOn = true;
  }
  else {
    lightStat = "Light is Off";
    //lightOn = false;
  }

  // This is where I need to check the value and then send and SMS message

  

  if (millis() - lastRefresh >= publishRate) //timer used to publish the values at a given rate
  {
    publishToPubNub();                      //execute the function that sends the values to pubnub
    lastRefresh = millis();                 //save the value so that the timer works

  }

}


void publishToPubNub()
{
  WiFiClient *client;
  DynamicJsonBuffer messageBuffer(600);                    //create a memory buffer to hold a JSON Object
  JsonObject& pMessage = messageBuffer.createObject();    //create a new JSON object in that buffer

  ///the imporant bit where you feed in values
  pMessage["who"] = whoAmI;
  pMessage["sensorVal2"] = sensorVal2;                     //add a new property and give it a value
  pMessage.prettyPrintTo(Serial);   //uncomment this to see the messages in the serial monitor


  int mSize = pMessage.measureLength() + 1;                   //determine the size of the JSON Message
  char msg[mSize];                                            //create a char array to hold the message
  pMessage.printTo(msg, mSize);                              //convert the JSON object into simple text (needed for the PN Arduino client)

  client = PubNub.publish(pubChannel, msg);                      //publish the message to PubNub

  if (!client)                                                //error check the connection
  {
    Serial.println("client error");
    delay(1000);
    return;
  }

  if (PubNub.get_last_http_status_code_class() != PubNub::http_scc_success)  //check that it worked
  {
    Serial.print("Got HTTP status code error from PubNub, class: ");
    Serial.print(PubNub.get_last_http_status_code_class(), DEC);
  }

  while (client->available())                                 //get feedback from PubNub
  {
    Serial.write(client->read());
  }
  client->stop();                                             //stop the connection
  Serial.print("Successful Publish:");
  Serial.print("\t");    // prints a tab
  Serial.print(whoAmI);
  Serial.print("\t");    // prints a tab        
  Serial.print(sensorVal2);           
  Serial.print("\t");  
  Serial.print(lightStat);    
}
