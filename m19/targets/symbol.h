#ifndef __M19_SEMANTICS_SYMBOL_H__
#define __M19_SEMANTICS_SYMBOL_H__

#include <string>
#include <cdk/basic_type.h>

namespace m19 {

  class symbol {
    basic_type *_type;
    std::string _name;
    int _qualifier;
    bool _function;
    bool _initialized;
    int _offset;
    std::vector<basic_type *> _arguments;

  public:
    symbol(basic_type *type, const std::string &name, int qualifier = 0, bool function=false, bool initialized = false) :
        _type(type), _name(name), _qualifier(qualifier), _function(function), _initialized(initialized) {
    }

    symbol(basic_type *type, const std::string &name, int qualifier, bool function, bool initialized, std::vector<basic_type *> arguments) :
        _type(type), _name(name), _qualifier(qualifier), _function(function), _initialized(initialized), _arguments(arguments) {
    }

    virtual ~symbol() {
      delete _type;
      for(basic_type *bt : _arguments)
        delete bt;
    }

    basic_type *type() const {
      return _type;
    }
    const std::string &name() const {
      return _name;
    }
    int qualifier() const {
      return _qualifier;
    }
    bool isFunction() const {
      return _function;
    }
    bool initialized() const {
      return _initialized;
    }
    int offset() const{
      return _offset;
    }
    void setOffset(int offset){
      _offset = offset;
    }
    std::vector<basic_type *> arguments() const {
        return _arguments;
    }
    void setArguments(std::vector<basic_type *> arguments){
        _arguments = arguments;
    }
  };

} // m19

#endif
