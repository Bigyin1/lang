#include "ir_comp/instructions.hpp"

std::string IR::InstrArg::DataTypeToStr(DataType dt)
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

std::string IR::RegArg::str() const
{
    return this->DataTypeToStr(type) + " " + regArg.get()->str();
}

std::string IR::ImmArg::str() const
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

void IR::ThreeAddrInstr::str(std::ostream& out) const
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

void IR::BranchInstr::str(std::ostream& out) const
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

void IR::CallInstr::str(std::ostream& out) const
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

void IR::RetInstr::str(std::ostream& out) const
{
    out << "ret";
    if (retVal == nullptr)
    {
        out << " " << InstrArg::DataTypeToStr(IR::DataType::Void) << std::endl;
        return;
    }

    out << " " << retVal.get()->str() << std::endl;
}
