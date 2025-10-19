#include "cpsq.h"
#include "render/cpsq.render.h"

namespace cpsq {
     int main() {
          cpsq::render::create();
          return 0;
     }

}

int main(int argc, char* argv[]) {
     cpsq::main();
}