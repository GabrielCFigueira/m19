#ifndef __M19_FUNCTION_BODY_NODE_H__
#define __M19_FUNCTION_BODY_NODE_H__

#include <cdk/ast/sequence_node.h>
//#include "section_node.h"

namespace m19 {

  class function_body_node: public cdk::basic_node {
    cdk::basic_node *_initial_section;
    cdk::basic_node *_final_section;
    cdk::sequence_node *_sections;


  public:
    inline function_body_node(int lineno, cdk::basic_node *initial_section, cdk::basic_node *final_section, cdk::sequence_node *sections) :
        cdk::basic_node(lineno), _initial_section(initial_section), _final_section(final_section), _sections(sections) {
    }

    inline cdk::basic_node *initial_section() {
        return _initial_section;
    }

    inline cdk::basic_node *final_section() {
        return _final_section;
    }

    inline cdk::sequence_node *sections() {
      return _sections;
    }

    void accept(basic_ast_visitor *sp, int level) {
      sp->do_function_body_node(this, level);
    }

  };

} // m19

#endif
