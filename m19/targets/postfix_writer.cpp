#include <string>
#include <sstream>
#include "targets/type_checker.h"
#include "targets/frame_size_calculator.h"
#include "targets/postfix_writer.h"
#include "ast/all.h"  // all.h is automatically generated

//-----------------------------------AUX------------------------------------------

void m19::postfix_writer::do_ComparisonExpression(cdk::binary_expression_node * const node, int lvl) {
  do_I2DExpression(node, lvl);
  
  if(node->type()->name() == basic_type::TYPE_DOUBLE) {
    _pf.DCMP();
    _pf.INT(0);
  }
}

void m19::postfix_writer::do_I2DExpression(cdk::binary_expression_node * const node, int lvl) {
  node->left()->accept(this, lvl);
  if(node->right()->type()->name() == basic_type::TYPE_DOUBLE && (node->left()->type()->name() == basic_type::TYPE_INT || node->left()->type()->name() == basic_type::TYPE_POINTER))
    _pf.I2D();

  node->right()->accept(this, lvl);
  if(node->left()->type()->name() == basic_type::TYPE_DOUBLE && (node->right()->type()->name() == basic_type::TYPE_INT || node->right()->type()->name() == basic_type::TYPE_POINTER))
    _pf.I2D();
}

void m19::postfix_writer::do_addsubExpression(cdk::binary_expression_node * const node, int lvl) {
  
  node->left()->accept(this, lvl);
  if(node->type()->name() == basic_type::TYPE_DOUBLE && node->left()->type()->name() == basic_type::TYPE_INT)
    _pf.I2D();
  else if(node->type()->name() == basic_type::TYPE_POINTER && node->left()->type()->name() == basic_type::TYPE_INT) {
    if(node->type()->subtype()->name() == basic_type::TYPE_DOUBLE)
      _pf.INT(3);
    else
      _pf.INT(2);
    _pf.SHTL();
  }

  node->right()->accept(this, lvl);
  if(node->type()->name() == basic_type::TYPE_DOUBLE && node->right()->type()->name() == basic_type::TYPE_INT)
    _pf.I2D();
  else if(node->type()->name() == basic_type::TYPE_POINTER && node->right()->type()->name() == basic_type::TYPE_INT) {
    if(node->type()->subtype()->name() == basic_type::TYPE_DOUBLE)
      _pf.INT(3);
    else
      _pf.INT(2);
    _pf.SHTL();
  }
}

//--------------------------------------------------------------------------------

void m19::postfix_writer::do_nil_node(cdk::nil_node * const node, int lvl) {
  // EMPTY
}
void m19::postfix_writer::do_data_node(cdk::data_node * const node, int lvl) {
  // EMPTY
}

void m19::postfix_writer::do_and_node(cdk::and_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  int lbl = ++_lbl;
  node->left()->accept(this, lvl + 2);
  _pf.DUP32();
  _pf.JZ(mklbl(lbl));
  node->right()->accept(this, lvl + 2);
  _pf.AND();
  _pf.ALIGN();
  _pf.LABEL(mklbl(lbl));
}
void m19::postfix_writer::do_or_node(cdk::or_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  int lbl = ++_lbl;
  node->left()->accept(this, lvl + 2);
  _pf.DUP32();
  _pf.JNZ(mklbl(lbl));
  node->right()->accept(this, lvl + 2);
  _pf.OR();
  _pf.ALIGN();
  _pf.LABEL(mklbl(lbl));
}

//--------------------------------------------------------------------------------

void m19::postfix_writer::do_sequence_node(cdk::sequence_node * const node, int lvl) {
  for (size_t i = 0; i < node->size(); i++) {
    node->node(i)->accept(this, lvl);
  }
}

//--------------------------------------------------------------------------------

void m19::postfix_writer::do_integer_node(cdk::integer_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  if(_inFunction)
    _pf.INT(node->value());
  else
    _pf.SINT(node->value());
}

void m19::postfix_writer::do_double_node(cdk::double_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  if(_inFunction)
    _pf.DOUBLE(node->value());
  else
    _pf.SDOUBLE(node->value());
}

void m19::postfix_writer::do_string_node(cdk::string_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  int lbl1;

  _pf.RODATA();
  _pf.ALIGN();
  _pf.LABEL(mklbl(lbl1 = ++_lbl));
  _pf.SSTRING(node->value());

  if (_inFunction) {
    _pf.TEXT();
    _pf.ADDR(mklbl(lbl1));
  } else {
    _pf.DATA();
    _pf.SADDR(mklbl(lbl1));
  }
}

