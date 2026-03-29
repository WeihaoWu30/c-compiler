#include "tacky/tacky.hpp"
#include "ast/ast.hpp"
#include "compiler/ir_gen.hpp"
#include <vector>
#include <string>
#include <iostream>
#include <memory>
#include <utility>

namespace ir_gen
{
    uint32_t id_counter = 0;    // Assists in creating unique temporary variables
    uint32_t label_counter = 0; // Assists in creating unique Labels

    // This function creates the name for a temporary variable
    std::shared_ptr<tacky::Identifier> make_identifier()
    {
        std::string tmp_name("tmp." + std::to_string(id_counter++));
        return std::make_shared<tacky::Identifier>(tmp_name);
    }

    // This function converts from a AST Unary operator to TACKY unary operator
    std::unique_ptr<tacky::Unary_Operator> convert_unop(std::unique_ptr<ast::Unary_Operator> exp)
    {
        ast::Complement *complement = dynamic_cast<ast::Complement *>(exp.get());
        ast::Negate *negate = dynamic_cast<ast::Negate *>(exp.get());
        ast::Unary_Not *not_op = dynamic_cast<ast::Unary_Not *>(exp.get());
        std::unique_ptr<tacky::Unary_Operator> res;
        if (complement)
        {
            res = std::make_unique<tacky::Complement>();
        }
        else if (negate)
        {
            res = std::make_unique<tacky::Negate>();
        }
        else if (not_op)
        {
            res = std::make_unique<tacky::Not>();
        }

        return res;
    }

    // This function converts from a AST binary operator to TACKY operator
    std::unique_ptr<tacky::Binary_Operator> convert_binop(std::unique_ptr<ast::Binary_Operator> op)
    {
        std::unique_ptr<tacky::Binary_Operator> res;
        ast::Add *add = dynamic_cast<ast::Add *>(op.get());
        ast::Subtract *subtract = dynamic_cast<ast::Subtract *>(op.get());
        ast::Multiply *multiply = dynamic_cast<ast::Multiply *>(op.get());
        ast::Divide *divide = dynamic_cast<ast::Divide *>(op.get());
        ast::Remainder *remainder = dynamic_cast<ast::Remainder *>(op.get());
        ast::Equal *equal = dynamic_cast<ast::Equal *>(op.get());
        ast::NotEqual *not_equal = dynamic_cast<ast::NotEqual *>(op.get());
        ast::LessThan *less_than = dynamic_cast<ast::LessThan *>(op.get());
        ast::LessOrEqual *less_or_equal = dynamic_cast<ast::LessOrEqual *>(op.get());
        ast::GreaterThan *greater_than = dynamic_cast<ast::GreaterThan *>(op.get());
        ast::GreaterOrEqual *greater_or_equal = dynamic_cast<ast::GreaterOrEqual *>(op.get());

        if (add)
        {
            res = std::make_unique<tacky::Add>();
        }
        else if (subtract)
        {
            res = std::make_unique<tacky::Subtract>();
        }
        else if (multiply)
        {
            res = std::make_unique<tacky::Multiply>();
        }
        else if (divide)
        {
            res = std::make_unique<tacky::Divide>();
        }
        else if (remainder)
        {
            res = std::make_unique<tacky::Remainder>();
        }
        else if (equal)
        {
            res = std::make_unique<tacky::Equal>();
        }
        else if (not_equal)
        {
            res = std::make_unique<tacky::NotEqual>();
        }
        else if (less_than)
        {
            res = std::make_unique<tacky::LessThan>();
        }
        else if (less_or_equal)
        {
            res = std::make_unique<tacky::LessOrEqual>();
        }
        else if (greater_than)
        {
            res = std::make_unique<tacky::GreaterThan>();
        }
        else if (greater_or_equal)
        {
            res = std::make_unique<tacky::GreaterOrEqual>();
        }
        return res;
    }

