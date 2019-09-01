#ifndef __M19_AST_FUNCTION_DECLARATION_H__
#define __M19_AST_FUNCTION_DECLARATION_H__

#include <string>
#include <cdk/ast/expression_node.h>

namespace m19 {

  /**
   * Class for describing function declarations.
   */
  class function_declaration_node: public cdk::basic_node {
    int _qualifier;
    basic_type *_type;
    std::string _identifier;
    cdk::sequence_node *_arguments;

  public:
    inline function_declaration_node(int lineno, int qualifier, basic_type *type, const std::string &identifier,
                              cdk::sequence_node *arguments) :
        cdk::basic_node(lineno), _qualifier(qualifier), _type(type), _identifier(identifier), _arguments(arguments) {
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
    
    inline cdk::sequence_node *arguments() {
      return _arguments;
    }

    void accept(basic_ast_visitor *sp, int level) {
      sp->do_function_declaration_node(this, level);
    }

  };

} // m19

#endif