//--------------------------------------------------------------------------------

void m19::postfix_writer::do_neg_node(cdk::neg_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->argument()->accept(this, lvl);
  _pf.NEG(); // 2-complement
}

void m19::postfix_writer::do_not_node(cdk::not_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->argument()->accept(this, lvl + 2);
  _pf.NOT();
}

//--------------------------------------------------------------------------------

void m19::postfix_writer::do_add_node(cdk::add_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  do_addsubExpression(node, lvl);

  if (node->type()->name() == basic_type::TYPE_DOUBLE)
    _pf.DADD();
  else
    _pf.ADD();
}

void m19::postfix_writer::do_sub_node(cdk::sub_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  do_addsubExpression(node, lvl);

  int lbl = ++_lbl;
  if(node->left()->type()->name() == basic_type::TYPE_POINTER && node->right()->type()->name() == basic_type::TYPE_POINTER) {
    _pf.SUB();
    _pf.DUP32();
    _pf.INT(0);
    _pf.LT();
    _pf.JZ(mklbl(lbl));
    _pf.NEG();
    _pf.ALIGN();
    _pf.LABEL(mklbl(lbl));
    if(node->left()->type()->subtype()->name() == basic_type::TYPE_DOUBLE)
      _pf.INT(3);
    else
      _pf.INT(2);
    _pf.SHTRU();
  }
  else if (node->type()->name() == basic_type::TYPE_DOUBLE)
    _pf.DSUB();
  else
    _pf.SUB();
}

void m19::postfix_writer::do_mul_node(cdk::mul_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  do_I2DExpression(node, lvl);

  if (node->type()->name() == basic_type::TYPE_DOUBLE)
  _pf.DMUL();
  else
  _pf.MUL();
}

void m19::postfix_writer::do_div_node(cdk::div_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  do_I2DExpression(node, lvl);

  if (node->type()->name() == basic_type::TYPE_DOUBLE)
    _pf.DDIV();
  else
    _pf.DIV();
}

void m19::postfix_writer::do_mod_node(cdk::mod_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->left()->accept(this, lvl);
  node->right()->accept(this, lvl);
  _pf.MOD();
}

void m19::postfix_writer::do_lt_node(cdk::lt_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  do_ComparisonExpression(node, lvl);
  _pf.LT();
}
void m19::postfix_writer::do_le_node(cdk::le_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  do_ComparisonExpression(node, lvl);
  _pf.LE();
}
void m19::postfix_writer::do_ge_node(cdk::ge_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  do_ComparisonExpression(node, lvl);
  _pf.GE();
}
void m19::postfix_writer::do_gt_node(cdk::gt_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  do_ComparisonExpression(node, lvl);
  _pf.GT();
}
void m19::postfix_writer::do_ne_node(cdk::ne_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  do_ComparisonExpression(node, lvl);
  _pf.NE();
}
void m19::postfix_writer::do_eq_node(cdk::eq_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  do_ComparisonExpression(node, lvl);
  _pf.EQ();
}

//--------------------------------------------------------------------------------

void m19::postfix_writer::do_variable_node(cdk::variable_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  const std::string &id = node->name();
  std::shared_ptr<m19::symbol> symbolFound = _symtab.find(id);
  if(symbolFound) {
    if(symbolFound->offset() != 0)
      _pf.LOCAL(symbolFound->offset());
    else
      _pf.ADDR(symbolFound->name());
  } else if (id == "@" && _inFunction) {
      _pf.LOCAL(-node->type()->size());
  } else {
    std::cerr << "ERROR: CANNOT HAPPEN!" << std::endl;
    exit(1);
  }
}

void m19::postfix_writer::do_rvalue_node(cdk::rvalue_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->lvalue()->accept(this, lvl);

  if(node->type()->name() == basic_type::TYPE_INT || node->type()->name() == basic_type::TYPE_STRING || 
      node->type()->name() == basic_type::TYPE_POINTER)
    _pf.LDINT();

  else if(node->type()->name() == basic_type::TYPE_DOUBLE)
    _pf.LDDOUBLE();

  else {
    std::cerr << "ERROR: CANNOT HAPPEN!" << std::endl;
    exit(1);
  }
}

