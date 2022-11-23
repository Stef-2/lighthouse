#include "lighthouse.hpp"

int main()
{
    lh::window window(lh::window::resolution.at(lh::window::common_resolutions::default_windowed));
    lh::engine::initialize(window);

    auto& kb = lh::input::key_binding::get_key_bindings();

    auto f1 = [](){std::cout << "im gonna... im gonna... execute\n ahh I'm executing, I'm executing\n";};
    lh::input::key_binding::bind({vkfw::Key::B, vkfw::ModifierKeyFlags{},vkfw::KeyAction::Press}, f1);

    lh::input::key_binding::bind({vkfw::Key::C}, [&f1]() { std::cout << "cool macro\n"; f1(); });
    lh::input::key_binding::bind({vkfw::MouseButton::_1}, [](){std::cout << "click\n";});
    lh::input::key_binding::bind({vkfw::Key::Escape}, [&window](){window.vkfw_window().destroy(); std::exit(0);});
    std::cout << lh::output::log();
    lh::engine::run();

    return 0;
}
