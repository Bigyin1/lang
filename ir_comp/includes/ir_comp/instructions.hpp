/** @file */
#pragma once

#include <stddef.h>

#include <iostream>
#include <memory>
#include <vector>

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

    static std::string DataTypeToStr(DataType dt)
    {

        switch (dt)
        {
            case DataType::I1:
                return "i1";

            case DataType::I64:
                return "i64";

            case DataType::F64:
                return "f64";

            case DataType::Void:
                return "void";

            default:
                return "";
        }
    }

protected:
    DataType type;
};

class RegArg : public InstrArg
{
public:
    RegArg(IR::RegPtr reg) : InstrArg(reg->type), regArg(reg) {}

    std::shared_ptr<const Register> GetReg() { return this->regArg; }

    std::string str() const override
    {
        return this->DataTypeToStr(type) + " " + regArg.get()->str();
    }

    ~RegArg() = default;

private:
    std::shared_ptr<const Register> regArg;
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

    std::string str() const override
    {
        switch (this->type)
        {
            case DataType::I1:
                return this->DataTypeToStr(type) + " " + std::to_string(this->i1);
            case DataType::I64:
                return this->DataTypeToStr(type) + " " + std::to_string(this->i64);

            case DataType::F64:
                return this->DataTypeToStr(type) + " " + std::to_string(this->f64);

            case DataType::Void:
            default:
                break;
        }

        return "";
    }

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

    virtual void str(std::ostream& out) const override
    {

        if (this->regRes != nullptr)
        {
            out << this->regRes.get()->str() << " = ";
        }

        switch (this->op)
        {
            case Opcode::BAD:
                out << "bad";
                break;
            case Opcode::SUB:
                out << "sub";
                break;
            case Opcode::ADD:
                out << "add";
                break;
            case Opcode::MUL:
                out << "mul";
                break;
            case Opcode::DIV:
                out << "div";
                break;
            case Opcode::NEQ:
                out << "neq";
                break;
            case Opcode::EQ:
                out << "equ";
                break;
            case Opcode::GT:
                out << "gt";
                break;
            case Opcode::GE:
                out << "ge";
                break;
            case Opcode::LT:
                out << "lt";
                break;
            case Opcode::LE:
                out << "le";
                break;
            case Opcode::NEG:
                out << "neg";
                break;
            case Opcode::RET:
                out << "ret";
                break;
            case Opcode::STORE:
                break;
            default:
                break;
        }

        out << " ";
        if (this->arg1 != nullptr)
            out << arg1.get()->str();

        if (this->arg2 != nullptr)
        {
            out << ", ";
            out << arg2.get()->str();
        }

        out << std::endl;
    }

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

    BranchInstr(BasicBlockName p) : ifTrue(p) {}

    virtual void str(std::ostream& out) const override
    {
        if (this->cond == nullptr)
        {
            out << "br "
                << "label "
                << "%" + ifTrue << std::endl;

            return;
        }

        out << "br"
            << " " << cond.get()->str() << ", "
            << "label "
            << "%" + ifTrue << ", "
            << "label "
            << "%" + ifFalse << std::endl;
    }

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

    virtual void str(std::ostream& out) const override
    {

        if (this->res.get()->type != IR::DataType::Void)
        {
            out << this->res.get()->str() << " = ";
        }

        out << "call " << InstrArg::DataTypeToStr(res.get()->type) << " " << name << "(";
        for (auto&& i : args)
        {
            out << i.get()->str() << ", ";
        }
        out << ")" << std::endl;
    }

private:
    FuncName name;

    std::vector<std::unique_ptr<InstrArg>> args;

    IR::RegPtr res;
};

class RetInstr : public Instruction
{
public:
    RetInstr(std::unique_ptr<InstrArg> r) : retVal(std::move(r)) {}

    virtual void str(std::ostream& out) const override
    {
        out << "ret";
        if (retVal == nullptr)
        {
            out << " " << InstrArg::DataTypeToStr(IR::DataType::Void) << std::endl;
            return;
        }

        out << " " << retVal.get()->str() << std::endl;
    }

private:
    std::unique_ptr<InstrArg> retVal;
};

} // namespace IR
