#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_MCP23017.h>
#include <U8g2lib.h>

const uint8_t gpio_sda = 21;
const uint8_t gpio_scl = 22;
const uint8_t gpio_led = 32;
const uint8_t gpio_int = 33;
const uint8_t mcp_gpio_switch = 0;
const uint8_t mcp_num = 5;
const uint8_t mcp_gpios = 16;
const uint8_t board_size = mcp_num * mcp_gpios + 1;

char lastBoard[board_size];

Adafruit_MCP23017 mcp[mcp_num];

U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE);

volatile bool awakenByInterrupt = false;

void callback() {
  awakenByInterrupt = true;
}

void lcdInit() {
  u8g2.begin();
  u8g2.clearBuffer();
  u8g2.sendBuffer();
}

void lcdStatus(const char *msg) {
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_ncenB08_tr);
  u8g2.drawStr(0, 10, msg);
  u8g2.sendBuffer();
}

bool readBoard(char *board) {
  bool has_changed = false;
  for (uint8_t n = 0; n < mcp_num; n++) {
    for (uint8_t p = 0; p < mcp_gpios; p++) {
      uint8_t b = mcp[n].digitalRead(p);
      char new_square = b ? '_' : 'X';
      char *square = &board[n * mcp_gpios + p];
      if (new_square != *square) {
        has_changed = true;
        *square = new_square;
      }
    }
  }
  board[board_size-1] = '\0';
  return has_changed;
}

void printBoard(char *board) {
  char msg[3];
  msg[0] = board[0];
  msg[1] = board[mcp_gpios];
  msg[2] = '\0';
  lcdStatus(msg);
}

void clearMCPInterrupt() {
  for (uint8_t n = 0; n < mcp_num; n++) {
    mcp[n].getLastInterruptPinValue();
  }
}

void clearMCUInterrupt() {
  awakenByInterrupt = false;
}

void i2cInit()
{
  Wire.begin(gpio_sda, gpio_scl);

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
  pinMode(gpio_int, INPUT);
  for (uint8_t n = 0; n < mcp_num; n++) {
    mcp[n].begin(n);
    mcp[n].setupInterrupts(true, false, HIGH);
    for (uint8_t p = 0; p < mcp_gpios; p++) {
      mcp[n].pinMode(p, INPUT);
      mcp[n].pullUp(p, HIGH);
      mcp[n].setupInterruptPin(p, CHANGE);
    }
  }
}

void setup() {
  Serial.begin(9600);
  i2cInit();
  lcdInit();
  mcpInit();

  pinMode(gpio_led, OUTPUT);
  digitalWrite(gpio_led, LOW);

  lcdStatus("Initialized");

  readBoard(lastBoard);
  printBoard(lastBoard);
}

void waitForInterrupt() {
  attachInterrupt(digitalPinToInterrupt(gpio_int), callback, RISING);
  clearMCPInterrupt();
  while (!awakenByInterrupt); // TODO: low energy sleep instead
  detachInterrupt(digitalPinToInterrupt(gpio_int));
  clearMCUInterrupt();
}

void loop() {
  waitForInterrupt();
  Serial.println("interrup!");
  if (readBoard(lastBoard)) {
    printBoard(lastBoard);
  }
}
