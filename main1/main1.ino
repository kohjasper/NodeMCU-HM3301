#include "CTBot.h"
#include <Seeed_HM330X.h>

#ifdef  ARDUINO_SAMD_VARIANT_COMPLIANCE
    #define SERIAL_OUTPUT SerialUSB
#else
    #define SERIAL_OUTPUT Serial
#endif

HM330X sensor;
uint8_t buf[30];

uint16_t PM1_0_value;
uint16_t PM2_5_value;
uint16_t PM10_value;

const char* str[] = {"sensor num: ", "PM1.0 concentration(CF=1,Standard particulate matter,unit:ug/m3): ",
                   "PM2.5 concentration(CF=1,Standard particulate matter,unit:ug/m3): ",
                   "PM10 concentration(CF=1,Standard particulate matter,unit:ug/m3): ",
                   "PM1.0:",
                   "PM2.5:",
                   "PM10:",
                  };

HM330XErrorCode print_result(const char* str, uint16_t value) {
  if (NULL == str) {
      return ERROR_PARAM;
  }
    //SERIAL_OUTPUT.print(str);
    //SERIAL_OUTPUT.print(value);
    //SERIAL_OUTPUT.print(",");    

  if (str == "PM1.0:") {
    PM1_0_value = value;
  }
  if (str == "PM2.5:") {
    PM2_5_value = value;
  }    
  if (str == "PM10:") {
    PM10_value = value;
  }
  
  return NO_ERROR;
}

/*parse buf with 29 uint8_t-data*/
HM330XErrorCode parse_result(uint8_t* data) {
  uint16_t value = 0;
  if (NULL == data) {
      return ERROR_PARAM;
  }
  for (int i = 5; i < 8; i++) {
      value = (uint16_t) data[i * 2] << 8 | data[i * 2 + 1];
      print_result(str[i - 1], value);

  }

  return NO_ERROR;
}

HM330XErrorCode parse_result_value(uint8_t* data) {
  if (NULL == data) {
      return ERROR_PARAM;
  }
  for (int i = 0; i < 28; i++) {
      SERIAL_OUTPUT.print(data[i], HEX);
      SERIAL_OUTPUT.print("  ");
      if ((0 == (i) % 5) || (0 == i)) {
          SERIAL_OUTPUT.println("");
      }
  }
  uint8_t sum = 0;
  for (int i = 0; i < 28; i++) {
      sum += data[i];
  }
  if (sum != data[28]) {
      SERIAL_OUTPUT.println("wrong checkSum!!");
  }
  SERIAL_OUTPUT.println("");
  return NO_ERROR;
}


// Sensor reading variables - start
String PM1_0 = "";
String PM2_5 = "";
String PM10 = "";

CTBot myBot;
TBMessage msg;

String ssid  = "SSID"    ; // Replace with your wifi SSID
String pass  = "Password"; // Replace with your wifi Password (if any)
String token = "TelegramBotToken"   ; // Replace with Telegram Bot Token

void setup() {
  // initialize the Serial
  Serial.begin(115200);
  Serial.println("Starting TelegramBot...");
  myBot.wifiConnect(ssid, pass);
  myBot.setTelegramToken(token);
  if (myBot.testConnection()){
   
    Serial.println("\ntestConnection OK");
  }
  else
  {
    Serial.println("\ntestConnection NOK");
  }
  if (sensor.init()) {
        SERIAL_OUTPUT.println("HM330X init failed!!");
        while (1);
  }
 
}

void loop() {
  // a variable to store telegram message data
  if (sensor.read_sensor_value(buf, 29)) {
      SERIAL_OUTPUT.println("HM330X read result failed!!");
  }
  parse_result(buf);

  PM1_0 = PM1_0_value;
  PM2_5 = PM2_5_value;
  PM10 = PM10_value;


  String PM1 = "PM1: " + String(PM1_0_value) + " ug/m3 \n";
  String PM25 = "PM2.5: " + String(PM2_5_value) + " ug/m3 \n";
  String PM10_ = "PM10: " + String(PM10_value) + " ug/m3 \n";

  //Output to Serial Monitor to check
  SERIAL_OUTPUT.println(PM1);
  SERIAL_OUTPUT.println(PM25);
  SERIAL_OUTPUT.println(PM10_);
  
  delay(2000);
  
  if (myBot.getNewMessage(msg)) {

     if (msg.text.equalsIgnoreCase("PM1")) {  
      myBot.sendMessage(msg.sender.id, PM1);
     }
     if (msg.text.equalsIgnoreCase("PM2.5")){  
      myBot.sendMessage(msg.sender.id, PM25);
     }
     if (msg.text.equalsIgnoreCase("PM10")){
     myBot.sendMessage(msg.sender.id, PM10_);
     }

}
  
delay(10);

}
