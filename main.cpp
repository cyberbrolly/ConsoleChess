#include "Renderer.h"

int main() {
    Renderer renderer;
    if (!renderer.init()) return 1;
    renderer.run();
    return 0;
}