//#include "Games/BreakOut.h"
#include "DemoGame/DemoGame.h"

int main() {
  //BreakOut demo = BreakOut();
  DemoGame demo = DemoGame();
  demo.setup();
  demo.run();
  return 0;
}
