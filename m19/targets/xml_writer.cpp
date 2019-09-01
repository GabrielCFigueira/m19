#include <string>
#include "targets/xml_writer.h"
#include "targets/type_checker.h"
#include "ast/all.h"  // automatically generated

//--------------------------------------------------------------------------------

static std::string type_name(basic_type *type){
  if(type->name() == basic_type::TYPE_INT) return "integer";
  if(type->name() == basic_type::TYPE_DOUBLE) return "double";
  if(type->name() == basic_type::TYPE_STRING) return "string";
  if(type->name() == basic_type::TYPE_VOID) return "void";
  if(type->name() == basic_type::TYPE_POINTER){
    std::string s = "pointer";
    basic_type *p = type->subtype();
    while (p != nullptr) {
      s += " to " + type_name(p);
      p = p->subtype();
    }
    return s;
  } else
    return "unknown type";
}

static std::string qualifier_name(int qualifier){
  if(qualifier == 0) return "private";
  if(qualifier == 1) return "public";
  if(qualifier == 2) return "extern";
  return "unknown qualifier";
}

//--------------------------------------------------------------------------------

void m19::xml_writer::do_data_node(cdk::data_node * const node, int lvl) {
  // EMPTY
}
void m19::xml_writer::do_nil_node(cdk::nil_node * const node, int lvl) {
  openTag(node, lvl);
  closeTag(node, lvl);
}
void m19::xml_writer::do_double_node(cdk::double_node * const node, int lvl) {
  process_literal(node, lvl);
}
void m19::xml_writer::do_not_node(cdk::not_node * const node, int lvl) {
  do_unary_expression(node, lvl);
}
void m19::xml_writer::do_and_node(cdk::and_node * const node, int lvl) {
  do_binary_expression(node, lvl);
}
void m19::xml_writer::do_or_node(cdk::or_node * const node, int lvl) {
  do_binary_expression(node, lvl);
}

//--------------------------------------------------------------------------------

void m19::xml_writer::do_sequence_node(cdk::sequence_node * const node, int lvl) {
  os() << std::string(lvl, ' ') << "<sequence_node size='" << node->size() << "'>" << std::endl;
  for (size_t i = 0; i < node->size(); i++)
    node->node(i)->accept(this, lvl + 2);
  closeTag(node, lvl);
}

//--------------------------------------------------------------------------------

void m19::xml_writer::do_integer_node(cdk::integer_node * const node, int lvl) {
  process_literal(node, lvl);
}

void m19::xml_writer::do_string_node(cdk::string_node * const node, int lvl) {
  process_literal(node, lvl);
}

//--------------------------------------------------------------------------------

void m19::xml_writer::do_unary_expression(cdk::unary_expression_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  openTag(node, lvl);
  node->argument()->accept(this, lvl + 2);
  closeTag(node, lvl);
}

void m19::xml_writer::do_neg_node(cdk::neg_node * const node, int lvl) {
  do_unary_expression(node, lvl);
}

//--------------------------------------------------------------------------------

void m19::xml_writer::do_binary_expression(cdk::binary_expression_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  openTag(node, lvl);
  node->left()->accept(this, lvl + 2);
  node->right()->accept(this, lvl + 2);
  closeTag(node, lvl);
}

void m19::xml_writer::do_add_node(cdk::add_node * const node, int lvl) {
  do_binary_expression(node, lvl);
}
void m19::xml_writer::do_sub_node(cdk::sub_node * const node, int lvl) {
  do_binary_expression(node, lvl);
}
void m19::xml_writer::do_mul_node(cdk::mul_node * const node, int lvl) {
  do_binary_expression(node, lvl);
}
void m19::xml_writer::do_div_node(cdk::div_node * const node, int lvl) {
  do_binary_expression(node, lvl);
}
void m19::xml_writer::do_mod_node(cdk::mod_node * const node, int lvl) {
  do_binary_expression(node, lvl);
}
void m19::xml_writer::do_lt_node(cdk::lt_node * const node, int lvl) {
  do_binary_expression(node, lvl);
}
void m19::xml_writer::do_le_node(cdk::le_node * const node, int lvl) {
  do_binary_expression(node, lvl);
}
void m19::xml_writer::do_ge_node(cdk::ge_node * const node, int lvl) {
  do_binary_expression(node, lvl);
}
void m19::xml_writer::do_gt_node(cdk::gt_node * const node, int lvl) {
  do_binary_expression(node, lvl);
}
void m19::xml_writer::do_ne_node(cdk::ne_node * const node, int lvl) {
  do_binary_expression(node, lvl);
}
void m19::xml_writer::do_eq_node(cdk::eq_node * const node, int lvl) {
  do_binary_expression(node, lvl);
}

