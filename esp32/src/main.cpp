#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_MCP23017.h>
#include <U8g2lib.h>
#include <ESP32Encoder.h>

const uint8_t gpioSDA = 21;
const uint8_t gpioSCL = 22;
const uint8_t gpioLED = 32;
const uint8_t gpioInterrupt = 33;
const uint8_t gpioRotA = 16;
const uint8_t gpioRotB = 17;
const uint8_t gpioRotButton = 39;

const uint8_t mcpSquaresUnits = 4;
const uint8_t mcpGPIOs = 16;

const uint8_t axisSquares = 8;
const uint8_t boardSize = axisSquares * axisSquares;

const unsigned long bouncingClearance = 500000; // 500ms

enum Piece {
  none = 0,
  king,
  queen,
  rook,
  bishop,
  knight,
  pawn
};

typedef Piece Board[boardSize];

typedef bool Footprint[boardSize];

Footprint lastFootprint;

Adafruit_MCP23017 mcpSquares[mcpSquaresUnits];
Adafruit_MCP23017 mcpLEDs;

U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE);

ESP32Encoder encoder;

volatile bool awakenByInterrupt = false;

void interruptCallback() {
  awakenByInterrupt = true;
}

void lcdInit() {
  u8g2.begin();
  u8g2.clearBuffer();
  u8g2.sendBuffer();
}

void lcdPrepare() {
  u8g2.clearBuffer();
}

void lcdDraw() {
  u8g2.sendBuffer();
}

void lcdPrint(const char *msg) {
  u8g2.setFontMode(0);
  u8g2.setFont(u8g2_font_u8glib_4_hr);
  u8g2.setCursor(65, 10);
  u8g2.print(msg);
}

void copyFootprint(const Footprint from, Footprint to) {
  memcpy(to, from, boardSize);
}

bool readFootprint(Footprint fp) {
  bool hasChanged = false;
  for (uint8_t n = 0; n < mcpSquaresUnits; n++) {
    const uint16_t gpios = mcpSquares[n].readGPIOAB();
    for (uint8_t p = 0; p < mcpGPIOs; p++) {
      const uint8_t idx = n * mcpGPIOs + p;
      bool square = ! (gpios & (1 << p));
      if (fp[idx] != square) {
        fp[idx] = square;
        hasChanged = true;
      }
    }
  }
  return hasChanged;
}

void printFootprint(const Footprint board) {

  const u8g2_uint_t squareDim = 8;
  const u8g2_uint_t boardDim = axisSquares * squareDim;

  for (uint8_t y = 0; y < axisSquares; y++) {
    for (uint8_t x = 0; x < axisSquares; x++) {
      const u8g2_uint_t xs = x * squareDim;
      const u8g2_uint_t ys = boardDim - (y + 1) * squareDim;
      const uint8_t bgColor = (x + y) % 2;
      const uint8_t fgColor = (bgColor + 1) % 2;
      const bool square = board[y * axisSquares + x];

      u8g2.setDrawColor(bgColor);
      u8g2.drawBox(xs, ys, squareDim, squareDim);
      if (square) {
        u8g2.setDrawColor(fgColor);
        u8g2.drawDisc(xs + squareDim/2, ys + squareDim/2, squareDim/4);
      }
    }
  }
}

void initBoard(Board board) {
}

void i2cInit()
{
  Wire.begin(gpioSDA, gpioSCL);

  Serial.println();
  Serial.println("I2C scanner. Scanning ...");
  byte count = 0;

  for (byte i = 8; i < 120; i++)
  {
    Wire.beginTransmission(i);
    if (Wire.endTransmission() == 0)
    {
      Serial.print("Found address: ");
      Serial.print(i, DEC);
      Serial.print(" (0x");
      Serial.print(i, HEX);
      Serial.println(")");
      count++;
    }
  }
  Serial.print("Found ");
  Serial.print(count, DEC);
  Serial.println(" device(s).");
}

