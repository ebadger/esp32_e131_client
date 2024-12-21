/*
* E131_Test.ino - Simple sketch to listen for E1.31 data on an ESP32 
*                  and print some statistics.
*
* Project: ESPAsyncE131 - Asynchronous E.131 (sACN) library for Arduino ESP8266 and ESP32
* Copyright (c) 2019 Shelby Merrick
* http://www.forkineye.com
*
*  This program is provided free for you to use in any way that you wish,
*  subject to the laws and regulations where you are using it.  Due diligence
*  is strongly suggested before using this code.  Please give credit where due.
*
*  The Author makes no warranty of any kind, express or implied, with regard
*  to this program or the documentation contained in this document.  The
*  Author shall not be liable in any event for incidental or consequential
*  damages in connection with, or arising out of, the furnishing, performance
*  or use of these programs.
*
*/
//#include <WiFi.h>
#include <ESPAsyncE131.h>
#include <Adafruit_NeoPixel.h>
#include <eeboom.h>

//#define UNIVERSE 1                      // First DMX Universe to listen for
//#define UNIVERSE_COUNT 6                // Total number of Universes to listen for, starting at UNIVERSE

struct _Config
{
  uint32_t devID = 0x12345678;
  uint8_t byteUniverse;                // First DMX Universe to listen for
  uint8_t byteUniverseCount;           // Total number of Universes to listen for, starting at UNIVERSE
  uint16_t countLEDs;
  char ssid[16] = "Empty_SSID";
  char password[16] = "Empty_PASS";
  uint8_t staticIP[4] = {0,0,0,0};
  uint8_t gateIP[4] = {0,0,0,0};
  uint8_t subnet[4] = {0,0,0,0};
  uint8_t primaryDNS[4] = {0,0,0,0};
  uint8_t secondaryDNS[4] = {0,0,0,0};
};

EEBoom<_Config> _config;


//#define WS2812_DEFAULT_COUNT 300
#define DATA_PIN 13

bool _initialized = false;

Adafruit_NeoPixel *pixels = NULL;
ESPAsyncE131 *e131 = NULL;


bool testEnterConfig()
{
    if (Serial.available()) 
    {
      char b = Serial.read();  // Read each character from the buffer

      // Check if the user pressed space
      if (b == ' ' ) 
      {
        return true;
      }        
    }
    return false;
}

void SerialWait()
{
  while (Serial.available() == 0)
  { 
  }
}

void SerialFlushInput()
{
  while(Serial.available())
  {
    Serial.read();
  }
}

void dumpConfig()
{
  Serial.println();
  Serial.print("Start universe: ");
  Serial.println(_config.data.byteUniverse);
  Serial.print("Universe count: ");
  Serial.println(_config.data.byteUniverseCount);
  Serial.print("LED count:      ");
  Serial.println(_config.data.countLEDs);
  Serial.print("ssid:           ");
  Serial.println(_config.data.ssid);
  Serial.println("password:       <hidden>");
  Serial.println("IP:             " + String(_config.data.staticIP[0]) + "." + String(_config.data.staticIP[1]) + "." + String(_config.data.staticIP[2]) + "." + String(_config.data.staticIP[3]));
  Serial.println("Gateway:        " + String(_config.data.gateIP[0]) + "." + String(_config.data.gateIP[1]) + "." + String(_config.data.gateIP[2]) + "." + String(_config.data.gateIP[3]));
  Serial.println("Subnet:         " + String(_config.data.subnet[0]) + "." + String(_config.data.subnet[1]) + "." + String(_config.data.subnet[2]) + "." + String(_config.data.subnet[3]));
  Serial.println("DNS1:           " + String(_config.data.primaryDNS[0]) + "." + String(_config.data.primaryDNS[1]) + "." + String(_config.data.primaryDNS[2]) + "." + String(_config.data.primaryDNS[3]));
  Serial.println("DNS2:           " + String(_config.data.secondaryDNS[0]) + "." + String(_config.data.secondaryDNS[1]) + "." + String(_config.data.secondaryDNS[2]) + "." + String(_config.data.secondaryDNS[3]));
}

void doConfig()
{
  Serial.setTimeout(99999999);
  Serial.println("BadgerBridge Config mode");
  Serial.println("");
  
  Serial.print("Starting Universe: ");
  SerialWait();
  _config.data.byteUniverse = (uint8_t)Serial.parseInt();
  SerialFlushInput();
  Serial.println("");

  Serial.print("Universe Count: ");
  SerialWait();
  _config.data.byteUniverseCount = (uint8_t)Serial.parseInt();
  SerialFlushInput();
  Serial.println("");

  Serial.print("LED Count: ");
  SerialWait();
  _config.data.countLEDs = (uint16_t)Serial.parseInt();
  SerialFlushInput();
  Serial.println("");

  Serial.print("SSID: ");
  String ssid = Serial.readStringUntil('\r');
  ssid.toCharArray(_config.data.ssid, 16);
  SerialFlushInput();
  Serial.println("");

  Serial.print("Passcode: ");
  String password = Serial.readStringUntil('\r');
  password.toCharArray(_config.data.password, 16);
  SerialFlushInput();
  Serial.println("");

  for(int i = 0; i < 4; i++)
  {
    Serial.print("Static IP[" + String(i) + "]");
    SerialWait();
    _config.data.staticIP[i] = (uint8_t)Serial.parseInt();
    SerialFlushInput();
    Serial.println("");    
  }

  for(int i = 0; i < 4; i++)
  {
    Serial.print("Gateway IP[" + String(i) + "]");
    SerialWait();
    _config.data.gateIP[i] = (uint8_t)Serial.parseInt();
    SerialFlushInput();
    Serial.println("");    
  }

  for(int i = 0; i < 4; i++)
  {
    Serial.print("Subnet IP[" + String(i) + "]");
    SerialWait();
    _config.data.subnet[i] = (uint8_t)Serial.parseInt();
    SerialFlushInput();
    Serial.println("");    
  }

  for(int i = 0; i < 4; i++)
  {
    Serial.print("Primary DNS[" + String(i) + "]");
    SerialWait();
    _config.data.primaryDNS[i] = (uint8_t)Serial.parseInt();
    SerialFlushInput();
    Serial.println("");    
  }

  for(int i = 0; i < 4; i++)
  {
    Serial.print("Secondary IP[" + String(i) + "]");
    SerialWait();
    _config.data.secondaryDNS[i] = (uint8_t)Serial.parseInt();
    SerialFlushInput();
    Serial.println("");    
  }

  _config.commit();
  
  dumpConfig();
}


