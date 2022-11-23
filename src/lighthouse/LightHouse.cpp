#include "lighthouse.hpp"

int main()
{
    lh::window window(lh::window::resolution.at(lh::window::common_resolutions::default_windowed));


    auto& kb = lh::input::key_binding::get_key_bindings();

    auto f1 = [](){std::cout << "im gonna... im gonna... execute\n ahh I'm executing, I'm executing\n";};
    lh::input::key_binding::bind({vkfw::Key::B}, f1);
    lh::input::key_binding::bind({vkfw::Key::C}, [&f1]() { std::cout << "cool macro\n"; f1(); });
    lh::input::key_binding::bind({vkfw::Key::Escape}, [&window](){window.vkfw_window().destroy(); std::exit(0);});



    window.vkfw_window().callbacks()->on_key = [&kb](vkfw::Window const&, vkfw::Key key, int32_t wtf, vkfw::KeyAction action,
                                      vkfw::ModifierKeyFlags mfs)
    {
        std::cout << "action: " << vkfw::to_string(action) << " key: " << vkfw::to_string(key) << " wtf: " << wtf << " mfs: "
                  << (int(mfs)) << ".\n";

        


        for (auto it = kb.equal_range({key}).first; it != kb.equal_range({key}).second; ++it)
            it->second();
    };

    // std::abort();
    while (true) vkfw::pollEvents();
    return 0;
}
