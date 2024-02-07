/*
This code send data to a Mongo database using WiFi (fake data)
We need to integrate it to the one designed to receive data via bluetooth

The IP adress of this gateway need to be allowed by mongo to access and connect to the database
  -> Log in on mong website
  -> Go to our database (cluster: IoT, database: IoT)
  -> On the left, go to Network Access menu
  -> Click on ADD IP ADDRESS and fill the IP and COMMENT sections
*/
#include <WiFi.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>

const char *ssid = "Socquet";        // Replace with good WiFi SSID
const char *password = "1029384756"; // Replace with good WiFi password
const char *mongoDbUrl = "https://eu-west-2.aws.data.mongodb-api.com/app/data-rdkuq/endpoint/data/v1/action/insertOne"; // url for Mongo database
const char *apiKey = "Y6Wvu2DllIPTCsZYaIumNhwx7ZBug6dQOSgS9rIV5fdHSsoZaLoDTL1MtDzh0gvj"; // api key for our database

WiFiClientSecure WiFiClientSecure;

void setup() {
  Serial.begin(115200);

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
}

void loop() {
  // We need to insert real data and arrange data fields below
  String data = "{\"dataSource\":\"IoT\",\"database\":\"IoT\",\"collection\":\"testData\",\"document\":{\"status\":\"open\",\"text\":\"Do the dishes\"}}";

  // Make the HTTP request
  HTTPClient http;
  WiFiClientSecure.setInsecure();
  http.begin(WiFiClientSecure, String(mongoDbUrl));
  http.addHeader("Content-Type", "application/json");
  http.addHeader("Accept", "application/json");
  http.addHeader("apikey", String(apiKey));
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

  // Wait for a moment before making the next request
  // Adjust delay depending on our usage requirement
  delay(5000);
}
