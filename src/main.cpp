//#include "Games/BreakOut.h"
//#include "DemoGame/DemoGame.h"
//#include "Games/Level2.h"
#include "Games/SamuraisVsMongols_Level1.h"

int main() {
  //BreakOut demo = BreakOut();
  //DemoGame demo = DemoGame();
  SamuraisVsMongols_Level1 demo = SamuraisVsMongols_Level1();
  demo.setup();
  demo.run();
  return 0;
}
