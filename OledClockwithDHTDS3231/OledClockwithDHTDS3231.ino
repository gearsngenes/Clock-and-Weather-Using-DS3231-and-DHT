#include <SPI.h>
#include <Wire.h>
#include "ds3231.h"  // clock chip

#include "DHT.h" // Humidity and Temp Sensor
#define DHTPIN 3     // what pin we're connected to
#define DHTTYPE DHT22   // DHT 22  (AM2302)
DHT dht(DHTPIN, DHTTYPE);// define dht type.


// ************** Adafruit Oled Initializations
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);
#define NUMFLAKES 10
#define XPOS 0
#define YPOS 1
#define DELTAY 2
#define LOGO16_GLCD_HEIGHT 16
#define LOGO16_GLCD_WIDTH  16
#if (SSD1306_LCDHEIGHT != 64)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif
// ************** Adafruit Oled Initializations Complete 


boolean colonCheck = true; // to determine if we should put colon
boolean AM; // is it AM or PM
int hum; // humidity
int far; // temperature
const int interval = 10; // after how many iterations of the "loop" calling dht, should we actually go to DHT.
int dhtcallcounter = 0; // counter for how many times dht is culled as of now.
struct ts timestamp; // global timestamp struct

void setup()
{
  Serial.begin(9600);
  Wire.begin();
  DS3231_init(DS3231_INTCN); // clock initialization

//  Serial.println("Setting time"); when you want to set time

 // setTime(int year, int month, int day, int weekday, int hour, int min, int sec) 

//   setTime(  2015,          6,        7,         7,         14,      49,     22);

    
  Serial.println("DHTxx test!");

  dht.begin();
  hum = dht.readHumidity();
  // Read temperature as Celsius
  far = dht.readTemperature(true);

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.display();
  delay(500);
  display.clearDisplay();
  /* end of Adafruit Default Specification */
  /* Basic "hello world!" */
  display.setTextSize(3);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.clearDisplay();
  String s = "AnnaSid";
  display.println(s);
  display.display();
  display.clearDisplay();
  delay(500);

}

void setTime(int year, int month, int day, int weekday, int hour, int min, int sec)
 {
   struct ts ts1;
  
    ts1.sec = sec;
    ts1.min = min;
    ts1.hour = hour;
    ts1.wday = weekday;
    ts1.mday = day;
    ts1.mon = month;
    ts1.year = year;
    DS3231_set(ts1);
 }

