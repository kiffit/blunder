#pragma once
#include "Component.hpp"

class RenderComponent : Component {
  public:
    void init(State &) override;
    void update(State &) override;
};