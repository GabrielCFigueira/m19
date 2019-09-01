#ifndef __M19_AST_VARIABLE_DECLARATION_H__
#define __M19_AST_VARIABLE_DECLARATION_H__

#include <cdk/ast/expression_node.h>

namespace m19 {

  class variable_declaration_node: public cdk::basic_node {
    int _qualifier;
    basic_type *_type;
    std::string _identifier;
    cdk::expression_node *_initializer;

  public:
    inline variable_declaration_node(int lineno, int qualifier, basic_type *type, const std::string &identifier,
                              cdk::expression_node *initializer) :
        cdk::basic_node(lineno), _qualifier(qualifier), _type(type), _identifier(identifier), _initializer(initializer) {
    }

    inline int qualifier() {
      return _qualifier;
    }

    inline basic_type *type() {
      return _type;
    }

    inline const std::string &identifier() const {
      return _identifier;
    }
    
    inline cdk::expression_node *initializer() {
      return _initializer;
    }
    
    void accept(basic_ast_visitor *sp, int level) {
      sp->do_variable_declaration_node(this, level);
    }

  };

} // m19

#endif