#include "lighthouse.hpp"

int main()
{
    auto engine = lh::engine {std::make_unique<lh::window>(std::pair{640, 320})};

    engine.run();
}
