// $Id: evaluation_node.h,v 1.2 2019/03/17 22:32:49 ist186426 Exp $
#ifndef __M19_EVALUATIONNODE_H__
#define __M19_EVALUATIONNODE_H__

#include <cdk/ast/expression_node.h>

namespace m19 {

  /**
   * Class for describing evaluation nodes.
   */
  class evaluation_node: public cdk::basic_node {
    cdk::expression_node *_argument;

  public:
    inline evaluation_node(int lineno, cdk::expression_node *argument) :
        cdk::basic_node(lineno), _argument(argument) {
    }

    inline cdk::expression_node *argument() {
      return _argument;
    }

    void accept(basic_ast_visitor *sp, int level) {
      sp->do_evaluation_node(this, level);
    }

  };

} // m19

#endif