//--------------------------------------------------------------------------------

void m19::xml_writer::do_variable_node(cdk::variable_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  os() << std::string(lvl, ' ') << "<" << node->label() << ">" << (node->name() == "@" ? function_name : node->name()) << "</" << node->label() << ">" << std::endl;
}

void m19::xml_writer::do_rvalue_node(cdk::rvalue_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  openTag(node, lvl);
  node->lvalue()->accept(this, lvl + 4);
  closeTag(node, lvl);
}

void m19::xml_writer::do_assignment_node(cdk::assignment_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  openTag(node, lvl);

  node->lvalue()->accept(this, lvl);

  node->rvalue()->accept(this, lvl + 4);
  closeTag(node, lvl);
}

//--------------------------------------------------------------------------------

void m19::xml_writer::do_evaluation_node(m19::evaluation_node * const node, int lvl) {
  openTag(node, lvl);
  node->argument()->accept(this, lvl + 2);
  closeTag(node, lvl);
}

void m19::xml_writer::do_print_node(m19::print_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  openTag(node, lvl);
  node->argument()->accept(this, lvl + 2);
  closeTag(node, lvl);
}

//--------------------------------------------------------------------------------

void m19::xml_writer::do_read_node(m19::read_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  openTag(node, lvl);
  closeTag(node, lvl);
}

//--------------------------------------------------------------------------------

void m19::xml_writer::do_for_node(m19::for_node * const node, int lvl) {
  openTag(node, lvl);

  _symtab.push();

  openTag("declaration", lvl + 2);
  node->declaration()->accept(this, lvl + 4);
  closeTag("declaration", lvl + 2);

  ASSERT_SAFE_EXPRESSIONS;
  
  openTag("condition", lvl + 2);
  node->condition()->accept(this, lvl + 4);
  closeTag("condition", lvl + 2);
  
  openTag("iteration", lvl + 2);
  node->iteration()->accept(this, lvl + 4);
  closeTag("iteration", lvl + 2);

  openTag("block", lvl + 2);
  node->instruction()->accept(this, lvl + 4);
  closeTag("block", lvl + 2);
  
  _symtab.pop();

  closeTag(node, lvl);
}

//--------------------------------------------------------------------------------

void m19::xml_writer::do_if_node(m19::if_node * const node, int lvl) {
  openTag(node, lvl);
  
  openTag("condition", lvl + 2);
  node->condition()->accept(this, lvl + 4);
  closeTag("condition", lvl + 2);

  openTag("then", lvl + 2);
  node->block()->accept(this, lvl + 4);
  closeTag("then", lvl + 2);

  closeTag(node, lvl);
}

void m19::xml_writer::do_if_else_node(m19::if_else_node * const node, int lvl) {
  openTag(node, lvl);
  openTag("condition", lvl + 2);
  node->condition()->accept(this, lvl + 4);
  closeTag("condition", lvl + 2);

  openTag("then", lvl + 2);
  node->thenblock()->accept(this, lvl + 4);
  closeTag("then", lvl + 2);

  openTag("else", lvl + 2);
  node->elseblock()->accept(this, lvl + 4);
  closeTag("else", lvl + 2);

  closeTag(node, lvl);
}

//--------------------------------------------------------------------------------

