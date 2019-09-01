// $Id: for_node.h,v 1.7 2019/03/20 16:22:03 ist186426 Exp $ -*- c++ -*-
#ifndef __M19_FORNODE_H__
#define __M19_FORNODE_H__

#include <cdk/ast/sequence_node.h>

namespace m19 {

  /**
   * Class for describing for-cycle nodes.
   */
  class for_node: public cdk::basic_node {
    
    cdk::sequence_node *_declaration;  //this could be expressions or variables(s) declaration
    cdk::sequence_node *_condition;
    cdk::sequence_node *_iteration;
    cdk::basic_node * _instruction;

  public:
    inline for_node(int lineno, cdk::sequence_node *declaration, cdk::sequence_node *condition, cdk::sequence_node *iteration, cdk::basic_node *instruction) :
        basic_node(lineno), _declaration(declaration), _condition(condition), _iteration(iteration), _instruction(instruction) {
    }

    inline cdk::sequence_node *declaration() {
      return _declaration;
    }
    inline cdk::sequence_node *condition() {
      return _condition;
    }
    inline cdk::sequence_node *iteration() {
      return _iteration;
    }
    inline cdk::basic_node *instruction() {
      return _instruction;
    }

    void accept(basic_ast_visitor *sp, int level) {
      sp->do_for_node(this, level);
    }

  };

} // m19

#endif
