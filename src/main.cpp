/*
 *
 */

#include <Arduino.h>
#include "HardwareSerial.h"
#include "SSD1306.h"
#include "PMS.h"
#include <TinyGPS++.h>

#include "mySD.h"

PMS pms(Serial1);
PMS::DATA data;
SSD1306 display(0x3c, 21, 22);

// The TinyGPS++ object
static const int RXPin = 0, TXPin = 4;
static const uint32_t GPSBaud = 4800;
TinyGPSPlus gps;

void setup()
{
  Serial.begin(115200);
  Serial1.begin(9600, SERIAL_8N1, 36, 39);   //pms5007
  Serial2.begin(9600, SERIAL_8N1, 0, 4);
  // Serial2.begin(GPSBaud);

  display.init();
  display.flipScreenVertically();
  display.setFont(ArialMT_Plain_10);

   setupMMC();
   appendFile(SD, "/log.txt", "date,time,lat,long,PM_2_5,PM_10_0\n");
}

void displayInfo();
bool bNewData = 0;

void loop(void)
{
  // delay(2000);
  if (pms.read(data))
  {
    display.clear();
    display.setTextAlignment(TEXT_ALIGN_LEFT);
    display.setFont(ArialMT_Plain_10);
    display.drawString(0, 0, "pm2.5");
    display.drawString(60, 0, "pm10");
    display.setFont(ArialMT_Plain_24);
    display.drawString(0, 20, String(data.PM_AE_UG_2_5));
    display.drawString(60, 20, String(data.PM_AE_UG_10_0));

    if (gps.location.isValid() && gps.time.isValid() && gps.date.isValid()) {
      char buf[100];
      int cnt = sprintf(buf, "%d,%d,", gps.date.value(), gps.time.value());
      cnt += sprintf(buf+cnt, "%.5f,%.5f,", gps.location.lat(), gps.location.lng());
      cnt += sprintf(buf+cnt, "%d,%d\n", data.PM_AE_UG_2_5, data.PM_AE_UG_10_0);
      appendFile(SD, "/log.txt", buf);
      buf[cnt-1] = '\r';
      Serial.print(buf);

      display.setFont(ArialMT_Plain_10);
      display.drawString(100,50, String("gps"));
    }

    display.display();
  }

  while (Serial2.available() > 0)
    if (gps.encode(Serial2.read())) {
    }

  if (millis() > 5000 && gps.charsProcessed() < 10) {
    Serial.println(F("No GPS detected: check wiring."));
    // while(true);
  }
}
