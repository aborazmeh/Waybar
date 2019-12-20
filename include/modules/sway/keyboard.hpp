#pragma once

#include <fmt/format.h>
#include "ALabel.hpp"
#include "bar.hpp"
#include "client.hpp"
#include "modules/sway/ipc/client.hpp"
#include "util/json.hpp"
#include "util/sleeper_thread.hpp"

namespace waybar::modules::sway {

class Keyboard: public ALabel, public sigc::trackable {
 public:
  Keyboard(const std::string&, const Json::Value&);
  ~Keyboard() = default;
  auto update() -> void;

 private:
  void onEvent(const struct Ipc::ipc_response&);
  void worker();
  void getTree();

  std::string      keyboard_;
  util::JsonParser parser_;
  std::mutex       mutex_;

  util::SleeperThread thread_;
  Ipc                 ipc_;
};

}  // namespace waybar::modules::sway
