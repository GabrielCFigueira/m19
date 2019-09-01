#ifndef __M19_ADDRESS_NODE_H__
#define __M19_ADDRESS_NODE_H__

#include <cdk/ast/expression_node.h>

namespace m19 {

  class address_node: public cdk::expression_node {
    cdk::lvalue_node *_lvalue;

  public:
    inline address_node(int lineno, cdk::lvalue_node *lvalue) :
        cdk::expression_node(lineno), _lvalue(lvalue) {
    }

    inline cdk::lvalue_node *lvalue() {
      return _lvalue;
    }

    void accept(basic_ast_visitor *sp, int level) {
      sp->do_address_node(this, level);
    }

  };

} // m19

#endif
