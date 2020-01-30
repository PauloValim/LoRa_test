// rf95_client.pde
// -*- mode: C++ -*-
// Example sketch showing how to create a simple messageing client
// with the RH_RF95 class. RH_RF95 class does not provide for addressing or
// reliability, so you should only use RH_RF95 if you do not need the higher
// level messaging abilities.
// It is designed to work with the other example rf95_server
// Tested with Anarduino MiniWirelessLoRa, Rocket Scream Mini Ultra Pro with
// the RFM95W, Adafruit Feather M0 with RFM95

#include <SPI.h>
#include <LoRa.h>

//define the pins used by the LoRa transceiver module TTGO
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
 
union MsgUnion msg_tx;

String LoRaData;

long lastSendTime = 0;        // last send time
int interval = 2000;          // interval between sends
uint8_t ackOk = 0;

void setup() 
{

  sprintf(msg_tx.msg.pangrama, "the quick brow fox jump over the crazy dog");
 //initialize Serial Monitor
  Serial.begin(115200);

  Serial.println("LoRa Sender Test");
  
  //SPI LoRa pins
  SPI.begin(SCK, MISO, MOSI, SS);
  //setup LoRa transceiver module
  LoRa.setPins(SS, RST, DIO0);

  if (!LoRa.begin(BAND)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }
  Serial.println("LoRa Initializing OK!");


  LoRa.setTxPower(20);
  LoRa.setFrequency(915E6);
  LoRa.setSpreadingFactor(7);
  LoRa.setSignalBandwidth(125E3);
  LoRa.setCodingRate4(5);
  LoRa.setPreambleLength(8);
  LoRa.setSyncWord(0x12);
  LoRa.enableCrc();

// inicializacao do header
 msg_tx.msg.header[0]= 255;
 msg_tx.msg.header[1]= 255;
 msg_tx.msg.header[2]= 0;
 msg_tx.msg.header[3]= 0;
   
  
}

void sendMessage(){
    Serial.println("Sending to rf95_server");
  // Send a message to rf95_server
 // uint8_t data[] = "Hello World!";

  LoRa.beginPacket();
  LoRa.write(msg_tx.msg_array, sizeof(msg_tx.msg_array));
  LoRa.endPacket();

  msg_tx.msg.count++;
  
}



void onReceive(int packetSize) {
  if (packetSize == 0) return;          // if there's no packet, return 0

  //received a packet
  Serial.print("Received packet of size: ");
  Serial.print(packetSize);
  Serial.print(": ");

  //read packet
  while (LoRa.available()) {
    LoRaData = LoRa.readString();
    Serial.println(LoRaData);
  }
  ackOk = 1; // indica que recebeu um pacote
}


void loop()
{

  if (millis() - lastSendTime > interval) {
    sendMessage();
    ackOk= 0; // indica que aguarda reconhecimento desta mensagem
    Serial.print("Sending ");
    Serial.println(msg_tx.msg.pangrama);
    lastSendTime = millis();            // timestamp the message
    interval = random(2000) + 1000;    // 2-3 seconds
  }

  // parse for a packet, and call onReceive with the result:
  onReceive(LoRa.parsePacket());
}


  