void m19::postfix_writer::do_assignment_node(cdk::assignment_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->rvalue()->accept(this, lvl + 2);
  if (node->type()->name() == basic_type::TYPE_DOUBLE) {
    if (node->rvalue()->type()->name() == basic_type::TYPE_INT)
      _pf.I2D();
    _pf.DUP64();
  } else {
    _pf.DUP32();
  }

  node->lvalue()->accept(this, lvl);

  if (node->type()->name() == basic_type::TYPE_DOUBLE)
    _pf.STDOUBLE();
  else
    _pf.STINT();
}

//--------------------------------------------------------------------------------

void m19::postfix_writer::do_evaluation_node(m19::evaluation_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->argument()->accept(this, lvl); // determine the value
  _pf.TRASH(node->argument()->type()->size()); // delete the evaluated value
}

void m19::postfix_writer::do_print_node(m19::print_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->argument()->accept(this, lvl); // determine the value to print
  if (node->argument()->type()->name() == basic_type::TYPE_INT) {
    _functions_to_declare.insert("printi");
    _pf.CALL("printi");
    _pf.TRASH(4); // delete the printed value
  } else if (node->argument()->type()->name() == basic_type::TYPE_STRING) {
    _functions_to_declare.insert("prints");
    _pf.CALL("prints");
    _pf.TRASH(4); // delete the printed value's address
  }else if (node->argument()->type()->name() == basic_type::TYPE_DOUBLE) {
    _functions_to_declare.insert("printd");
    _pf.CALL("printd");
    _pf.TRASH(8); // delete the printed value's address
  } else {
    std::cerr << "ERROR: CANNOT HAPPEN!" << std::endl;
    exit(1);
  }
  if(node->newline()){
    _functions_to_declare.insert("println");
    _pf.CALL("println"); // print a newline
  }
}

//--------------------------------------------------------------------------------

void m19::postfix_writer::do_read_node(m19::read_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  if(node->type()->name() == basic_type::TYPE_INT) {
    _functions_to_declare.insert("readi");
    _pf.CALL("readi");
    _pf.LDFVAL32();
  } else if (node->type()->name() == basic_type::TYPE_DOUBLE) {
    _functions_to_declare.insert("readd");
    _pf.CALL("readd");
    _pf.LDFVAL64();
  } else {
    std::cerr << "ERROR: CANNOT HAPPEN!" << std::endl;
    exit(1);
  }
}

//--------------------------------------------------------------------------------

void m19::postfix_writer::do_for_node(m19::for_node * const node, int lvl) {

  _symtab.push();
  int lbl1 = ++_lbl, lbl2 = ++_lbl, lbl3 = ++_lbl;
  _forIni.push(lbl1);
  _forStep.push(lbl2);
  _forEnd.push(lbl3);

  for (size_t i = 0; i < node->declaration()->size(); i++) {
    node->declaration()->node(i)->accept(this, lvl);
    cdk::expression_node *en = dynamic_cast<cdk::expression_node*>(node->declaration()->node(i));
    if(en)
      _pf.TRASH(en->type()->size());
  }
  ASSERT_SAFE_EXPRESSIONS;

  _pf.ALIGN();
  _pf.LABEL(mklbl(_forIni.top()));

  if(node->condition()->size() == 0)
    _pf.INT(1);
  else
    for (size_t i = 0; i < node->condition()->size(); i++) {
      cdk::expression_node *en = dynamic_cast<cdk::expression_node*>(node->condition()->node(i));
      en->accept(this, lvl);
      if(en->type()->name() == basic_type::TYPE_DOUBLE) {
        _pf.DOUBLE(0);
        _pf.DCMP();
      }
      if(i < node->condition()->size() - 1)
        _pf.TRASH(4);
    }
  _pf.JZ(mklbl(_forEnd.top()));

  node->instruction()->accept(this, lvl + 2);
  
  _pf.ALIGN();
  _pf.LABEL(mklbl(_forStep.top()));
  for (size_t i = 0; i < node->iteration()->size(); i++) {
    cdk::expression_node *en = dynamic_cast<cdk::expression_node*>(node->iteration()->node(i));
    en->accept(this, lvl);
    _pf.TRASH(en->type()->size());
  }
  _pf.JMP(mklbl(_forIni.top()));

  _pf.ALIGN();
  _pf.LABEL(mklbl(_forEnd.top()));

  _symtab.pop();

  _forIni.pop();
  _forStep.pop();
  _forEnd.pop();
}

