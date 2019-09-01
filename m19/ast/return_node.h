#ifndef __M19_RETURN_NODE_H__
#define __M19_RETURN_NODE_H__

#include <cdk/ast/basic_node.h>

namespace m19{

    class return_node: public cdk::basic_node {

    public:
        inline return_node(int lineno) :
            cdk::basic_node(lineno){
        }

        void accept(basic_ast_visitor *sp, int level) {
            sp->do_return_node(this, level);
        }

    };

} //m19

#endif