void mcpInit() {
  pinMode(gpioInterrupt, INPUT);
  for (uint8_t n = 0; n < mcpSquaresUnits; n++) {
    mcpSquares[n].begin(n);
    mcpSquares[n].setupInterrupts(true, false, HIGH);
    for (uint8_t p = 0; p < mcpGPIOs; p++) {
      mcpSquares[n].pinMode(p, INPUT);
      mcpSquares[n].pullUp(p, HIGH);
      mcpSquares[n].setupInterruptPin(p, CHANGE);
    }
  }
  mcpLEDs.begin(mcpSquaresUnits);
  for (uint8_t p = 0; p < mcpGPIOs; p++) {
    mcpLEDs.pinMode(p, OUTPUT);
  }
  mcpLEDs.writeGPIOAB(0);
  attachInterrupt(digitalPinToInterrupt(gpioInterrupt), interruptCallback, RISING);
}

volatile int32_t dialCounter = 0;
uint8_t lastAB = 0;

uint8_t readDial() {
  return (digitalRead(gpioRotA) << 1) | digitalRead(gpioRotB);
}

int32_t getDialCounter() {
  // return dialCounter;
  return encoder.getCount();
}

void dialInterrupt() {
    uint8_t newAB = readDial();
    uint8_t tr = (lastAB << 2) | newAB;
    int32_t d = 0;
    switch (tr) {
      // case 0b0000:         break;
      case 0b0001: d = -1; break;
      case 0b0010: d = 1;  break;
      // case 0b0011:         break;
      case 0b0100: d = 1;  break;
      // case 0b0101:         break;
      // case 0b0110:         break;
      case 0b0111: d = -1; break;
      case 0b1000: d = -1; break;
      // case 0b1001:         break;
      // case 0b1010:         break;
      case 0b1011: d = 1;  break;
      // case 0b1100:         break;
      case 0b1101: d = 1;  break;
      case 0b1110: d = -1; break;
      // case 0b1111:         break;
    }
    dialCounter += d;
    lastAB = newAB;
}

void dialInit() {
  // pinMode(gpioRotA, INPUT_PULLUP);
  // pinMode(gpioRotB, INPUT_PULLUP);
  pinMode(gpioRotButton, INPUT_PULLUP);

  // lastAB = readDial();

  // attachInterrupt(gpioRotA, dialInterrupt, CHANGE);
  // attachInterrupt(gpioRotB, dialInterrupt, CHANGE);

  ESP32Encoder::useInternalWeakPullResistors = UP;
  encoder.attachSingleEdge(gpioRotA, gpioRotB);
  encoder.setCount(0);
}

void setup() {
  Serial.begin(9600);
  i2cInit();
  lcdInit();
  mcpInit();
  dialInit();

  pinMode(gpioLED, OUTPUT);
  digitalWrite(gpioLED, LOW);

  readFootprint(lastFootprint);
  lcdPrepare();
  printFootprint(lastFootprint);
  lcdDraw();
}

void waitForInterrupt() {
  for (uint8_t n = 0; n < mcpSquaresUnits; n++) {
    mcpSquares[n].getLastInterruptPinValue();
  }
  while (!awakenByInterrupt) { delay(10); }
}

void clearInterrupt() {
  awakenByInterrupt = false;
}

void loop() {
  lcdPrepare();

  char buffer[20];
  itoa(getDialCounter(), buffer, 10);
  Serial.println(buffer);
  lcdPrint(buffer);

  waitForInterrupt();

  Footprint fp;
  copyFootprint(lastFootprint, fp);

  unsigned long lastChange = micros();
  while (micros() - lastChange < bouncingClearance) {
    if (readFootprint(fp)) {
      lastChange = micros();
    }
  }
  clearInterrupt();

  // TODO: process the move

  copyFootprint(fp, lastFootprint);
  printFootprint(lastFootprint);

  lcdDraw();
}
