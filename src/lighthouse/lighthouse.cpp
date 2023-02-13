#include "lighthouse.hpp"

int main()
{
    auto engine = lh::engine {std::make_unique<lh::window>(vk::Extent2D{640, 320})};
    auto b = 32_b;
    engine.run();
}
