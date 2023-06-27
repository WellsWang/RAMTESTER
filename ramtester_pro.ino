/* RAMTESTER
 * 
 * Tester for 4116/4164/41256 or compatible DRAM chip.
 *
 * This project is based on 4116 Tester developed by <uffe@frostbox-net>.
 * Original project could be found at: 
 * https://labs.frostbox.net/2020/03/24/4116-d-ram-tester-with-schematics-and-code/
 *
 * This version was added following new features:
 *  - 4164/41256 or compatible support
 *  - OLED display
 *  - Serial output port
 *  - In 4164/41256 mode, auto detect 4164 chip
 *  - Chinese UI support
 *
 * Written & released by Wells Wang <wellswang@osall.com>, 2023.
 * Released under GPL v3 License.
 * See the LISENCE file for more details.
 *
 */

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);


#define DI          5     //  Arduino D5 > 4116 pin 2
#define DO          15    //  Arduino A1 > 4116 pin 14
#define CAS         16    //  Arduino A2 > 4116 pin 15
#define RAS         7     //  Arduino D7 > 4116 pin 4
#define WE          6     //  Arduino D6 > 4116 pin 3

#define XA0         2     //  Arduino D2 > 4116 pin 5
#define XA1         4     //  Arduino D4 > 4116 pin 7
#define XA2         3     //  Arduino D3 > 4116 pin 6
#define XA3         8     //  Arduino D8 > 4116 pin 12
#define XA4         10    //  Arduino D10 > 4116 pin 11
#define XA5         9     //  Arduino D9 > 4116 pin 10
#define XA6         14    //  Arduino A0 > 4164/41256 PIN 13
#define XA7         11    //  Arduino D11 > 4164/41256 PIN 9
#define XA8         12    //  Arduino D12 > 4164/41256 PIN 1

#define R_LED       20     // Option > Cathode on red LED. Anode connected through 470ohm resisor to +5VDC 
#define G_LED       13     // Arduino D13/SCK > Cathode on green LED. Anode connected through 470ohm resisor to +5VDC
#define SEL         17     // A3 > Detect PIN8 Voltage for 4116 or 4164/41256 chip

//Power to the 4116 ic: -5VDC 0.2mA to pin 1 . +12VDC 35mA to pin 8. +5VDC to pin 9. GND to pin 16.
//Power to the 4164/41256 ic: +5VDC to pin 8. GND to pin 16.
//You can use an MC34063 ic to create the -5VDC and the +12VDC from the USB +5VDC rail. Please check the datasheet for the correct values
//Use the reset button to start the test on solder an external momentary button between RST pin and GND pin on arduino.

#define BUS_SIZE     9  // Max buffer size for 41256.

//  Following is Chinese Character Matrix for Chinese UI.
//  测(0) 试(1) 中(2) 通(3) 过(4) 失(5) 败(6)
#define TXT_HEIGHT   12
#define TXT_WIDTH    12
static const unsigned char PROGMEM txt[] =
{0x00,0x20,0xBE,0x20,0x62,0xA0,0x2A,0xA0,0x2A,0xA0,0xAA,0xA0,0x6A,0xA0,0x2A,0xA0,0x2A,0xA0,0x48,0x20,0x94,0x20,0x22,0x60,/*"测",0*/
 0x80,0xA0,0x40,0x80,0x0F,0xE0,0x00,0x80,0xC0,0x80,0x4E,0x80,0x44,0x80,0x44,0x80,0x44,0x80,0x46,0xA0,0x6C,0x60,0x40,0x20,/*"试",1*/
 0x04,0x00,0x04,0x00,0x04,0x00,0x7F,0xC0,0x44,0x40,0x44,0x40,0x44,0x40,0x7F,0xC0,0x44,0x40,0x04,0x00,0x04,0x00,0x04,0x00,/*"中",2*/
 0x9F,0x80,0x44,0x80,0x03,0x00,0x1F,0xC0,0xD2,0x40,0x5F,0xC0,0x52,0x40,0x5F,0xC0,0x52,0x40,0x52,0xC0,0x40,0x00,0xBF,0xE0,/*"通",3*/
 0x40,0x40,0x20,0x40,0x00,0x40,0x0F,0xE0,0xE0,0x40,0x24,0x40,0x22,0x40,0x22,0x40,0x20,0x40,0x21,0xC0,0x50,0x00,0x8F,0xE0,/*"过",4*/
 0xFB,0xFF,0xDB,0xFF,0xDB,0xFF,0xC0,0x7F,0xBB,0xFF,0x7B,0xFF,0xFB,0xFF,0x00,0x1F,0xF5,0xFF,0xEE,0xFF,0xDF,0x7F,0x3F,0x9F,/*"-失",5*/
 0xFE,0xFF,0x06,0xFF,0x75,0xFF,0x54,0x1F,0x53,0xBF,0x55,0xBF,0x55,0xBF,0x55,0xBF,0x55,0x7F,0xDE,0xFF,0xAD,0x7F,0x73,0x9F,/*"-败",6*/
 0x00,0x00,0x1F,0x80,0x3F,0xC0,0x76,0xE0,0x36,0xC0,0x7F,0xE0,0x3F,0xC0,0x79,0xE0,0x36,0xC0,0x7F,0xE0,0x3F,0xC0,0x1F,0x80,/* FAILED ICON 故障内存图标,7*/
 0x00,0x00,0x00,0x00,0x00,0x00,0x1F,0x80,0x36,0xC0,0x10,0x80,0x30,0xC0,0x10,0x80,0x30,0xC0,0x10,0x80,0x30,0xC0,0x1F,0x80, /* MEMCHIP ICON 内存图标,8*/
 0x00,0x00,0x1F,0x80,0x3F,0xC0,0x76,0xE0,0x36,0xC0,0x7F,0xE0,0x3F,0xC0,0x76,0xE0,0x39,0xC0,0x7F,0xE0,0x3F,0xC0,0x1F,0x80};/* PASSED ICON 内存笑脸图标,9*/
