#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_MCP23017.h>
// #include <LiquidCrystal_I2C.h>

const uint8_t gpio_led = LED_BUILTIN;
const uint8_t gpio_int = 2;
const uint8_t mcp_gpio_switch = 0;
const uint8_t mcp_num = 2;
const uint8_t mcp_gpios = 16;
const uint8_t board_size = mcp_num * mcp_gpios + 1;

char lastBoard[board_size];

Adafruit_MCP23017 mcp[mcp_num];
// LiquidCrystal_I2C lcd(0x27, 16, 2);

volatile bool awakenByInterrupt = false;

void callback() {
  awakenByInterrupt = true;
}

void lcdInit() {
  // lcd.init();
  // lcd.backlight();
}

void lcdStatus(const char *msg) {
  // lcd.clear();
  // lcd.setCursor(0, 0);
  // lcd.print(msg);
  Serial.println(msg);
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
  EIFR = 0x01;
  awakenByInterrupt = false;
}

void setup() {
  Serial.begin(9600);

  lcdInit();

  lcdStatus("Initializing...");

  pinMode(gpio_led, OUTPUT);
  pinMode(gpio_int, INPUT);

  for (uint8_t n = 0; n < mcp_num; n++) {
    mcp[n].begin(n);
    mcp[n].setupInterrupts(true, false, HIGH);
    mcp[n].pinMode(mcp_gpio_switch, INPUT);
    mcp[n].pullUp(mcp_gpio_switch, HIGH);
    mcp[n].setupInterruptPin(mcp_gpio_switch, CHANGE);
  }

  digitalWrite(gpio_led, LOW);

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
  if (readBoard(lastBoard)) {
    printBoard(lastBoard);
  }
}