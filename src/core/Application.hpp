#pragma once
#include "InputComponent.hpp"
#include "RenderComponent.hpp"

class Application {
  public:
    void run();

  private:
    State *state{nullptr};
    InputComponent *input{nullptr};
    RenderComponent *render{nullptr};

    void init();
    void cleanUp();
};