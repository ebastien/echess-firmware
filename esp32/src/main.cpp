#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_MCP23017.h>
#include <U8g2lib.h>

const uint8_t gpioSDA = 21;
const uint8_t gpioSCL = 22;
const uint8_t gpioLED = 32;
const uint8_t gpioInterrupt = 33;
const uint8_t mcpSquaresUnits = 4;
const uint8_t mcpGPIOs = 16;
const uint8_t axisSquares = 8;
const uint8_t boardSize = axisSquares * axisSquares;

uint8_t lastBoard[boardSize];

Adafruit_MCP23017 mcpSquares[mcpSquaresUnits];
Adafruit_MCP23017 mcpLEDs;

U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE);

volatile bool awakenByInterrupt = false;

void interruptCallback() {
  awakenByInterrupt = true;
}

void lcdInit() {
  u8g2.begin();
  u8g2.clearBuffer();
  u8g2.sendBuffer();
}

void lcdPrint(const char *msg) {
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_ncenB08_tr);
  u8g2.drawStr(0, 10, msg);
  u8g2.sendBuffer();
}

bool readBoard(uint8_t *board) {
  bool hasChanged = false;
  for (uint8_t n = 0; n < mcpSquaresUnits; n++) {
    const uint16_t gpios = mcpSquares[n].readGPIOAB();
    for (uint8_t p = 0; p < mcpGPIOs; p++) {
      uint8_t newSquare = gpios & (1 << p) ? 0 : 1;
      uint8_t *lastSquare = board + n * mcpGPIOs + p;
      if (newSquare != *lastSquare) {
        hasChanged = true;
        *lastSquare = newSquare;
      }
    }
  }
  return hasChanged;
}

void printBoard(uint8_t *board) {

  const u8g2_uint_t squareDim = 8;
  const u8g2_uint_t boardDim = axisSquares * squareDim;

  u8g2.clearBuffer();

  for (uint8_t y = 0; y < axisSquares; y++) {
    for (uint8_t x = 0; x < axisSquares; x++) {
      const u8g2_uint_t xs = x * squareDim;
      const u8g2_uint_t ys = boardDim - (y + 1) * squareDim;
      const uint8_t bgColor = (x + y) % 2;
      const uint8_t fgColor = (bgColor + 1) % 2;
      const uint8_t square = board[y * axisSquares + x];

      u8g2.setDrawColor(bgColor);
      u8g2.drawBox(xs, ys, squareDim, squareDim);
      if (square == 1) {
        u8g2.setDrawColor(fgColor);
        u8g2.drawDisc(xs + squareDim/2, ys + squareDim/2, squareDim/4);
      }
    }
  }

  u8g2.sendBuffer();
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
}

void setup() {
  Serial.begin(9600);
  i2cInit();
  lcdInit();
  mcpInit();

  pinMode(gpioLED, OUTPUT);
  digitalWrite(gpioLED, LOW);

  lcdPrint("Initialized");

  readBoard(lastBoard);
  printBoard(lastBoard);

  attachInterrupt(digitalPinToInterrupt(gpioInterrupt), interruptCallback, RISING);
}

void waitForInterrupt() {
  for (uint8_t n = 0; n < mcpSquaresUnits; n++) {
    mcpSquares[n].getLastInterruptPinValue();
  }
  while (!awakenByInterrupt);
}

void loop() {
  Serial.println("waiting for interrupt...");
  waitForInterrupt();
  Serial.println("interrupt!");

  const unsigned long clearance = 500000; // 500ms
  unsigned long lastChange = micros();
  while (micros() - lastChange < clearance) {
    if (readBoard(lastBoard)) {
      lastChange = micros();
    }
  }

  awakenByInterrupt = false;

  printBoard(lastBoard);
}
