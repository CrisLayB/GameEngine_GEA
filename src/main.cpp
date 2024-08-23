//#include "Games/BreakOut.h"
//#include "DemoGame/DemoGame.h"
#include "Games/SamuraisVsMongols.h"

int main() {
  //BreakOut demo = BreakOut();
  //DemoGame demo = DemoGame();
  SamuraisVsMongols demo = SamuraisVsMongols();
  demo.setup();
  demo.run();
  return 0;
}