//--------------------------------------------------------------------------------

void m19::postfix_writer::do_if_node(m19::if_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  int lbl1;
  node->condition()->accept(this, lvl);
  if(node->condition()->type()->name() == basic_type::TYPE_DOUBLE) {
    _pf.DOUBLE(0);
    _pf.DCMP();
  }
  _pf.JZ(mklbl(lbl1 = ++_lbl));
  node->block()->accept(this, lvl + 2);
  _pf.ALIGN();
  _pf.LABEL(mklbl(lbl1));
}

//--------------------------------------------------------------------------------

void m19::postfix_writer::do_if_else_node(m19::if_else_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  int lbl1, lbl2;
  node->condition()->accept(this, lvl);
  if(node->condition()->type()->name() == basic_type::TYPE_DOUBLE) {
    _pf.DOUBLE(0);
    _pf.DCMP();
  }
  _pf.JZ(mklbl(lbl1 = ++_lbl));
  node->thenblock()->accept(this, lvl + 2);
  _pf.JMP(mklbl(lbl2 = ++_lbl));
  _pf.ALIGN();
  _pf.LABEL(mklbl(lbl1));
  node->elseblock()->accept(this, lvl + 2);
  _pf.ALIGN();
  _pf.LABEL(mklbl(lbl2));
}

//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------

void m19::postfix_writer::do_function_declaration_node(m19::function_declaration_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  if (_inFunctionBody || _inFunctionArgs) {
    std::cerr << node->lineno() <<  ": cannot declare function in body" << std::endl;
    return;
  }

  std::set<std::string>::iterator it = _functions_to_declare.find(node->identifier());
  if(it == _functions_to_declare.end())
    _functions_to_declare.insert(node->identifier());
}

void m19::postfix_writer::do_function_definition_node(m19::function_definition_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  if (_inFunctionBody || _inFunctionArgs) {
    std::cerr << node->lineno() <<  ": cannot define function in body or in arguments" << std::endl;
    return;
  }

  functionLabel = 0;

  _inFunction = true;

  function_name = new_symbol()->name();
  reset_new_symbol();

  _functions_to_declare.erase(function_name);

  _offset = 8;

  _pf.TEXT();
  _pf.ALIGN();
  if (node->qualifier() == 1) _pf.GLOBAL(function_name, _pf.FUNC());
  _pf.LABEL(function_name);

  _symtab.push();
  _inFunctionArgs = true;
  if(node->arguments())
    node->arguments()->accept(this, lvl + 2);
  _inFunctionArgs = false;

  _offset = -node->type()->size();


  frame_size_calculator lsc(_compiler, _symtab);
  node->accept(&lsc, lvl);
  _pf.ENTER(lsc.localsize());


  if(node->return_value()) {
    node->return_value()->accept(this, lvl + 2);
    if(node->return_value()->type()->name() == basic_type::TYPE_INT || node->return_value()->type()->name() == basic_type::TYPE_STRING || 
      node->return_value()->type()->name() == basic_type::TYPE_POINTER) {
        if(node->type()->name() == basic_type::TYPE_DOUBLE) {
          _pf.I2D();
          _pf.LOCAL(-8);
          _pf.STDOUBLE();
        }
        else {
          _pf.LOCAL(-4);
          _pf.STINT();
        }

      }
    else if (node->return_value()->type()->name() == basic_type::TYPE_DOUBLE) {
      _pf.LOCAL(-8);
      _pf.STDOUBLE();
    }
  }
  else if(node->type()->name() == basic_type::TYPE_INT || node->type()->name() == basic_type::TYPE_POINTER) {
    _pf.INT(0);
    _pf.LOCAL(-4);
    _pf.STINT();
  }

  _inFunctionBody = true;
  node->body()->accept(this, lvl + 2);
  _inFunctionBody = false;

  _symtab.pop();

  _inFunction = false;

  if(node->type()->name() == basic_type::TYPE_VOID);
    
  else if(node->type()->name() == basic_type::TYPE_DOUBLE) {
    _pf.LOCAL(-8);
    _pf.LDDOUBLE();
    _pf.STFVAL64();
  }
  else {
    _pf.LOCAL(-4);
    _pf.LDINT();
    _pf.STFVAL32();
  }

  _pf.LEAVE();
  _pf.RET();

  if(function_name == "_main")
    for(std::string s: _functions_to_declare)
      _pf.EXTERN(s);

  function_name = "";

}

