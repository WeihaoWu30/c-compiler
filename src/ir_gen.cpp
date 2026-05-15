#include "tacky/tacky.hpp"
#include "ast/ast.hpp"
#include "compiler/ir_gen.hpp"
#include <vector>
#include <string>
#include <memory>
#include <utility>
#include <cstdint>

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

   // These next 2 functions converts from a AST compound/binary operator to tacky binary operators
   tacky::Binary_Operator convert_to_binop(ast::Binary_Operator op)
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

   tacky::Binary_Operator convert_to_binop(ast::Compound_Operator op)
   { // desurgared
      switch (op)
      {
      case ast::Compound_Operator::AdditionAssignment:
         return tacky::Binary_Operator::Add;
      case ast::Compound_Operator::SubtractionAssignment:
         return tacky::Binary_Operator::Subtract;
      case ast::Compound_Operator::MultiplicationAssignment:
         return tacky::Binary_Operator::Multiply;
      case ast::Compound_Operator::DivisionAssignment:
         return tacky::Binary_Operator::Divide;
      case ast::Compound_Operator::ModulusAssignment:
         return tacky::Binary_Operator::Remainder;
      case ast::Compound_Operator::BitwiseAndAssignment:
         return tacky::Binary_Operator::BitAnd;
      case ast::Compound_Operator::BitwiseOrAssignment:
         return tacky::Binary_Operator::BitOr;
      case ast::Compound_Operator::BitwiseXorAssignment:
         return tacky::Binary_Operator::BitXor;
      case ast::Compound_Operator::LeftShiftAssignment:
         return tacky::Binary_Operator::BitLeftShift;
      case ast::Compound_Operator::RightShiftAssignment:
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
      ast::Compound *compound = dynamic_cast<ast::Compound *>(e);
      ast::Conditional *conditional = dynamic_cast<ast::Conditional *>(e);
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
         tacky::Binary_Operator binary_operator = convert_to_binop(binary->binary_operator);
         std::unique_ptr<tacky::Binary> new_binary = std::make_unique<tacky::Binary>(binary_operator, src1, src2, dst.get());
         values.push_back(std::move(dst));
         instructions.push_back(std::move(new_binary));
         return values.back().get();
      }
      else if (compound)
      {
         ast::Var *v = dynamic_cast<ast::Var *>(compound->left);
         if (v)
         {
            // we have to make a var ourselves instead of using emit_tacky since as an lvalue it cant be evaluated like an expression
            tacky::Identifier *dst_name = new tacky::Identifier(v->identifier->name);
            std::unique_ptr<tacky::Var> dst_var = std::make_unique<tacky::Var>(dst_name);
            tacky::Val *dst = dst_var.get();
            values.push_back(std::move(dst_var));

            tacky::Val *src = emit_tacky(compound->right, instructions, values);
            tacky::Binary_Operator binary_operator = convert_to_binop(compound->compound_operator);
            std::unique_ptr<tacky::Compound> new_compound = std::make_unique<tacky::Compound>(binary_operator, src, dst);
            instructions.push_back(std::move(new_compound));
            return dst;
         }
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
            std::unique_ptr<tacky::Copy> copy = std::make_unique<tacky::Copy>(result, variable.get());
            instructions.push_back(std::move(copy));
            values.push_back(std::move(variable));
            return values.back().get();
         }
      }
      else if (conditional)
      {
         // emit instructions for the condition
         tacky::Val *condition_result = emit_tacky(conditional->condition, instructions, values);

         tacky::Identifier *expression_two_identifier = new tacky::Identifier("e2" + std::to_string(label_counter++));
         std::unique_ptr<tacky::JumpIfZero> conditional_jmp = std::make_unique<tacky::JumpIfZero>(condition_result, expression_two_identifier); // jump to else branch of ternary operator if condition is false
         instructions.push_back(std::move(conditional_jmp));

         // emit instructions for true branch and copy the result into a shared result variable
         tacky::Val *expression_one_result = emit_tacky(conditional->left, instructions, values);
         tacky::Identifier *expression_one_result_identifier = make_identifier();
         std::unique_ptr<tacky::Var> expression_one_result_dst = std::make_unique<tacky::Var>(expression_one_result_identifier);
         std::unique_ptr<tacky::Copy> expression_one_copy = std::make_unique<tacky::Copy>(expression_one_result, expression_one_result_dst.get());
         values.push_back(std::move(expression_one_result_dst));
         instructions.push_back(std::move(expression_one_copy));

         // after true branch, jump to the end
         tacky::Identifier *end_jmp_identifier = new tacky::Identifier("end" + std::to_string(label_counter++));
         std::unique_ptr<tacky::Jump> end_jmp = std::make_unique<tacky::Jump>(end_jmp_identifier);
         instructions.push_back(std::move(end_jmp));

         // false branch label
         tacky::Identifier *expression_two_label_identifier = new tacky::Identifier(expression_two_identifier->name);
         std::unique_ptr<tacky::Label> expression_two_label = std::make_unique<tacky::Label>(expression_two_label_identifier);
         instructions.push_back(std::move(expression_two_label));

         // emit instructions for false branch and copy the result into a shared result variable
         tacky::Val *expression_two_result = emit_tacky(conditional->right, instructions, values);
         tacky::Identifier *expression_two_result_identifier = new tacky::Identifier(expression_one_result_identifier->name);
         std::unique_ptr<tacky::Var> expression_two_result_dst = std::make_unique<tacky::Var>(expression_two_result_identifier);
         std::unique_ptr<tacky::Copy> expression_two_copy = std::make_unique<tacky::Copy>(expression_two_result, expression_two_result_dst.get());
         values.push_back(std::move(expression_two_result_dst));
         instructions.push_back(std::move(expression_two_copy));

         // end label
         tacky::Identifier *end_label_identifier = new tacky::Identifier(end_jmp_identifier->name);
         std::unique_ptr<tacky::Label> end_label = std::make_unique<tacky::Label>(end_label_identifier);
         instructions.push_back(std::move(end_label));

         // return a Var with the shared destination name as the result of the conditional
         tacky::Identifier *result_identifier = new tacky::Identifier(expression_two_result_identifier->name);
         std::unique_ptr<tacky::Var> result_var = std::make_unique<tacky::Var>(result_identifier);
         values.push_back(std::move(result_var));
         return values.back().get();
      }
      return nullptr;
   }

   // This function recursively converts AST statements into TACKY instructions
   void emit_tacky_statements(ast::Statement *s, std::vector<std::unique_ptr<tacky::Instruction>> &instructions, std::vector<std::unique_ptr<tacky::Val>> &values, bool &has_return)
   {
      ast::Expression_Statement *exp_statement = dynamic_cast<ast::Expression_Statement *>(s);
      ast::Return *ret = dynamic_cast<ast::Return *>(s);
      ast::If *if_statement = dynamic_cast<ast::If *>(s);

      if (exp_statement)
      {
         emit_tacky(exp_statement->exp, instructions, values);
      }
      else if (ret) // standalone return statements
      {
         std::unique_ptr<tacky::Return> t_return = std::make_unique<tacky::Return>(emit_tacky(ret->exp, instructions, values));
         instructions.push_back(std::move(t_return));
      }

      else if (if_statement)
      {
         if (!(if_statement->else_statement))
         {
            tacky::Val *if_value = emit_tacky(if_statement->condition, instructions, values);
            tacky::Identifier *condition_end_identifier = new tacky::Identifier("end" + std::to_string(label_counter++));
            std::unique_ptr<tacky::JumpIfZero> condition_jmp_if_zero = std::make_unique<tacky::JumpIfZero>(if_value, condition_end_identifier); // jump if condition is false
            instructions.push_back(std::move(condition_jmp_if_zero));

            bool branch_has_return = false; // local variable to track whether current branch has return statement
            emit_tacky_statements(if_statement->then_statement, instructions, values, branch_has_return);

            tacky::Identifier *end_label_identifier = new tacky::Identifier(condition_end_identifier->name);
            std::unique_ptr<tacky::Label> false_label = std::make_unique<tacky::Label>(end_label_identifier);
            instructions.push_back(std::move(false_label));
         }
         else
         {
            tacky::Val *if_value = emit_tacky(if_statement->condition, instructions, values);
            tacky::Identifier *condition_else_identifier = new tacky::Identifier("false" + std::to_string(label_counter++));
            std::unique_ptr<tacky::JumpIfZero> condition_jmp_if_zero = std::make_unique<tacky::JumpIfZero>(if_value, condition_else_identifier); // jump to else block if condition is false
            instructions.push_back(std::move(condition_jmp_if_zero));

            bool then_has_return = false; // handles cases where there is not a return in both branches....if this is the case, we need the default return in generate_function
            bool else_has_return = false;

            emit_tacky_statements(if_statement->then_statement, instructions, values, then_has_return);

            tacky::Identifier *end_jmp_identifier = new tacky::Identifier("end" + std::to_string(label_counter++));
            std::unique_ptr<tacky::Jump> end_jmp = std::make_unique<tacky::Jump>(end_jmp_identifier);
            instructions.push_back(std::move(end_jmp));

            tacky::Identifier *else_identifier = new tacky::Identifier(condition_else_identifier->name);
            std::unique_ptr<tacky::Label> else_label = std::make_unique<tacky::Label>(else_identifier);
            instructions.push_back(std::move(else_label));

            emit_tacky_statements(if_statement->else_statement, instructions, values, else_has_return);

            tacky::Identifier *end_label_identifier = new tacky::Identifier(end_jmp_identifier->name);
            std::unique_ptr<tacky::Label> end_label = std::make_unique<tacky::Label>(end_label_identifier);
            instructions.push_back(std::move(end_label));

            if (then_has_return && else_has_return)
            { // return in all paths
               has_return = true;
            }
         }
      }
   }

   // This function converts AST function to Tacky function
   tacky::Function *generate_function(ast::Function *func)
   {
      std::vector<std::unique_ptr<tacky::Instruction>> instructions;
      std::vector<std::unique_ptr<tacky::Val>> values;
      ast::Identifier *a_identifier = func->name;
      tacky::Identifier *t_identifier = new tacky::Identifier(a_identifier->name);
      bool has_return = false; // variable to track whether a return happens in all paths of function
      for (std::unique_ptr<ast::Block_Item> &b : func->body)
      {
         ast::D *d = dynamic_cast<ast::D *>(b.get());
         ast::S *s = dynamic_cast<ast::S *>(b.get());
         if (d)
         {
            if (d->declaration->init)
            {
               emit_tacky(d->declaration->init, instructions, values);
            }
         }
         else if (s)
         {
            ast::Expression_Statement *exp_statement = dynamic_cast<ast::Expression_Statement *>(s->statement);
            ast::Return *ret = dynamic_cast<ast::Return *>(s->statement);
            ast::If *if_statement = dynamic_cast<ast::If *>(s->statement);
            if (exp_statement)
            {
               emit_tacky_statements(exp_statement, instructions, values, has_return);
            }
            else if (ret)
            {
               emit_tacky_statements(ret, instructions, values, has_return);
               return new tacky::Function(t_identifier, std::move(instructions), std::move(values));
            }
            else if (if_statement)
            {
               emit_tacky_statements(if_statement, instructions, values, has_return);
            }
         }
      }

      // if not all paths gurantee a return, add default Return 0
      if (!has_return)
      {
         std::unique_ptr<tacky::Constant> const_zero = std::make_unique<tacky::Constant>(0);
         std::unique_ptr<tacky::Return> default_return = std::make_unique<tacky::Return>(const_zero.get()); // default case to handle no return statements
         values.push_back(std::move(const_zero));
         instructions.push_back(std::move(default_return));
      }
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
