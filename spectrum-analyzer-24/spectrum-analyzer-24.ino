#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <SPI.h>
#include "cc2500_REG.h"           // CC2500 registers description

#define SCAN_CS           10      // scanner Select

////SPI0
//#define OLED_DC_SPI0      7
//#define OLED_CS_SPI0      9
//#define OLED_CLK_SPI0     5 // 13
//#define OLED_MOSI_SPI0    6 // 11
//#define OLED_RESET_SPI0   4
//
////SPI1
//#define OLED_DC_SPI1      7
//#define OLED_CS_SPI1      8
//#define OLED_CLK_SPI1     5 // 13
//#define OLED_MOSI_SPI1    6 // 11
//#define OLED_RESET_SPI1   3

//I2C
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     16 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 displayI2C(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);



#define RSSI_OFFSET      95  // offset for displayed data
#define MAX_CHAN_QTY    255  // max number of channel for spacing 405.456543 kHz
#define MAX_DISP_LINE   127  // limit vertical display resolution if need for small display
#define MAX_SAMPLING    100  // qty of samples in each iteration (1...100) to found a max RSSI value
/*
static const unsigned char PROGMEM logo16_glcd_bmp[] = {
0x00, 0x1f, 0xf3, 0xfe, 0x7f, 0xcf, 0xf8, 0x00, 0x00, 0x1f, 0xf3, 0xfe, 0x7f, 0xce, 0xf8, 0x00, 
0x00, 0x1f, 0xf3, 0xee, 0x7f, 0x4f, 0xb0, 0x00, 0x00, 0x1f, 0xf3, 0xfe, 0x5f, 0xc5, 0x68, 0x00, 
0x00, 0x1e, 0xf3, 0xfe, 0x7f, 0xcf, 0xd8, 0x00, 0x00, 0x1f, 0xf3, 0xfe, 0x7d, 0x8a, 0x60, 0x00, 
0x00, 0x1f, 0xf3, 0xfe, 0x6f, 0xcb, 0xb0, 0x00, 0x00, 0x1f, 0xf3, 0xfe, 0x3e, 0xc6, 0x48, 0x00, 
0x00, 0x1f, 0xf3, 0xbe, 0x7b, 0x8b, 0x50, 0x00, 0x00, 0x1f, 0xf3, 0xf4, 0x5a, 0x89, 0x40, 0x00, 
0x00, 0x1f, 0xf3, 0xfe, 0x7d, 0xc4, 0x90, 0x00, 0x00, 0x1f, 0xb3, 0xfc, 0x2b, 0x05, 0x40, 0x00, 
0x00, 0x1f, 0xf1, 0xb6, 0x6a, 0xcc, 0x20, 0x00, 0x00, 0x1f, 0xf3, 0xfa, 0x3a, 0x42, 0x80, 0x00, 
0x00, 0x1f, 0xf3, 0x6e, 0x4a, 0x88, 0x00, 0x00, 0x00, 0x1d, 0xf3, 0xba, 0x55, 0x04, 0x00, 0x00, 
0x00, 0x1f, 0xb1, 0xda, 0x28, 0x80, 0x00, 0x00, 0x00, 0x1f, 0xd2, 0xd4, 0x24, 0x80, 0x00, 0x00, 
0x00, 0x17, 0xf3, 0x6a, 0x10, 0x00, 0x00, 0x00, 0x00, 0x1e, 0xa1, 0x28, 0x45, 0x00, 0x00, 0x00, 
0x00, 0x1b, 0xd2, 0xd6, 0x10, 0x00, 0x00, 0x00, 0x00, 0x1a, 0xd2, 0x20, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x1b, 0x61, 0xaa, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0d, 0x52, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x16, 0xa1, 0x54, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1a, 0x90, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x0a, 0x42, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x15, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x08, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0a, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x15, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
};*/

byte cal[MAX_CHAN_QTY], rssi_data[MAX_DISP_LINE], data, RSSI_data;
int RSSI_dbm, RSSI_max;