volatile int bs;

const unsigned int a_bus[BUS_SIZE] = {
  XA0, XA1, XA2, XA3, XA4, XA5, XA6, XA7, XA8
};

void setBus(unsigned int a) {
  int i;
  for (i = 0; i < bs; i++) {
    digitalWrite(a_bus[i], a & 1);
    a /= 2;
  }
}

void writeAddress(unsigned int r, unsigned int c, int v) {
  /* row */
  setBus(r);
  digitalWrite(RAS, LOW);

  /* rw */
  digitalWrite(WE, LOW);

  /* val */
  digitalWrite(DI, (v & 1)? HIGH : LOW);

  /* col */
  setBus(c);
  digitalWrite(CAS, LOW);

  digitalWrite(WE, HIGH);
  digitalWrite(CAS, HIGH);
  digitalWrite(RAS, HIGH);
}

int readAddress(unsigned int r, unsigned int c) {
  int ret = 0;

  /* row */
  setBus(r);
  digitalWrite(RAS, LOW);

  /* col */
  setBus(c);
  digitalWrite(CAS, LOW);

  /* get current value */
  ret = digitalRead(DO);

  digitalWrite(CAS, HIGH);
  digitalWrite(RAS, HIGH);

  return ret;
}

void error(int r, int c)
{
  unsigned long a = ((unsigned long)c << bs) + r;
  digitalWrite(R_LED, LOW);
  digitalWrite(G_LED, HIGH);
  interrupts();
  Serial.print(" FAILED $");
  Serial.println(a, HEX);   // Output failed address.
  Serial.flush();

  display.fillRect(0,20,79,display.height()-1,BLACK);
  drawTxt(0,20,5);
  drawTxt(12,20,6);
  drawTxt(36,20,7);
  display.setTextSize(1);   // Normal 1:1 pixel scale
  display.setCursor(50,24);   
  display.setTextColor(SSD1306_WHITE);
  display.print(a, HEX);    // Display failed address
  display.display();
  while (1)
    ;
}

void ok(void)
{
  digitalWrite(R_LED, HIGH);
  digitalWrite(G_LED, LOW);
  interrupts();
  Serial.println(" OK!");
  Serial.flush();

  //Display PASSED message
  display.fillRect(0,20,79,display.height()-1,BLACK);
  drawTxt(0,20,0);
  drawTxt(12,20,1);
  drawTxt(24,20,3);
  drawTxt(36,20,4);
  drawTxt(54,20,9);
  display.display();

  while (1)
    ;
}

void blink(void)
{
  digitalWrite(G_LED, LOW);
  digitalWrite(R_LED, LOW);
  delay(1000);
  digitalWrite(R_LED, HIGH);
  digitalWrite(G_LED, HIGH);
}

void green(int v) {
  digitalWrite(G_LED, v);
}


void fill(int v) {
  int r, c, g = 0;
  v &= 1;
  for (c = 0; c < (1<<bs); c++) {
    green(g? HIGH : LOW);
    for (r = 0; r < (1<<bs); r++) {
      writeAddress(r, c, v);
      if (v != readAddress(r, c))
        error(r, c);
    }
    g ^= 1;
  }
  blink();
}

void fillx(int v) {
  int r, c, g = 0;
  v &= 1;
  for (c = 0; c < (1<<bs); c++) {
    green(g? HIGH : LOW);
    for (r = 0; r < (1<<bs); r++) {
      writeAddress(r, c, v);
      if (v != readAddress(r, c))
        error(r, c);
      v ^= 1;
    }
    g ^= 1;
  }
  blink();
}

