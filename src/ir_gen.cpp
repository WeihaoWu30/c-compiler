#include "ast/initial_values/initial_value.hpp"
#include "tacky/tacky.hpp"
#include "ast/ast.hpp"
#include "compiler/ir_gen.hpp"
#include "compiler/symbols.hpp"
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
      if(!e) return nullptr;
      ast::Constant *constant = dynamic_cast<ast::Constant *>(e);
      if (constant)
      {
         std::unique_ptr<tacky::Constant> t_constant = std::make_unique<tacky::Constant>(constant->val);
         values.push_back(std::move(t_constant));
         return values.back().get();
      }
      ast::Unary *unary = dynamic_cast<ast::Unary *>(e);
      if (unary)
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
      ast::Binary *binary = dynamic_cast<ast::Binary *>(e);
      if (binary && binary->binary_operator == ast::Binary_Operator::And)
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
      ast::Var *var = dynamic_cast<ast::Var *>(e);
      if (var)
      {
         tacky::Identifier *var_identifier = new tacky::Identifier(var->identifier->text);
         std::unique_ptr<tacky::Var> variable = std::make_unique<tacky::Var>(var_identifier);
         values.push_back(std::move(variable));
         return values.back().get();
      }
      ast::Assignment *assignment = dynamic_cast<ast::Assignment *>(e);
      if (assignment)
      {
         ast::Var *v = dynamic_cast<ast::Var *>(assignment->lvalue);
         if (v)
         {
            tacky::Val *result = emit_tacky(assignment->exp, instructions, values);
            tacky::Identifier *identifer = new tacky::Identifier(v->identifier->text);
            std::unique_ptr<tacky::Var> variable = std::make_unique<tacky::Var>(identifer);
            std::unique_ptr<tacky::Copy> copy = std::make_unique<tacky::Copy>(result, variable.get());
            instructions.push_back(std::move(copy));
            values.push_back(std::move(variable));
            return values.back().get();
         }
      }
      ast::Compound *compound = dynamic_cast<ast::Compound *>(e);
      if (compound)
      {
         ast::Var *v = dynamic_cast<ast::Var *>(compound->left);
         if (v)
         {
            // we have to make a var ourselves instead of using emit_tacky since as an lvalue it cant be evaluated like an expression
            tacky::Identifier *dst_name = new tacky::Identifier(v->identifier->text);
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
      ast::Conditional *conditional = dynamic_cast<ast::Conditional *>(e);
      if (conditional)
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
      ast::Function_Call *function_call = dynamic_cast<ast::Function_Call *>(e);
      if(function_call) {
         tacky::Identifier *fun_name = new tacky::Identifier(function_call->identifier->text);
         std::vector<tacky::Val *> args;
         for(auto &arg :function_call->args) {
            args.push_back(emit_tacky(arg, instructions, values));
         }
         
         tacky::Identifier *dst_name = make_identifier();
         std::unique_ptr<tacky::Var> dst = std::make_unique<tacky::Var>(dst_name);
         std::unique_ptr<tacky::Fun_Call> fun_call = std::make_unique<tacky::Fun_Call>(fun_name, std::move(args), dst.get());
         instructions.push_back(std::move(fun_call));
         values.push_back(std::move(dst));
         return values.back().get();
      }
      return nullptr;
   }

   // This function recursively converts AST statements into TACKY instructions
   void emit_tacky_statements(ast::Statement *s, std::vector<std::unique_ptr<tacky::Instruction>> &instructions, std::vector<std::unique_ptr<tacky::Val>> &values, bool &has_return)
   {
      ast::Expression_Statement *exp_statement = dynamic_cast<ast::Expression_Statement *>(s);
      if (exp_statement)
      {
         emit_tacky(exp_statement->exp, instructions, values);
         return;
      }
      ast::Return *ret = dynamic_cast<ast::Return *>(s);
      if (ret) // standalone return statements
      {
         std::unique_ptr<tacky::Return> t_return = std::make_unique<tacky::Return>(emit_tacky(ret->exp, instructions, values));
         instructions.push_back(std::move(t_return));
         has_return = true;
         return;
      }
      ast::If *if_statement = dynamic_cast<ast::If *>(s);
      if (if_statement)
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
         return;
      }
      ast::Compound_Statement *compound_statement = dynamic_cast<ast::Compound_Statement *>(s);
      if (compound_statement)
      {
         for (std::unique_ptr<ast::Block_Item> &b : compound_statement->block->block_items)
         {
            ast::D *d = dynamic_cast<ast::D *>(b.get());
            ast::S *s = dynamic_cast<ast::S *>(b.get());
            if (d)
            {
               ast::Var_Decl *var_decl = dynamic_cast<ast::Var_Decl *>(d->declaration);
               if(var_decl) {
                  emit_variable_initialization(var_decl, instructions, values);
               }
               // again we don't need to handle function declarations since you cannot define functions within function definitions
            }
            else if (s)
            {
               emit_tacky_statements(s->statement, instructions, values, has_return);
            }
         }
         return;
      }
      ast::Break *break_statement = dynamic_cast<ast::Break *>(s);
      if(break_statement)
      {
         tacky::Identifier *break_identifier = new tacky::Identifier("break_" + break_statement->label->text);
         std::unique_ptr<tacky::Jump> break_jump = std::make_unique<tacky::Jump>(break_identifier);
         instructions.push_back(std::move(break_jump));
         return;
      }
      ast::Continue *continue_statement = dynamic_cast<ast::Continue *>(s);
      if(continue_statement)
      {
         tacky::Identifier *continue_identifier = new tacky::Identifier("continue_" + continue_statement->label->text);
         std::unique_ptr<tacky::Jump> continue_jump = std::make_unique<tacky::Jump>(continue_identifier);
         instructions.push_back(std::move(continue_jump));
         return;
      }
      ast::DoWhile *do_while_statement = dynamic_cast<ast::DoWhile *>(s);
      if(do_while_statement)
      {
         // branch name
         tacky::Identifier *start_label_identifier = new tacky::Identifier("do_while_start_" + do_while_statement->label->text);
         std::unique_ptr<tacky::Label> start_label = std::make_unique<tacky::Label>(start_label_identifier);
         instructions.push_back(std::move(start_label));

         // branch instructions
         emit_tacky_statements(do_while_statement->body, instructions, values, has_return);

         // continue jump start
         tacky::Identifier *continue_identifier = new tacky::Identifier("continue_" + do_while_statement->label->text);
         std::unique_ptr<tacky::Label> continue_label = std::make_unique<tacky::Label>(continue_identifier);
         instructions.push_back(std::move(continue_label));

         // branch condition, jump to start if condition is not zero
         tacky::Val *condiiton_result = emit_tacky(do_while_statement->condition, instructions, values);
         tacky::Identifier *jmp_identifier = new tacky::Identifier(start_label_identifier->name);
         std::unique_ptr<tacky::JumpIfNotZero> start_jmp = std::make_unique<tacky::JumpIfNotZero>(condiiton_result, jmp_identifier);
         instructions.push_back(std::move(start_jmp));

         // break jump
         tacky::Identifier *break_identifier = new tacky::Identifier("break_" + do_while_statement->label->text);
         std::unique_ptr<tacky::Label> break_label = std::make_unique<tacky::Label>(break_identifier);
         instructions.push_back(std::move(break_label));
         return;
      }
      ast::While *while_statement = dynamic_cast<ast::While *>(s);
      if(while_statement)
      {
         // branch name
         tacky::Identifier *continue_identifier = new tacky::Identifier("continue_" + while_statement->label->text);
         std::unique_ptr<tacky::Label> continue_label = std::make_unique<tacky::Label>(continue_identifier);
         instructions.push_back(std::move(continue_label));

         // branch condition
         tacky::Val *condition_result = emit_tacky(while_statement->condition, instructions, values);
         tacky::Identifier *jmp_end_identifier = new tacky::Identifier("break_" + while_statement->label->text);
         std::unique_ptr<tacky::JumpIfZero> end_jmp = std::make_unique<tacky::JumpIfZero>(condition_result, jmp_end_identifier);
         instructions.push_back(std::move(end_jmp));

         // branch instructions
         emit_tacky_statements(while_statement->body, instructions, values, has_return);

         // jump to start
         tacky::Identifier *jmp_start_identifier = new tacky::Identifier(continue_identifier->name);
         std::unique_ptr<tacky::Jump> jmp_start = std::make_unique<tacky::Jump>(jmp_start_identifier);
         instructions.push_back(std::move(jmp_start));

         // end label
         tacky::Identifier *break_label_identifier = new tacky::Identifier(jmp_end_identifier->name);
         std::unique_ptr<tacky::Label> break_label = std::make_unique<tacky::Label>(break_label_identifier);
         instructions.push_back(std::move(break_label));
         return;
      }
      ast::For *for_statement = dynamic_cast<ast::For *>(s);
      if(for_statement)
      {
         // initialization
         ast::For_Init *for_init = for_statement->init;
         ast::Init_Decl *init_declaration = dynamic_cast<ast::Init_Decl *>(for_init);
         ast::Init_Exp *init_expression = dynamic_cast<ast::Init_Exp *>(for_init);
         if(init_declaration)
         {
            emit_variable_initialization(init_declaration->variable_declaration, instructions, values);
         } else if(init_expression) {
            emit_tacky(init_expression->expression, instructions, values);
         }

         // branch name
         tacky::Identifier *start_label_identifier = new tacky::Identifier("for_start_" + for_statement->label->text);
         std::unique_ptr<tacky::Label> start_label = std::make_unique<tacky::Label>(start_label_identifier);
         instructions.push_back(std::move(start_label));

         // branch condition
         tacky::Val *condition_result = emit_tacky(for_statement->condition, instructions, values);
         if(condition_result) {
            tacky::Identifier *jmp_end_identifier = new tacky::Identifier("break_" + for_statement->label->text);
            std::unique_ptr<tacky::JumpIfZero> end_jmp = std::make_unique<tacky::JumpIfZero>(condition_result, jmp_end_identifier);
            instructions.push_back(std::move(end_jmp));
         }

         // branch instructions
         emit_tacky_statements(for_statement->body, instructions, values, has_return);

         // continue label
         tacky::Identifier *continue_identifier = new tacky::Identifier("continue_" + for_statement->label->text);
         std::unique_ptr<tacky::Label> continue_label = std::make_unique<tacky::Label>(continue_identifier);
         instructions.push_back(std::move(continue_label));

         // post instructions
         emit_tacky(for_statement->post, instructions, values);

         // jump to start
         tacky::Identifier *jmp_start_identifier = new tacky::Identifier(start_label_identifier->name);
         std::unique_ptr<tacky::Jump> jmp_start = std::make_unique<tacky::Jump>(jmp_start_identifier);
         instructions.push_back(std::move(jmp_start));

         // break label
         tacky::Identifier *break_label_identifier = new tacky::Identifier("break_" + for_statement->label->text);
         std::unique_ptr<tacky::Label> break_label = std::make_unique<tacky::Label>(break_label_identifier);
         instructions.push_back(std::move(break_label));
         return;
      }
   }

   void emit_variable_initialization(ast::Var_Decl *var_decl, std::vector<std::unique_ptr<tacky::Instruction>> &instructions, std::vector<std::unique_ptr<tacky::Val>> &values)
   {
      if(var_decl->init && var_decl->storage_class == ast::Storage_Class::NONE) {
         tacky::Val *init_result = emit_tacky(var_decl->init, instructions, values);
         tacky::Identifier *var_identifier = new tacky::Identifier(var_decl->name->text);
         std::unique_ptr<tacky::Var> var = std::make_unique<tacky::Var>(var_identifier);
         std::unique_ptr<tacky::Copy> copy = std::make_unique<tacky::Copy>(init_result, var.get());
         instructions.push_back(std::move(copy));
         values.push_back(std::move(var));
      }
   }

   // This function converts AST function to Tacky function
   tacky::Function generate_function(ast::Fun_Decl *function_declaration)
   {
      std::vector<std::unique_ptr<tacky::Identifier>> params;
      for(auto &param : function_declaration->params) {
         params.emplace_back(new tacky::Identifier(param->text));
      }
      std::vector<std::unique_ptr<tacky::Instruction>> instructions;
      std::vector<std::unique_ptr<tacky::Val>> values;
      ast::Identifier *a_identifier = function_declaration->name;
      std::unique_ptr<tacky::Identifier> t_identifier = std::make_unique<tacky::Identifier>(a_identifier->text);
      bool has_return = false; // variable to track whether a return happens in all paths of function
      std::pair<std::unique_ptr<ast::Type>, ast::Identifier_Attr> &type_and_attr = symbols::symbols.find(function_declaration->name->text)->second;
      ast::Fun_Attr fun_attr = std::get<ast::Fun_Attr>(type_and_attr.second);
      for (std::unique_ptr<ast::Block_Item> &b : function_declaration->body->block_items)
      {
         ast::D *d = dynamic_cast<ast::D *>(b.get());
         ast::S *s = dynamic_cast<ast::S *>(b.get());
         if (d)
         {
            ast::Var_Decl *var_decl = dynamic_cast<ast::Var_Decl *>(d->declaration);
            if(var_decl) {
               emit_variable_initialization(var_decl, instructions, values);
            }
            // we don't need to handle function declarations since you cannot define functions within function definitions
         }
         else if (s)
         {
            emit_tacky_statements(s->statement, instructions, values, has_return);
            ast::Return *ret = dynamic_cast<ast::Return *>(s->statement);
            if (ret)
            {
               return tacky::Function(std::move(t_identifier), std::move(params), std::move(instructions), std::move(values), fun_attr.global);
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
      return tacky::Function(std::move(t_identifier), std::move(params), std::move(instructions), std::move(values), fun_attr.global);
   }

   void generate_static_variables(std::vector<tacky::Top_Level>& top_levels)
   {
      for(auto &[name, type_and_attr] : symbols::symbols)
      {
         ast::Identifier_Attr identifier_attr = type_and_attr.second;
         if(holds_alternative<ast::Static_Attr>(identifier_attr)) {
            ast::Static_Attr static_attr = get<ast::Static_Attr>(identifier_attr);
            if(std::holds_alternative<ast::Initial>(static_attr.init)) {
               ast::Initial initial = get<ast::Initial>(static_attr.init);
               top_levels.emplace_back(tacky::Static_Variable(std::make_unique<tacky::Identifier>(name), initial.value, static_attr.global));
            } else if(std::holds_alternative<ast::Tentative>(static_attr.init)) {
               top_levels.emplace_back(tacky::Static_Variable(std::make_unique<tacky::Identifier>(name), 0, static_attr.global));
            } else if(std::holds_alternative<ast::No_Initializer>(static_attr.init)) continue; // no initializer, skip
         }
      }
   }

   // This function converts AST program to Tacky program
   tacky::Program *generate_tacky(ast::Program *program)
   {
      std::vector<tacky::Top_Level> top_levels;
      for(auto &declaration : program->declarations)
      {
         ast::Fun_Decl *fun_decl = dynamic_cast<ast::Fun_Decl *>(declaration.get());
         if(fun_decl && fun_decl->body != nullptr) {
            top_levels.emplace_back(generate_function(fun_decl));
         }
      }
      generate_static_variables(top_levels);
      tacky::Program *tacky = new tacky::Program(std::move(top_levels));
      return tacky;
   }
}
