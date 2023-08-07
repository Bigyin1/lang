/** @file */
#pragma once

#include <stddef.h>

#include <iostream>
#include <map>
#include <vector>

#include "ir_comp/instructions.hpp"
#include "ir_comp/register.hpp"

namespace IR
{

struct BasicBlock
{
    BasicBlock(BasicBlockName name) : name(name), instrs() {}

    BasicBlockName name;

    std::vector<std::unique_ptr<Instruction>> instrs;

    void print(std::ostream& out) const
    {
        out << name << ":" << std::endl;

        for (auto&& i : instrs)
        {
            out << "\t";
            i.get()->str(out);
        }
    }
};

struct FuncParam
{
    RegName  name;
    DataType type;
};

struct FuncDef
{
    FuncName name;

    std::vector<IR::RegPtr> params;

    DataType retType;
};

struct Function
{

    FuncDef def;

    std::vector<std::unique_ptr<BasicBlock>> blocks;

    void print(std::ostream& out) const
    {
        out << def.name << ":" << std::endl;

        for (auto&& i : blocks)
        {
            out << "  ";
            i.get()->print(out);
        }
    }
};

} // namespace IR