// Check the value of two differrent address to see the IC is 4164 or not.
// 4164 will ignore XA8, so the value of address XA8=0 and XA8=1 should be same.
bool check4164(){ 
  int i, r , c;
  noInterrupts();
  for (i=0; i<3; i++){
    r = random(256);
    c = random(256);
    writeAddress(r,c,0);
    if (0 != readAddress(r+256,c+256)){
      interrupts();
      return false;
    }
    writeAddress(r,c,1);
    if (1 != readAddress(r+256,c+256)) {
      interrupts();
      return false;
    }
  }
  interrupts();
  return true;
}

void drawTxt(int x, int y, byte id) {
  unsigned char ch[24];
  for (int i=0; i<24; i++){
    ch[i] = pgm_read_word_near(txt + id*24 +i);
   // Serial.println(txt[i],BIN);
  }
  display.drawBitmap(x,y,ch, TXT_WIDTH, TXT_HEIGHT, 1);
}

void drawUI(){
  display.clearDisplay();
  display.setTextSize(2);             // Normal 1:1 pixel scale
  display.setCursor(0,0);   
  display.setTextColor(SSD1306_WHITE);        // Draw white text
  //display.setTextColor(BLACK,WHITE);
  display.print(F("RAMTESTER"));
  display.setTextSize(1);
  display.setCursor(110,8);   
  display.print(F("1.0"));
  display.drawLine(0, 17, display.width()-1, 17, SSD1306_WHITE);
  drawTxt(80,20,8);
  display.display();
}

void drawModel(int m){
  display.setTextSize(1);
  display.setCursor(92,24);   
  display.setTextColor(SSD1306_WHITE);        // Draw white text
  switch(m){ 
    case 0: 
      display.print(F("ERROR"));
      break;
    case 1:
      display.print(F("4116"));
      break;
    case 2:
      display.print(F("4164"));
      break;
    default:
      display.print(F("41256?"));
  }
  display.display();
}

void drawProgress(int p){
  if (p==0) {
    drawTxt(0,20,0);
    drawTxt(12,20,1);
    drawTxt(24,20,2);
  }
  display.setTextSize(1);
  display.setCursor(36+p*6,24);   
  display.print(F("."));
  display.display();
}


void setup() {
  int i;
  int type;

  Serial.begin(9600);
  while (!Serial)
    ; /* wait */
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  Serial.println();
  Serial.println("RAM TESTER v1.0");
  drawUI();


  for (i = 0; i < BUS_SIZE; i++)
    pinMode(a_bus[i], OUTPUT);

  pinMode(CAS, OUTPUT);
  pinMode(RAS, OUTPUT);
  pinMode(WE, OUTPUT);
  pinMode(DI, OUTPUT);

  pinMode(R_LED, OUTPUT);
  pinMode(G_LED, OUTPUT);

  //pinMode(M_TYPE, INPUT);
  pinMode(DO, INPUT);

  digitalWrite(WE, HIGH);
  digitalWrite(RAS, HIGH);
  digitalWrite(CAS, HIGH);

  digitalWrite(R_LED, HIGH);
  digitalWrite(G_LED, HIGH);

  Serial.flush();

  digitalWrite(R_LED, LOW);
  digitalWrite(G_LED, LOW);

  noInterrupts();
  for (i = 0; i < (1 << bs); i++) {
    digitalWrite(RAS, LOW);
    digitalWrite(RAS, HIGH);
  }
  digitalWrite(R_LED, HIGH);
  digitalWrite(G_LED, HIGH);
  interrupts();

  // Check IC Model  
  type = analogRead(SEL);
  if (type > 900) {
    Serial.println("Chip Model: 4116");
    drawModel(1);
    bs = BUS_SIZE - 2;
  }else if (type >300 and type < 550){
    Serial.print("Chip Model: ");
    bs = BUS_SIZE;
    if (check4164()){
      bs = BUS_SIZE - 1;
      Serial.println("4164/4264");
      drawModel(2);
    } else {
      Serial.println("41256");
      drawModel(3);
    }
  }else {
    Serial.println("ERROR on Model Selector!");
    drawModel(0);
    bs = 0;
  }


}

void loop() {
  interrupts(); Serial.print("."); Serial.flush(); drawProgress(0); noInterrupts(); fillx(0);
  interrupts(); Serial.print("."); Serial.flush(); drawProgress(1); noInterrupts(); fillx(1);
  interrupts(); Serial.print("."); Serial.flush(); drawProgress(2); noInterrupts(); fill(0);
  interrupts(); Serial.print("."); Serial.flush(); drawProgress(3); noInterrupts(); fill(1);
  ok();
}
