
#include <ESP32Servo.h>
#include <WiFi.h> // header we need to include for WiFi functions
#define SERVO1 23
#define SERVO2 18
#define SERVO3 15
#include <esp_now.h>

Servo baseServo;  // create servo object to control a servo
Servo elbowServo;
Servo clawServo;
int pos = 0;

int yaw = 0;
int pitch = 0;
int roll = 0;
int angle = 0;

int yawNeutral = -1;
int yawOffset = -1;
int yawOld;

int rollNeutral = -1;
int rollOffset = -1;

float flexMed[10];
int flexDex = 0;

float flexMedian(){
  for(int i = 0; i < 10; i++){
    for(int j = 0; j < 10 - 1; j++){
      if(flexMed[j] > flexMed[j + 1]){
        float temp = flexMed[j];
        flexMed[j] = flexMed[j + 1];
        flexMed[j + 1] = temp;
      }
    }
  }
  return flexMed[5];
}

void setup() {
	baseServo.setPeriodHertz(50);    // standard 50 hz servo
	baseServo.attach(SERVO1); // attaches the servo on pin 18 to the servo object
  elbowServo.attach(SERVO2);
  clawServo.attach(SERVO3);
  Serial.begin(115200);
  
  WiFi.mode(WIFI_STA);

  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
    esp_now_register_recv_cb(onReceiveData);
}

void onReceiveData(const uint8_t *mac, const uint8_t *data, int len) {

  static uint8_t nhan_mac[] = { 0x24, 0x6F, 0x28, 0x16, 0xE6, 0xBC};
 
  Serial.print("Received from MAC: ");
  for (int i = 0; i < 6; i++) {
    if (mac[i] != nhan_mac[i]) {
      Serial.println("not nhan's mac");
      return;
    }
    Serial.printf("%02X", mac[i]);
    if (i < 5)Serial.print(":");
  }
  Serial.println();
 
  yaw = data[0];
  pitch = data[1];
  roll = data[2];
  angle = data[3];

  flexMed[flexDex] = angle;
  flexDex = (flexDex + 1) % 10;
  
  if(yawNeutral == -1){
    yawNeutral = yaw;
    yawOffset = 64 - yawNeutral;
  }
  int realYaw = ((yaw +  255) + yawOffset) % 255;
  if(realYaw < 255 && realYaw > 191){
    realYaw = 0;
  }
  if(rollNeutral == -1){
    rollNeutral = roll;
    rollOffset = 64 - rollNeutral;
  }
  int realRoll = ((roll +  255) + rollOffset) % 255;
  if(realRoll < 255 && realRoll > 191){
    realRoll = 0;
  }

  float yawInterp = 180.0 + -1.0 * (realYaw/255.0 * 360.0);
  float rollInterp = realRoll/255.0 * 360.0 ;
  
  baseServo.write(yawInterp);    // tell servo to go to position in variable 'pos'
  elbowServo.write(rollInterp);
  float realAngle = 90.0 + flexMedian();
  clawServo.write(realAngle);

  Serial.println(yaw);
  Serial.println(yawOffset);
  Serial.println(yawNeutral);
  Serial.println(realYaw);
  Serial.println(yawInterp);

}

void loop() {
  


}
