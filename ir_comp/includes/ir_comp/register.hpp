/** @file */
#pragma once

#include <stddef.h>

#include <iostream>
#include <memory>
#include <string>

#include "ir_comp/types.hpp"

namespace IR
{

typedef std::string RegName;

struct Register
{
    Register(RegName name, DataType type) : name(name), type(type) {}

    RegName name;

    DataType type;

    std::string str() const { return "%" + name; }
};

typedef std::shared_ptr<const IR::Register> RegPtr;
} // namespace IR
