#include "tacky/tacky.hpp"
#include "ast/ast.hpp"
#include "compiler/ir_gen.hpp"
#include <vector>
#include <string>
#include <iostream>

namespace ir_gen
{
    uint32_t id_counter = 0;    // Assists in creating unique temporary variables
    uint32_t label_counter = 0; // Assists in creating unique Labels

    // This function creates the name for a temporary variable
    tacky::Identifier *make_identifier()
    {
        std::string tmp_name("tmp." + std::to_string(id_counter++));
        return new tacky::Identifier(tmp_name);
    }

    // This function converts from a AST Unary operator to TACKY unary operator
    tacky::Unary_Operator *convert_unop(ast::Unary_Operator *exp)
    {
        ast::Complement *complement = dynamic_cast<ast::Complement *>(exp);
        ast::Negate *negate = dynamic_cast<ast::Negate *>(exp);
        ast::Unary_Not *not_op = dynamic_cast<ast::Unary_Not *>(exp);
        tacky::Unary_Operator *res = nullptr;
        if (complement)
        {
            res = new tacky::Complement();
        }
        else if (negate)
        {
            res = new tacky::Negate();
        }
        else if (not_op)
        {
            res = new tacky::Not();
        }

        return res;
    }

    // This function converts from a AST binary operator to TACKY operator
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
        return res;
    }

    // This function is recursive and converts AST expressions to Tacky Values
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
            tacky::Identifier *dst_name = make_identifier();
            tacky::Var *dst = new tacky::Var(dst_name);
            tacky::Unary_Operator *unary_operator = convert_unop(unary->unary_operator);
            tacky::Unary *new_unary = new tacky::Unary(unary_operator, src, dst);
            instructions.push_back(new_unary);
            return dst;
        }
        else if (binary && dynamic_cast<ast::And *>(binary->binary_operator))
        {
            tacky::Val *left = emit_tacky(binary->left, instructions);
            tacky::Identifier *left_false_identifier = new tacky::Identifier("false" + std::to_string(label_counter++));
            tacky::JumpIfZero *left_jmp_if_zero = new tacky::JumpIfZero(left, left_false_identifier);
            instructions.push_back(left_jmp_if_zero);

            tacky::Val *right = emit_tacky(binary->right, instructions);
            tacky::Identifier *right_false_identifier = new tacky::Identifier(left_false_identifier->name);
            tacky::JumpIfZero *right_jmp_if_zero = new tacky::JumpIfZero(right, right_false_identifier);
            instructions.push_back(right_jmp_if_zero);

            tacky::Constant *const_one = new tacky::Constant(1);
            tacky::Identifier *result_identifier_one = make_identifier();
            tacky::Var *result_one = new tacky::Var(result_identifier_one);
            tacky::Copy *copy_one = new tacky::Copy(const_one, result_one);
            instructions.push_back(copy_one);

            tacky::Identifier *end_jmp_identifier = new tacky::Identifier("end" + std::to_string(label_counter++));
            tacky::Jump *end_jmp = new tacky::Jump(end_jmp_identifier);
            instructions.push_back(end_jmp);

            tacky::Identifier *false_identifier = new tacky::Identifier(right_false_identifier->name);
            tacky::Label *false_label = new tacky::Label(false_identifier);
            instructions.push_back(false_label);

            tacky::Constant *const_zero = new tacky::Constant(0);
            tacky::Identifier *result_identifier_zero = new tacky::Identifier(result_identifier_one->name);
            tacky::Var *result_zero = new tacky::Var(result_identifier_zero);
            tacky::Copy *copy_zero = new tacky::Copy(const_zero, result_zero);
            instructions.push_back(copy_zero);

            tacky::Identifier *end_label_identifier = new tacky::Identifier(end_jmp_identifier->name);
            tacky::Label *end_label = new tacky::Label(end_label_identifier);
            instructions.push_back(end_label);

            tacky::Identifier *dst_name = new tacky::Identifier(result_identifier_zero->name);
            tacky::Var *dst = new tacky::Var(dst_name);
            return dst;
        }
        else if (binary && dynamic_cast<ast::Or *>(binary->binary_operator))
        {
            tacky::Val *left = emit_tacky(binary->left, instructions);
            tacky::Identifier *left_true_identifier = new tacky::Identifier("true" + std::to_string(label_counter++));
            tacky::JumpIfNotZero *left_jmp_if_not_zero = new tacky::JumpIfNotZero(left, left_true_identifier);
            instructions.push_back(left_jmp_if_not_zero);

            tacky::Val *right = emit_tacky(binary->right, instructions);
            tacky::Identifier *right_true_identifier = new tacky::Identifier(left_true_identifier->name);
            tacky::JumpIfNotZero *right_jmp_if_not_zero = new tacky::JumpIfNotZero(right, right_true_identifier);
            instructions.push_back(right_jmp_if_not_zero);

            tacky::Constant *const_zero = new tacky::Constant(0);
            tacky::Identifier *result_identifier_zero = make_identifier();
            tacky::Var *result_zero = new tacky::Var(result_identifier_zero);
            tacky::Copy *copy_zero = new tacky::Copy(const_zero, result_zero);
            instructions.push_back(copy_zero);

            tacky::Identifier *end_jmp_identifier = new tacky::Identifier("end" + std::to_string(label_counter++));
            tacky::Jump *end_jmp = new tacky::Jump(end_jmp_identifier);
            instructions.push_back(end_jmp);

            tacky::Identifier *true_identifier = new tacky::Identifier(right_true_identifier->name);
            tacky::Label *true_label = new tacky::Label(true_identifier);
            instructions.push_back(true_label);

            tacky::Constant *const_one = new tacky::Constant(1);
            tacky::Identifier *result_identifier_one = new tacky::Identifier(result_identifier_zero->name);
            tacky::Var *result_one = new tacky::Var(result_identifier_one);
            tacky::Copy *copy_one = new tacky::Copy(const_one, result_one);
            instructions.push_back(copy_one);

            tacky::Identifier *end_label_identifier = new tacky::Identifier(end_jmp_identifier->name);
            tacky::Label *end_label = new tacky::Label(end_label_identifier);
            instructions.push_back(end_label);

            tacky::Identifier *dst_name = new tacky::Identifier(result_identifier_one->name);
            tacky::Var *dst = new tacky::Var(dst_name);
            return dst;
        }
        else if (binary)
        {
            tacky::Val *src1 = emit_tacky(binary->left, instructions);
            tacky::Val *src2 = emit_tacky(binary->right, instructions);
            tacky::Identifier *dst_name = make_identifier();
            tacky::Var *dst = new tacky::Var(dst_name);
            tacky::Binary_Operator *binary_operator = convert_binop(binary->binary_operator);
            tacky::Binary *new_binary = new tacky::Binary(binary_operator, src1, src2, dst);
            instructions.push_back(new_binary);
            return dst;
        }
        return nullptr;
    }

    // This function converts AST function to Tacky function
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

    // This function converts AST program to Tacky program
    tacky::Program *generate_tacky(ast::Program *program)
    {
        tacky::Function *function_definition = generate_function(program->func);
        tacky::Program *tacky = new tacky::Program(function_definition);
        return tacky;
    }
}
