#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <FirebaseArduino.h>// firebase kütüphanesini ekle
#include <Wire.h>             //I2C kullanılacağı için "Wire" kütüphanesi programa dahil ediliyor.
#include <Adafruit_SSD1306.h> //SSD1306 için kullanılacak Adafruit kütüphanesi dahil ediliyor.


#define SCREEN_WIDTH 128      // OLED display width, in pixels
#define SCREEN_HEIGHT 64      // OLED display height, in pixels

const char *ssid = "FiberHGW_ZT7CK2_2.4GHz";                 // bağlanılacak olan kablosuz ağın SSID si
const char *password = "d9YhKNbuRUek";                  // bağlanılacak olan kablosuz ağın şifresi

#define FIREBASE_HOST "iotprojectdb-70038-default-rtdb.firebaseio.com"     // veritabanının linki
#define FIREBASE_AUTH "HqYgbhSCaaAY7Duumx02dnFl4opJ8GhU4JL9pJcz"              // veritabanı secret key

const String serverWeather = "api.weatherapi.com";
const String endpointWeather = "/v1/current.json?key=81a346ad4b254b1eb52132424222012&q=";

const String serverTime = "time.is";
const String endpointTime = "/Kütahya,_Kütahya";

String weatherLocation = "";
String temperature = "";
String rainPosibility = "";

String note = "";

String timeIn = "";

String showWeather = "true";
String showTime = "true";
String showNote = "true";

//*********             SETUP               ****************
Adafruit_SSD1306 ekran(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1); //Displayin rst piniolmadığından '-1' yazıyoruz.
void setup() 
{
  ekran.begin(SSD1306_SWITCHCAPVCC, 0x3C);  //Display başlatılıyor ve adresi belirtiliyor (0x3c) 
  
  ekran.clearDisplay();                     //Display temizleniyor 
  ekran.setCursor(0,0);                     // İmleç sol üst köşeye alınıyor.                    
  ekran.display();                          // Ekrana gönderilen yazıların tamamı gösteriliyor.

//*********************************************************************************************************************

  Serial.begin(9600);                             //Seri iletişim hızı 9600 bps olarak ayarlanarak başlatılıyor.
  delay(1000);
  Serial.println("Wifi agina baglaniliyor");

  WiFi.mode(WIFI_STA);                           //ESP8266 istasyon moduna alınıyor. 
  WiFi.begin(ssid, password);                    //SSID ve şifre girilierek wifi başlatılıyor.

  //------- Wifi ağına bağlanıncaya kadar beklenilen kısım ------------
  while (WiFi.status() != WL_CONNECTED) 
  { 
    delay(500);                                 // Bu arada her 500 ms de bir seri ekrana yan yana noktalar yazdırılarak
    Serial.print(".");                          //görsellik sağlanıyor.
  }
  //-------------------------------------------------------------------

  Serial.println("");                           //Bir alt satıra geçiliyor.
  Serial.println("Bağlantı sağlandı...");       //seri ekrana bağlantının kurulduğu bilgisi gönderiliyor.

  Serial.print("Alınan IP addresi: ");          // kablosuz ağdan alınan IP adresi
  Serial.println(WiFi.localIP());               // kablosuz ağdan alınan IP adresi

  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH); // firabase i başlat
}

//*********             LOOP               ****************//*********             LOOP               ****************
//*********             LOOP               ****************//*********             LOOP               ****************
//*********             LOOP               ****************//*********             LOOP               ****************
void loop() 
{
  Control();

  if(showWeather == "true")
    GetWeather();

  if(showNote == "true")
    GetNote();

  if(showTime == "true")
    GetTime();
}

//*********             NOTE               NOTE             NOTE               ****************
//*********             NOTE               NOTE             NOTE               ****************
//*********             NOTE               NOTE             NOTE               ****************
void GetNote()
{
  if(Firebase.getString("note") != "")
  {
    note = Firebase.getString("note");

    Serial.println(note);
    ekran.setTextSize(2);
    ekran.setTextColor(WHITE);
    ekran.setCursor(0,0);
    
    ekran.println(note);
    ekran.display();
    delay(10000);
    ekran.clearDisplay(); 

      
  }
  else
  {
    Serial.println("Not Alınamadı!");
  }

  delay(5000);
}

//*********             WEATHER               WEATHER             WEATHER               ****************
//*********             WEATHER               WEATHER             WEATHER               ****************
//*********             WEATHER               WEATHER             WEATHER               ****************
void GetWeather()
{
  WiFiClient client;
  
  if (!client.connect(serverWeather, 80))           // web bağlantısı yap, bağlanamadı mı?
  {
    Serial.println("Baglanti kurulamadi!!!");    // bağlanamadı
    return;
  }

  Serial.println("Web Sunucusuna baglandi");
    
  client.print(String("GET ")+ endpointWeather+ weatherLocation + "&aqi=no" + " HTTP/1.1\r\n" + "Host: " + serverWeather + "\r\n" +
             "Connection: close\r\n\r\n");
  
  String result = client.readString();

  int startTemperature = result.indexOf("temp_c")+8; 
  int lastTemperature = result.indexOf("temp_f")-2;

  int startRainPosibility = result.indexOf("humidity")+10;
  int lastRainPosibility = result.indexOf("cloud")-2;
  
  temperature = result.substring(startTemperature,lastTemperature);
  rainPosibility = result.substring(startRainPosibility, lastRainPosibility);

    ekran.setTextSize(2);
    ekran.setTextColor(WHITE);
    ekran.setCursor(0,0);
    ekran.println(temperature);
    ekran.display();
    delay(10000);
    ekran.clearDisplay(); 

    ekran.setTextSize(2);
    ekran.setTextColor(WHITE);
    ekran.setCursor(0,0);
    ekran.println(rainPosibility);
    ekran.display();
    delay(10000);
    ekran.clearDisplay(); 


 
}

//*********             TIME               TIME             TIME               ****************
//*********             TIME               TIME             TIME               ****************
//*********             TIME               TIME             TIME               ****************
void GetTime()
{

  WiFiClient client;                       // istemci nesnesi oluşturuldu

  if (!client.connect(serverTime, 80))           // web bağlantısı yap, bağlanamadı mı?
  {
    Serial.println("Baglanti kurulamadi!!!");    // bağlanamadı, hata ver
    return;
  }
   
  client.print(String("GET ")+ endpointTime + " HTTP/1.1\r\n" + "Host: " + serverTime + "\r\n" +
             "Connection: close\r\n\r\n");

  String result = client.readString();

  int start=result.indexOf("Date:");                                      

  timeIn=result.substring(start+6,start+22);


  
    ekran.setTextSize(2);
    ekran.setTextColor(WHITE);
    ekran.setCursor(0,0);
    ekran.println(timeIn);
    ekran.display();
    delay(10000);
    ekran.clearDisplay(); 

}

//*********             CONTROL               CONTROL             CONTROL               ****************
//*********             CONTROL               CONTROL             CONTROL               ****************
//*********             CONTROL               CONTROL             CONTROL               ****************
void Control()
{
    showWeather = Firebase.getString("showWeather");
    showTime = Firebase.getString("showTime");
    showNote = Firebase.getString("showNote");
    weatherLocation = Firebase.getString("weatherLocation");
}