    // This function is recursive and converts AST expressions to Tacky Values
    std::shared_ptr<tacky::Val> emit_tacky(std::unique_ptr<ast::Expression> e, std::vector<std::unique_ptr<tacky::Instruction>> &instructions)
    {
        ast::Constant *constant = dynamic_cast<ast::Constant *>(e.get());
        ast::Unary *unary = dynamic_cast<ast::Unary *>(e.get());
        ast::Binary *binary = dynamic_cast<ast::Binary *>(e.get());
        ast::Var *var = dynamic_cast<ast::Var *>(e.get());
        ast::Assignment *assignment = dynamic_cast<ast::Assignment *>(e.get());
        if (constant)
        {
            std::shared_ptr<tacky::Constant> t_constant = std::make_shared<tacky::Constant>(constant->val);
            return t_constant;
        }
        else if (unary)
        {
            std::shared_ptr<tacky::Val> src = emit_tacky(std::move(unary->exp), instructions);
            std::shared_ptr<tacky::Identifier> dst_name = make_identifier();
            std::shared_ptr<tacky::Var> dst = std::make_shared<tacky::Var>(std::move(dst_name));
            std::unique_ptr<tacky::Unary_Operator> unary_operator = convert_unop(std::move(unary->unary_operator));
            std::unique_ptr<tacky::Unary> new_unary = std::make_unique<tacky::Unary>(std::move(unary_operator), std::move(src), dst);
            instructions.push_back(std::move(new_unary));
            return dst;
        }
        else if (binary && dynamic_cast<ast::And *>(binary->binary_operator.get()))
        {
            std::shared_ptr<tacky::Val> left = emit_tacky(std::move(binary->left), instructions);
            std::shared_ptr<tacky::Identifier> false_identifier = std::make_shared<tacky::Identifier>("false" + std::to_string(label_counter++));
            std::unique_ptr<tacky::JumpIfZero> left_jmp_if_zero = std::make_unique<tacky::JumpIfZero>(std::move(left), false_identifier);
            instructions.push_back(std::move(left_jmp_if_zero));

            std::shared_ptr<tacky::Val> right = emit_tacky(std::move(binary->right), instructions);
            std::unique_ptr<tacky::JumpIfZero> right_jmp_if_zero = std::make_unique<tacky::JumpIfZero>(std::move(right), false_identifier);
            instructions.push_back(std::move(right_jmp_if_zero));

            std::shared_ptr<tacky::Constant> const_one = std::make_shared<tacky::Constant>(1);
            std::shared_ptr<tacky::Identifier> result_identifier_one = make_identifier();
            std::shared_ptr<tacky::Var> result_one = std::make_shared<tacky::Var>(result_identifier_one);
            std::unique_ptr<tacky::Copy> copy_one = std::make_unique<tacky::Copy>(std::move(const_one), std::move(result_one));
            instructions.push_back(std::move(copy_one));

            std::shared_ptr<tacky::Identifier> end_jmp_identifier = std::make_shared<tacky::Identifier>("end" + std::to_string(label_counter++));
            std::unique_ptr<tacky::Jump> end_jmp = std::make_unique<tacky::Jump>(end_jmp_identifier);
            instructions.push_back(std::move(end_jmp));

            std::unique_ptr<tacky::Label> false_label = std::make_unique<tacky::Label>(false_identifier);
            instructions.push_back(std::move(false_label));

            std::shared_ptr<tacky::Constant> const_zero = std::make_shared<tacky::Constant>(0);
            std::shared_ptr<tacky::Var> result_zero = std::make_shared<tacky::Var>(result_identifier_one);
            std::unique_ptr<tacky::Copy> copy_zero = std::make_unique<tacky::Copy>(std::move(const_zero), std::move(result_zero));
            instructions.push_back(std::move(copy_zero));

            std::unique_ptr<tacky::Label> end_label = std::make_unique<tacky::Label>(end_jmp_identifier);
            instructions.push_back(std::move(end_label));

            std::shared_ptr<tacky::Var> dst = std::make_shared<tacky::Var>(result_identifier_one);
            return dst;
        }
        else if (binary && dynamic_cast<ast::Or *>(binary->binary_operator.get()))
        {
            std::shared_ptr<tacky::Val> left = emit_tacky(std::move(binary->left), instructions);
            std::shared_ptr<tacky::Identifier> true_identifier = std::make_shared<tacky::Identifier>("true" + std::to_string(label_counter++));
            std::unique_ptr<tacky::JumpIfNotZero> left_jmp_if_not_zero = std::make_unique<tacky::JumpIfNotZero>(std::move(left), true_identifier);
            instructions.push_back(std::move(left_jmp_if_not_zero));

            std::shared_ptr<tacky::Val> right = emit_tacky(std::move(binary->right), instructions);
            std::unique_ptr<tacky::JumpIfNotZero> right_jmp_if_not_zero = std::make_unique<tacky::JumpIfNotZero>(std::move(right), true_identifier);
            instructions.push_back(std::move(right_jmp_if_not_zero));

            std::shared_ptr<tacky::Constant> const_zero = std::make_shared<tacky::Constant>(0);
            std::shared_ptr<tacky::Identifier> result_identifier_zero = make_identifier();
            std::shared_ptr<tacky::Var> result_zero = std::make_shared<tacky::Var>(result_identifier_zero);
            std::unique_ptr<tacky::Copy> copy_zero = std::make_unique<tacky::Copy>(std::move(const_zero), std::move(result_zero));
            instructions.push_back(std::move(copy_zero));

            std::shared_ptr<tacky::Identifier> end_jmp_identifier = std::make_shared<tacky::Identifier>("end" + std::to_string(label_counter++));
            std::unique_ptr<tacky::Jump> end_jmp = std::make_unique<tacky::Jump>(end_jmp_identifier);
            instructions.push_back(std::move(end_jmp));

            std::unique_ptr<tacky::Label> true_label = std::make_unique<tacky::Label>(true_identifier);
            instructions.push_back(std::move(true_label));

            std::shared_ptr<tacky::Constant> const_one = std::make_shared<tacky::Constant>(1);
            std::shared_ptr<tacky::Var> result_one = std::make_shared<tacky::Var>(result_identifier_zero);
            std::unique_ptr<tacky::Copy> copy_one = std::make_unique<tacky::Copy>(std::move(const_one), std::move(result_one));
            instructions.push_back(std::move(copy_one));

            std::unique_ptr<tacky::Label> end_label = std::make_unique<tacky::Label>(end_jmp_identifier);
            instructions.push_back(std::move(end_label));

            std::shared_ptr<tacky::Var> dst = std::make_shared<tacky::Var>(result_identifier_zero);
            return dst;
        }
        else if (binary)
        {
            std::shared_ptr<tacky::Val> src1 = emit_tacky(std::move(binary->left), instructions);
            std::shared_ptr<tacky::Val> src2 = emit_tacky(std::move(binary->right), instructions);
            std::shared_ptr<tacky::Identifier> dst_name = make_identifier();
            std::shared_ptr<tacky::Var> dst = std::make_shared<tacky::Var>(std::move(dst_name));
            std::unique_ptr<tacky::Binary_Operator> binary_operator = convert_binop(std::move(binary->binary_operator));
            std::unique_ptr<tacky::Binary> new_binary = std::make_unique<tacky::Binary>(std::move(binary_operator), std::move(src1), std::move(src2), dst);
            instructions.push_back(std::move(new_binary));
            return dst;
        }
        else if (var)
        {
            std::shared_ptr<tacky::Identifier> var_identifier = std::make_shared<tacky::Identifier>(var->identifier->name);
            return std::make_unique<tacky::Var>(var_identifier);
        }
        else if (assignment)
        {
            ast::Var *v = dynamic_cast<ast::Var *>(assignment->lvalue.get());
            if (v)
            {
                std::shared_ptr<tacky::Val> result = emit_tacky(std::move(assignment->exp), instructions);
                std::shared_ptr<tacky::Identifier> name = std::make_shared<tacky::Identifier>(v->identifier->name);
                std::shared_ptr<tacky::Var> tacky_var = std::make_shared<tacky::Var>(std::move(name));
                instructions.push_back(std::make_unique<tacky::Copy>(std::move(result), tacky_var));
                return tacky_var;
            }
        }
        return nullptr;
    }

