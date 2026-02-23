#pragma once

// System includes
#include <cmath>
#include <iostream>

// No OpenGL headers
#define GLFW_INCLUDE_NONE

// GLFW/GLAD includes
#include <GLFW/glfw3.h>
#include <glad/glad.h>

// State
#include "State.hpp"

class Component {
  public:
    virtual ~Component() = default;
    virtual void init(State &) = 0;
    virtual void update(State &) = 0;
};