void setup() {                
  Serial.begin(9600);
  
  pinMode(SCAN_CS, OUTPUT);
//  pinMode(OLED_CS_SPI0, OUTPUT);
//  pinMode(OLED_CS_SPI1, OUTPUT);
  
  SPI.begin();
  SPI.setClockDivider(SPI_CLOCK_DIV2);  // max possible SPI speed, 1/2 F_CLOCK
  
  digitalWrite(SCAN_CS, HIGH);
//  digitalWrite(OLED_CS_SPI0, HIGH);
//  digitalWrite(OLED_CS_SPI1, HIGH);
  
  init_CC2500();  // initialize CC2500 registers
  init_displays();
  
  // calibration procedure
  // collect and save calibration data for each displayed channel-
  for (int i = 0; i <= 255; i++) {
    CC2500_Write(CHANNR, i);            // set channel
    CC2500_Write(SIDLE, 0x3D);          // idle mode
    CC2500_Write(SCAL, 0x3D);           // start manual calibration
    delayMicroseconds(800);             // wait for calibration
    data = CC2500_Read(FSCAL1);         // read calibration value
    cal[i] = data;                      // and store it
  }

  CC2500_Write(CHANNR, 0x00);           // set channel
  CC2500_Write(SFSTXON, 0x3D);          // calibrate and wait
  delayMicroseconds(800);               // settling time, refer to datasheet
  CC2500_Write(SRX, 0x3D);              // enable rx

  pinMode(17, INPUT);                   // set pin to input
  digitalWrite(17, HIGH);               // turn on pull-up resistors
}

void loop() {
  if (digitalRead(17) == HIGH) {
  
    for (int i = 0; i <= 255; i++) {
      CC2500_Write(CHANNR, i);          // set channel
      CC2500_Write(FSCAL1, cal[i]);     // restore calibration value for this channel
      delayMicroseconds(300);           // settling time, refer to datasheet
  
      RSSI_max = -120;
      RSSI_dbm = 0; // 
      
      for (int j = 0; j <= MAX_SAMPLING; j++) {// collect samples for max value
        digitalWrite(SCAN_CS,LOW);        // select the scanner chip
        SPI.transfer(REG_RSSI);           // read RSSI register
        RSSI_data = SPI.transfer(0);
        digitalWrite(SCAN_CS,HIGH);
  
        // convert RSSI data from 2's complement to signed decimal
        if (RSSI_data >= 128) {
          RSSI_dbm += (RSSI_data - 256) / 2 - 70;
        } else {
          RSSI_dbm += RSSI_data / 2 - 70;
        }    
        if (RSSI_dbm > RSSI_max) RSSI_max = RSSI_dbm; // keep maximum   
      }
      //RSSI_max = RSSI_dbm / MAX_SAMPLING;
      RSSI_max += RSSI_OFFSET;
      if (RSSI_max > 63) RSSI_max = 63; // 110
      if (RSSI_max < 0) RSSI_max = 0;
      rssi_data[i] = RSSI_max;
    }
    DrawScreen();
  }
}

void DrawScreen() {
  displayI2C.clearDisplay();
  // grid
  displayI2C.drawLine(123, 56, 123, 63, WHITE);
  displayI2C.drawLine(111, 60, 111, 63, WHITE);
  displayI2C.drawLine( 99, 56,  99, 63, WHITE);
  displayI2C.drawLine( 86, 60,  86, 63, WHITE);
  displayI2C.drawLine( 74, 56,  74, 63, WHITE);
  displayI2C.drawLine( 62, 60,  62, 63, WHITE);
  displayI2C.drawLine( 49, 56,  49, 63, WHITE);
  displayI2C.drawLine( 37, 60,  37, 63, WHITE);
  displayI2C.drawLine( 25, 56,  25, 63, WHITE);
  displayI2C.drawLine( 12, 60,  12, 63, WHITE);
  displayI2C.drawLine(  0, 56,   0, 63, WHITE);
  displayI2C.display();

  // horizontal numbers
  displayI2C.setTextSize(1);
  displayI2C.setTextColor(WHITE);
  displayI2C.setRotation(2);
  displayI2C.setCursor(26, 10);  displayI2C.write(49); // 1
  displayI2C.setCursor(51, 10);  displayI2C.write(50); // 2
  displayI2C.setCursor(76, 10);  displayI2C.write(51); // 3
  displayI2C.setCursor(99, 10);  displayI2C.write(52); // 4
  //displayI2C.setCursor(123, 10);  displayI2C.write(53); // 5
  displayI2C.display();

  // vertical numbers
  displayI2C.setTextSize(1);
  displayI2C.setTextColor(WHITE);
  displayI2C.setRotation(2);
  displayI2C.setCursor(110, 11);  displayI2C.write(45); // -
  displayI2C.setCursor(116, 11);  displayI2C.write(52); // 4
  displayI2C.setCursor(122, 11);  displayI2C.write(50); // 2
  displayI2C.setCursor(110, 21);  displayI2C.write(45); // -
  displayI2C.setCursor(116, 21);  displayI2C.write(53); // 5
  displayI2C.setCursor(122, 21);  displayI2C.write(50); // 2
  displayI2C.setCursor(110, 31);  displayI2C.write(45); // -
  displayI2C.setCursor(116, 31);  displayI2C.write(54); // 6
  displayI2C.setCursor(122, 31);  displayI2C.write(50); // 2
  displayI2C.setCursor(110, 41);  displayI2C.write(45); // -
  displayI2C.setCursor(116, 41);  displayI2C.write(55); // 7
  displayI2C.setCursor(122, 41);  displayI2C.write(50); // 2
  displayI2C.setCursor(110, 51);  displayI2C.write(45); // -
  displayI2C.setCursor(116, 51);  displayI2C.write(56); // 8
  displayI2C.setCursor(122, 51);  displayI2C.write(50); // 2
  displayI2C.display();

  displayI2C.setRotation(0);
  for (int x = 0; x <= 127; x++) {
    if (rssi_data[x] >= 63) rssi_data[x] = 221 - rssi_data[x];
    displayI2C.drawLine(127 - x, 0, 127 - x, rssi_data[x] - 1, WHITE);
  }
  displayI2C.display();

}

