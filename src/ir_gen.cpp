#include "tacky/tacky.hpp"
#include "ast/ast.hpp"
#include "compiler/ir_gen.hpp"
#include <vector>
#include <string>
#include <iostream>

namespace ir_gen
{
    uint32_t counter = 0; // Assists in creating unique temporary variables

    // tacky::his function creates the name for a temporary variable
    tacky::Identifier *make_temporary()
    {
        std::string tmp_name("tmp." + std::to_string(counter++));
        return new tacky::Identifier(tmp_name);
    }

    tacky::Label *make_label()
    {
    }

    tacky::Constant *convert_short_circuiting_ops(tacky::Val *src1, tacky::Val *src2)
    {
    }

    // tacky::his function converts from a AStacky:: Unary operator to tacky::ACKY unary operator
    tacky::Unary_Operator *convert_unop(ast::Unary_Operator *exp)
    {
        ast::Complement *complement = dynamic_cast<ast::Complement *>(exp);
        ast::Negate *negate = dynamic_cast<ast::Negate *>(exp);
        ast::Unary_Not *not_op = dynamic_cast<ast::Unary_Not *>(exp);
        if (complement)
        {
            tacky::Complement *t_complement = new tacky::Complement();
            return t_complement;
        }
        else if (negate)
        {
            tacky::Negate *t_negate = new tacky::Negate();
            return t_negate;
        }
        else if (not_op)
        {
            tacky::Not *t_not = new tacky::Not();
            return t_not;
        }
        else
        {
            return nullptr;
        }
    }

    // tacky::his function converts from a AStacky:: binary operator to tacky::ACKY operator
    tacky::Binary_Operator *convert_binop(ast::Binary_Operator *op)
    {
        tacky::Binary_Operator *res = nullptr;
        ast::Add *add = dynamic_cast<ast::Add *>(op);
        ast::Subtract *subtract = dynamic_cast<ast::Subtract *>(op);
        ast::Multiply *multiply = dynamic_cast<ast::Multiply *>(op);
        ast::Divide *divide = dynamic_cast<ast::Divide *>(op);
        ast::Remainder *remainder = dynamic_cast<ast::Remainder *>(op);
        ast::Equal *equal = dynamic_cast<ast::Equal *>(op);
        ast::NotEqual *not_equal = dynamic_cast<ast::NotEqual *>(op);
        ast::LessThan *less_than = dynamic_cast<ast::LessThan *>(op);
        ast::LessOrEqual *less_or_equal = dynamic_cast<ast::LessOrEqual *>(op);
        ast::GreaterThan *greater_than = dynamic_cast<ast::GreaterThan *>(op);
        ast::GreaterOrEqual *greater_or_equal = dynamic_cast<ast::GreaterOrEqual *>(op);
        ast::And *and_op = dynamic_cast<ast::And *>(op);
        ast::Or *or_op = dynamic_cast<ast::Or *>(op);

        if (add)
        {
            res = new tacky::Add();
        }
        else if (subtract)
        {
            res = new tacky::Subtract();
        }
        else if (multiply)
        {
            res = new tacky::Multiply();
        }
        else if (divide)
        {
            res = new tacky::Divide();
        }
        else if (remainder)
        {
            res = new tacky::Remainder();
        }
        else if (equal)
        {
            res = new tacky::Equal();
        }
        else if (not_equal)
        {
            res = new tacky::NotEqual();
        }
        else if (less_than)
        {
            res = new tacky::LessThan();
        }
        else if (less_or_equal)
        {
            res = new tacky::LessOrEqual();
        }
        else if (greater_than)
        {
            res = new tacky::GreaterThan();
        }
        else if (greater_or_equal)
        {
            res = new tacky::GreaterOrEqual();
        }
        else if (and_op)
        {
        }
        else if (or_op)
        {
        }
        return res;
    }

