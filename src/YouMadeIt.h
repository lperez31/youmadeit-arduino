/*
 YouMadeIt.h - Connector to YouMadeIt service for Arduino.
  Copyright Enersy
  http://www.youmadeit.fr
*/

#ifndef YouMadeIt_h
#define YouMadeIt_h

#include "YMIPubSubClient.h"

#ifdef ESP8266
#include <functional>
// On message callback function arguments: parameter_name, data_string, data_double, is_numeric
#define ON_MESSAGE_CALLBACK_SIGNATURE std::function<void(const char*, const char*, double, boolean)> onMessageCallback
#else
#define ON_MESSAGE_CALLBACK_SIGNATURE void (*onMessageCallback)(const char*, const char*, double, boolean)
#endif

class YouMadeIt {

private:
  const char* mqtt_server = "api.youmadeit.fr";
  const char* api_key;
  const char* device_name;
  const char* encoded_device_name;
  YMIPubSubClient client;
  void sendPayload(String topic, String payload_json);
  void sendStringToMobile(const char* param_name, const char* data, const char* subtitle, const char* data_type, boolean is_numeric);
  void sendStringToDevice(const char* target_device_name, const char* param_name, const char* data, const char* data_type, boolean is_numeric);

public:
   YouMadeIt(Client&);
   void reconnect();
   void setup(const char* my_api_key, const char* my_device_name);
   void setCallback(ON_MESSAGE_CALLBACK_SIGNATURE);
   ON_MESSAGE_CALLBACK_SIGNATURE;
   void loop();
   void sendToMobile(const char* param_name, const char* data);
   void sendToMobile(const char* param_name, const char* data, const char* subtitle);
   void sendToMobile(const char* param_name, double data);
   void sendToMobile(const char* param_name, double data, const char* subtitle);
   void sendToDevice(const char* target_device_name, const char* param_name, const char* data);
   void sendToDevice(const char* target_device_name, const char* param_name, double data);

};

#endif