void init_CC2500() {
  CC2500_Write(0x30,     0x3D);   // software reset for CC2500
  CC2500_Write(FSCTRL1,  0x0F);   // Frequency Synthesizer Control (0x0F)
  CC2500_Write(PKTCTRL0, 0x12);   // Packet Automation Control (0x12)
  CC2500_Write(FREQ2,    0x5C);   // Frequency control word, high byte
  CC2500_Write(FREQ1,    0x4E);   // Frequency control word, middle byte
  CC2500_Write(FREQ0,    0xDE);   // Frequency control word, low byte
  CC2500_Write(MDMCFG4,  0x0D);   // Modem Configuration
  CC2500_Write(MDMCFG3,  0x3B);   // Modem Configuration (0x3B)
  CC2500_Write(MDMCFG2,  0x00);   // Modem Configuration 0x30 - OOK modulation, 0x00 - FSK modulation (better sensitivity)
  CC2500_Write(MDMCFG1,  0x23);   // Modem Configuration
  CC2500_Write(MDMCFG0,  0xFF);   // Modem Configuration (0xFF)
  CC2500_Write(MCSM1,    0x0F);   // Always stay in RX mode
  CC2500_Write(MCSM0,    0x04);   // Main Radio Control State Machine Configuration (0x04)
  CC2500_Write(FOCCFG,   0x15);   // Frequency Offset Compensation configuration
  CC2500_Write(AGCCTRL2, 0x83);   // AGC Control (0x83)
  CC2500_Write(AGCCTRL1, 0x00);   // AGC Control
  CC2500_Write(AGCCTRL0, 0x91);   // AGC Control
  CC2500_Write(FSCAL3,   0xEA);   // Frequency Synthesizer Calibration
  CC2500_Write(FSCAL2,   0x0A);   // Frequency Synthesizer Calibration
  CC2500_Write(FSCAL1,   0x00);   // Frequency Synthesizer Calibration
  CC2500_Write(FSCAL0,   0x11);   // Frequency Synthesizer Calibration
}

void CC2500_Write(char CC2500_addr, char CC2500_value) {
  digitalWrite(SCAN_CS, LOW);
  while (digitalRead(MISO) == HIGH) { };
  SPI.transfer(CC2500_addr);
  SPI.transfer(CC2500_value);
  digitalWrite(SCAN_CS, HIGH);
}

char CC2500_Read(char CC2500_addr) {
  CC2500_addr += 0x80;
  digitalWrite(SCAN_CS, LOW);
  while (digitalRead(MISO) == HIGH) { };
  SPI.transfer(CC2500_addr);
  char CC2500_reg = SPI.transfer(0);
  digitalWrite(SCAN_CS, HIGH);
  return CC2500_reg;
}

void init_displays() {
//  // by default, we'll generate the high voltage from the 3.3v line internally! (neat!)
//  digitalWrite(OLED_CS_SPI0, LOW);
//  displayI2C.begin(SSD1306_SWITCHCAPVCC);
//  //displayI2C.display();
//  digitalWrite(OLED_CS_SPI0, HIGH);
//  
//  digitalWrite(OLED_CS_SPI1, LOW);
//  displaySPI1.begin(SSD1306_SWITCHCAPVCC);
//  //displaySPI1.display();
//  digitalWrite(OLED_CS_SPI1, HIGH);
  
  displayI2C.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3D (for the 128x64)
  // miniature bitmap display
  displayI2C.clearDisplay();
//  displayI2C.drawBitmap(32, 16, logo16_glcd_bmp, 64, 32, WHITE);
  displayI2C.display();
  // init done
}
