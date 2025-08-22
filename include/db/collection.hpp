#pragma once

#include <optional>
#include <string>

struct Collection {
  std::string id;
  std::string name;
  std::string node_type;
  std::optional<std::string> parent_id;
};


