/*
 YouMadeIt.cpp - Connector to YouMadeIt service for Arduino.
  Copyright Enersy
  http://www.youmadeit.fr
*/

#include "YouMadeIt.h"
#include "YMIPubSubClient.h"
#include "Arduino.h"

YouMadeIt::YouMadeIt(Client& myClient) {
  client = YMIPubSubClient(myClient);
}

void YouMadeIt::setup(const char* my_api_key, const char* my_device_name) {
  api_key = my_api_key;
  device_name = my_device_name;
  // TODO encode non ascii characters, spaces, etc.
  encoded_device_name = my_device_name;
  client.setServer(mqtt_server, 1883);
}

void YouMadeIt::setCallback(ON_MESSAGE_CALLBACK_SIGNATURE) {
  client.setOnMessageCallback(onMessageCallback);
}

void YouMadeIt::reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting connection to YouMadeIt server...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      // client.publish("outTopic", "hello world");
      // ... and resubscribe
      // Create subscribe topic
      String subscribe_topic = "client/";
      subscribe_topic += api_key;
      subscribe_topic += "/";
      subscribe_topic += encoded_device_name;
      // Convert subscribe topic to char*
      int str_len = subscribe_topic.length() + 1;
      char char_subscribe_topic[str_len];
      subscribe_topic.toCharArray(char_subscribe_topic, str_len);
      // Subscribe to topic
      client.subscribe(char_subscribe_topic);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void YouMadeIt::loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}

void YouMadeIt::sendPayload(String topic, String payload_json) {
  // Convert topic to char*
  int str_len = topic.length() + 1;
  char char_topic[str_len];
  topic.toCharArray(char_topic, str_len);
  // Convert payload to char*
  str_len = payload_json.length() + 1;
  char char_payload[str_len];
  payload_json.toCharArray(char_payload, str_len);
  // Send data
  if (client.connected()) {
    // Serial.println("Sending message");
    // Serial.println(char_topic);
    // Serial.println(char_payload);
    client.publish(char_topic, char_payload);
  }  
}

void YouMadeIt::sendStringToMobile(const char* param_name, const char* data, const char* subtitle, const char* data_type, boolean is_numeric) {
  // Create topic
  String topic = "mobile/";
  topic += api_key;
  // Create payload
  String payload_json = "{\"deviceName\":\"";
  payload_json += device_name;
  payload_json += "\",\"paramName\":\"";
  payload_json += param_name;
  payload_json += "\",\"dataType\":\"";
  payload_json += data_type;
  payload_json+= "\",\"data\":";
  if (!is_numeric) {
    payload_json += "\"";
  }
  payload_json += data;
  if (!is_numeric) {
    payload_json += "\"";
  }
  payload_json += ",\"subtitle\":\"";
  payload_json += subtitle;
  payload_json += "\"";

  payload_json += "}";

  // Send via MQTT
  sendPayload(topic, payload_json);
}

void YouMadeIt::sendStringToDevice(const char* target_device_name, const char* param_name, const char* data, const char* data_type, boolean is_numeric) {
  // Create topic
  String topic = "client/";
  topic += api_key;
  topic += "/";
  // TODO encode target device name
  topic += target_device_name;
  // Create payload
  String payload_json = "{\"deviceName\":\"";
  payload_json += device_name;
  payload_json += "\",\"paramName\":\"";
  payload_json += param_name;
  payload_json += "\",\"dataType\":\"";
  payload_json += data_type;
  payload_json+= "\",\"data\":";
  if (!is_numeric) {
    payload_json += "\"";
  }
  payload_json += data;
  if (!is_numeric) {
    payload_json += "\"";
  }

  payload_json += "}";

  // Send via MQTT
  sendPayload(topic, payload_json);
}

void YouMadeIt::sendToMobile(const char* param_name, const char* data) {
  sendStringToMobile(param_name, data, "", "str", false);
}

void YouMadeIt::sendToMobile(const char* param_name, const char* data, const char* subtitle) {
  sendStringToMobile(param_name, data, subtitle, "str", false);
}

void YouMadeIt::sendToMobile(const char* param_name, double data) {
  sendToMobile(param_name, data, "");  
}

void YouMadeIt::sendToMobile(const char* param_name, double data, const char* subtitle) {
  // Convert data to char array
  String tempStr = String(data);
  int str_len = tempStr.length() + 1;
  char tempChar[str_len];
  tempStr.toCharArray(tempChar, str_len);
  sendStringToMobile(param_name, tempChar, subtitle, "num", true);
}

void YouMadeIt::sendToDevice(const char* target_device_name, const char* param_name, const char* data) {
  sendStringToDevice(target_device_name, param_name, data, "str", false);
}

void YouMadeIt::sendToDevice(const char* target_device_name, const char* param_name, double data) {
  // Convert data to char array
  String tempStr = String(data);
  int str_len = tempStr.length() + 1;
  char tempChar[str_len];
  tempStr.toCharArray(tempChar, str_len);
  sendStringToDevice(target_device_name, param_name, tempChar, "num", true);
}