void m19::postfix_writer::do_function_call_node(m19::function_call_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  std::string id = node->identifier();
  std::shared_ptr<m19::symbol> symbol;

  if(id == "@" && function_name != "")
    symbol = _symtab.find(function_name);
  else
    symbol = _symtab.find(id);

  size_t argsSize = 0;
  if (node->arguments()) {
    for (size_t i = node->arguments()->size(); i > 0; i--) {
      node->arguments()->node(i-1)->accept(this, lvl + 2);
      cdk::expression_node *en = dynamic_cast<cdk::expression_node*>(node->arguments()->node(i-1));
      if(en->type()->name() == basic_type::TYPE_INT && symbol->arguments()[i-1]->name() == basic_type::TYPE_DOUBLE) {
        _pf.I2D();
        argsSize += 8;
      }
      else
        argsSize += en->type()->size();
    }
  }

  _pf.CALL(symbol->name());

  if (argsSize != 0) {
    _pf.TRASH(argsSize);
  }

  basic_type *type = symbol->type();
  if (type->name() == basic_type::TYPE_INT ||
      type->name() == basic_type::TYPE_POINTER ||
      type->name() == basic_type::TYPE_STRING)

    _pf.LDFVAL32();

  else if (type->name() == basic_type::TYPE_DOUBLE)
    _pf.LDFVAL64();
}

void m19::postfix_writer::do_block_node(m19::block_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  bool pop = inInitialSection;
  if(!inInitialSection)
    _symtab.push();
  if(node->declarations())
    node->declarations()->accept(this, lvl + 2);
  inInitialSection = false;
  if(node->instructions())
    node->instructions()->accept(this, lvl + 2);

  if(!pop)
    _symtab.pop();
}

void m19::postfix_writer::do_return_node(m19::return_node *const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  _pf.JMP(function_name + "end");
}

void m19::postfix_writer::do_continue_node(m19::continue_node *const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  if (_forIni.size() != 0)
    _pf.JMP(mklbl(_forStep.top()));
  else
    std::cerr << node->lineno() << ": 'restart' outside 'for'" << std::endl;
}

void m19::postfix_writer::do_break_node(m19::break_node *const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  if (_forIni.size() != 0)
    _pf.JMP(mklbl(_forEnd.top()));
  else
    std::cerr << node->lineno() << ": 'break' outside 'for'" << std::endl;
}

void m19::postfix_writer::do_variable_declaration_node(m19::variable_declaration_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  
  const std::string &id = node->identifier();
  std::shared_ptr<m19::symbol> symbol;

  int offset = 0, typesize = node->type()->size();
  if (_inFunctionBody) {
    _offset -= typesize;
    offset = _offset;
  } else if (_inFunctionArgs) {
    offset = _offset;
    _offset += typesize;
  } else
    offset = 0;

  symbol = new_symbol();
  if (symbol) {
    symbol->setOffset(offset);
    reset_new_symbol();
  }
  else
    std::cerr << "'" << id << "' redeclared" << std::endl;

  if (_inFunctionBody) {
    if (node->initializer() != nullptr) {
      node->initializer()->accept(this, lvl + 2);

      if (node->type()->name() == basic_type::TYPE_INT ||
          node->type()->name() == basic_type::TYPE_STRING ||
          node->type()->name() == basic_type::TYPE_POINTER) {

        _pf.LOCAL(symbol->offset());
        _pf.STINT();

      } 
      else if (node->type()->name() == basic_type::TYPE_DOUBLE) {

        if(node->initializer()->type()->name() == basic_type::TYPE_INT)
          _pf.I2D();

        _pf.LOCAL(symbol->offset());
        _pf.STDOUBLE();

      } 
      else {
        std::cerr << "cannot initialize" << std::endl;
      }
    }

  } 
  else if (!_inFunction) {
    if (node->initializer() == nullptr) {
      if(node->qualifier() == 2)
        _pf.EXTERN(id);
      else {
        _pf.BSS();
        _pf.ALIGN();
        if(node->qualifier() == 1) _pf.GLOBAL(id, _pf.OBJ());
        _pf.LABEL(id);
        _pf.SALLOC(typesize);
      }
    }
    else {

      if (node->type()->name() == basic_type::TYPE_INT || node->type()->name() == basic_type::TYPE_DOUBLE || node->type()->name() == basic_type::TYPE_POINTER) {

        _pf.DATA();
        _pf.ALIGN();
        if(node->qualifier() == 1) _pf.GLOBAL(id, _pf.OBJ());
        _pf.LABEL(id);

        if (node->type()->name() == basic_type::TYPE_INT || node->type()->name() == basic_type::TYPE_POINTER)
          node->initializer()->accept(this, lvl + 2);

        else if (node->type()->name() == basic_type::TYPE_DOUBLE) {

          if (node->initializer()->type()->name() == basic_type::TYPE_DOUBLE)
            node->initializer()->accept(this, lvl + 2);

          else if (node->initializer()->type()->name() == basic_type::TYPE_INT) {
            cdk::integer_node *dclini = dynamic_cast<cdk::integer_node *>(node->initializer());
            cdk::double_node ddi(dclini->lineno(), dclini->value());
            ddi.accept(this, lvl);
          }
          else
            std::cerr << node->lineno() << ": '" << id << "' has bad initializer for real value" << std::endl;
        }

      }
      else if (node->type()->name() == basic_type::TYPE_STRING) {
        
        _pf.DATA();
        _pf.ALIGN();
        _pf.LABEL(id);
        node->initializer()->accept(this, lvl + 2);
      } 
      else 
        std::cerr << node->lineno() << ": '" << id << "' has unexpected initializer" << std::endl;
    }
  }
}

