#include <esp_now.h>
#include <WiFi.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>
#include <SPI.h>

#define TFT_CS   14
#define TFT_RST  17
#define TFT_DC   15
#define TFT_MOSI 23
#define TFT_SCLK 18

Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);

typedef struct struct_message {
  float lat;
  float lng;
  int estado1;
  int estado2;
} struct_message;

struct_message myData;

int last_estado1 = -1;
int last_estado2 = -1;

void OnDataRecv(const uint8_t* mac, const uint8_t* incomingData, int len) {
  memcpy(&myData, incomingData, sizeof(myData));

  Serial.print("Latitude: ");
  Serial.println(myData.lat);

  Serial.print("Longitude: ");
  Serial.println(myData.lng);

  Serial.print("Estado 1: ");
  Serial.println(myData.estado1);

  Serial.print("Estado 2: ");
  Serial.println(myData.estado2);

  Serial.println();
}

void setup() {
  Serial.begin(115200);
  Serial.println(F("Iniciando..."));

  tft.init(240, 240, SPI_MODE2);
  tft.setRotation(2);
  tft.fillScreen(ST77XX_BLACK);
  tft.setTextWrap(true);

  WiFi.mode(WIFI_STA);

  if (esp_now_init() != ESP_OK) {
    Serial.println("Erro na comunicação dos ESP");
    return;
  }

  esp_now_register_recv_cb(OnDataRecv);
}

void loop() {
  if (myData.estado1 != last_estado1 || myData.estado2 != last_estado2) {
    last_estado1 = myData.estado1;
    last_estado2 = myData.estado2;

    tft.fillScreen(ST77XX_BLACK);
    tft.setCursor(0, 10);
    tft.setTextColor(ST77XX_WHITE);
    tft.setTextSize(3);

    if (myData.estado1 == 1 && myData.estado2 == 1) {
      tft.println("INATEL: ja");
      tft.println("passou");
      tft.println("_____________");
      tft.println("ETE: proximo");
      tft.println("_____________");
      tft.println("RODOVIARIA:");
      tft.println("ainda nao");
      tft.println("passou");
    }

    // Rodoviária é o próximo
    else if (myData.lat == myData.lat && myData.lng == myData.lng &&
             myData.estado1 == 1 && myData.estado2 == 0) {
      tft.println("INATEL: ja");
      tft.println("passou");
      tft.println("_____________");
      tft.println("ETE: ja");
      tft.println("passou");
      tft.println("_____________");
      tft.println("RODOVIARIA:");
      tft.println("proximo");
    }

    // Rodoviária ainda não
    else if (myData.estado1 == 0 && myData.estado2 == 1) {
      tft.println("INATEL: ja");
      tft.println("passou");
      tft.println("_____________");
      tft.println("ETE: ja");
      tft.println("passou");
      tft.println("_____________");
      tft.println("RODOVIARIA:");
      tft.println("ainda nao");
    }
  }
}