void m19::xml_writer::do_function_declaration_node(m19::function_declaration_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  openTag(node, lvl);

  openTag("qualifier", lvl+2);
  os() << std::string(lvl+4 , ' ') << qualifier_name(node->qualifier()) << std::endl;
  closeTag("qualifier", lvl+2);

  openTag("type", lvl+2);
  os() << std::string(lvl+4 , ' ') <<  type_name(node->type()) << std::endl;
  closeTag("type", lvl+2);

  openTag("identifier", lvl+2);
  function_name = node->identifier();
  os() << std::string(lvl+4 , ' ') << node->identifier() << std::endl;
  closeTag("identifier", lvl+2);

  if(node->arguments() != nullptr){
    openTag("arguments", lvl+2);
    node->arguments()->accept(this, lvl+4);
    closeTag("arguments", lvl+2);
  }

  function_name = "";

  closeTag(node, lvl);
}

void m19::xml_writer::do_function_definition_node(m19::function_definition_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  openTag(node, lvl);

  _symtab.push();

  openTag("qualifier", lvl+2);
  os() << std::string(lvl+4 , ' ') << qualifier_name(node->qualifier()) << std::endl;
  closeTag("qualifier", lvl+2);

  openTag("type", lvl+2);
  os() << std::string(lvl+4 , ' ') << type_name(node->type()) << std::endl;
  closeTag("type", lvl+2);

  openTag("identifier", lvl+2);
  function_name = node->identifier();
  os() << std::string(lvl+4 , ' ') << node->identifier() << std::endl;
  closeTag("identifier", lvl+2);

  if(node->arguments() != nullptr){
    openTag("arguments", lvl+2);
    node->arguments()->accept(this, lvl+4);
    closeTag("arguments", lvl+2);
  }
  
  openTag("return", lvl+2);

  if(node->return_value() != nullptr){
    node->return_value()->accept(this, lvl+4);
  } else if(node->type()->name() == basic_type::TYPE_INT || node->type()->name() == basic_type::TYPE_POINTER){
    os() << std::string(lvl+4 , ' ') << "0" << std::endl;
  }else{
    os() << std::string(lvl+4 , ' ') << "indef" << std::endl;
  }

  closeTag("return", lvl+2);

  openTag("body", lvl+2);
  node->body()->accept(this, lvl+4);
  closeTag("body", lvl+2);
  
  closeTag(node, lvl);

  _symtab.pop();

  function_name = "";
}

void m19::xml_writer::do_function_call_node(m19::function_call_node * const node, int lvl) {
  openTag(node, lvl);

  openTag("identifier", lvl+2);
  os() << std::string(lvl+4 , ' ') << (node->identifier() == "@" ? function_name : node->identifier()) << std::endl;
  closeTag("identifier", lvl+2);

  if(node->arguments() != nullptr) {
    openTag("arguments", lvl+2);
    node->arguments()->accept(this, lvl+4);
    closeTag("arguments", lvl+2);
  }

  closeTag(node, lvl);
}

void m19::xml_writer::do_block_node(m19::block_node * const node, int lvl) {
  openTag(node, lvl);

  bool pop = inInitialSection;
  if(!inInitialSection)
    _symtab.push();

  if(node->declarations() != nullptr){
    openTag("declarations", lvl+2);
    node->declarations()->accept(this, lvl+4);
    closeTag("declarations", lvl+2);
  }

  inInitialSection = false;

  if(node->instructions() != nullptr){
    openTag("instructions", lvl+2);
    node->instructions()->accept(this, lvl+4);
    closeTag("instructions", lvl+2);
  }

  if(!pop)
    _symtab.pop();

  closeTag(node, lvl);
}

void m19::xml_writer::do_return_node(m19::return_node *const node, int lvl) {
  openTag(node, lvl);
  closeTag(node, lvl);
}

void m19::xml_writer::do_continue_node(m19::continue_node *const node, int lvl) {
  openTag(node, lvl);
  closeTag(node, lvl);
}

