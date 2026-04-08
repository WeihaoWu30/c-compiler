#include "tacky/tacky.hpp"
#include "ast/ast.hpp"
#include "compiler/ir_gen.hpp"
#include <vector>
#include <string>
#include <memory>
#include <utility>

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
    tacky::Unary_Operator convert_unop(ast::Unary_Operator op)
    {
        switch (op)
        {
        case ast::Unary_Operator::Complement:
            return tacky::Unary_Operator::Complement;
        case ast::Unary_Operator::Negate:
            return tacky::Unary_Operator::Negate;
        case ast::Unary_Operator::Not:
            return tacky::Unary_Operator::Not;
        default:
            return tacky::Unary_Operator::Invalid;
        }
    }

    // This function converts from a AST binary operator to TACKY operator
    tacky::Binary_Operator convert_binop(ast::Binary_Operator op)
    {
        switch (op)
        {
        case ast::Binary_Operator::Add:
            return tacky::Binary_Operator::Add;
        case ast::Binary_Operator::Subtract:
            return tacky::Binary_Operator::Subtract;
        case ast::Binary_Operator::Multiply:
            return tacky::Binary_Operator::Multiply;
        case ast::Binary_Operator::Divide:
            return tacky::Binary_Operator::Divide;
        case ast::Binary_Operator::Remainder:
            return tacky::Binary_Operator::Remainder;
        case ast::Binary_Operator::Equal:
            return tacky::Binary_Operator::Equal;
        case ast::Binary_Operator::NotEqual:
            return tacky::Binary_Operator::NotEqual;
        case ast::Binary_Operator::LessThan:
            return tacky::Binary_Operator::LessThan;
        case ast::Binary_Operator::LessOrEqual:
            return tacky::Binary_Operator::LessOrEqual;
        case ast::Binary_Operator::GreaterThan:
            return tacky::Binary_Operator::GreaterThan;
        case ast::Binary_Operator::GreaterOrEqual:
            return tacky::Binary_Operator::GreaterOrEqual;
        case ast::Binary_Operator::BitAnd:
            return tacky::Binary_Operator::BitAnd;
        case ast::Binary_Operator::BitOr:
            return tacky::Binary_Operator::BitOr;
        case ast::Binary_Operator::BitXor:
            return tacky::Binary_Operator::BitXor;
        case ast::Binary_Operator::BitLeftShift:
            return tacky::Binary_Operator::BitLeftShift;
        case ast::Binary_Operator::BitRightShift:
            return tacky::Binary_Operator::BitRightShift;
        default:
            return tacky::Binary_Operator::Invalid;
        }
    }

    // This function is recursive and converts AST expressions to Tacky Values
    tacky::Val *emit_tacky(ast::Expression *e, std::vector<std::unique_ptr<tacky::Instruction>> &instructions, std::vector<std::unique_ptr<tacky::Val>> &values)
    {
        ast::Constant *constant = dynamic_cast<ast::Constant *>(e);
        ast::Unary *unary = dynamic_cast<ast::Unary *>(e);
        ast::Binary *binary = dynamic_cast<ast::Binary *>(e);
        ast::Var *var = dynamic_cast<ast::Var *>(e);
        ast::Assignment *assignment = dynamic_cast<ast::Assignment *>(e);
        if (constant)
        {
            std::unique_ptr<tacky::Constant> t_constant = std::make_unique<tacky::Constant>(constant->val);
            values.push_back(std::move(t_constant));
            return values.back().get();
        }
        else if (unary)
        {
            tacky::Val *src = emit_tacky(unary->exp, instructions, values);
            tacky::Identifier *dst_name = make_identifier();
            std::unique_ptr<tacky::Var> dst = std::make_unique<tacky::Var>(dst_name);
            tacky::Unary_Operator unary_operator = convert_unop(unary->unary_operator);
            std::unique_ptr<tacky::Unary> new_unary = std::make_unique<tacky::Unary>(unary_operator, src, dst.get());
            instructions.push_back(std::move(new_unary));
            values.push_back(std::move(dst));
            return values.back().get();
        }
        else if (binary && binary->binary_operator == ast::Binary_Operator::And)
        {
            tacky::Val *left = emit_tacky(binary->left, instructions, values);
            tacky::Identifier *left_false_identifier = new tacky::Identifier("false" + std::to_string(label_counter++));
            std::unique_ptr<tacky::JumpIfZero> left_jmp_if_zero = std::make_unique<tacky::JumpIfZero>(left, left_false_identifier);
            instructions.push_back(std::move(left_jmp_if_zero));

            tacky::Val *right = emit_tacky(binary->right, instructions, values);
            tacky::Identifier *right_false_identifier = new tacky::Identifier(left_false_identifier->name);
            std::unique_ptr<tacky::JumpIfZero> right_jmp_if_zero = std::make_unique<tacky::JumpIfZero>(right, right_false_identifier);
            instructions.push_back(std::move(right_jmp_if_zero));

            std::unique_ptr<tacky::Constant> const_one = std::make_unique<tacky::Constant>(1);
            tacky::Identifier *result_identifier_one = make_identifier();
            std::unique_ptr<tacky::Var> result_one = std::make_unique<tacky::Var>(result_identifier_one);
            std::unique_ptr<tacky::Copy> copy_one = std::make_unique<tacky::Copy>(const_one.get(), result_one.get());
            values.push_back(std::move(const_one));
            values.push_back(std::move(result_one));
            instructions.push_back(std::move(copy_one));

            tacky::Identifier *end_jmp_identifier = new tacky::Identifier("end" + std::to_string(label_counter++));
            std::unique_ptr<tacky::Jump> end_jmp = std::make_unique<tacky::Jump>(end_jmp_identifier);
            instructions.push_back(std::move(end_jmp));

            tacky::Identifier *false_identifier = new tacky::Identifier(right_false_identifier->name);
            std::unique_ptr<tacky::Label> false_label = std::make_unique<tacky::Label>(false_identifier);
            instructions.push_back(std::move(false_label));

            std::unique_ptr<tacky::Constant> const_zero = std::make_unique<tacky::Constant>(0);
            tacky::Identifier *result_identifier_zero = new tacky::Identifier(result_identifier_one->name);
            std::unique_ptr<tacky::Var> result_zero = std::make_unique<tacky::Var>(result_identifier_zero);
            std::unique_ptr<tacky::Copy> copy_zero = std::make_unique<tacky::Copy>(const_zero.get(), result_zero.get());
            values.push_back(std::move(const_zero));
            values.push_back(std::move(result_zero));
            instructions.push_back(std::move(copy_zero));

            tacky::Identifier *end_label_identifier = new tacky::Identifier(end_jmp_identifier->name);
            std::unique_ptr<tacky::Label> end_label = std::make_unique<tacky::Label>(end_label_identifier);
            instructions.push_back(std::move(end_label));

            tacky::Identifier *dst_name = new tacky::Identifier(result_identifier_zero->name);
            std::unique_ptr<tacky::Var> dst = std::make_unique<tacky::Var>(dst_name);
            values.push_back(std::move(dst));
            return values.back().get();
        }
        else if (binary && binary->binary_operator == ast::Binary_Operator::Or)
        {
            tacky::Val *left = emit_tacky(binary->left, instructions, values);
            tacky::Identifier *left_true_identifier = new tacky::Identifier("true" + std::to_string(label_counter++));
            std::unique_ptr<tacky::JumpIfNotZero> left_jmp_if_not_zero = std::make_unique<tacky::JumpIfNotZero>(left, left_true_identifier);
            instructions.push_back(std::move(left_jmp_if_not_zero));

            tacky::Val *right = emit_tacky(binary->right, instructions, values);
            tacky::Identifier *right_true_identifier = new tacky::Identifier(left_true_identifier->name);
            std::unique_ptr<tacky::JumpIfNotZero> right_jmp_if_not_zero = std::make_unique<tacky::JumpIfNotZero>(right, right_true_identifier);
            instructions.push_back(std::move(right_jmp_if_not_zero));

            std::unique_ptr<tacky::Constant> const_zero = std::make_unique<tacky::Constant>(0);
            tacky::Identifier *result_identifier_zero = make_identifier();
            std::unique_ptr<tacky::Var> result_zero = std::make_unique<tacky::Var>(result_identifier_zero);
            std::unique_ptr<tacky::Copy> copy_zero = std::make_unique<tacky::Copy>(const_zero.get(), result_zero.get());
            values.push_back(std::move(const_zero));
            values.push_back(std::move(result_zero));
            instructions.push_back(std::move(copy_zero));

            tacky::Identifier *end_jmp_identifier = new tacky::Identifier("end" + std::to_string(label_counter++));
            std::unique_ptr<tacky::Jump> end_jmp = std::make_unique<tacky::Jump>(end_jmp_identifier);
            instructions.push_back(std::move(end_jmp));

            tacky::Identifier *true_identifier = new tacky::Identifier(right_true_identifier->name);
            std::unique_ptr<tacky::Label> true_label = std::make_unique<tacky::Label>(true_identifier);
            instructions.push_back(std::move(true_label));

            std::unique_ptr<tacky::Constant> const_one = std::make_unique<tacky::Constant>(1);
            tacky::Identifier *result_identifier_one = new tacky::Identifier(result_identifier_zero->name);
            std::unique_ptr<tacky::Var> result_one = std::make_unique<tacky::Var>(result_identifier_one);
            std::unique_ptr<tacky::Copy> copy_one = std::make_unique<tacky::Copy>(const_one.get(), result_one.get());
            values.push_back(std::move(const_one));
            values.push_back(std::move(result_one));
            instructions.push_back(std::move(copy_one));

            tacky::Identifier *end_label_identifier = new tacky::Identifier(end_jmp_identifier->name);
            std::unique_ptr<tacky::Label> end_label = std::make_unique<tacky::Label>(end_label_identifier);
            instructions.push_back(std::move(end_label));

            tacky::Identifier *dst_name = new tacky::Identifier(result_identifier_one->name);
            std::unique_ptr<tacky::Var> dst = std::make_unique<tacky::Var>(dst_name);
            values.push_back(std::move(dst));
            return values.back().get();
        }
        else if (binary)
        {
            tacky::Val *src1 = emit_tacky(binary->left, instructions, values);
            tacky::Val *src2 = emit_tacky(binary->right, instructions, values);
            tacky::Identifier *dst_name = make_identifier();
            std::unique_ptr<tacky::Var> dst = std::make_unique<tacky::Var>(dst_name);
            tacky::Binary_Operator binary_operator = convert_binop(binary->binary_operator);
            std::unique_ptr<tacky::Binary> new_binary = std::make_unique<tacky::Binary>(binary_operator, src1, src2, dst.get());
            values.push_back(std::move(dst));
            instructions.push_back(std::move(new_binary));
            return values.back().get();
        }
        else if (var)
        {
            tacky::Identifier *var_identifier = new tacky::Identifier(var->identifier->name);
            std::unique_ptr<tacky::Var> variable = std::make_unique<tacky::Var>(var_identifier);
            values.push_back(std::move(variable));
            return values.back().get();
        }
        else if (assignment)
        {
            ast::Var *v = dynamic_cast<ast::Var *>(assignment->lvalue);
            if (v)
            {
                tacky::Val *result = emit_tacky(assignment->exp, instructions, values);
                tacky::Identifier *identifer = new tacky::Identifier(v->identifier->name);
                std::unique_ptr<tacky::Var> variable = std::make_unique<tacky::Var>(identifer);
                instructions.push_back(std::make_unique<tacky::Copy>(result, variable.get()));
                values.push_back(std::move(variable));
                return values.back().get();
            }
        }
        return nullptr;
    }

    // This function converts AST function to Tacky function
    tacky::Function *generate_function(ast::Function *func)
    {
        std::vector<std::unique_ptr<tacky::Instruction>> instructions;
        std::vector<std::unique_ptr<tacky::Val>> values;
        ast::Identifier *a_identifier = func->name;
        tacky::Identifier *t_identifier = new tacky::Identifier(a_identifier->name);
        for (std::unique_ptr<ast::Block_Item> &b : func->body)
        {
            ast::D *d = dynamic_cast<ast::D *>(b.get());
            ast::S *s = dynamic_cast<ast::S *>(b.get());
            ast::Return *ret = dynamic_cast<ast::Return *>(b.get());
            if (d)
            {
                if (d->declaration->init)
                {
                    emit_tacky(d->declaration->init, instructions, values);
                }
            }
            else if (s)
            {
                ast::Expression_Statement *exp_statement = dynamic_cast<ast::Expression_Statement *>(s);
                if (exp_statement)
                {
                    emit_tacky(exp_statement->exp, instructions, values);
                }
            }
            else if (ret)
            {
                std::unique_ptr<tacky::Return> t_return = std::make_unique<tacky::Return>(emit_tacky(ret->exp, instructions, values));
                instructions.push_back(std::move(t_return));
                return new tacky::Function(t_identifier, std::move(instructions), std::move(values));
            }
        }
        std::unique_ptr<tacky::Constant> const_zero = std::make_unique<tacky::Constant>(0);
        std::unique_ptr<tacky::Return> default_return = std::make_unique<tacky::Return>(const_zero.get()); // default case to handle no return statements
        values.push_back(std::move(const_zero));
        instructions.push_back(std::move(default_return));
        return new tacky::Function(t_identifier, std::move(instructions), std::move(values));
    }

    // This function converts AST program to Tacky program
    tacky::Program *generate_tacky(ast::Program *program)
    {
        tacky::Function *function_definition = generate_function(program->func);
        tacky::Program *tacky = new tacky::Program(function_definition);
        return tacky;
    }
}
