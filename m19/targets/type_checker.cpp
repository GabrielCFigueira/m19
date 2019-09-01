#include <string>
#include <stack>
#include "targets/type_checker.h"
#include "ast/all.h"  // automatically generated

#define ASSERT_UNSPEC \
    { if (node->type() != nullptr && \
          node->type()->name() != basic_type::TYPE_UNSPEC) return; }

std::string function_name = "";
std::stack<basic_type*> context = std::stack<basic_type*>();

//-----------------------------------AUX------------------------------------------

inline static bool checkPointers(basic_type* firstType, basic_type* secondType){ //this function only works when the types passed as agrs are pointers
  basic_type *first = firstType, *second = secondType;

  while(first->name() == basic_type::TYPE_POINTER && second->name() == basic_type::TYPE_POINTER) {
    first = first->subtype();
    second = second->subtype();

    if(first->name() != second->name())
      return false;
  }

  return true;
}

inline static basic_type* newBasicType(basic_type* type){
  basic_type *bt = new basic_type(type->size(), type->name());
  basic_type *btr = bt;

  if(type->subtype()){
    basic_type *subtype = type;
    do {
      subtype = subtype->subtype();
      bt->_subtype = new basic_type(subtype->size(), subtype->name());
      bt = bt->subtype();
    } while(subtype->name() == basic_type::TYPE_POINTER);
  }

  return btr;
}

//--------------------------------------------------------------------------------

void m19::type_checker::do_nil_node(cdk::nil_node * const node, int lvl) {
  // EMPTY
}
void m19::type_checker::do_data_node(cdk::data_node * const node, int lvl) {
  // EMPTY
}

void m19::type_checker::do_return_node(m19::return_node *const node, int lvl) {
  // EMPTY
}

void m19::type_checker::do_continue_node(m19::continue_node *const node, int lvl) {
  // EMPTY
}

void m19::type_checker::do_break_node(m19::break_node *const node, int lvl) {
  // EMPTY
}

void m19::type_checker::do_block_node(m19::block_node * const node, int lvl) {
  // EMPTY
}

void m19::type_checker::do_final_section_node(m19::final_section_node *const node, int lvl) {
  // EMPTY
}

void m19::type_checker::do_initial_section_node(m19::initial_section_node *const node, int lvl) {
  // EMPTY
}

void m19::type_checker::do_function_body_node(m19::function_body_node *const node, int lvl) {
  // EMPTY
}

//--------------------------------------------------------------------------------

void m19::type_checker::do_sequence_node(cdk::sequence_node * const node, int lvl) {
  for (size_t i = 0; i < node->size(); i++)
    node->node(i)->accept(this, lvl);
}

//--------------------------------------------------------------------------------

void m19::type_checker::do_integer_node(cdk::integer_node * const node, int lvl) {
  ASSERT_UNSPEC;
  node->type(new basic_type(4, basic_type::TYPE_INT));
}

void m19::type_checker::do_string_node(cdk::string_node * const node, int lvl) {
  ASSERT_UNSPEC;
  node->type(new basic_type(4, basic_type::TYPE_STRING));
}

void m19::type_checker::do_double_node(cdk::double_node * const node, int lvl) {
  ASSERT_UNSPEC;
  node->type(new basic_type(8, basic_type::TYPE_DOUBLE));
}

//--------------------------------------------------------------------------------

void m19::type_checker::do_not_node(cdk::not_node * const node, int lvl) {
  ASSERT_UNSPEC;
  context.push(new basic_type(4, basic_type::TYPE_INT));
  node->argument()->accept(this, lvl);
  basic_type* bt = context.top();
  context.pop();
  delete bt;

  if (node->argument()->type()->name() == basic_type::TYPE_INT) //just to be sure
    node->type(newBasicType(node->argument()->type()));
  else
    throw std::string("wrong type in argument of unary expression (boolean NOT)");
}

