#include "lighthouse.hpp"

int main()
{
    window window(lh::window::resolution.at(lh::window::common_resolutions::default_windowed));
    engine::initialize(window, vulkan_version {1, 3, 0});

    input::key_binding::bind({vkfw::Key::Escape}, [&window]() {
        window.vkfw_window().destroy();
        std::exit(0);
    });

    engine::run();

    return 0;
}
