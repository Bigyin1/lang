/** @file */
#pragma once

#include <stddef.h>

#include <iostream>
#include <map>
#include <memory>
#include <vector>

#include "ir_comp/defs.hpp"
#include "ir_comp/register.hpp"
namespace IR
{

typedef std::string FuncName;

typedef std::string BasicBlockName;

class InstrArg
{

public:
    InstrArg(DataType t) : type(t) {}

    DataType GetType() { return this->type; }

    virtual std::string str() const = 0;

    virtual ~InstrArg() = default;

    static std::string DataTypeToStr(DataType dt);

protected:
    DataType type;
};

class RegArg : public InstrArg
{
public:
    RegArg(IR::RegPtr reg) : InstrArg(reg->type), regArg(reg) {}

    std::string str() const override;

    IR::RegPtr GetReg() const { return regArg; }

    ~RegArg() = default;

private:
    IR::RegPtr regArg;
};

class ImmArg : public InstrArg
{
public:
    ImmArg(double d) : InstrArg(DataType::F64), f64(d) {}
    ImmArg(int64_t i) : InstrArg(DataType::I64), i64(i) {}
    ImmArg(bool b) : InstrArg(DataType::I1), i1(b) {}

    double  F64() { return this->f64; }
    int64_t I64() { return this->i64; }
    bool    I1() { return this->i1; }

    std::string str() const override;

private:
    union
    {
        double  f64;
        int64_t i64;
        bool    i1;
    };
};

class Instruction
{
public:
    virtual void str(std::ostream& out) const = 0;

    virtual ~Instruction() = default;
};

class ThreeAddrInstr : public Instruction
{
public:
    enum class Opcode
    {
        BAD,

        SUB,
        ADD,
        MUL,
        DIV,
        NEQ,
        EQ,
        GT,
        GE,
        LT,
        LE,

        NEG,

        RET,
        STORE,

    };

    virtual void str(std::ostream& out) const override;

    ThreeAddrInstr(Opcode opc, std::unique_ptr<IR::InstrArg> a1, std::unique_ptr<IR::InstrArg> a2,
                   IR::RegPtr res)
        : op(opc), regRes(res), arg1(a1.release()), arg2(a2.release())
    {
    }

    ThreeAddrInstr(Opcode opc, std::unique_ptr<IR::InstrArg> arg, IR::RegPtr res)
        : op(opc), regRes(res), arg1(arg.release()), arg2()
    {
    }

    ThreeAddrInstr(Opcode opc, std::unique_ptr<IR::InstrArg> arg)
        : op(opc), regRes(), arg1(arg.release()), arg2()
    {
    }

    ~ThreeAddrInstr() = default;

private:
    Opcode op;

    IR::RegPtr regRes;

    std::unique_ptr<InstrArg> arg1;
    std::unique_ptr<InstrArg> arg2;
};

class BranchInstr : public Instruction
{
public:
    BranchInstr(std::unique_ptr<InstrArg> cnd, BasicBlockName t, BasicBlockName f)
        : cond(cnd.release()), ifTrue(t), ifFalse(f)
    {
    }

    BranchInstr(BasicBlockName p) : cond(nullptr), ifTrue(p), ifFalse() {}

    virtual void str(std::ostream& out) const override;

private:
    std::unique_ptr<InstrArg> cond;

    BasicBlockName ifTrue;
    BasicBlockName ifFalse;
};

class CallInstr : public Instruction
{
public:
    CallInstr(FuncName n, std::vector<std::unique_ptr<InstrArg>> a, IR::RegPtr r)
        : name(std::move(n)), args(std::move(a)), res(r)
    {
    }

    virtual void str(std::ostream& out) const override;

private:
    FuncName name;

    std::vector<std::unique_ptr<InstrArg>> args;

    IR::RegPtr res;
};

class RetInstr : public Instruction
{
public:
    RetInstr(std::unique_ptr<InstrArg> r) : retVal(std::move(r)) {}

    virtual void str(std::ostream& out) const override;

private:
    std::unique_ptr<InstrArg> retVal;
};

} // namespace IR
