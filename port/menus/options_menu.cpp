#include "options_menu.h"

#include "../input.h"

int OptionsMenu::ProcessEvent() {
    const bool had_input = in.anyKeyPressed();

    MenuType next = MenuType(active_menu_->ProcessEvent());
    if (had_input) in.waitClean();
    switch (next) {
        case MenuType::Main:
            return MenuType::Main;
        case MenuType::Options:
            active_menu_ = &options_menu_;
            break;
        case MenuType::Keys_1:
            active_menu_ = &p1_menu_;
            break;
        case MenuType::Keys_2:
            active_menu_ = &p2_menu_;
            break;
    }
    return MenuType::Options;
}
