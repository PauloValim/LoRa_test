/*********
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com/ttgo-lora32-sx1276-arduino-ide/
*********/

//Libraries for LoRa
#include <SPI.h>
#include <LoRa.h>

// estrutura da mensagem que vai ser trocada
struct MsgStruct {
  uint8_t header[4];
  char  pangrama[44]; //= "the quick brown fox jump over the crazy dog";
  uint16_t count;
};

// permite acessar a mnensagem como um array de bytes.
union MsgUnion {
  uint8_t msg_array[sizeof(MsgStruct)];
  MsgStruct msg;
};
 
union MsgUnion msg_rx;

//Libraries for OLED Display
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

//define the pins used by the LoRa transceiver module
#define SCK 5
#define MISO 19
#define MOSI 27
#define SS 18
#define RST 14
#define DIO0 26

//433E6 for Asia
//866E6 for Europe
//915E6 for North America
#define BAND 915E6

//OLED pins
#define OLED_SDA 4
#define OLED_SCL 15 
#define OLED_RST 16
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RST);

String LoRaData;

void setup() { 
  
  //reset OLED display via software
  pinMode(OLED_RST, OUTPUT);
  digitalWrite(OLED_RST, LOW);
  delay(20);
  digitalWrite(OLED_RST, HIGH);
  
  //initialize OLED
  Wire.begin(OLED_SDA, OLED_SCL);
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3c, false, false)) { // Address 0x3C for 128x32
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.setCursor(0,0);
  display.print("LORA RECEIVER ");
  display.display();
  
  //initialize Serial Monitor
  Serial.begin(115200);

  Serial.println("LoRa Receiver Test");
  
  //SPI LoRa pins
  SPI.begin(SCK, MISO, MOSI, SS);
  //setup LoRa transceiver module
  LoRa.setPins(SS, RST, DIO0);

  if (!LoRa.begin(BAND)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }
  Serial.println("LoRa Initializing OK!");
  display.setCursor(0,10);
  display.println("LoRa Initializing OK!");

  LoRa.setTxPower(20);
  LoRa.setFrequency(915E6);
  LoRa.setSpreadingFactor(7);
  LoRa.setSignalBandwidth(125E3);
  LoRa.setCodingRate4(5);
  LoRa.setPreambleLength(8);
//  LoRa.setSyncWord(0x12);
  
  display.display();  
}
uint8_t i;
uint8_t msg_ack[] = "Ack";
void loop() {

  //try to parse packet
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    //received a packet
    Serial.print("Received packet of size: ");
    Serial.print(packetSize);
    Serial.print(" ");

    //read packet
    while (LoRa.available()) {
      LoRaData = LoRa.readString();
  //    Serial.print(LoRaData);
    }
    // copia os dados recebidos para a estrutura
    for (i=0; i< sizeof(msg_rx.msg_array); i++) {
      msg_rx.msg_array[i] = LoRaData[i];
    }
    Serial.print(msg_rx.msg.pangrama);
    Serial.print(" ");
    Serial.println(msg_rx.msg.count);

    //print RSSI of packet
    int rssi = LoRa.packetRssi();
    Serial.print(" with RSSI ");    
    Serial.println(rssi);
 //exibe o valor dos primeiros 8 bytes recebidos
    Serial.print((uint8_t)LoRaData[0]);
    Serial.print(" ");
    Serial.print((uint8_t)LoRaData[1]);
    Serial.print(" ");
    Serial.print((uint8_t)LoRaData[2]);
    Serial.print(" ");
    Serial.println((uint8_t)LoRaData[3]);
        
    Serial.print((uint8_t)LoRaData[4]);
    Serial.print(" ");
    Serial.print((uint8_t)LoRaData[5]);
    Serial.print(" ");
    Serial.print((uint8_t)LoRaData[6]);
    Serial.print(" ");
    Serial.println((uint8_t)LoRaData[7]);
/****
   // Dsiplay information
   display.clearDisplay();
   display.setCursor(0,0);
   display.print("LORA RECEIVER");
   display.setCursor(0,20);
   display.print("Received packet:");
   display.setCursor(0,30);
   display.print(LoRaData);
   display.setCursor(0,40);
   display.print("RSSI:");
   display.setCursor(30,40);
   display.print(rssi);
   display.display();   
*****/
/******/
   //delay(300);
   // envia uma resposta de reconhecimento do dado recebido
   LoRa.beginPacket();
   LoRa.print("ack de volta");
 //  LoRa.write(msg_ack, sizeof(msg_ack));
 //  LoRa.write(0xFF);
 //  LoRa.write(0xFE);
 //  LoRa.write(0);
 //  LoRa.write(0);
//   LoRa.write(msg_rx.msg_array, sizeof(msg_rx.msg_array));
   LoRa.endPacket();
/*****/   
  }
}
