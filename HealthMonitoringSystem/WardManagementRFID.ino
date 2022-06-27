#include <NTPClient.h>
#include <SPI.h>
#include <MFRC522.h>
#include <WiFiUdp.h>
#include <Firebase_ESP_Client.h>
#include <addons/TokenHelper.h>
#include <addons/RTDBHelper.h>

#define WIFI_SSID "Atharv"
#define WIFI_PASSWORD "atharv123"
#define API_KEY "~~Firebase RealtimeDB API key~~"
#define DATABASE_URL "~~Firebase Realtime DB URL~~" //<databaseName>.firebaseio.com or <databaseName>.<region>.firebasedatabase.app
#define USER_EMAIL "~~User email~~"
#define USER_PASSWORD "~~Password~~"
#define RST_PIN D0 // Slave-Select Pin
#define SS_PIN D8 // Reset Pin
#define RED_LED D1
#define GREEN_LED D2
#define BUZZER D3

FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;
MFRC522 rfid(SS_PIN, RST_PIN); // Instance of the class
MFRC522::MIFARE_Key key;

// Array for storing new NUID
byte nuidPICC[4];
int uniqueID = 0;
const long utcOffsetInSeconds = 3600;
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds);

void setup()
{
    Serial.begin(115200); // Initiate Serial Communication    
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    Serial.print("Connecting to Wi-Fi");
    while (WiFi.status() != WL_CONNECTED)
    {
        Serial.print(".");
        delay(300);
    }
    Serial.print("\nConnected with IP: ");
    Serial.println(WiFi.localIP());
    Serial.println();
    Serial.printf("Firebase Client v%s\n\n", FIREBASE_CLIENT_VERSION);
    timeClient.begin();
    config.api_key = API_KEY;
    auth.user.email = USER_EMAIL;
    auth.user.password = USER_PASSWORD;
    config.database_url = DATABASE_URL;
    config.token_status_callback = tokenStatusCallback; // see addons/TokenHelper.h
    Firebase.begin(&config, &auth);
    Firebase.reconnectWiFi(true); // Comment or pass false value when WiFi reconnection will control by your code or third party library
    Firebase.setDoubleDigits(5);

    pinMode(RED_LED, OUTPUT); // Failure LED
    pinMode(GREEN_LED, OUTPUT); // Success LED
    pinMode(BUZZER, OUTPUT); // Buzzer

    SPI.begin();  // Initiate SPI Bus
    rfid.PCD_Init(); // Initiate MFRC522
    Serial.println("Hold You Card In Front Of The Reader : ");   
}

void loop() 
{
    if ( ! rfid.PICC_IsNewCardPresent())
    {
      // Reset the loop if no new card present on the sensor/reader
      return;
    } 
 
    if ( ! rfid.PICC_ReadCardSerial())
    {
      // Verify if the NUID has been read
      return;
    } 

    if (rfid.uid.uidByte[0] != nuidPICC[0] || rfid.uid.uidByte[1] != nuidPICC[1] || rfid.uid.uidByte[2] != nuidPICC[2] || rfid.uid.uidByte[3] != nuidPICC[3]) 
    {
      Serial.println("\nNew Card Detected !!!");
      // Store NUID into nuidPICC array
      uniqueID = 0;
      for (byte i = 0; i < 4; i++) 
      {
        nuidPICC[i] = rfid.uid.uidByte[i];
        uniqueID = (uniqueID * 10 ) + rfid.uid.uidByte[i];
      }
      
      Serial.print("NUID Tag : ");
      printDec(rfid.uid.uidByte, rfid.uid.size);
      
      delay(100);
      digitalWrite(BUZZER, HIGH);
      digitalWrite(GREEN_LED, HIGH);
      int i=0;
      for(i=0;i<3;i++){
        delay(100);
        digitalWrite(BUZZER, LOW);
        delay(100);
        digitalWrite(BUZZER, HIGH);
      }
      digitalWrite(GREEN_LED, LOW);
      digitalWrite(BUZZER, LOW);
    }
    else
    {
      Serial.println(F("\nCard Read Previously !!!"));
      delay(100);
      digitalWrite(RED_LED, HIGH);
      digitalWrite(BUZZER, HIGH);
      delay(1000);                       
      digitalWrite(RED_LED, LOW);
      digitalWrite(BUZZER, LOW);
      delay(1000);   
    }

    Serial.println();
    Serial.println(uniqueID);
    mapUIDToUser(uniqueID);

    // Halt PICC
    rfid.PICC_HaltA();
    // Stop encryption on PCD
    rfid.PCD_StopCrypto1();
}

void printDec(byte *buffer, byte bufferSize) 
{
  for (byte i = 0; i < bufferSize; i++) 
  {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], DEC);
  }
}

void mapUIDToUser(int uid){
  timeClient.update();
  long epochTime = timeClient.getEpochTime();

  char drName[20];
  switch(uid){
    case 267266: 
      strcpy(drName, "Dr. Natu");
      break;
    case 161800: 
      strcpy(drName, "Dr.Patel");
      break;
    case 45580: 
      strcpy(drName, "Dr.Nikumbh");
      break;
    case 148630:
      strcpy(drName, "Dr. Palwe");
      break;
    case 13460: 
      strcpy(drName, "Dr. Naik");
      break;
  }

  boolean didUpdate = Firebase.RTDB.setString(&fbdo, F("/lastScanBy"), drName);
  if(didUpdate){
    Serial.println("Values Updated");
  } else {
    Serial.println("Values NOT Updated");
  }
  boolean didTimeUpdate = Firebase.RTDB.setInt(&fbdo, F("/lastScanTime"), epochTime);
  if(didTimeUpdate){
    Serial.println("Time Updated");
  } else {
    Serial.println("Time NOT Updated");
  }
}