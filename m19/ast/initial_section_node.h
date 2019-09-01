#ifndef __M19_INITIAL_SECTION_NODE_H__
#define __M19_INITIAL_SECTION_NODE_H__

#include <cdk/ast/basic_node.h>

namespace m19 {

  class initial_section_node: public cdk::basic_node {
    m19::block_node *_block;


  public:
    inline initial_section_node(int lineno, m19::block_node *block) :
        cdk::basic_node(lineno), _block(block) {
    }

    inline m19::block_node *block() {
        return _block;
    }

    void accept(basic_ast_visitor *sp, int level) {
      sp->do_initial_section_node(this, level);
    }

  };

} // m19

#endif
