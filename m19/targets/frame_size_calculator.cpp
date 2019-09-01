#include <string>
#include "targets/frame_size_calculator.h"
#include "targets/symbol.h"
#include "ast/all.h"

m19::frame_size_calculator::~frame_size_calculator() {
  os().flush();
}

void m19::frame_size_calculator::do_nil_node(cdk::nil_node * const node, int lvl) {
  // EMPTY
}

void m19::frame_size_calculator::do_data_node(cdk::data_node * const node, int lvl) {
  // EMPTY
}

void m19::frame_size_calculator::do_double_node(cdk::double_node * const node, int lvl) {
  // EMPTY
}

void m19::frame_size_calculator::do_not_node(cdk::not_node * const node, int lvl) {
  // EMPTY
}

void m19::frame_size_calculator::do_and_node(cdk::and_node * const node, int lvl) {
  // EMPTY
}

void m19::frame_size_calculator::do_or_node(cdk::or_node * const node, int lvl) {
  // EMPTY
}

void m19::frame_size_calculator::do_integer_node(cdk::integer_node * const node, int lvl) {
  // EMPTY
}

void m19::frame_size_calculator::do_string_node(cdk::string_node * const node, int lvl) {
// EMPTY
}

void m19::frame_size_calculator::do_neg_node(cdk::neg_node * const node, int lvl) {
  // EMPTY
}

void m19::frame_size_calculator::do_add_node(cdk::add_node * const node, int lvl) {
  // EMPTY
}

void m19::frame_size_calculator::do_sub_node(cdk::sub_node * const node, int lvl) {
  // EMPTY
}

void m19::frame_size_calculator::do_mul_node(cdk::mul_node * const node, int lvl) {
  // EMPTY
}

void m19::frame_size_calculator::do_div_node(cdk::div_node * const node, int lvl) {
  // EMPTY
}

void m19::frame_size_calculator::do_mod_node(cdk::mod_node * const node, int lvl) {
  // EMPTY
}

void m19::frame_size_calculator::do_lt_node(cdk::lt_node * const node, int lvl) {
  // EMPTY
}

void m19::frame_size_calculator::do_le_node(cdk::le_node * const node, int lvl) {
  // EMPTY
}

void m19::frame_size_calculator::do_ge_node(cdk::ge_node * const node, int lvl) {
  // EMPTY
}

void m19::frame_size_calculator::do_gt_node(cdk::gt_node * const node, int lvl) {
  // EMPTY
}

void m19::frame_size_calculator::do_ne_node(cdk::ne_node * const node, int lvl) {
  // EMPTY
}

void m19::frame_size_calculator::do_eq_node(cdk::eq_node * const node, int lvl) {
  // EMPTY
}

void m19::frame_size_calculator::do_variable_node(cdk::variable_node * const node, int lvl) {
  // EMPTY
}

void m19::frame_size_calculator::do_rvalue_node(cdk::rvalue_node * const node, int lvl) {
  // EMPTY
}

void m19::frame_size_calculator::do_assignment_node(cdk::assignment_node * const node, int lvl) {
  // EMPTY
}

void m19::frame_size_calculator::do_evaluation_node(m19::evaluation_node * const node, int lvl) {
  // EMPTY
}

void m19::frame_size_calculator::do_print_node(m19::print_node * const node, int lvl) {
  // EMPTY
}

void m19::frame_size_calculator::do_read_node(m19::read_node * const node, int lvl) {
  // EMPTY
}

void m19::frame_size_calculator::do_function_declaration_node(m19::function_declaration_node * const node, int lvl) {
  // EMPTY
}

void m19::frame_size_calculator::do_function_call_node(m19::function_call_node * const node, int lvl) {
  // EMPTY
}

void m19::frame_size_calculator::do_return_node(m19::return_node *const node, int lvl) {
  // EMPTY
}

void m19::frame_size_calculator::do_continue_node(m19::continue_node *const node, int lvl) {
  // EMPTY
}

void m19::frame_size_calculator::do_break_node(m19::break_node *const node, int lvl) {
  // EMPTY
}

void m19::frame_size_calculator::do_memory_alloc_node(m19::memory_alloc_node *const node, int lvl) {
  // EMPTY
}

void m19::frame_size_calculator::do_address_node(m19::address_node *const node, int lvl) {
  // EMPTY
}

void m19::frame_size_calculator::do_index_node(m19::index_node *const node, int lvl) {
  // EMPTY
}

void m19::frame_size_calculator::do_identity_node(m19::identity_node *const node, int lvl) {
  // EMPTY
}

void m19::frame_size_calculator::do_sequence_node(cdk::sequence_node * const node, int lvl) {
  for (size_t i = 0; i < node->size(); i++) {
    cdk::basic_node *n = node->node(i);
    if (n == nullptr) break;
    n->accept(this, lvl + 2);
  }
}

void m19::frame_size_calculator::do_block_node(m19::block_node * const node, int lvl) {
  if (node->declarations())
    node->declarations()->accept(this, lvl + 2);

  if (node->instructions())
    node->instructions()->accept(this, lvl + 2);
}

void m19::frame_size_calculator::do_for_node(m19::for_node * const node, int lvl) {
  node->declaration()->accept(this, lvl + 2);
  node->instruction()->accept(this, lvl + 2);
}

void m19::frame_size_calculator::do_if_node(m19::if_node * const node, int lvl) {
  node->block()->accept(this, lvl + 2);
}

void m19::frame_size_calculator::do_if_else_node(m19::if_else_node * const node, int lvl) {
  node->thenblock()->accept(this, lvl + 2);

  if (node->elseblock())
    node->elseblock()->accept(this, lvl + 2);
}

void m19::frame_size_calculator::do_variable_declaration_node(m19::variable_declaration_node * const node, int lvl) {
  _localsize += node->type()->size();
}

void m19::frame_size_calculator::do_function_definition_node(m19::function_definition_node * const node, int lvl) {
  _localsize += node->type()->size();
  node->body()->accept(this, lvl + 2);
}

void m19::frame_size_calculator::do_function_body_node(m19::function_body_node *const node, int lvl) {
  if (node->initial_section())
    node->initial_section()->accept(this, lvl + 2);

  if (node->sections())
    node->sections()->accept(this, lvl + 2);

  if (node->final_section())
    node->final_section()->accept(this, lvl + 2);
}

void m19::frame_size_calculator::do_section_node(m19::section_node *const node, int lvl) {
  node->block()->accept(this, lvl + 2);
}

void m19::frame_size_calculator::do_final_section_node(m19::final_section_node *const node, int lvl) {
  node->block()->accept(this, lvl + 2);
}

void m19::frame_size_calculator::do_initial_section_node(m19::initial_section_node *const node, int lvl) {
  node->block()->accept(this, lvl + 2);
}