void m19::xml_writer::do_break_node(m19::break_node *const node, int lvl) {
  openTag(node, lvl);
  closeTag(node, lvl);
}

void m19::xml_writer::do_variable_declaration_node(m19::variable_declaration_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  openTag(node, lvl);

  //reset_new_symbol();

  openTag("qualifier", lvl+2);
  os() << std::string(lvl+4 , ' ') << qualifier_name(node->qualifier()) << std::endl;
  closeTag("qualifier", lvl+2);

  openTag("variable_type", lvl+2);
  os() << std::string(lvl+4 , ' ') << type_name(node->type()) << std::endl;
  closeTag("variable_type", lvl+2);

  openTag("identifier", lvl+2);
  os() << std::string(lvl+4 , ' ') << node->identifier() << std::endl;
  closeTag("identifier", lvl+2);

  if(node->initializer() != nullptr){
    openTag("initializer", lvl+2);
    node->initializer()->accept(this, lvl+4);
    closeTag("initializer", lvl+2);
  }
    
  closeTag(node, lvl);
}

void m19::xml_writer::do_memory_alloc_node(m19::memory_alloc_node *const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  openTag(node ,lvl);
  node->argument()->accept(this, lvl+2);
  closeTag(node ,lvl);
}

void m19::xml_writer::do_address_node(m19::address_node *const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  openTag(node, lvl);

  openTag("left_value", lvl+2);
  node->lvalue()->accept(this, lvl+4);
  closeTag("left_value", lvl+2);

  closeTag(node, lvl);
}

void m19::xml_writer::do_index_node(m19::index_node *const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  openTag(node, lvl);

  openTag("base", lvl+2);
  node->base()->accept(this, lvl+4);
  closeTag("base", lvl+2);

  openTag("index", lvl+2);
  node->index()->accept(this, lvl+4);
  closeTag("index", lvl+2);

  closeTag(node, lvl);
}

void m19::xml_writer::do_function_body_node(m19::function_body_node *const node, int lvl) {
  openTag(node, lvl);

  _symtab.push();

  if(node->initial_section() != nullptr){
    openTag("initial_section", lvl+2);
    node->initial_section()->accept(this, lvl+4);
    closeTag("initial_section", lvl+2);
  }

  if(node->sections() != nullptr){
    openTag("sections", lvl+2);
    node->sections()->accept(this, lvl+4);
    closeTag("sections", lvl+2);
  }

  if(node->final_section() != nullptr){
    openTag("final_section", lvl+2);
    node->final_section()->accept(this, lvl+4);
    closeTag("final_section", lvl+2);
  }

  _symtab.pop();

  closeTag(node, lvl);
}

void m19::xml_writer::do_section_node(m19::section_node *const node, int lvl) {
  openTag(node, lvl);

  openTag("block", lvl+2);
  node->block()->accept(this, lvl+4);
  closeTag("block", lvl+2);

  openTag("exclusive", lvl+2);
  os() << std::string(lvl+4 , ' ') << (node->exclusive() ? "true" : "false")  << std::endl;
  closeTag("exclusive", lvl+2);

  if(node->expression() != nullptr){
    openTag("expression", lvl+2);
    node->expression()->accept(this, lvl+4);
    closeTag("expression", lvl+2);
  }
    
  closeTag(node, lvl);
}

void m19::xml_writer::do_final_section_node(m19::final_section_node *const node, int lvl) {
  openTag(node, lvl);

  openTag("block", lvl+2);
  node->block()->accept(this, lvl+4);
  closeTag("block", lvl+2);

  closeTag(node, lvl);
}

void m19::xml_writer::do_initial_section_node(m19::initial_section_node *const node, int lvl) {
  openTag(node, lvl);

  inInitialSection = true;

  openTag("block", lvl+2);
  node->block()->accept(this, lvl+4);
  closeTag("block", lvl+2);

  inInitialSection = false;

  closeTag(node, lvl);
}

void m19::xml_writer::do_identity_node(m19::identity_node *const node, int lvl) {
  do_unary_expression(node, lvl);
}