void m19::type_checker::do_neg_node(cdk::neg_node * const node, int lvl) {
  ASSERT_UNSPEC;
  node->argument()->accept(this, lvl);
  if (node->argument()->type()->name() == basic_type::TYPE_INT ||
      node->argument()->type()->name() == basic_type::TYPE_DOUBLE)
    node->type(newBasicType(node->argument()->type()));
  else
    throw std::string("wrong type in argument of unary expression (symmetric expression)");
}

void m19::type_checker::do_identity_node(m19::identity_node *const node, int lvl) {
  ASSERT_UNSPEC;
  node->argument()->accept(this, lvl);
  if (node->argument()->type()->name() == basic_type::TYPE_INT ||
      node->argument()->type()->name() == basic_type::TYPE_DOUBLE)
    node->type(newBasicType(node->argument()->type()));
  else
    throw std::string("wrong type in argument of unary expression (identity expression)");
}

void m19::type_checker::do_address_node(m19::address_node *const node, int lvl) {
  ASSERT_UNSPEC;

  node->lvalue()->accept(this, lvl);

  basic_type *bt = new basic_type(4, basic_type::TYPE_POINTER);
  bt->_subtype = newBasicType(node->lvalue()->type());
  node->type(bt);
}

//--------------------------------------------------------------------------------
void m19::type_checker::do_MulDivExpression(cdk::binary_expression_node * const node, int lvl) {
  ASSERT_UNSPEC;
  node->left()->accept(this, lvl + 2);
  node->right()->accept(this, lvl + 2);
  
  if((node->left()->type()->name() == basic_type::TYPE_DOUBLE && node->right()->type()->name() == basic_type::TYPE_INT)    || 
     (node->left()->type()->name() == basic_type::TYPE_INT    && node->right()->type()->name() == basic_type::TYPE_DOUBLE) ||
     (node->left()->type()->name() == basic_type::TYPE_DOUBLE && node->right()->type()->name() == basic_type::TYPE_DOUBLE))
    node->type(new basic_type(8, basic_type::TYPE_DOUBLE));

  else if(node->left()->type()->name() == basic_type::TYPE_INT && node->right()->type()->name() == basic_type::TYPE_INT)
    node->type(new basic_type(4, basic_type::TYPE_INT));

  else
    throw std::string("wrong type in argument of binary expression (multiplication/division/mod)");
}

void m19::type_checker::do_LogicalExpression(cdk::binary_expression_node * const node, int lvl) {
  ASSERT_UNSPEC;
  node->left()->accept(this, lvl + 2);
  node->right()->accept(this, lvl + 2);

  if (node->left()->type()->name() != basic_type::TYPE_INT || node->right()->type()->name() != basic_type::TYPE_INT)
    throw std::string("wrong type in argument of binary expression (logical OR/AND)");
  
  node->type(new basic_type(4, basic_type::TYPE_INT));
}

void m19::type_checker::do_ComparisonExpression(cdk::binary_expression_node * const node, int lvl) {
  ASSERT_UNSPEC;
  node->left()->accept(this, lvl + 2);
  node->right()->accept(this, lvl + 2);

  if ((node->left()->type()->name()  != basic_type::TYPE_INT && node->left()->type()->name()  != basic_type::TYPE_DOUBLE) || 
      (node->right()->type()->name() != basic_type::TYPE_INT && node->right()->type()->name() != basic_type::TYPE_DOUBLE))
    throw std::string("wrong type in argument of binary expression (boolean comparision)");

  node->type(new basic_type(4, basic_type::TYPE_INT));
}

void m19::type_checker::do_EqualityExpression(cdk::binary_expression_node * const node, int lvl) {
  ASSERT_UNSPEC;
  node->left()->accept(this, lvl + 2);
  node->right()->accept(this, lvl + 2);

  if ((node->left()->type()->name() != basic_type::TYPE_INT  && node->left()->type()->name() != basic_type::TYPE_DOUBLE 
    && node->left()->type()->name() != basic_type::TYPE_POINTER) || 
      (node->right()->type()->name() != basic_type::TYPE_INT && node->right()->type()->name() != basic_type::TYPE_DOUBLE
    && node->right()->type()->name() != basic_type::TYPE_POINTER))
    throw std::string("wrong type in argument of binary expression (equality/inequality expression)");

  node->type(new basic_type(4, basic_type::TYPE_INT));
}


