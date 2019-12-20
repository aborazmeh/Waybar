#include "modules/sway/keyboard.hpp"
#include <spdlog/spdlog.h>

namespace waybar::modules::sway {

Keyboard::Keyboard(const std::string& id, const Json::Value& config)
    : ALabel(config, "keyboard", id, "{layout}", 0, true) {
  ipc_.subscribe(R"(["input"])");
  ipc_.signal_event.connect(sigc::mem_fun(*this, &Keyboard::onEvent));
  // Launch worker
  worker();
  dp.emit();
}

void Keyboard::onEvent(const struct Ipc::ipc_response& res) {
  try {
    std::lock_guard<std::mutex> lock(mutex_);
    auto                        payload = parser_.parse(res.payload);
    if (payload["pango_markup"].asBool()) {
      keyboard_ = payload["input"]["xkb_active_layout_name"].asString();
    } else {
      keyboard_ = Glib::Markup::escape_text(payload["input"]["xkb_active_layout_name"].asString());
    }
    dp.emit();
  } catch (const std::exception& e) {
    spdlog::error("Keyboard: {}", e.what());
  }
}

void Keyboard::worker() {
  thread_ = [this] {
    try {
      ipc_.handleEvent();
    } catch (const std::exception& e) {
      spdlog::error("Keyboard: {}", e.what());
    }
  };
}

auto Keyboard::update() -> void {
  label_.set_markup(fmt::format(format_, fmt::arg("layout", keyboard_)));
  if (tooltipEnabled()) {
    label_.set_tooltip_text(keyboard_);
  }
  event_box_.show();
}

}  // namespace waybar::modules::sway
