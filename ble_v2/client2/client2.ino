#include <BLEDevice.h>
#include <BLEClient.h>

BLERemoteService* pRemoteService;
BLERemoteCharacteristic* pRemoteCharacteristic;

void setup() {
  Serial.begin(115200); // Initialisation de la communication série
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
}

void loop() {
  // Lecture des données du capteur
  int sensorData = random(300);

  // Conversion des données du capteur en chaîne de caractères
  std::string str = std::to_string(sensorData);

  // Écriture des données du capteur sur la caractéristique BLE
  pRemoteCharacteristic->writeValue(str);

  Serial.print("Données envoyées : ");
  Serial.println(sensorData); // Affichage des données envoyées

  // Attente avant la prochaine lecture du capteur
  delay(1000);
}
