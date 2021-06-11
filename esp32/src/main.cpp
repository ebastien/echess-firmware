
#include "main.h"

#include <array>

using namespace echess;

void Main::fatal(const char* msg) {
  unsigned long tick = 0;
  buzzer_.error();
  pointer_.flash();
  dial_.init(0);
  do {
    display_.prepare();
    switch (tick % 3) {
      case 0 : display_.print(msg); break;
      case 1 : display_.print("press button"); break;
      case 2 : display_.print("to reset");
    }
    display_.draw();
    ++tick;
  }
  while (!dial_.wait(1000));
  reset();
}

void Main::reset() {
  ESP.restart();
}

void Main::setup() {
  Serial.begin(9600);
  delay(1000);
  splash();

  if (!storage_.initialize()) {
    fatal("no SD");
  }

  remote_.connect(storage_.ssid(), storage_.password());

  lichess_.setToken(storage_.lichessKey());
  lichess_.findGame();
  while (!lichess_.isGamePlaying()) {
    display_.prepare();
    display_.print("waiting for game");
    display_.draw();
    delay(1000);
    lichess_.findGame();
  }
  const auto state = lichess_.waitGameState();
  if (!state || state->initial().empty()) {
    fatal("remote error");
  }
  Board board(state->initial());
  if (!board.fromMoves(state->moves())) {
    fatal("remote error");
  }
  m_.reset(board);

  Footprint fp;
  scanner_.read(fp);
  m_.transition(fp);

  redraw();

  buzzer_.beep();
}

void Main::splash() {
  buzzer_.beep();
  display_.prepare();
  display_.print("starting ...");
  display_.draw();
  pointer_.flash();
}

void Main::notify(bool error) {
  if (error) {
    buzzer_.error();
  } else {
    buzzer_.beep();
  }
  pointer_.flash();
}

void Main::redraw() {

  const auto lastMove = m_.board().lastMove();

  display_.prepare();

  if (lastMove) {
    if (tick_ % 2 == 0) {
      pointer_.point(lastMove->from());
    } else {
      pointer_.point(lastMove->to());
    }
  } else {
    pointer_.clear();
  }

  if (tick_ % 4 == 0) {
    display_.print(m_.explain());
  } else {
    display_.print(m_.footprint());
    display_.print(m_.board());
  }

  display_.draw();
}

UCIMove::Promotion Main::askPromotion() {

  static const std::array<UCIMove::Promotion, 4> pieces(
    {UCIMove::queen, UCIMove::rook, UCIMove::bishop, UCIMove::knight}
  );

  UCIMove::Promotion choice(pieces[0]);

  dial_.init(pieces.size() - 1);
  do {
    choice = pieces[dial_.position()];
    display_.prepare();
    display_.print(choice);
    display_.draw();
  } while (!dial_.wait(1000));

  display_.prepare();
  display_.draw();
  notify();

  return choice;
}

void Main::loop() {

  const int nbMoves(m_.board().length());

  if (m_.isReady() && m_.board().player() != lichess_.player()) {

    const auto state = lichess_.waitGameState(nbMoves);

    if (!state) {
      fatal("remote error");
    }
    if (!m_.transition(state->moves())) {
      fatal("remote error");
    }

    notify();

  } else {

    Footprint fp(m_.footprint());

    if (scanner_.wait(fp, 1000)) {
      if (!m_.transition(fp)) {
        fatal("local error");
      }

      notify(m_.isError());

      if (m_.isPromotion()) {
        const auto p = askPromotion();
        if (!m_.transition(p)) {
          fatal("local error");
        }
      }
    }

    if (m_.board().length() > nbMoves) {
      lichess_.makeMove(m_.board().lastMove()->uci());
    }
  }

  redraw();

  // Serial.print("Free heap (kB): "); Serial.println(ESP.getFreeHeap() / 1024);

  ++tick_;
}

void setup() {
  echess::Main::getInstance().setup();
}

void loop() {
  echess::Main::getInstance().loop();
}