void m19::type_checker::do_add_node(cdk::add_node * const node, int lvl) {
  ASSERT_UNSPEC;
  node->left()->accept(this, lvl + 2);
  node->right()->accept(this, lvl + 2);
  
  if((node->left()->type()->name() == basic_type::TYPE_DOUBLE && node->right()->type()->name() == basic_type::TYPE_INT)    || 
     (node->left()->type()->name() == basic_type::TYPE_INT    && node->right()->type()->name() == basic_type::TYPE_DOUBLE) ||
     (node->left()->type()->name() == basic_type::TYPE_DOUBLE && node->right()->type()->name() == basic_type::TYPE_DOUBLE))
    node->type(new basic_type(8, basic_type::TYPE_DOUBLE));

  else if (node->left()->type()->name() == basic_type::TYPE_POINTER && node->right()->type()->name() == basic_type::TYPE_INT)
    node->type(newBasicType(node->left()->type()));
  else if (node->left()->type()->name() == basic_type::TYPE_INT     && node->right()->type()->name() == basic_type::TYPE_POINTER)
    node->type(newBasicType(node->right()->type()));
  else if(node->left()->type()->name() == basic_type::TYPE_INT      && node->right()->type()->name() == basic_type::TYPE_INT)
    node->type(new basic_type(4, basic_type::TYPE_INT));

  else
    throw std::string("wrong type in argument of binary expression (add expression)");
}

void m19::type_checker::do_sub_node(cdk::sub_node * const node, int lvl) {
  ASSERT_UNSPEC;
  node->left()->accept(this, lvl + 2);
  node->right()->accept(this, lvl + 2);
  
  if((node->left()->type()->name() == basic_type::TYPE_DOUBLE && node->right()->type()->name() == basic_type::TYPE_INT)    || 
     (node->left()->type()->name() == basic_type::TYPE_INT    && node->right()->type()->name() == basic_type::TYPE_DOUBLE) ||
     (node->left()->type()->name() == basic_type::TYPE_DOUBLE && node->right()->type()->name() == basic_type::TYPE_DOUBLE))
    node->type(new basic_type(8, basic_type::TYPE_DOUBLE));

  else if (node->left()->type()->name() == basic_type::TYPE_POINTER && node->right()->type()->name() == basic_type::TYPE_INT)
    node->type(newBasicType(node->left()->type()));

  else if (node->left()->type()->name() == basic_type::TYPE_POINTER && node->right()->type()->name() == basic_type::TYPE_POINTER) {
    if(!checkPointers(node->left()->type(), node->right()->type()))
      throw std::string("wrong type in argument of binary expression (expression between pointers of different types)");
    node->type(new basic_type(4, basic_type::TYPE_INT));
  }

  else if(node->left()->type()->name() == basic_type::TYPE_INT && node->right()->type()->name() == basic_type::TYPE_INT)
    node->type(new basic_type(4, basic_type::TYPE_INT));

  else
    throw std::string("wrong type in argument of binary expression (sub expression)");
}



void m19::type_checker::do_mul_node(cdk::mul_node * const node, int lvl) {
  do_MulDivExpression(node, lvl);
}
void m19::type_checker::do_div_node(cdk::div_node * const node, int lvl) {
  do_MulDivExpression(node, lvl);
}
void m19::type_checker::do_mod_node(cdk::mod_node * const node, int lvl) {
  do_MulDivExpression(node, lvl);
  if (node->type()->name() != basic_type::TYPE_INT)
    throw std::string("wrong type in argument of binary expression (mod expression)");
}

