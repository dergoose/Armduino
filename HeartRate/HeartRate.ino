#include <WiFi.h>
#include <HTTPClient.h>
void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA); // set in STATION mode so we have Internet
  WiFi.begin("MAKE", "cics290m");
  Serial.println("Connecting...");
  while(WiFi.status() != WL_CONNECTED) { delay(500); Serial.print("."); }
  HTTPClient http;
  http.begin("http://weather.opensprinkler.com/weatherData.py?loc=01002"); 
  int httpCode = http.GET();
  if(httpCode == HTTP_CODE_OK) {
    String payload = http.getString();
    Serial.println(payload);
  }
  http.end();
}

void loop() {
}
