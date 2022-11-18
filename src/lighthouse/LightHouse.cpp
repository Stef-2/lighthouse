#include "LightHouse.hpp"

int main()
{
    lh::Window window(lh::Window::common_resolutions.at(lh::Window::CommonResolutions::Default_windowed));

    lh::Output::log() << "wtf"
                      << "omfg";

    std::cout << lh::Output::log().get_last_line();
    // std::abort();
    while (true);
    return 0;
}