void m19::type_checker::do_and_node(cdk::and_node * const node, int lvl) {
  do_LogicalExpression(node, lvl);
}
void m19::type_checker::do_or_node(cdk::or_node * const node, int lvl) {
  do_LogicalExpression(node, lvl);
}

void m19::type_checker::do_lt_node(cdk::lt_node * const node, int lvl) {
  do_ComparisonExpression(node, lvl);
}
void m19::type_checker::do_le_node(cdk::le_node * const node, int lvl) {
  do_ComparisonExpression(node, lvl);
}
void m19::type_checker::do_ge_node(cdk::ge_node * const node, int lvl) {
  do_ComparisonExpression(node, lvl);
}
void m19::type_checker::do_gt_node(cdk::gt_node * const node, int lvl) {
  do_ComparisonExpression(node, lvl);
}

void m19::type_checker::do_ne_node(cdk::ne_node * const node, int lvl) {
  do_EqualityExpression(node, lvl);
}
void m19::type_checker::do_eq_node(cdk::eq_node * const node, int lvl) {
  do_EqualityExpression(node, lvl);
}

//--------------------------------------------------------------------------------

void m19::type_checker::do_variable_node(cdk::variable_node * const node, int lvl) {
  ASSERT_UNSPEC;
  const std::string &id = node->name();
  std::shared_ptr<m19::symbol> symbol;

  if(id == "@" && function_name != "")
    symbol = _symtab.find(function_name);
  else
    symbol = _symtab.find(id);

  if (symbol != nullptr) {
    node->type(newBasicType(symbol->type()));
  } else {
    throw std::string("undeclared variable '" + id + "'");
  }
}

void m19::type_checker::do_rvalue_node(cdk::rvalue_node * const node, int lvl) {
  ASSERT_UNSPEC;

  node->lvalue()->accept(this, lvl);
  node->type(newBasicType(node->lvalue()->type()));

}

void m19::type_checker::do_assignment_node(cdk::assignment_node * const node, int lvl) {
  ASSERT_UNSPEC;

  node->lvalue()->accept(this, lvl+2);


  context.push(node->lvalue()->type());
  node->rvalue()->accept(this, lvl + 2);
  context.pop();

  if(node->lvalue()->type()->name() == basic_type::TYPE_POINTER && node->rvalue()->type()->name() == basic_type::TYPE_POINTER) {
    if(!checkPointers(node->lvalue()->type(), node->rvalue()->type()))
      throw std::string("wrong type in argument of assignment expression (assignment between pointers of different types)");
    node->type(newBasicType(node->lvalue()->type()));
  }

  else if (node->lvalue()->type()->name() == node->rvalue()->type()->name() || 
      (node->lvalue()->type()->name() == basic_type::TYPE_DOUBLE       &&
      node->rvalue()->type()->name() == basic_type::TYPE_INT))

    node->type(newBasicType(node->lvalue()->type()));

  else if (node->lvalue()->type()->name() == basic_type::TYPE_POINTER &&
           node->rvalue()->type()->name() == basic_type::TYPE_INT) {
    
    cdk::integer_node *in = dynamic_cast<cdk::integer_node*>(node->rvalue());
    if(!in || in->value() != 0)
      throw std::string("wrong type in right argument of assignment expression (integer assigned to a pointer must be the 0 literal)");
    
    node->type(newBasicType(node->lvalue()->type()));
  }

  else
    throw std::string("wrong type in right argument of assignment expression");

}

//--------------------------------------------------------------------------------

void m19::type_checker::do_evaluation_node(m19::evaluation_node * const node, int lvl) {
  node->argument()->accept(this, lvl + 2);
}

void m19::type_checker::do_print_node(m19::print_node * const node, int lvl) {
  context.push(new basic_type(4, basic_type::TYPE_INT));
  node->argument()->accept(this, lvl + 2);
  basic_type* bt = context.top();
  context.pop();
  delete bt;

  if (node->argument()->type()->name() != basic_type::TYPE_INT    &&
      node->argument()->type()->name() != basic_type::TYPE_DOUBLE &&
      node->argument()->type()->name() != basic_type::TYPE_STRING)
    throw std::string("wrong type in argument of print statement");
}

