#include <BLEDevice.h>
#include <BLEServer.h>

BLECharacteristic *pCharacteristic;

void setup() {
  Serial.begin(115200); // Initialisation de la communication série
  const char* SERVICE_UUID ="91bad492-b950-4226-aa2b-4ede9fa42f59";
  const char* CHARACTERISTIC_UUID = "cba1d466-344c-4be3-ab3f-189f80dd7518";
  // Initialisation du dispositif BLE
  BLEDevice::init("Mon serveur BLE");

  // Création du serveur BLE
  BLEServer *pServer = BLEDevice::createServer();

  // Création d'un service BLE
  BLEService *pService = pServer->createService(SERVICE_UUID);

  // Création d'une caractéristique BLE
  pCharacteristic = pService->createCharacteristic(
                                         CHARACTERISTIC_UUID,
                                         BLECharacteristic::PROPERTY_READ |
                                         BLECharacteristic::PROPERTY_WRITE
                                       );

  // Démarrage du service
  pService->start();

  // Démarrage de l'annonce du serveur BLE
  BLEAdvertising *pAdvertising = pServer->getAdvertising();
  pAdvertising->start();
  Serial.println("Serveur BLE démarré");
}

void loop() {
  // Lecture de la caractéristique
  std::string value = pCharacteristic->getValue();

  Serial.print("Données reçues :");
  Serial.println(value.c_str());

  delay(2000);

  // Traitement des données reçues
  // ...
}
