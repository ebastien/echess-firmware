#include <Arduino.h>
#include <Wire.h>
#include "Adafruit_MCP23017.h"

const uint8_t pin_led = LED_BUILTIN;
const uint8_t pin_int = 2;
const uint8_t pin_button = 0;
const uint8_t mcp_num = 2;
const uint8_t mcp_gpios = 16;
const uint8_t board_size = mcp_num * mcp_gpios + 1;

char lastBoard[board_size];

Adafruit_MCP23017 mcp[mcp_num];

volatile bool awakenByInterrupt = false;

void callback() {
  awakenByInterrupt = true;
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
  Serial.print(board[0]);
  Serial.print(board[mcp_gpios]);
  Serial.println();
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

  pinMode(pin_led, OUTPUT);
  pinMode(pin_int, INPUT);

  for (uint8_t n = 0; n < mcp_num; n++) {
    mcp[n].begin(n);
    mcp[n].setupInterrupts(true, false, LOW);
    mcp[n].pinMode(pin_button, INPUT);
    mcp[n].pullUp(pin_button, HIGH);
    mcp[n].setupInterruptPin(pin_button, CHANGE);
  }

  digitalWrite(pin_led, LOW);

  readBoard(lastBoard);
  printBoard(lastBoard);
}

void loop() {
  attachInterrupt(digitalPinToInterrupt(pin_int), callback, RISING);

  clearMCPInterrupt();

  Serial.print("...");
  while (!awakenByInterrupt);
  Serial.println();

  detachInterrupt(digitalPinToInterrupt(pin_int));

  clearMCUInterrupt();

  if (readBoard(lastBoard)) {
    printBoard(lastBoard);
  }
}