void m19::postfix_writer::do_memory_alloc_node(m19::memory_alloc_node *const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->argument()->accept(this, lvl + 2);
  if(node->type()->subtype()->name() == basic_type::TYPE_DOUBLE)
    _pf.INT(3);
  else if (node->type()->subtype()->name() == basic_type::TYPE_INT || node->type()->subtype()->name() == basic_type::TYPE_POINTER)
    _pf.INT(2);
  else if (node->type()->subtype()->name() == basic_type::TYPE_UNSPEC)
    _pf.INT(0);
  else {
    std::cerr << "ERROR: CANNOT HAPPEN!" << std::endl;
    exit(1);
  }

  _pf.SHTL();
  _pf.ALLOC();
  _pf.SP();

}

void m19::postfix_writer::do_address_node(m19::address_node *const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->lvalue()->accept(this, lvl + 2);
}

void m19::postfix_writer::do_index_node(m19::index_node *const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->base()->accept(this, lvl);
  node->index()->accept(this, lvl);
  
  _pf.INT(node->type()->size());
  _pf.MUL();
  _pf.ADD();
}

void m19::postfix_writer::do_function_body_node(m19::function_body_node *const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  _symtab.push();
  if(node->initial_section())
    node->initial_section()->accept(this, lvl + 2);
  if(node->sections())
    node->sections()->accept(this, lvl + 2);
  
  _pf.ALIGN();
  _pf.LABEL(function_name + std::to_string(functionLabel));
  _pf.ALIGN();
  _pf.LABEL(function_name + "end");

  if(node->final_section())
    node->final_section()->accept(this, lvl + 2);
  _symtab.pop();
}

void m19::postfix_writer::do_section_node(m19::section_node *const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  _pf.ALIGN();
  _pf.LABEL(function_name + std::to_string(functionLabel));
  if(node->expression()) {
    node->expression()->accept(this, lvl + 2);
    if(node->expression()->type()->name() == basic_type::TYPE_DOUBLE) {
      _pf.DOUBLE(0);
      _pf.DCMP();
    }
  }
  else
    _pf.INT(1);
  _pf.JZ(function_name + std::to_string(++functionLabel));
  
  node->block()->accept(this, lvl + 2);

  if(node->exclusive())
    _pf.JMP(function_name + "end");
}

void m19::postfix_writer::do_final_section_node(m19::final_section_node *const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->block()->accept(this, lvl + 2);
}

void m19::postfix_writer::do_initial_section_node(m19::initial_section_node *const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  inInitialSection = true;
  node->block()->accept(this, lvl + 2);
  inInitialSection = false;
}

void m19::postfix_writer::do_identity_node(m19::identity_node *const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->argument()->accept(this, lvl + 2);
}