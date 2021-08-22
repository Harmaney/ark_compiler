#include <string>
#include <map>
#include <vector>
#include "const.h"

#ifndef _CPP_SYMBOLS
#define _CPP_SYMBOLS
////////////////////////////////////////

/// 符号定义
class SymbolDescriptor {
   public:
    DescriptorType type;
    std::string name;
    SymbolDescriptor(DescriptorType type, std::string name)
        : type(type), name(name) {}
};

/// 类型定义。因为编译到C++，所以类型定义中并不需要携带任何关于大小和长度一类的信息
class TypeDescriptor : public SymbolDescriptor {
   public:
    TypeDescriptor(std::string name)
        : SymbolDescriptor(DESCRIPTOR_TYPE, name) {}
};

/// 指针定义
///
/// 一定注意，此处的指针定义是和Pascal中的指针对应的。变量中的ref标记，指的是某种pascal的写法，使得对于该变量的访问成为了引用格式的，翻译到代码时对应到C++指针。
class PointerTypeDescriptor : public SymbolDescriptor {
   public:
    SymbolDescriptor* ref;
    PointerTypeDescriptor(std::string name, SymbolDescriptor* ref)
        : SymbolDescriptor(DESCRIPTOR_POINTER_TYPE, name), ref(ref) {}
};

/// 数组定义
class ArrayTypeDescriptor : public SymbolDescriptor {
   public:
    int sz;
    int beg;
    SymbolDescriptor* itemDescriptor;
    ArrayTypeDescriptor(std::string name, SymbolDescriptor* itemDescriptor,
                        int sz, int beg)
        : SymbolDescriptor(DESCRIPTOR_ARRAY, name),
          itemDescriptor(itemDescriptor),
          sz(sz),
          beg(beg) {}
};

/// 变量定义
class VariableDescriptor : public SymbolDescriptor {
   public:
    SymbolDescriptor* varType;
    // FIX: i dont know if it is proper.
    bool isRef;
    // 现在没有用上
    bool isConst;
    bool isLeftVar;

    VariableDescriptor(std::string name, SymbolDescriptor* varType, bool isRef,
                       bool isConst, bool isLeftVar = true)
        : SymbolDescriptor(DESCRIPTOR_VARIABLE, name),
          varType(varType),
          isRef(isRef),
          isConst(isConst),
          isLeftVar(isLeftVar) {}
};

typedef VariableDescriptor Value;

class StructDescriptor : public SymbolDescriptor {
   public:
    std::map<std::string, SymbolDescriptor*> refVar;

    StructDescriptor(std::string name,
                     std::map<std::string, SymbolDescriptor*> refVar)
        : SymbolDescriptor(DESCRIPTOR_STRUCT, name), refVar(refVar) {}

    void push(std::string sig, SymbolDescriptor* varDescriptor) {
        refVar[sig] = varDescriptor;
    }
};

class FunctionDescriptor : public SymbolDescriptor {
   public:
    std::vector<VariableDescriptor*> args;
    SymbolDescriptor* resultDescriptor;
    FunctionDescriptor(std::string name, std::vector<VariableDescriptor*> args,
                       SymbolDescriptor* resultDescriptor)
        : SymbolDescriptor(DESCRIPTOR_FUNCTION, name),
          args(args),
          resultDescriptor(resultDescriptor) {}
};

#endif