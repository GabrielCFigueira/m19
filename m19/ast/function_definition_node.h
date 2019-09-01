#ifndef __M19_AST_FUNCTION_DEFINITION_H__
#define __M19_AST_FUNCTION_DEFINITION_H__

#include <string>
#include <cdk/ast/sequence_node.h>
#include <cdk/ast/expression_node.h>
//#include "ast/function_body_node.h"
//#include "ast/block_node.h"

namespace m19 {

  /**
   * Class for describing function definitions.
   */
  class function_definition_node: public cdk::basic_node {
    int _qualifier;
    basic_type *_type;
    std::string _identifier;
    cdk::sequence_node *_arguments;
    cdk::expression_node *_return_value;
    cdk::basic_node *_body;

  public:
    inline function_definition_node(int lineno, int qualifier, basic_type *type, const std::string &identifier,
                             cdk::sequence_node *arguments, cdk::expression_node *return_value, cdk::basic_node *body) :
        cdk::basic_node(lineno), _qualifier(qualifier), _type(type), _identifier(identifier), _arguments(arguments), _return_value(return_value), _body(body) {
    }

    inline function_definition_node(int lineno, int qualifier, basic_type *type, const std::string &identifier,
                             cdk::sequence_node *arguments, cdk::basic_node *body) :
        cdk::basic_node(lineno), _qualifier(qualifier), _type(type), _identifier(identifier), _arguments(arguments), _return_value(NULL), _body(body) {
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

    inline cdk::expression_node *return_value() {
      return _return_value;
    }

    inline cdk::basic_node *body() {
      return _body;
    }

    void accept(basic_ast_visitor *sp, int level) {
      sp->do_function_definition_node(this, level);
    }

  };

} // m19

#endif
