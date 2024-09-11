#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <ArduinoJson.h>

const char* ssid = "darly";
const char* password = "password";

// inisialisasi Bot Token
#define BOTtoken "6796853247:AAGdEcS_w_pLht_FNk3W71qyY9YO3ciKaSk"  // Bot Token dari BotFather

// chat id dari @myidbot
#define CHAT_ID "6557116402"

WiFiClientSecure client;
UniversalTelegramBot bot(BOTtoken, client);

#include <LiquidCrystal_I2C.h>
#include <Wire.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

#define trigPin 12
#define echoPin 13
#define SOUND_VELOCITY 0.034

long duration;
float distanceCm;

int botRequestDelay = 1000;
unsigned long lastTimeBotRan;

const int relay= 25;
bool Statusrelay = LOW;



void handleNewMessages(int numNewMessages) {
  Serial.println("handleNewMessages");
  Serial.println(String(numNewMessages));

  for (int i=0; i<numNewMessages; i++) {
    String chat_id = String(bot.messages[i].chat_id);
    if (chat_id != CHAT_ID){
      bot.sendMessage(chat_id, "Unauthorized user", "");
      continue;
    }
    
    String text = bot.messages[i].text;
    Serial.println(text);

    String from_name = bot.messages[i].from_name;

    if (text == "/start") {
      String control = "Selamat Datang, " + from_name + ".\n";
      control += "Gunakan Commands Di Bawah Untuk Membuka atau Mengunci Pintu.\n\n";
      control += "/kunci Untuk Mengunci Pintu 1 \n";
      control += "/buka Untuk Membuka Pintu 1\n";
      control += "/Status Untuk Cek Status Pintu Saat Ini \n";
      bot.sendMessage(chat_id, control, "");
    }

    if (text == "/kunci") {
      bot.sendMessage(chat_id, "Pintu Terkunci", "");
      Statusrelay = HIGH;
      digitalWrite(relay, Statusrelay);
    }
    
    if (text == "/buka") {
      bot.sendMessage(chat_id, "Pintu Terbuka", "");
      Statusrelay = LOW;
      digitalWrite(relay, Statusrelay);
    }

    
    if (text == "/Status") {
      if (digitalRead(relay)){
        bot.sendMessage(chat_id, "Pintu Terkunci", "");
      }
      else{
        bot.sendMessage(chat_id, "Pintu Terbuka", "");
      }
   
    }
  }
}

void setup() {
  Serial.begin(115200);

  pinMode(relay, OUTPUT);
  digitalWrite(relay, Statusrelay);
  pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin, INPUT);
  lcd.begin();
  lcd.setCursor(0,0);
  lcd.print ("Smart Door Lock");
  lcd.setCursor(0,1);
  lcd.print("Loading...");
  lcd.clear();
  delay(1000);
  
  
  // Koneksi Ke Wifi
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  #ifdef ESP32
    client.setCACert(TELEGRAM_CERTIFICATE_ROOT);
  #endif
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }
  // Print ESP32 Local IP Address
  Serial.println(WiFi.localIP());
}

void loop() {
  if (millis() > lastTimeBotRan + botRequestDelay)  {
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);

    while(numNewMessages) {
      Serial.println("got response");
      handleNewMessages(numNewMessages);
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }
    lastTimeBotRan = millis();
    digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(echoPin, HIGH);
  
  // Calculate the distance
  distanceCm = duration * SOUND_VELOCITY/2;
  
  // Prints the distance on the Serial Monitor
  Serial.print("Jarak (cm): ");
  Serial.println(distanceCm);
  delay (500);
  if (distanceCm <= 30){
    lcd.setCursor(0,0);
    lcd.print("Welcome");
    lcd.setCursor(0,1);
    lcd.print("Open with ur hp");
    
  }else if (distanceCm >30){
    lcd.setCursor(0,0);
    lcd.print("Good Bye");
    
  }
  
  }
}