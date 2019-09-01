#ifndef __M19_SEMANTICS_POSTFIX_WRITER_H__
#define __M19_SEMANTICS_POSTFIX_WRITER_H__

#include <string>
#include <set>
#include <stack>
#include <iostream>
#include <cdk/symbol_table.h>
#include <cdk/emitters/basic_postfix_emitter.h>
#include "targets/basic_ast_visitor.h"
#include "targets/symbol.h"

namespace m19 {

  //!
  //! Traverse syntax tree and generate the corresponding assembly code.
  //!
  class postfix_writer: public basic_ast_visitor {
    cdk::symbol_table<m19::symbol> &_symtab;
    cdk::basic_postfix_emitter &_pf;
    int _lbl;

    std::string function_name;
    int _offset, functionLabel;
    bool _inFunction, _inFunctionBody, _inFunctionArgs, inInitialSection;
    std::stack<int> _forIni, _forStep, _forEnd;
    std::set<std::string> _functions_to_declare;

  public:
    postfix_writer(std::shared_ptr<cdk::compiler> compiler, cdk::symbol_table<m19::symbol> &symtab,
                   cdk::basic_postfix_emitter &pf) :
        basic_ast_visitor(compiler), _symtab(symtab), _pf(pf), _lbl(0), function_name(""), _offset(0), functionLabel(0),
        _inFunction(false), _inFunctionBody(false), _inFunctionArgs(false), inInitialSection(false) {
    }

  public:
    ~postfix_writer() {
      os().flush();
    }
  
  protected:
    void do_ComparisonExpression(cdk::binary_expression_node * const node, int lvl);
    void do_I2DExpression(cdk::binary_expression_node * const node, int lvl);
    void do_addsubExpression(cdk::binary_expression_node * const node, int lvl);

  private:
    /** Method used to generate sequential labels. */
    inline std::string mklbl(int lbl) {
      std::ostringstream oss;
      if (lbl < 0)
        oss << ".L" << -lbl;
      else
        oss << "_L" << lbl;
      return oss.str();
    }

  public:
  // do not edit these lines
#define __IN_VISITOR_HEADER__
#include "ast/visitor_decls.h"       // automatically generated
#undef __IN_VISITOR_HEADER__
  // do not edit these lines: end

  };

} // m19

#endif
