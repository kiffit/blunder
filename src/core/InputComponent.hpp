#pragma once
#include "Component.hpp"

class InputComponent : Component {
  public:
    void init(State &) override;
    void update(State &) override;
};