    // This function converts AST function to Tacky function
    std::unique_ptr<tacky::Function> generate_function(std::unique_ptr<ast::Function> func)
    {
        std::vector<std::unique_ptr<tacky::Instruction>> instructions;
        std::unique_ptr<tacky::Identifier> t_identifier = std::make_unique<tacky::Identifier>(func->name->name);
        for (std::unique_ptr<ast::Block_Item> &b : func->body)
        {
            ast::D *d = dynamic_cast<ast::D *>(b.get());
            ast::S *s = dynamic_cast<ast::S *>(b.get());
            ast::Return *ret = dynamic_cast<ast::Return *>(b.get());
            if (d)
            {
                if (d->declaration->init)
                {
                    emit_tacky(std::move(d->declaration->init), instructions);
                }
            }
            else if (s)
            {
                ast::Expression_Statement *exp_statement = dynamic_cast<ast::Expression_Statement *>(s);
                if (exp_statement)
                {
                    emit_tacky(std::move(exp_statement->exp), instructions);
                }
            }
            else if (ret)
            {
                std::unique_ptr<tacky::Return> t_return = std::make_unique<tacky::Return>(emit_tacky(std::move(ret->exp), instructions));
                instructions.push_back(std::move(t_return));
                return std::make_unique<tacky::Function>(std::move(t_identifier), std::move(instructions));
            }
        }
        std::unique_ptr<tacky::Return> default_return = std::make_unique<tacky::Return>(std::make_unique<tacky::Constant>(0)); // default case to handle no return statements
        instructions.push_back(std::move(default_return));
        return std::make_unique<tacky::Function>(std::move(t_identifier), std::move(instructions));
    }

    // This function converts AST program to Tacky program
    std::unique_ptr<tacky::Program> generate_tacky(std::unique_ptr<ast::Program> program)
    {
        std::unique_ptr<tacky::Function> function_definition = generate_function(std::move(program->func));
        std::unique_ptr<tacky::Program> tacky = std::make_unique<tacky::Program>(std::move(function_definition));
        return tacky;
    }
}