//--------------------------------------------------------------------------------

void m19::type_checker::do_read_node(m19::read_node * const node, int lvl) {
  ASSERT_UNSPEC;
  if(context.empty() || context.top()->name() == basic_type::TYPE_POINTER)
    node->type(new basic_type(4, basic_type::TYPE_INT));
  else if(context.top()->name() == basic_type::TYPE_DOUBLE || context.top()->name() == basic_type::TYPE_INT)
    node->type(newBasicType(context.top()));
  else
    throw std::string("wrong type in argument of read statement");
}

//--------------------------------------------------------------------------------

void m19::type_checker::do_for_node(m19::for_node * const node, int lvl) {
  node->condition()->accept(this, lvl + 4);
  for (size_t i = 0; i < node->condition()->size(); i++) {
    cdk::expression_node *en = dynamic_cast<cdk::expression_node*>(node->condition()->node(i));
    if(en->type()->name() != basic_type::TYPE_INT && en->type()->name() != basic_type::TYPE_DOUBLE && 
       en->type()->name() != basic_type::TYPE_STRING && en->type()->name() != basic_type::TYPE_POINTER)
      throw std::string("wrong type in condition statement");
  }
}

//--------------------------------------------------------------------------------

void m19::type_checker::do_if_node(m19::if_node * const node, int lvl) {
  node->condition()->accept(this, lvl + 4);
  if(node->condition()->type()->name() != basic_type::TYPE_INT && node->condition()->type()->name() != basic_type::TYPE_DOUBLE && 
     node->condition()->type()->name() != basic_type::TYPE_STRING && node->condition()->type()->name() != basic_type::TYPE_POINTER)
    throw std::string("wrong type in condition statement");
}

void m19::type_checker::do_if_else_node(m19::if_else_node * const node, int lvl) {
  node->condition()->accept(this, lvl + 4);
  if(node->condition()->type()->name() != basic_type::TYPE_INT && node->condition()->type()->name() != basic_type::TYPE_DOUBLE && 
     node->condition()->type()->name() != basic_type::TYPE_STRING && node->condition()->type()->name() != basic_type::TYPE_POINTER)
    throw std::string("wrong type in condition statement");
}

//--------------------------------------------------------------------------------

void m19::type_checker::do_function_declaration_node(m19::function_declaration_node * const node, int lvl) {
  std::string id = node->identifier();
  std::shared_ptr<m19::symbol> symbol = std::make_shared<m19::symbol>(newBasicType(node->type()), id, node->qualifier(),true);

  if(id == "m19")
    throw std::string("name \"m19\" is a reserved word for the main function");
  else if (id == "_main")
    id = "._main";

  if (!_symtab.insert(id, symbol))
    throw std::string("'" + id + "' redeclared");
  
  if (node->arguments() != nullptr){
    std::vector<basic_type *> arguments;
    for(size_t i = 0; i < node->arguments()->size() ; i++){
      m19::variable_declaration_node *vd = dynamic_cast<m19::variable_declaration_node*>(node->arguments()->node(i));
      arguments.push_back(newBasicType(vd->type()));
    }
    symbol->setArguments(arguments);
  }

  _parent->set_new_symbol(symbol);
}

