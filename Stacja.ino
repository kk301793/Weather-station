#include <EEPROM.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <BME280I2C.h>
#include <BH1750.h>


bool errorOccured = false;
unsigned long pomiarTime = 0;
int sensorStatus = 0;
float temp(NAN), hum(NAN), pres(NAN), lux(NAN);

//////// Czujniki ////////// 
BME280I2C bme; 
BH1750 lightMeter;

//////// USTAWIENIA OLED //////////
#define SCREEN_WIDTH  64 //
#define SCREEN_HEIGHT 48  //

#define OLED_RESET     -1 //  Używany jest wspólny pin resetujący Arduino)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);


//////// SETUP ///////
void setup(void){
  Serial.begin(9600);
  Serial.println("");
  initOledDisplay(); //startowanie wyswietlacza
  displaySettings(); //pokazanie danych na ekranie 
  initSensors();     //inicjacja sensorów
}

////////   LOOP
 
void loop(void){
  
  handleOledTimer();
}


////////   WYSWIETLACZ
////////   INICJALIZACJA OLEDA

void initOledDisplay() {
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Adres 0x3C dla 128x32
    Serial.println(F("SSD1306 allocation failed"));
  }  
}


////////   WYSWIETLENIE INFO

void displayOledInfo(String header, String line1, String line2) {
  display.clearDisplay();             // Wyczyść bufor wyświetlacza
  display.setTextSize(1);             
  display.setTextColor(BLACK, WHITE);  
  display.setCursor(32,0);             
  display.println(header);
  display.setTextColor(WHITE);        
  display.setCursor(32,16);   
  display.println(line1);
  display.setCursor(32,25);   
  display.println(line2);
  display.display();
}


////////   WYSWIETL USTAWIENIA INICJALIZACJI

  void displaySettings() {
  display.clearDisplay();               
  display.setTextSize(1);               

  display.setTextColor(WHITE);            
  display.setCursor(0,0);               
  display.println(F("Stacja"));
  display.setTextColor(WHITE);          
  display.setCursor(0,7);               
  display.print(F("pogodowa"));
  
  display.display();
  display.startscrollleft(0x00, 0x0F);

  if (errorOccured) delay(20000);
  else delay(8000);
  display.stopscroll();
 }

////////   WYPISANIE ODCZYTOW

 void paintOledReadings() {
  display.clearDisplay(); 
  display.setTextSize(1); 
  display.setTextColor(WHITE);
  display.setCursor(45,1);  
  display.println(getTemperatureCelcius()+" C");
  display.setCursor(35,24);  
  display.println(getHumidityPercent()+"% "+getLuxStr()+"Lx");
  display.setCursor(35,41);  
  display.println(getPressurePa()+" Pa");
  display.display();
   
 }

 
////////   CZUJNIKI
////////   INICJALIZACJA CZUJNIKOW

 void initSensors() {

  Wire.begin();
   
  if(!lightMeter.begin())
  {
    Serial.println("Nie odnaleziono czujnika BH1750!");
    sensorStatus = 0;
  }
  
  if(!bme.begin())
  {
    Serial.println("Nie odnaleziono czujnika BME280!");
    sensorStatus = 0;
  }
 }

////////   POBRANIE DANYCH Z CZUJNIKOW

//BME 
 void readValuesFromSensor() {
  
   BME280::TempUnit tempUnit(BME280::TempUnit_Celsius);
   BME280::PresUnit presUnit(BME280::PresUnit_Pa);

   bme.read(pres, temp, hum, tempUnit, presUnit);  
   
   //BH
   lux = lightMeter.readLightLevel();
 }

 float getTemperatureCelciusNumber() {
  return temp; 
 }

 int getPressurePaNumber() {
  return pres; 
 }

 int getHumidityPercentNumber() {
  return hum; 
 }
 
 String getTemperatureCelcius() {
  return (String)getTemperatureCelciusNumber();
 }

 String getPressurePa() {
  return (String)getPressurePaNumber();
 }

 String getHumidityPercent() {
  return (String)getHumidityPercentNumber();
 }


 String getLuxStr() {
  int int_value = (int)(lux + 0.5);
  if (int_value<50000)
  {
   return (String)int_value;  
  }
  else
  {
    return "ERR";
  }
  
 }
 

////////   TIMER   ////////

 void handleOledTimer() {   
  
    if (millis() - pomiarTime >= 5000UL) {
      pomiarTime = millis();
      readValuesFromSensor();
      paintOledReadings();
    }
 }
