/*
 Basic You Made It example
 Runs on ESP8266 hardware
 http://www.youmadeit.fr
*/

#include <ESP8266WiFi.h>
#include <YouMadeIt.h>

// ********************************************************
// Put your own parameters here
// ********************************************************

// Update these with values suitable for your wifi network.
const char* ssid = "PUT_YOUR_WIFI_SSID_HERE";
const char* password = "PUT_YOUR_WIFI_PASSWORD_HERE";

// Update this with your YouMadeIt api key
const char* api_key = "PUT_YOUR_YOUMADEIT_API_KEY_HERE";

// Give a name to your device (ASCII only, no spaces)
const char* device_name = "myProtoBoard";

// *********************************************************

WiFiClient espClient;
YouMadeIt youMadeIt(espClient);
long lastMsg = -99999999;

void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println();
  Serial.println("WiFi connected");
  Serial.println("IP address: "); Serial.println(WiFi.localIP());
}

// This function will be called when the device receives a message from another device or from YouMadeIt! mobile app
void onMessageCallback(const char* parameter_name, const char* data_string, double data_double, boolean is_numeric) {
  Serial.println("New message received!");
  Serial.print("Parameter name: "); Serial.println(parameter_name);
  Serial.print("Data string: "); Serial.println(data_string);
  Serial.print("Data double: "); Serial.println(data_double);
  Serial.print("Is numeric: "); Serial.println(is_numeric);
  Serial.println();
  // If parameter name is "parameter1", switch led on or off according to data received
  if (strcmp(parameter_name,"parameter1") == 0) {
    if (data_double > 0.5) {
      digitalWrite(LED_BUILTIN, LOW);
    }
    else if (data_double < 0.5) {
      digitalWrite(LED_BUILTIN, HIGH); 
    }    
  }
}

void setup() {
  pinMode(BUILTIN_LED, OUTPUT);               // Initialize the BUILTIN_LED pin as an output
  Serial.begin(115200);
  setup_wifi();
  youMadeIt.setup(api_key, device_name);      // Setup YouMadeIt! connection
  youMadeIt.setCallback(onMessageCallback);   // Register callback function which will be called when a message is received
}

void loop() {
  
  youMadeIt.loop();   // This line is required for YouMadeIt

  long now = millis();
  if (now - lastMsg > 120000) {   // Every 120 seconds
    lastMsg = now;
    Serial.println("Sending a message to YouMadeIt");
    youMadeIt.sendToMobile("arduino", "Bonjour !");
    youMadeIt.sendToMobile("arduino 3", 1.5588846, "subtitle");
    youMadeIt.sendToDevice("myProtoBoard", "hello", "world");
    youMadeIt.sendToDevice("myProtoBoard", "numericData", 1.5);
  }
}