void initNeoPixels()
{
  pixels = new Adafruit_NeoPixel(_config.data.countLEDs, DATA_PIN, NEO_GRB + NEO_KHZ800);  
}

bool initWifi()
{
    // Make sure you're in station mode    
    WiFi.mode(WIFI_STA);

    Serial.println("");
    Serial.print(F("Connecting to "));
    Serial.print(_config.data.ssid);

    // Set your Static IP address
    IPAddress local_IP(_config.data.staticIP[0], _config.data.staticIP[1], _config.data.staticIP[2], _config.data.staticIP[3]);
    // Set your Gateway IP address
    IPAddress gateway(_config.data.gateIP[0], _config.data.gateIP[1], _config.data.gateIP[2], _config.data.gateIP[3]);
    
    IPAddress subnet(_config.data.subnet[0], _config.data.subnet[1], _config.data.subnet[2], _config.data.subnet[3]);
    IPAddress primaryDNS(_config.data.primaryDNS[0], _config.data.primaryDNS[1], _config.data.primaryDNS[2], _config.data.primaryDNS[3]);   //optional
    IPAddress secondaryDNS(_config.data.secondaryDNS[0], _config.data.secondaryDNS[2], _config.data.secondaryDNS[3], _config.data.secondaryDNS[4]); //optional

    // Configures static IP address
    if (!WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS)) {
      Serial.println("STA Failed to configure");
      return false;
    }
    
    if (_config.data.password != NULL)
        WiFi.begin(_config.data.ssid, _config.data.password);
    else
        WiFi.begin(_config.data.ssid);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
        if (testEnterConfig())
        {
          return false;
        }
    }

    Serial.println("");
    Serial.print(F("Connected with IP: "));
    Serial.println(WiFi.localIP());
   return true;
}

void setup() {
  
    Serial.begin(115200);
    
    delay(1000);
    
    _config.setPort(Serial);
    _config.begin(1000,2);
    _config.printMsg();

    Serial.println ("BadgerLED bridge");

    for (int t = 0; t < 50; t++)
    {
      Serial.print("*");
      if (testEnterConfig())
      {
        doConfig();
        break;      
      }
      delay(100);
    }

    dumpConfig();
    
    pinMode(DATA_PIN, OUTPUT);

    initNeoPixels();
    
    delay(10);

    pixels->begin();
    pixels->clear();
    for (int lamp = 0; lamp < _config.data.countLEDs; lamp++)
    {
      pixels->setPixelColor(lamp,  pixels->Color(255, 0, 0));
    }
    pixels->setBrightness(20);
    pixels->show();    

    while(false == initWifi())
    {
      // enter config mode
      doConfig();
    }

    for (int lamp = 0; lamp < _config.data.countLEDs; lamp++)
    {
        pixels->setPixelColor(lamp,  pixels->Color(0, 0, 255));
    }    
    pixels->show();    

    e131 = new ESPAsyncE131(_config.data.byteUniverseCount);

    

    // Choose one to begin listening for E1.31 data
    //if (e131.begin(E131_MULTICAST, UNIVERSE, UNIVERSE_COUNT))   // Listen via Multicast
    if (e131->begin(E131_UNICAST))                               // Listen via Unicast
        Serial.println(F("Listening for data..."));
    else 
        Serial.println(F("*** e131.begin failed ***"));

    for (int lamp = 0; lamp < _config.data.countLEDs; lamp++)
    {
      pixels->setPixelColor(lamp,  pixels->Color(0, 255, 0));
    }
    pixels->show();    

    
}

void loop() {

    if (!e131->isEmpty()) {
        e131_packet_t packet;
        e131->pull(&packet);     // Pull packet from ring buffer

        int led = (htons(packet.universe) - 1) * 48;
      
        for(int i = 0; i < htons(packet.property_value_count); i+=3)
        {        
          int red = packet.property_values[i + 1];
          int green = packet.property_values[i + 2];
          int blue = packet.property_values[i + 3];
          
          pixels->setPixelColor(led++, pixels->Color(red, green, blue));
        }
        pixels->show();
/*
        Serial.printf("Universe %u / %u Channels | Packet#: %u / Errors: %u / CH1: %u\n",
                htons(packet.universe),                 // The Universe for this packet
                htons(packet.property_value_count) - 1, // Start code is ignored, we're interested in dimmer data
                e131.stats.num_packets,                 // Packet counter
                e131.stats.packet_errors,               // Packet error counter
                packet.property_values[1]);             // Dimmer data for Channel 1
*/
    }
    
}
