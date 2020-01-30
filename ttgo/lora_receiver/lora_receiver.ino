/*********
recebimento de mensagem de teste. Necessario definir o tipo de hardware que esta sendo usado.
*********/
#define TTGO_BOARD
//#define ESP32_BOARD

//Libraries for LoRa
#include <SPI.h>
#include <LoRa.h>
#include <string.h> 


//Libraries for OLED Display
#ifdef TTGO_BOARD
    #include <Wire.h>
    #include <Adafruit_GFX.h>
    #include <Adafruit_SSD1306.h>
#endif

//define the pins used by the LoRa transceiver module ttgo
#ifdef TTGO_BOARD 
    #define SCK 5
    #define MISO 19
    #define MOSI 27
    #define SS 18
    #define RST 14
    #define DIO0 26
#endif


//define the pins used by the LoRa transceiver module (for esp32)
#ifdef ESP32_BOARD
    #define SCK 18
    #define MISO 19
    #define MOSI 23
    #define SS 05
    #define RST 14
    #define DIO0 02
    #define TXEN 25
    #define RXEN 26
    sadas
#endif
#define BUILTIN_LED 2
int ledState;



//433E6 for Asia
//866E6 for Europe
//915E6 for North America
#define BAND 915E6

#define BW 125E3
#define SF 7
#define EC 5
#define POWER 20


#ifdef TTGO_BOARD
//OLED pins
    #define OLED_SDA 4
    #define OLED_SCL 15 
    #define OLED_RST 16
    #define SCREEN_WIDTH 128 // OLED display width, in pixels
    #define SCREEN_HEIGHT 64 // OLED display height, in pixels

    Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RST);
#endif
String LoRaData;
char  pangrama[44];


struct MsgStruct {
  uint8_t header[4]; // cabeçalho usado pela biblioteca rf95.h
  uint16_t count;
  char  pangrama[44]; //= "the quick brown fox jump over the crazy dog";
};

union MsgUnion {
  char msg_array[sizeof(MsgStruct)];
  MsgStruct msg;
};
 
MsgUnion msg_rx; 
// inicia variaveis de controle estatístico das mensagens recebidas.
unsigned int  dataError = 0;
unsigned int  seqError = 0;
unsigned int  lastSeq = 0;
int           rssi;

void VerificaMsgRx(MsgUnion dataRx) {
  if (strcmp(dataRx.msg.pangrama, "the quick brow fox jump over the crazy dog")) dataError++;
  else 
     if (dataRx.msg.count != (lastSeq+1)) seqError++;
  lastSeq= dataRx.msg.count;   
  Serial.print("count = ");
  Serial.print(dataRx.msg.count);
  Serial.print("  data error = ");
  Serial.print(dataError);
  Serial.print("  seq. error = ");
  Serial.print(seqError);
  rssi = LoRa.packetRssi();
  Serial.print("  with RSSI ");    
  Serial.println(rssi);
  Serial.flush();
}


void setup() { 
#ifdef TTGO_BOARD 
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
 #endif 

  pinMode(BUILTIN_LED, OUTPUT);
  digitalWrite(BUILTIN_LED, LOW);
  ledState= 0;
  //initialize Serial Monitor
  Serial.begin(115200);

  Serial.println("LoRa Receiver Test");
  
  //SPI LoRa pins
  SPI.begin(SCK, MISO, MOSI, SS);
  //setup LoRa transceiver module
  LoRa.setPins(SS, RST, DIO0);

  #ifdef ESP32_BOARD
      // inicia pinos de controle TX RX do modulo E19
      pinMode(TXEN, OUTPUT);
      digitalWrite(TXEN, LOW);
      pinMode(RXEN, OUTPUT);
      digitalWrite(RXEN, LOW); 
  #endif
  LoRa.setTxPower(POWER);
  LoRa.setFrequency(915E6);
  LoRa.setSpreadingFactor(SF);
  LoRa.setSignalBandwidth(BW);
  LoRa.setCodingRate4(EC);
  LoRa.setPreambleLength(8);
  LoRa.setSyncWord(0x12);
  LoRa.enableCrc();

  if (!LoRa.begin(BAND)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }
  Serial.println("LoRa Initializing OK!");

  #ifdef TTGO_BOARD
      display.setCursor(0,10);
      display.println("LoRa Initializing OK!");
      display.display();  
#endif
  Serial.println("");
  Serial.print("BW= ");
  Serial.print(BW);
  Serial.print("  SF= ");
  Serial.print(SF);
  Serial.print("  EC= 4/");
  Serial.println(EC);
  #ifdef ESP32_BOARD
    digitalWrite(RXEN, HIGH);
  #endif 
}


char buf[255];

void loop() {
  int i;
  //LoRa.receive(sizeof(msg_rx));
  //try to parse packet
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    //received a packet
    Serial.print("Size of received packet ");
    Serial.println(packetSize);

    LoRa.readBytes(buf, packetSize);

    for (i=0; i<sizeof(msg_rx.msg_array); i++){
      msg_rx.msg_array[i]= buf[i];
    }

    
    while (LoRa.read() != -1);
    VerificaMsgRx(msg_rx);


    ledState = !ledState;
    digitalWrite(BUILTIN_LED, ledState);

    Serial.print(msg_rx.msg.pangrama);

    Serial.print("  ");
    Serial.print(msg_rx.msg.count);

    //print RSSI of packet
    int rssi = LoRa.packetRssi();
    Serial.print(" with RSSI ");    
    Serial.println(rssi);
    Serial.print(msg_rx.msg.header[0]);
    Serial.print("  ");
     Serial.print(msg_rx.msg.header[1]);
    Serial.print("  ");   
    Serial.print(msg_rx.msg.header[2]);
    Serial.print("  ");
    Serial.println(msg_rx.msg.header[3]);

#ifdef TTGO_BOARD
   // Dsiplay information
   display.clearDisplay();
   display.setCursor(0,0);
   display.print("LORA RECEIVER");
 //  display.setCursor(0,15);
 //  display.print("Received packet:");
   display.setCursor(0,15);
   display.print(msg_rx.msg.pangrama);
   display.setCursor(0,38);
   display.print("Count:");
   display.setCursor(40,38);
   display.print(msg_rx.msg.count);
   display.setCursor(0,50);
   display.print("RSSI: ");
   display.setCursor(40,50);
   display.print(rssi);
   display.display();   
#endif
  }
}