void processDHT()
{

  long int start, endt;
  start = millis();
  dhtcallcounter++;
  if (dhtcallcounter > interval)
  {
    hum = dht.readHumidity();
    // Read temperature as Celsius
    far = dht.readTemperature(true);
    dhtcallcounter = 0;
  }
  endt = millis();
//  Serial.print("Start:"); Serial.print(start); Serial.print(" End:"); Serial.print(endt); Serial.print("Diff:"); Serial.println(endt - start);
  if (isnan(hum) ||  isnan(far)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

/*

  Serial.print("Humidity: ");
  Serial.print(hum);
  Serial.print(" %\t");
  Serial.print("Temperature: ");
  Serial.print(far);
  Serial.print(" *F\t");
  */

}



void loop()
{
  char in;


  unsigned long now = millis();
  struct ts t;

  DS3231_get(&t); //Get time
//  parse_cmd("C", 1);

  OledDsplayTime(t);
{
  t.wday = 6;
  Serial.print("YYMMDD:"); Serial.print(t.year); Serial.print(":"); Serial.print(t.mon); Serial.print(":");Serial.print(t.mday);Serial.print(" ");Serial.print(t.wday);
  Serial.println();
  Serial.print("Time:"); Serial.print(t.hour); Serial.print(":"); Serial.print(t.min); Serial.print(":"); Serial.println(t.sec);
}
//  SerialPrintTime(t);
  if ((t.sec % 2) ==0){colonCheck = true;} else {colonCheck = false;}

  processDHT();
  delay(100);

}

void OledDisplayDate(ts & t)
{
  display.setCursor(0, 0);  display.clearDisplay(); //setting the screen
  display.setTextSize(2); //size 2 for month and day and year.
  displayPrintNumber(t.mday); displayPrintMonth(t.mon); display.print(" ");  display.print (t.year);

  // dont use display.display here because then it refreshes every second the same date.
}

void OledDsplayTime(ts & t)
{
  OledDisplayDate(t);
  display.setTextSize(1); display.println(); // by reducing the text size, you get a finer separation
  display.setCursor(7, 20);
  display.setTextSize(3); // size 3 for Time.
  if (t.hour > 12) {
    AM = false;
    t.hour = t.hour - 12;
  }  else {
    AM = true;
  }; displayPrintNumber(t.hour);

  if (colonCheck) {
    display.print(":");
  } else {
    display.print(" ");
  }; displayPrintNumber(t.min);

  display.setTextSize(2);
  if (AM) {
    display.print("AM");
  } else {
    display.print("PM");
  }

  display.setTextSize(2);
  display.setCursor(52, 50); // to match with the right column and row. setCursor(column,row).
  displayPrintNumber(t.sec);
  display.setTextSize(2);
  display.setCursor(0, 50);
  display.print(far); display.setTextSize(1); display.print("0"); display.setTextSize(2);display.print("F");
  display.setCursor(88, 50);
  display.print(hum); display.print("%");

  display.display();

}

void SerialPrintTime(ts & t)
{

  Serial.print("YYMMDD:"); Serial.print(t.year); Serial.print(":"); Serial.print(t.mon); Serial.print(":");Serial.print(t.mday);Serial.print(" ");Serial.print(t.wday); Serial.print(" ");Serial.print(t.isdst);
  Serial.println();
  Serial.print("Time:"); Serial.print(t.hour); Serial.print(":"); Serial.print(t.min); Serial.print(":"); Serial.println(t.sec);
}


/*
void parse_cmd(char *cmd, int cmdsize)
{
  uint8_t i;
  uint8_t reg_val;
  char buff[BUFF_MAX];
  struct ts t;

  //snprintf(buff, BUFF_MAX, "cmd was '%s' %d\n", cmd, cmdsize);
  //Serial.print(buff);

  // TssmmhhWDDMMYYYY aka set time
  if (cmd[0] == 84 && cmdsize == 16) {
    //T355720619112011
    t.sec = inp2toi(cmd, 1);
    t.min = inp2toi(cmd, 3);
    t.hour = inp2toi(cmd, 5);
    t.wday = inp2toi(cmd, 7);
    t.mday = inp2toi(cmd, 8);
    t.mon = inp2toi(cmd, 10);
    t.year = inp2toi(cmd, 12) * 100 + inp2toi(cmd, 14);
    DS3231_set(t);
    Serial.println("OK");
  }

}
*/


void displayPrintNumber(int number)
{

  switch (number)
  {
    case 0: display.print("00"); break;
    case 1: display.print("01"); break;
    case 2: display.print("02"); break;
    case 3: display.print("03"); break;
    case 4: display.print("04"); break;
    case 5: display.print("05"); break;
    case 6: display.print("06"); break;
    case 7: display.print("07"); break;
    case 8: display.print("08"); break;
    case 9: display.print("09"); break;
    default: display.print(number); break;
  }
}






void displayPrintMonth(int month)
{
  switch (month)
  {
    case 1: display.print("Jan"); break;
    case 2: display.print("Feb"); break;
    case 3: display.print("Mar"); break;
    case 4: display.print("Apr"); break;
    case 5: display.print("May"); break;
    case 6: display.print("Jun"); break;
    case 7: display.print("Jul"); break;
    case 8: display.print("Aug"); break;
    case 9: display.print("Sep"); break;
    case 10: display.print("Oct"); break;
    case 11: display.print("Nov"); break;
    case 12: display.print("Dec"); break;
    default: display.print("Err"); break;
  }
}

