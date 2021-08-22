#include "stdsupport.h"

// void init_std_write() {
//     // write 以及其符号
//     CodeCollector::push_back("void write_integer(int x){cout<<x;}");
//     SymbolTable::insert_function(
//         "write",
//         new FunctionDescriptor(
//             "write",
//             {new VariableDescriptor(
//                 "x", SymbolTable::lookfor_type(TYPE_BASIC_INT), false, false)},
//             SymbolTable::lookfor_type(TYPE_BASIC_VOID)));

//     CodeCollector::push_back("void write_string(string x){cout<<x;}");
//     SymbolTable::insert_function(
//         "write", new FunctionDescriptor(
//                      "write",
//                      {new VariableDescriptor(
//                          "x", SymbolTable::lookfor_type(TYPE_BASIC_STRING),
//                          false, false)},
//                      SymbolTable::lookfor_type(TYPE_BASIC_VOID)));

//     CodeCollector::push_back("void write_int64(long long x){cout<<x;}");
//     SymbolTable::insert_function(
//         "write",
//         new FunctionDescriptor(
//             "write",
//             {new VariableDescriptor(
//                 "x", SymbolTable::lookfor_type(TYPE_BASIC_INT64), false, false)},
//             SymbolTable::lookfor_type(TYPE_BASIC_VOID)));

//     CodeCollector::push_back(
//         "void write_integer_string(int a1,string a2){cout<<a1<<\" \"<<a2;}");
//     SymbolTable::insert_function(
//         "write",
//         new FunctionDescriptor(
//             "write",
//             {new VariableDescriptor(
//                  "a1", SymbolTable::lookfor_type(TYPE_BASIC_INT), false, false),
//              new VariableDescriptor("a2",
//                                     SymbolTable::lookfor_type(TYPE_BASIC_STRING),
//                                     false, false)},
//             SymbolTable::lookfor_type(TYPE_BASIC_VOID)));
// }
// void init_std_read() {
//     // read int以及其符号
//     CodeCollector::push_back("void read_integer(int *x){scanf(\"%d\",x);}");
//     SymbolTable::insert_function(
//         "read",
//         new FunctionDescriptor(
//             "read",
//             {new VariableDescriptor(
//                 "x", SymbolTable::lookfor_type(TYPE_BASIC_INT), true, false)},
//             SymbolTable::lookfor_type(TYPE_BASIC_VOID)));
    
//     CodeCollector::push_back("void read_integer_integer(int *x,int *y){scanf(\"%d%d\",x,y);}");
//     SymbolTable::insert_function(
//         "read",
//         new FunctionDescriptor(
//             "read",
//             {new VariableDescriptor(
//                 "x", SymbolTable::lookfor_type(TYPE_BASIC_INT), true, false),new VariableDescriptor(
//                 "y", SymbolTable::lookfor_type(TYPE_BASIC_INT), true, false)},
//             SymbolTable::lookfor_type(TYPE_BASIC_VOID)));

//     CodeCollector::push_back(
//         "void read_int64(long long *x){scanf(\"%lld\",x);}");
//     SymbolTable::insert_function(
//         "read",
//         new FunctionDescriptor(
//             "read",
//             {new VariableDescriptor(
//                 "x", SymbolTable::lookfor_type(TYPE_BASIC_INT64), true, false)},
//             SymbolTable::lookfor_type(TYPE_BASIC_VOID)));
// }