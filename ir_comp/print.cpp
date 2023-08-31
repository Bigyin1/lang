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

std::string IR::ThreeAddrInstr::opToStr(Opcode op)
{

    switch (op)
    {
        case Opcode::BAD:
            return "bad";

        case Opcode::SUB:
            return "sub";

        case Opcode::ADD:
            return "add";

        case Opcode::MUL:
            return "mul";

        case Opcode::DIV:
            return "div";

        case Opcode::NEQ:
            return "neq";

        case Opcode::EQ:
            return "equ";

        case Opcode::GT:
            return "gt";

        case Opcode::GE:
            return "ge";

        case Opcode::LT:
            return "lt";

        case Opcode::LE:
            return "le";

        case Opcode::NEG:
            return "neg";

        case Opcode::STORE:
            return "st";
        default:
            return "";
    }
}

void IR::ThreeAddrInstr::str(std::ostream& out) const
{

    if (this->regRes != nullptr)
    {
        out << this->regRes.get()->str() << " = ";
    }

    out << this->opToStr(this->op);

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

    if (this->res.get()->GetType() != IR::DataType::Void)
    {
        out << this->res.get()->str() << " = ";
    }

    out << "call " << InstrArg::DataTypeToStr(res.get()->GetType()) << " " << name << "(";
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
