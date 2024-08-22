#define BLYNK_TEMPLATE_ID "TMPL6bxBLVsc-"
#define BLYNK_TEMPLATE_NAME "smart farm"
#define BLYNK_AUTH_TOKEN "8w3OQsJvX4SyBpMriYqqYkw1Vfr_y9aB"
#define LINE_TOKEN  "59dV1hKr7vsrUoKv04qbK0P4L45Ha1QBLy4NZ8CXT8E"

#include <WiFi.h>
#include <WiFiClient.h>
#include <TridentTD_LineNotify.h>
#include <BlynkSimpleEsp32.h>
#include <DHT.h>

#define DHT_PIN 32  // GPIO32 P32
#define DHTTYPE DHT11
#define SOIL_MOISTURE_PIN 34 // GPIO34 P34
#define PUMP_WATER_PIN 9 // GPIO9 SD2
#define PUMP_WATER_SWITCH V4  
#define PUMP_fertilizer_PIN 10 // GPIO10 SD3
#define PUMP_fertilizer_SWITCH V5

#define auth        "8w3OQsJvX4SyBpMriYqqYkw1Vfr_y9aB"
#define ssid        "meimintt"                                   
#define password    "mint22883?"    

BlynkTimer timer;

bool isPumpwaterOn = false;
bool isPumpfertilizerOn = false;
DHT dht(DHT_PIN, DHTTYPE);

void sendSensorData()
{
  // Soil Moisture Data
  int soilMoisture = analogRead(SOIL_MOISTURE_PIN);
  int soilMoisturePercentage = map(soilMoisture, 0, 4095, 100, 0);
  Serial.print("Soil Moisture Percentage: ");
  Serial.println(soilMoisturePercentage);

  if (soilMoisturePercentage < 25) {
  LINE.notify("ความชื้นในดินขณะนี้มีค่า " + String(soilMoisturePercentage) + "%");
  Serial.println("soilMoisture alert sent via LINE");
  }

  Blynk.virtualWrite(V3, soilMoisturePercentage);

  // Water Pump Control
  if (isPumpwaterOn | soilMoisturePercentage < 25) {
    digitalWrite(PUMP_WATER_PIN, LOW);  // Turn pump on
      if (!isPumpwaterOn) {
      LINE.notify("ขณะนี้ปั้มนำ้กำลังทำงานรดนำ้ให้แก่ไร่อ้อย");
      LINE.notifySticker(11537, 52002756);
      Serial.println("Pump water turned ON and alert sent line");
    }
  } else {
    digitalWrite(PUMP_WATER_PIN, HIGH);  // Turn pump off
      if (isPumpwaterOn) {
      LINE.notify("ปั้มนำ้หยุดการทำงาน");
      Serial.println("Pump water turned OFF and alert sent line");
    }
  }

  // Fertilizer Pump Control
  if (isPumpfertilizerOn) {
    digitalWrite(PUMP_fertilizer_PIN, LOW);  // Turn pump on
  } else {
    digitalWrite(PUMP_fertilizer_PIN, HIGH);  // Turn pump off
  }

   // DHT11 Sensor Data
  float dhtTemp = dht.readTemperature(); 
  float dhtHum = dht.readHumidity();     

  if (isnan(dhtTemp) || isnan(dhtHum)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  Blynk.virtualWrite(V1, dhtTemp);
  Blynk.virtualWrite(V2, dhtHum);

  Serial.print("Temp (DHT): ");
  Serial.print(dhtTemp);
  Serial.print("°C, Humidity (DHT): ");
  Serial.println(dhtHum);

  //if (dhtTemp) {
  //LINE.notify("อุณหภูมิขณะนี้มีค่า " + String(dhtTemp) + "°C");
  //Serial.println("Temperature alert sent via LINE");
  //}

  if (dhtTemp > 43) {
  LINE.notify("อุณหภูมิขณะนี้มีค่า " + String(dhtTemp) + "°C ไม่เหมาะสมกับการเจริญเติบโตของต้นอ้อย");
  Serial.println("Temperature alert sent via LINE");
  }

  //if (dhtHum) {
  //LINE.notify("ค่าความชื้นสัมพัทธ์ในอากาศมีค่า " + String(dhtHum) + "%");
  //Serial.println("Humidity alert sent via LINE");
  //}
  //delay (60UL * 60UL * 1000UL);

}

BLYNK_WRITE(PUMP_WATER_SWITCH)
{
  isPumpwaterOn = param.asInt();
  if (isPumpwaterOn) {
    LINE.notify("ปั้มนำ้กำลังทำงาน");
    Serial.println("Pump water turned ON");
  } else {
    LINE.notify("ปั้มนำ้หยุดการทำงาน");
    Serial.println("Pump water turned OFF");
  }
}

BLYNK_WRITE(PUMP_fertilizer_SWITCH)
{
  isPumpfertilizerOn = param.asInt();
  if (isPumpfertilizerOn) {
    LINE.notify("ปั้มนำ้ผสมปุ๋ยกำลังทำงาน");
    Serial.println("Pump fertilizer turned ON");
  } else {
    LINE.notify("ปั้มนำ้ผสมปุ๋ยหยุดการทำงาน");
    Serial.println("Pump fertilizer turned OFF");
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(PUMP_WATER_PIN, OUTPUT);
  pinMode(PUMP_fertilizer_PIN, OUTPUT);

  Blynk.begin(auth, ssid, password);

  timer.setInterval(1000L, sendSensorData);

  Blynk.syncVirtual(PUMP_WATER_SWITCH);
  Blynk.syncVirtual(PUMP_fertilizer_SWITCH);

  Serial.println();
  Serial.println(LINE.getVersion());

  WiFi.begin(ssid, password);
  Serial.printf("WiFi connecting to %s\n",  ssid);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(100);
  }
  Serial.printf("\nWiFi connected\nIP : ");
  Serial.println(WiFi.localIP());

  // กำหนด Line Token
  LINE.setToken(LINE_TOKEN);
}

void loop()
{
  Blynk.run();
  timer.run();

}
