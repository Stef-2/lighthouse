#include "lighthouse.hpp"

int main()
{
    auto engine = lh::engine {std::pair{320u, 200u}};

    engine.run();
}
