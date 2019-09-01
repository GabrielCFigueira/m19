#ifndef __M19_SECTION_NODE_H__
#define __M19_SECTION_NODE_H__

#include <cdk/ast/expression_node.h>

namespace m19 {

  class section_node: public cdk::basic_node {
    bool _exclusive;
    m19::block_node *_block;
    cdk::expression_node *_expression;


  public:
    inline section_node(int lineno, bool exclusive, m19::block_node *block, cdk::expression_node *expression) :
        cdk::basic_node(lineno), _exclusive(exclusive), _block(block), _expression(expression) {
    }

    inline section_node(int lineno, bool exclusive, m19::block_node *block) :
        cdk::basic_node(lineno), _exclusive(exclusive), _block(block), _expression(NULL) {
    }

    inline m19::block_node *block() {
        return _block;
    }

    inline bool exclusive() {
        return _exclusive;
    }

    inline cdk::expression_node *expression() {
      return _expression;
    }

    void accept(basic_ast_visitor *sp, int level) {
      sp->do_section_node(this, level);
    }

  };

} // m19

#endif
