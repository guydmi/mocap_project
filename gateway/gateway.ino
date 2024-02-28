/*
This code sends data received from sensors to a Mongo database using WiFi

The IP adress of this gateway need to be allowed by mongo to access and connect to the database
  -> Log in on mong website
  -> Go to our database (cluster: IoT, database: IoT)
  -> On the left, go to Network Access menu
  -> Click on ADD IP ADDRESS and fill the IP and COMMENT sections
*/

/*********
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com/esp-now-many-to-one-esp32/
  
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files.
  
  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
*********/

#include <esp_now.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include <esp_wifi.h>

#define BUFF_SIZE 5

const char* SSID = "Freebox-0B3678";        // Replace with good WiFi SSID
const char* PASSWORD = "d4wn7qwtz4qdxt53qcnd4w"; // Replace with good WiFi password
const char* mongoDbUrl = "https://eu-west-2.aws.data.mongodb-api.com/app/data-rdkuq/endpoint/data/v1/action/insertOne"; // url for Mongo database
const char* APIKEY = "Y6Wvu2DllIPTCsZYaIumNhwx7ZBug6dQOSgS9rIV5fdHSsoZaLoDTL1MtDzh0gvj"; // api key for our database

// Structure example to receive data
// Must match the sender structure
typedef struct struct_message {
  int id; // must be unique for each sender board
  float imu_data;
  float accel_x;
  float accel_y;
  float accel_z;
  float gyro_x;
  float gyro_y;
  float gyro_z;
}struct_message;

// Create a struct_message called myData
struct_message myData;

// Create a structure to hold the readings from each board
struct_message board1;
struct_message board2;

WiFiClientSecure WiFiClientSecure;

// Create an array with all the structures
struct_message boardsStruct[2] = {board1, board2};

struct_message buff[BUFF_SIZE];


void initEspNow(){
  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }else{
    Serial.println("ESP-NOW initialized!");
    // Registering callback function
    esp_now_register_recv_cb(OnDataRecv);
  }  
}


void deinitEspNow(){
  // Deinitialize ESP-NOW
  if (esp_now_deinit() != ESP_OK) {
    Serial.println("Error deinitializing ESP-NOW");
  } else {
    Serial.println("ESP-NOW deinitialized");
  }
}


void initWiFi(){
  // Connecting to Wi-Fi
  Serial.println("Connecting to WiFi");
  WiFi.mode(WIFI_AP_STA);
  WiFi.begin(SSID, PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("Connected to WiFi");
  //Serial.println(WiFi.localIP());
}


void deinitWiFi(){
  Serial.print("Disconnecting WiFi");
  WiFi.disconnect();
  while(WiFi.status() != WL_DISCONNECTED){
    delay(300);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("Disconnected from WiFi");
}


void sendToDB(){
  deinitEspNow();
  initWiFi();
  int i;
  for(i=0; i<BUFF_SIZE; i++){
    String dataString = "";
    dataString += "\"id\":\"";
    dataString += String(buff[i].id);
    dataString += "\",\"accelX\":\"";
    dataString += String(buff[i].accel_x);
    dataString += "\",\"accelY\":\"";
    dataString += String(buff[i].accel_y);
    dataString += "\",\"accelZ\":\"";
    dataString += String(buff[i].accel_z);
    dataString += "\",\"gyroX\":\"";
    dataString += String(buff[i].gyro_x);
    dataString += "\",\"gyroY\":\"";
    dataString += String(buff[i].gyro_y);
    dataString += "\",\"gyroZ\":\"";
    dataString += String(buff[i].gyro_z);
    dataString += "\"";

    String data = "{\"dataSource\":\"IoT\","
                  "\"database\":\"IoT\","
                  "\"collection\":\"testRealData\","
                  "\"document\":{"+dataString+"}}";
    Serial.println(data);

    // Make the HTTP request
    HTTPClient http;
    WiFiClientSecure.setInsecure();
    http.begin(WiFiClientSecure, String(mongoDbUrl));
    http.addHeader("Content-Type", "application/json");
    http.addHeader("Accept", "application/json");
    http.addHeader("apikey", String(APIKEY));
    // Sending POST request
    int httpResponseCode = http.POST(data);

    if (httpResponseCode > 0) {
      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);
      String payload = http.getString();
      Serial.println(payload);
    } else {
      Serial.print("HTTP Request failed, error: ");
      Serial.println(httpResponseCode);
      Serial.println(http.getString());
    }
    http.end();
  }
}


int idx = 0;
// callback function that will be executed when data is received
void OnDataRecv(const uint8_t * mac_addr, const uint8_t *incomingData, int len) {

  char macStr[18];
  Serial.print("Packet received from: ");
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
           mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
  Serial.println(macStr);
  memcpy(&myData, incomingData, sizeof(myData));

  buff[idx].id = myData.id;
  buff[idx].imu_data = myData.imu_data;
  buff[idx].accel_x = myData.accel_x;
  buff[idx].accel_y = myData.accel_y;
  buff[idx].accel_z = myData.accel_z;
  buff[idx].gyro_x = myData.gyro_x;
  buff[idx].gyro_y = myData.gyro_y;
  buff[idx].gyro_z = myData.gyro_z;
  idx ++;
}


void setup() {
  Serial.begin(115200);

  WiFi.mode(WIFI_AP_STA);
  initEspNow();
}


void loop() {

  if(idx == BUFF_SIZE){
    idx = 0;
    sendToDB();
    deinitWiFi();
    WiFi.mode(WIFI_AP_STA);
    initEspNow();
  }

}


