/*
 * LoRa E32-TTL-100
 * Start device, reset or write to the Serial to send a message.
 * https://www.mischianti.org/2019/10/15/lora-e32-device-for-arduino-esp32-or-esp8266-specs-and-basic-usage-part-1/
 *
 * E32-TTL-100----- Arduino UNO
 * M0         ----- GND
 * M1         ----- GND
 * RX         ----- PIN 2 (PullUP & Voltage divider)
 * TX         ----- PIN 3 (PullUP)
 * AUX        ----- Not connected
 * VCC        ----- 3.3v/5v
 * GND        ----- GND
 *
 */
#include "Arduino.h"
#include "LoRa_E32.h"
#include "struct_msg.h"
#include <avr/wdt.h>

// ---------- esp8266 pins --------------
//LoRa_E32 e32ttl100(D2, D3, D5, D7, D6);
//LoRa_E32 e32ttl(D2, D3); // Config without connect AUX and M0 M1

//#include <SoftwareSerial.h>
//SoftwareSerial mySerial(D2, D3); // RX, TX
//LoRa_E32 e32ttl(&mySerial, D5, D7, D6);
// -------------------------------------

// ---------- Arduino pins --------------
LoRa_E32 e32ttl100(2, 3, 5, 7, 6);
//LoRa_E32 e32ttl100(2, 3); // Config without connect AUX and M0 M1

//#include <SoftwareSerial.h>
//SoftwareSerial mySerial(2, 3); // RX, TX
//LoRa_E32 e32ttl(&mySerial, 5, 7, 6);
// -------------------------------------

void setup() {
  wdt_disable();
  Serial.begin(115200);
  delay(500);

  // Startup all pins and UART
  e32ttl100.begin();

}



  struct Msg_teste msg;

void loop() {

  if (e32ttl100.available()  > 1){
  //  Serial.print(e32ttl100.available());
    ResponseStructContainer rsc = e32ttl100.receiveMessage(sizeof(Msg_teste));

 //   Serial.println(rsc.status.getResponseDescription());

    msg = *(Msg_teste*) rsc.data;
    
    Serial.print(msg.pangrama);
    Serial.print(" ");
    Serial.println(msg.count);
    Serial.flush();
  }
  delay(100);


}