void m19::type_checker::do_function_definition_node(m19::function_definition_node * const node, int lvl) {
  std::string id = node->identifier();

  //m19 must be public and return an integer
  if(id == "m19"){
    if(node->qualifier() == 1 && !node->arguments())
      id = "_main";
    else
      throw std::string("wrong usage of m19 function");
  }else if (id == "_main")
    id = "._main";

  function_name = id;

  std::shared_ptr<m19::symbol> function = std::make_shared<m19::symbol>(newBasicType(node->type()), id, node->qualifier(), true, true);
  std::shared_ptr<m19::symbol> symbolFound = _symtab.find(id);


  if(node->return_value()) {
    node->return_value()->accept(this, lvl + 2);

    if (node->type()->name() == basic_type::TYPE_POINTER &&
        node->return_value()->type()->name() == basic_type::TYPE_INT) {
    
      cdk::integer_node *in = dynamic_cast<cdk::integer_node*>(node->return_value());
      if(!in || in->value() != 0)
        throw std::string("conflict between return value and return type (integer assigned to a pointer must be the 0 literal)");
    }
    
    else if(node->type()->name() != node->return_value()->type()->name() &&
      (node->type()->name() == basic_type::TYPE_DOUBLE && node->return_value()->type()->name() != basic_type::TYPE_INT))
        throw std::string("conflict between return value and return type");
  }

  if(symbolFound && symbolFound->isFunction() && !symbolFound->initialized()){

    if(node->qualifier() != symbolFound->qualifier() && node->type()->name() != symbolFound->type()->name())
      throw std::string("conflicting function definition '" + id + "'");

    size_t n = 0;

    if (node->arguments() != nullptr) {
      if(node->arguments()->size() == symbolFound->arguments().size())
        for(size_t i = 0; i < node->arguments()->size() ; i++){
          m19::variable_declaration_node *vd = dynamic_cast<m19::variable_declaration_node*>(node->arguments()->node(i));

          if(vd->type()->name() == symbolFound->arguments()[i]->name())
            if(checkPointers(vd->type(), symbolFound->arguments()[i]))
              n++;
        }

      if(n != node->arguments()->size())
        throw std::string("invalid arguments in function definition '" + id + "'");
    }

    if(node->arguments() == nullptr && !symbolFound->arguments().empty())
      throw std::string("conflicting definition for '" + id + "'");

    _symtab.replace(id, function);

  }
  
  else {
    if(!_symtab.insert(id, function))
      throw std::string("conflicting definition for '" + id + "'");
  }

  if (node->arguments() != nullptr ){
    std::vector<basic_type *> arguments;
    for(size_t i = 0; i < node->arguments()->size() ; i++){
      m19::variable_declaration_node *vd = dynamic_cast<m19::variable_declaration_node*>(node->arguments()->node(i));
      arguments.push_back(newBasicType(vd->type()));
    }
    function->setArguments(arguments);
  }

  _parent->set_new_symbol(function);

}

void m19::type_checker::do_function_call_node(m19::function_call_node * const node, int lvl) {
  ASSERT_UNSPEC;
  const std::string &id = node->identifier();
  std::shared_ptr<m19::symbol> symbol;
  if(node->identifier() == "@" && function_name != "")
    symbol = _symtab.find(function_name);
  else
    symbol = _symtab.find(id);

  if(!symbol->isFunction())
    throw std::string("symbol \"" + id + "\" is not a function");

  if (symbol != nullptr)
    node->type(newBasicType(symbol->type()));
  else
    throw std::string("undeclared function '" + id + "' or is not a function");
  
  size_t n = 0;

  if (node->arguments() && node->arguments()->size() == symbol->arguments().size())
    for(size_t i = 0; i < node->arguments()->size() ; i++){
      context.push(symbol->arguments()[i]);
      node->arguments()->node(i)->accept(this, lvl + 2);
      context.pop();

      cdk::expression_node *en = dynamic_cast<cdk::expression_node*>(node->arguments()->node(i));
      if((en->type()->name() == symbol->arguments()[i]->name()) ||
         (symbol->arguments()[i]->name() == basic_type::TYPE_DOUBLE && en->type()->name() == basic_type::TYPE_INT))
          if(checkPointers(en->type(), symbol->arguments()[i]))
            n++;
    }

  if(n != node->arguments()->size())
    throw std::string("invalid arguments in function call '" + id + "'");
  else
    node->arguments()->accept(this, lvl + 4);

}

