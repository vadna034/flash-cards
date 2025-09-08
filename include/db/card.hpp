#pragma once

#include <string>

struct Card {
  int64_t id;
  int64_t collection_id;
  std::string front;
  std::string back;
};


