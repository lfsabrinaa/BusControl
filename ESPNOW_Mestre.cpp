#include <esp_now.h>  
#include <WiFi.h>  
#include <TinyGPS++.h>  

uint8_t broadcastAddress[] = {0xe4, 0x65, 0xb8, 0xd8, 0xc3, 0xfc};  
const int RX_PIN = 16;  
const int TX_PIN = 17;  
const int GPS_BAUD = 9600;  

const int pos1 = 34;  
const int pos2 = 35; 
int pos1estado = 0; 
int pos2estado = 0;  

TinyGPSPlus gps;  

#define ss Serial2  

typedef struct struct_message {  
  float lat;  
  float lng;  
  int estado1; 
  int estado2; 
} struct_message;  

struct_message myData;  
esp_now_peer_info_t peerInfo;  

void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {  
  Serial.print("\r\nÚltimo pacote::\t");  
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Sucesso na entrega" : "Falha na entrega");  
} 

void setup() { 
  Serial.begin(115200); 
  pinMode(pos1, INPUT_PULLUP); 
  pinMode(pos2, INPUT_PULLUP); 
    WiFi.mode(WIFI_STA); 
  //Serial.print("oi");  
  Serial2.begin(GPS_BAUD, SERIAL_8N1, RX_PIN, TX_PIN);

  if (esp_now_init() != ESP_OK) {  
    Serial.println("Erro na comunicação dos ESP");  
    return;  
  }  

  esp_now_register_send_cb(OnDataSent);  
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);  
  peerInfo.channel = 0;    
  peerInfo.encrypt = false;  

  if (esp_now_add_peer(&peerInfo) != ESP_OK){  
    Serial.println("Falha ao se conectar com o outro ESP");  
    return;  
  }  
} 

void loop() { 
     pos1estado = digitalRead(pos1); 
     pos2estado = digitalRead(pos2); 

    if (pos1estado == LOW && pos2estado == HIGH) { 
      Serial.println("Posição 1"); 
    }  

    if (pos1estado == HIGH && pos2estado == LOW) { 
      Serial.println("Posição 2"); 
    }  

    if (pos1estado == HIGH && pos2estado == HIGH) { 
      Serial.println("Posição 3");
    } 

   while (ss.available() > 0) {  
    gps.encode(ss.read());  
 }  

   if (gps.location.isUpdated()) {  
    float lat = gps.location.lat();  
    float lng = gps.location.lng();  
    Serial.print("Latitude: "); 
    Serial.print(lat, 6);  
    Serial.print(", Longitude: ");  
    Serial.println(lng, 6);  
 }  

  myData.lat = gps.location.lat();  
  myData.lng = gps.location.lng();  
  myData.estado1 = digitalRead (pos1); 
  myData.estado2 = digitalRead (pos2); 
  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &myData, sizeof(myData));  

  if (result == ESP_OK) {  
    Serial.println("Enviado com successo");  
  }  
  else {  
    Serial.println("Erro ao enviar dados"); 
  }  
  delay(1000); 
}
