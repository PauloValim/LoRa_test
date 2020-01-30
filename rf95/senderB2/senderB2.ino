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
#include <RH_RF95.h>


//define the pins used by the LoRa transceiver module (for pro mini)

#define RFM95_CS 10
#define RFM95_RST 9
#define RFM95_INT 2
#define TXEN 8
#define RXEN 7


//433E6 for Asia
//866E6 for Europe
//915E6 for North America
#define BAND 915.0

// estrutura da mensagem que vai ser trocada
struct MsgStruct {
//  uint8_t header[4];
  char  pangrama[44]; //= "the quick brown fox jump over the crazy dog";
  uint16_t count;
};

// permite acessar a mnensagem como um array de bytes.
union MsgUnion {
  uint8_t msg_array[sizeof(MsgStruct)];
  MsgStruct msg;
};

union MsgUnion msg_tx;

RH_RF95 rf95(RFM95_CS, RFM95_INT);

String LoRaData;

long lastSendTime = 0;        // last send time
int interval = 2000;          // interval between sends
uint8_t ackOk = 0;

void setup() 
{

  // inicia pinos de controle TX RX do modulo E19
  pinMode(TXEN, OUTPUT);
  digitalWrite(TXEN, LOW);
  pinMode(RXEN, OUTPUT);
  digitalWrite(RXEN, LOW); 

  sprintf(msg_tx.msg.pangrama, "the quick brow fox jump over the crazy dog");
 //initialize Serial Monitor
  Serial.begin(9600);

  Serial.println("LoRa Sender Test");

    pinMode(RFM95_RST, OUTPUT);
  digitalWrite(RFM95_RST, HIGH);
  // manual reset
  digitalWrite(RFM95_RST, LOW);
  delay(10);
  digitalWrite(RFM95_RST, HIGH);
  delay(10);
  if (!rf95.init())
    Serial.println("init failed");
  // Defaults after init are 434.0MHz, 13dBm, Bw = 125 kHz, Cr = 4/5, Sf = 128chips/symbol, CRC on
  if (!rf95.setFrequency(BAND)) {
    Serial.println("setFrequency failed");
    while (1);
  }
  // The default transmitter power is 13dBm, using PA_BOOST.
  // If you are using RFM95/96/97/98 modules which uses the PA_BOOST transmitter pin, then 
  // you can set transmitter powers from 5 to 23 dBm:
//  driver.setTxPower(23, false);
  // If you are using Modtronix inAir4 or inAir9,or any other module which uses the
  // transmitter RFO pins and not the PA_BOOST pins
  // then you can configure the power transmitter power for -1 to 14 dBm and with useRFO true. 
  // Failure to do that will result in extremely low transmit powers.
//  driver.setTxPower(14, true);
  
  rf95.setTxPower(23);
  rf95.setFrequency(915.0);
  rf95.setSpreadingFactor(7);
  rf95.setSignalBandwidth(125E3);  
  rf95.setCodingRate4(5);
  rf95.setPreambleLength(8);
}

void sendMessage(){
  digitalWrite(TXEN, HIGH);
    Serial.println("Sending to rf95_server");
  // Send a message to rf95_server
 // uint8_t data[] = "Hello World!";

  rf95.send(msg_tx.msg_array, sizeof(msg_tx.msg_array));
//  rf95.waitPacketSent();
  delay(300);
  digitalWrite(TXEN, LOW);
  msg_tx.msg.count++;
  
}

  uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
  uint8_t len = sizeof(buf);

void onReceive(int packetSize) {
  if (packetSize == 0) return;          // if there's no packet, return 0

  //received a packet
  Serial.print("Received packet of size: ");
  Serial.print(packetSize);
  Serial.print(": ");

  //read packet
  while (rf95.available()) {
    rf95.recv(buf, &len);
    Serial.print("got reply: ");
    Serial.println((char*)buf);
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
//  digitalWrite(RXEN, HIGH);
//  onReceive(rf95.available());
//  digitalWrite(RXEN, LOW);
}


  