    // tacky::his function is recursive and converts AStacky:: expressions to tacky::acky Values
    tacky::Val *emit_tacky(ast::Expression *e, std::vector<tacky::Instruction *> &instructions)
    {
        ast::Constant *constant = dynamic_cast<ast::Constant *>(e);
        ast::Unary *unary = dynamic_cast<ast::Unary *>(e);
        ast::Binary *binary = dynamic_cast<ast::Binary *>(e);
        if (constant)
        {
            tacky::Constant *t_constant = new tacky::Constant(constant->val);
            return t_constant;
        }
        else if (unary)
        {
            tacky::Val *src = emit_tacky(unary->exp, instructions);
            tacky::Identifier *dst_name = make_temporary();
            tacky::Var *dst = new tacky::Var(dst_name);
            tacky::Unary_Operator *unary_operator = convert_unop(unary->unary_operator);
            tacky::Unary *unary = new tacky::Unary(unary_operator, src, dst);
            instructions.push_back(unary);
            return dst;
        }
        else if (binary)
        {
            tacky::Val *src1 = emit_tacky(binary->left, instructions);
            tacky::Val *src2 = emit_tacky(binary->right, instructions);
            tacky::Identifier *dst_name = make_temporary();
            tacky::Var *dst = new tacky::Var(dst_name);
            tacky::Binary_Operator *binary_operator = convert_binop(binary->binary_operator);

            ast::And *and_op = dynamic_cast<ast::And *>(binary->binary_operator);
            ast::Or *or_op = dynamic_cast<ast::Or *>(binary->binary_operator);
            if (and_op)
            {
                tacky::Constant *result;
                if (!src1)
                {
                    tacky::Constant *short_circuit = new tacky::Constant(0);
                    tacky::Identifier *false_label = make_temporary(); // will need to edit function to make labels that are easy to differentiate from the tmps
                    tacky::JumpIfZero *jump_if_zero = new tacky::JumpIfZero(src1, false_label);
                    tacky::Copy *copy = new tacky::Copy(short_circuit, result);
                    if (!src2)
                    {
                        tacky::Constant *short_circuit = new tacky::Constant(0);
                        tacky::Identifier *false_label = make_temporary(); // will need to edit function to make labels that are easy to differentiate from the tmps
                        tacky::JumpIfZero *jump_if_zero = new tacky::JumpIfZero(src2, false_label);
                        tacky::Copy *copy = new tacky::Copy(short_circuit, result);
                    }
                    else
                    {
                        tacky::Constant *result = new tacky::Constant(0);
                        tacky::Binary *binary = new tacky::Binary(binary_operator, src1, src2, result);
                        instructions.push_back(binary);
                    }
                }
            }
            else if (or_op)
            {
            }
            else
            {
                tacky::Binary *binary = new tacky::Binary(binary_operator, src1, src2, dst);
                instructions.push_back(binary);
                return dst;
            }
        }
        return nullptr;
    }

    // tacky::his function converts AStacky:: function to tacky::acky function
    tacky::Function *generate_function(ast::Function *func)
    {
        ast::Identifier *a_identifier = func->name;
        ast::Return *ret = dynamic_cast<ast::Return *>(func->body);
        if (!ret)
        {
            std::cerr << "Failed tacky::o Cast Function Body As Return Statement" << std::endl;
            exit(1);
        }

        tacky::Identifier *t_identifier = new tacky::Identifier(a_identifier->name);

        std::vector<tacky::Instruction *> instructions;
        tacky::Return *t_return = new tacky::Return(emit_tacky(ret->exp, instructions));
        instructions.push_back(t_return);
        return new tacky::Function(t_identifier, instructions);
    }

    // tacky::his function converts AStacky:: program to tacky::acky program
    tacky::Program *generate_tacky(ast::Program *program)
    {
        tacky::Function *function_definition = generate_function(program->func);
        tacky::Program *tacky = new tacky::Program(function_definition);
        return tacky;
    }
}