void m19::type_checker::do_variable_declaration_node(m19::variable_declaration_node * const node, int lvl) {
  
  const std::string &id = node->identifier();
  
  std::shared_ptr<m19::symbol> symbol = std::make_shared<m19::symbol>(newBasicType(node->type()), id);

  if (!_symtab.insert(id, symbol))
    throw std::string("'" + id + "' redeclared");

  if (node->initializer() != nullptr) {
    
    context.push(newBasicType(node->type()));
    node->initializer()->accept(this, lvl + 2);
    context.pop();


    if(node->type()->name() == basic_type::TYPE_INT) {
      if(node->initializer()->type()->name() != basic_type::TYPE_INT)
        throw std::string("wrong type for initializer (integer expected).");
    }

    else if(node->type()->name() == basic_type::TYPE_DOUBLE) {
      if(node->initializer()->type()->name() != basic_type::TYPE_INT && node->initializer()->type()->name() != basic_type::TYPE_DOUBLE)
        throw std::string("wrong type for initializer (integer or double expected).");
    }

    else if(node->type()->name() == basic_type::TYPE_STRING) {
      if (node->initializer()->type()->name() != basic_type::TYPE_STRING)
        throw std::string("wrong type for initializer (string expected).");
    }

    else if(node->type()->name() == basic_type::TYPE_POINTER) {
      if (node->initializer()->type()->name() == basic_type::TYPE_POINTER) {
        if(!checkPointers(node->type(), node->initializer()->type()))
          throw std::string("wrong type for initializer (pointers have different subtypes).");
      }
      else if (node->initializer()->type()->name() == basic_type::TYPE_INT) {
        cdk::integer_node *in = dynamic_cast<cdk::integer_node*>(node->initializer());
        if(!in || in->value() != 0)
          throw std::string("wrong type for initializer (integer assigned to a pointer must be the 0 literal)");
      }
      else
         throw std::string("wrong type for initializer (pointer or 0 expected).");
    }

    else
      throw std::string("unknown type for initializer.");
  }
  _parent->set_new_symbol(symbol);
}

void m19::type_checker::do_memory_alloc_node(m19::memory_alloc_node *const node, int lvl) {
  ASSERT_UNSPEC;
  context.push(new basic_type(4, basic_type::TYPE_INT));
  node->argument()->accept(this, lvl + 4);
  basic_type* bt = context.top();
  context.pop();
  delete bt;

  if (node->argument()->type()->name() != basic_type::TYPE_INT)
    throw std::string("wrong type in argument of a memory allocation statement");

  bt = new basic_type(4, basic_type::TYPE_POINTER);
  if(context.empty())
    bt->_subtype = new basic_type(1, basic_type::TYPE_UNSPEC);
  else
    bt->_subtype = newBasicType(context.top()->subtype());
  node->type(bt);
}

void m19::type_checker::do_index_node(m19::index_node *const node, int lvl) {
  ASSERT_UNSPEC;
  if(!context.empty()) {
    basic_type *bt = new basic_type(4, basic_type::TYPE_POINTER);
    bt->_subtype = newBasicType(context.top());
    context.push(bt);
  }
  node->base()->accept(this, lvl + 4);
  if(!context.empty()) {
    basic_type* bt = context.top();
    context.pop();
    delete bt;
  }
  if (node->base()->type()->name() != basic_type::TYPE_POINTER)
    throw std::string("wrong type in the base of an index statement");

  context.push(new basic_type(4, basic_type::TYPE_INT));
  node->index()->accept(this, lvl + 4);
  basic_type* bt = context.top();
  context.pop();
  delete bt;

  if (node->index()->type()->name() != basic_type::TYPE_INT)
    throw std::string("wrong type in the index of an index statement");
  node->type(newBasicType(node->base()->type()->subtype()));
}

void m19::type_checker::do_section_node(m19::section_node *const node, int lvl) {
  if(node->expression()) {
    node->expression()->accept(this, lvl + 4);
    if(node->expression()->type()->name() != basic_type::TYPE_INT && node->expression()->type()->name() != basic_type::TYPE_DOUBLE)
      throw std::string("wrong type in condition statement");
  }
}