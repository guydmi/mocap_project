#include <BLEDevice.h>
#include <BLEClient.h>
#include "mpu6500.h"

/* Mpu6500 object */
bfs::Mpu6500 imu;


BLERemoteService* pRemoteService;
BLERemoteCharacteristic* pRemoteCharacteristic;

unsigned long currentTime = 0;
String sensorName = "Sensor1";
String sensorData = "";



void setup() {
  Serial.begin(115200); // Initialisation de la communication série
  while (!Serial) {}
  const char* SERVICE_UUID ="91bad492-b950-4226-aa2b-4ede9fa42f59";
  const char* CHARACTERISTIC_UUID = "cba1d466-344c-4be3-ab3f-189f80dd7518";
  BLEAddress myServerAddress("3C:71:BF:AA:9C:0E");
  // Initialisation du dispositif BLE
  BLEDevice::init("");

  // Création du client BLE
  BLEClient* pClient = BLEDevice::createClient();

  // Connexion au serveur BLE
  pClient->connect(myServerAddress);

  Serial.println("Connecte au serveur BLE");

  // Obtention du service BLE
  pRemoteService = pClient->getService(SERVICE_UUID);

  // Obtention de la caractéristique BLE
  pRemoteCharacteristic = pRemoteService->getCharacteristic(CHARACTERISTIC_UUID);

  Wire.begin();
  Wire.setClock(400000);
  /* I2C bus,  0x68 address */
  imu.Config(&Wire, bfs::Mpu6500::I2C_ADDR_PRIM);
  /* Initialize and configure IMU */
  if (!imu.Begin()) {
    Serial.println("Error initializing communication with IMU");
    while(1) {}
  }
  /* Set the sample rate divider */
  if (!imu.ConfigSrd(19)) {
    Serial.println("Error configured SRD");
    while(1) {}
  }


}

void loop() {
  // Lecture des données du capteur
  if (imu.Read()){
    currentTime = millis();
    sensorData = "";
    sensorData += String(currentTime) + ",";  // Add current time to the string
    sensorData += sensorName + ",";   
    sensorData += String(imu.new_imu_data()) + ",";
    sensorData += String(imu.accel_x_mps2()) + ",";
    sensorData += String(imu.accel_y_mps2()) + ",";
    sensorData += String(imu.accel_z_mps2()) + ",";
    sensorData += String(imu.gyro_x_radps()) + ",";
    sensorData += String(imu.gyro_y_radps()) + ",";
    sensorData += String(imu.gyro_z_radps()) + ",";
    + "\n";
 
    }



  // Conversion des données du capteur en chaîne de caractères
  //std::string str = std::to_string(sensorData);
  // Écriture des données du capteur sur la caractéristique BLE
  pRemoteCharacteristic->writeValue(sensorData.c_str());

  Serial.print("Données envoyées : ");
  Serial.println(sensorData); // Affichage des données envoyées

  // Attente avant la prochaine lecture du capteur
  delay(1000);
}