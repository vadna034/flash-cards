#pragma once

#include <cstdint>
#include <optional>
#include <string>

struct Collection {
  int64_t id;
  std::string name;
  std::string node_type;
  std::optional<int64_t> parent_id;
};


