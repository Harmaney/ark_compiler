#pragma once
#include "data.h"
#include "gen.h"
#include "codegen.h"
#include <vector>
#include <fstream>

using namespace std;

void TEST_vardecl() {
    auto ast = new VariableDeclAST(new VariableExprAST("a"),
                                   new BasicTypeAST(TYPE_BASIC_INT),false);

    ASTDispatcher dispatcher;
    ast->accept(dispatcher);
}

void TEST_arraydecl() {
    auto ast = new VariableDeclAST(
        new VariableExprAST("a"),
        new ArrayTypeDeclAST(new BasicTypeAST(TYPE_BASIC_INT), new NumberExprAST(1), new NumberExprAST(2)),false);

    ASTDispatcher dispatcher;
    ast->accept(dispatcher);
}

void TEST_arrayofarray() {
    BlockAST *ast = new BlockAST({new VariableDeclAST(
        new VariableExprAST("a"),
        new ArrayTypeDeclAST(new ArrayTypeDeclAST(new BasicTypeAST(TYPE_BASIC_INT),new NumberExprAST(1),new NumberExprAST(10)), new NumberExprAST(1), new NumberExprAST(100)),false)});

    ASTDispatcher dispatcher;
    ast->accept(dispatcher);
}

void TEST_arrayuse() {
    auto ast = new BlockAST({
        new VariableDeclAST(
            new VariableExprAST("a"),
            new ArrayTypeDeclAST(new BasicTypeAST(TYPE_BASIC_INT), new NumberExprAST(1), new NumberExprAST(2)),
            false
        ),
        new VariableDeclAST(
            new VariableExprAST("i"),
            new BasicTypeAST(TYPE_BASIC_INT),
            false
        ),
        new BinaryExprAST(
            "=",
            new VariableExprAST("i"),
            new BinaryExprAST(
                "[]",
                new VariableExprAST("a"),
                new NumberExprAST(1)
            )
        ),
    });

    ASTDispatcher dispatcher;
    ast->accept(dispatcher);
}

void TEST_arrayofarrayuse() {
    auto ast = new BlockAST({
        new VariableDeclAST(
            new VariableExprAST("a"),
            new ArrayTypeDeclAST(new ArrayTypeDeclAST(new BasicTypeAST(TYPE_BASIC_INT),new NumberExprAST(1),new NumberExprAST(10)), 
            new NumberExprAST(1),
            new NumberExprAST(100)),
            false
        ),
        new VariableDeclAST(
            new VariableExprAST("i"),
            new BasicTypeAST(TYPE_BASIC_INT),
            false
        ),
        new BinaryExprAST(
            "=",
            new BinaryExprAST(
                "[]",
                new BinaryExprAST(
                    "[]",
                    new VariableExprAST("a"),
                    new NumberExprAST(2)
                ),
                new NumberExprAST(1)
            ),
            new NumberExprAST(233)
        ),
        new BinaryExprAST(
            "=",
            new VariableExprAST("i"),
            new BinaryExprAST(
                "[]",
                new BinaryExprAST(
                    "[]",
                    new VariableExprAST("a"),
                    new NumberExprAST(2)
                ),
                new NumberExprAST(1)
            )
        ),
    });

    ASTDispatcher dispatcher;
    ast->accept(dispatcher);
}

void TEST_struct() {
    BlockAST *block_ast=new BlockAST(
        {
            new StructDeclAST(
                "Test",
                {
                    new VariableDeclAST(
                        new VariableExprAST("a"),
                        new BasicTypeAST(TYPE_BASIC_INT),
                        false
                    )
                }
            ),
            new VariableDeclAST(
                new VariableExprAST("test"),
                new BasicTypeAST("Test"),
                false
            ),
            new VariableDeclAST(
                new VariableExprAST("p"),
                new BasicTypeAST(TYPE_BASIC_INT),
                false
            ),
            new BinaryExprAST(
                "=",
                new BinaryExprAST(
                    ".",
                    new VariableExprAST("test"),
                    new StringExprAST("a")
                ),
                new CallExprAST("read_int",{})
            ),
            new BinaryExprAST(
                "=",
                new VariableExprAST("p"),
                new BinaryExprAST(
                    ".",
                    new VariableExprAST("test"),
                    new StringExprAST("a")
                )
            ),
            new CallExprAST(
                "write_int",
                {
                    new VariableExprAST("p")
                }
            )
        }
    );

    GlobalAST *ast=new GlobalAST({},{},block_ast);

    ASTDispatcher dispacher;
    ast->accept(dispacher);
}

void TEST_arrinstruct(){
    BlockAST *ast=new BlockAST({
        new StructDeclAST(
            "SP",
            {
                new VariableDeclAST(
                    new VariableExprAST("arr1"),
                    new ArrayTypeDeclAST(new BasicTypeAST(TYPE_BASIC_INT),new NumberExprAST(0),new NumberExprAST(10)),
                    false
                ),
            }
        ),
        new VariableDeclAST(
            new VariableExprAST("sp"),
            new ArrayTypeDeclAST(new BasicTypeAST("SP"),new NumberExprAST(0),new NumberExprAST(10)),
            false
        ),
        new BinaryExprAST(
            "=",
            new BinaryExprAST(
                "[]",
                new BinaryExprAST(
                    ".",
                    new BinaryExprAST(
                        "[]",
                        new VariableExprAST("sp"),
                        new NumberExprAST(0)
                    ),
                    new StringExprAST("arr1")
                ),
                new NumberExprAST(2)
            ),
            new NumberExprAST(2)
        )
    });

    ASTDispatcher dispatcher;
    ast->accept(dispatcher);
}

void TEST_while() {
    BlockAST *env = new BlockAST({});
    env->exprs.push_back(new VariableDeclAST(new VariableExprAST("i"),
                                             new BasicTypeAST(TYPE_BASIC_INT),false));

    BlockAST *while_block = new BlockAST({});

    while_block->exprs.push_back(new VariableDeclAST(
        new VariableExprAST("domjudge"), new BasicTypeAST(TYPE_BASIC_INT),false));
    WhileStatementAST *while_ast = new WhileStatementAST(
        new BinaryExprAST("<", new VariableExprAST("i"), new NumberExprAST(5)),
        while_block);

    env->exprs.push_back(while_ast);

    ASTDispatcher dispacher;
    env->accept(dispacher);
}

void TEST_for() {
    BlockAST *env = new BlockAST({});
    env->exprs.push_back(new VariableDeclAST(new VariableExprAST("i"),
                                             new BasicTypeAST(TYPE_BASIC_INT),false));

    BlockAST *for_block = new BlockAST({});

    for_block->exprs.push_back(new VariableDeclAST(
        new VariableExprAST("domjudge"), new BasicTypeAST(TYPE_BASIC_DOUBLE),false));
    ForStatementAST *for_ast =
        new ForStatementAST(new VariableExprAST("i"), new NumberExprAST(1),
                            new NumberExprAST(5), for_block);

    env->exprs.push_back(for_ast);

    ASTDispatcher dispacher;
    env->accept(dispacher);
}

void TEST_funcdef(){
    auto ast=new FunctionAST(
        new FunctionSignatureAST(
            "tryy",
            {
                new VariableDeclAST(
                    new VariableExprAST("a"),
                    new BasicTypeAST(TYPE_BASIC_INT),
                    true
                ),
            },
            new BasicTypeAST(TYPE_BASIC_INT)
        ),
        {},
        new BlockAST({
            new BinaryExprAST(
                "=",
                new VariableExprAST("a"),
                new NumberExprAST(1)
            )
        })
    );

    ASTDispatcher dispacher;
    ast->accept(dispacher);
}

void TEST_funccall() {
    BlockAST *block = new BlockAST({});

    VariableDeclAST *decl = new VariableDeclAST(
        new VariableExprAST("a"), new BasicTypeAST(TYPE_BASIC_INT),false);
    block->exprs.push_back(decl);

    BinaryExprAST *t1 = new BinaryExprAST("=", new VariableExprAST("a"),
                                          new NumberExprAST(123));
    block->exprs.push_back(t1);

    vector<ExprAST *> callargs;
    callargs.push_back(
        new VariableExprAST("a"));
    CallExprAST *call = new CallExprAST("write_int", callargs);
    block->exprs.push_back(call);

    GlobalAST *global = new GlobalAST({}, {},block);

    ASTDispatcher dispacher;
    global->accept(dispacher);
}

void TEST_pointer() {
    BlockAST *ast = new BlockAST(
        {new VariableDeclAST(new VariableExprAST("a"),
                             new BasicTypeAST(TYPE_BASIC_INT),false),
         new BinaryExprAST("=", new VariableExprAST("a"), new NumberExprAST(1)),
         new VariableDeclAST(
             new VariableExprAST("p"),
             new PointerTypeDeclAST(new BasicTypeAST(TYPE_BASIC_INT)),false),
         new BinaryExprAST("=", new VariableExprAST("a"),
                           new UnaryExprAST("*", new VariableExprAST("p")))});

    ASTDispatcher dispatcher;
    ast->accept(dispatcher);
}

void TEST_case1() {
    GlobalAST *global = new GlobalAST({
        new VariableDeclAST(
            new VariableExprAST("i"),
            new BasicTypeAST(TYPE_BASIC_INT),
            false
        )},
        {},
        new BlockAST(
            {
                new ForStatementAST(
                    new VariableExprAST("i"),
                    new NumberExprAST(1),
                    new NumberExprAST(5),
                    new BlockAST(
                        {
                            new CallExprAST("write_int", {
                                new VariableExprAST("i")
                            }
                            ),
                            new CallExprAST(
                                "write_str",
                                {
                                    new StringExprAST("hello, world\\n")
                                }
                            )
                        }
                    )
                )
            }
        )
    );

    ASTDispatcher dispacher;
    global->accept(dispacher);
}

void TEST_fibonacci(){
    GlobalAST *global = new GlobalAST(
        {
            new VariableDeclAST(
                new VariableExprAST("i"),
                new BasicTypeAST(TYPE_BASIC_INT),
                false
            ),
            new VariableDeclAST(
                new VariableExprAST("a"),
                new BasicTypeAST(TYPE_BASIC_INT),
                false
            ),
            new VariableDeclAST(
                new VariableExprAST("b"),
                new BasicTypeAST(TYPE_BASIC_INT),
                false
            ),
            new VariableDeclAST(
                new VariableExprAST("c"),
                new BasicTypeAST(TYPE_BASIC_INT),
                false
            ),
        },
        {
            new FunctionAST(
                new FunctionSignatureAST(
                    "add",
                    {
                        new VariableDeclAST(new VariableExprAST("a"),new BasicTypeAST(TYPE_BASIC_INT),false),
                        new VariableDeclAST(new VariableExprAST("b"),new BasicTypeAST(TYPE_BASIC_INT),false),
                        new VariableDeclAST(new VariableExprAST("c"),new BasicTypeAST(TYPE_BASIC_INT),true),
                    },
                    new BasicTypeAST(TYPE_BASIC_VOID)
                ),
                {},
                new BlockAST(
                    {
                        new BinaryExprAST(
                            "=",
                            new VariableExprAST("c"),
                            new BinaryExprAST(
                                "+",
                                new VariableExprAST("a"),
                                new VariableExprAST("b")
                            )
                        ),
                    }
                )
            )
        },
        new BlockAST(
            {
                new BinaryExprAST(
                    "=",
                    new VariableExprAST("a"),
                    new NumberExprAST(0)
                ),
                new BinaryExprAST(
                    "=",
                    new VariableExprAST("b"),
                    new NumberExprAST(1)
                ),
                new ForStatementAST(
                    new VariableExprAST("i"),
                    new NumberExprAST(1),
                    new NumberExprAST(5),
                    new BlockAST(
                        {
                            new CallExprAST(
                                "add",
                                {
                                    new VariableExprAST("a"),
                                    new VariableExprAST("b"),
                                    new VariableExprAST("c"),
                                }
                            ),
                            new BinaryExprAST(
                                "=",
                                new VariableExprAST("a"),
                                new VariableExprAST("b")
                            ),
                            new BinaryExprAST(
                                "=",
                                new VariableExprAST("b"),
                                new VariableExprAST("c")
                            ),
                            new CallExprAST("write_int", 
                                {
                                    new VariableExprAST("a")
                                }
                            ),
                            new CallExprAST(
                                "write_str",
                                {
                                    new StringExprAST("\\n")
                                }
                            )
                        }
                    )
                )
            }
        )
    );

    ASTDispatcher dispacher;
    global->accept(dispacher);
}

////////////////////////////////////////////////////////////

void TEST_typeequal(){
    auto *ast=new BlockAST({
        new VariableDeclAST(new VariableExprAST("a"),new BasicTypeAST(TYPE_BASIC_INT),false),
        new BinaryExprAST("=",new VariableExprAST("a"),new NumberExprAST(1)),
    });

    ASTDispatcher dispatcher;
    ast->accept(dispatcher);
}

void TEST_typeassignerror(){
    auto *ast=new BlockAST({
        new VariableDeclAST(new VariableExprAST("a"),new BasicTypeAST(TYPE_BASIC_INT),false),
        new BinaryExprAST("=",new VariableExprAST("a"),new NumberExprAST(1.1)),
    });

    ASTDispatcher dispatcher;
    ast->accept(dispatcher);
}

void TEST_leftvarassignerror(){
    auto *ast=new BlockAST({
        new VariableDeclAST(new VariableExprAST("a"),new BasicTypeAST(TYPE_BASIC_INT),false),
        new BinaryExprAST("=",new NumberExprAST(1),new VariableExprAST("a")),
    });

    ASTDispatcher dispatcher;
    ast->accept(dispatcher);
}

void TEST_undefinedval(){
    auto *ast=new BlockAST({
        new VariableDeclAST(new VariableExprAST("a"),new BasicTypeAST(TYPE_BASIC_INT),false),
        new BinaryExprAST("=",new VariableExprAST("b"),new NumberExprAST(1)),
    });

    ASTDispatcher dispatcher;
    ast->accept(dispatcher);
}

void TEST_nomember(){
    auto *ast=new BlockAST({
        new StructDeclAST(
            "st",
            {
                new VariableDeclAST(new VariableExprAST("a"),new BasicTypeAST(TYPE_BASIC_INT),false)
            }
        ),
        new VariableDeclAST(new VariableExprAST("a"),new BasicTypeAST("st"),false),
        new BinaryExprAST("=",new BinaryExprAST(".",new VariableExprAST("a"),new StringExprAST("a")),new NumberExprAST(1)),
    });

    ASTDispatcher dispatcher;
    ast->accept(dispatcher);
}

void TEST_opbetweendiffobj(){
    auto *ast=new BlockAST({
        new VariableDeclAST(new VariableExprAST("a"),new BasicTypeAST(TYPE_BASIC_INT),false),
        new VariableDeclAST(new VariableExprAST("b"),new BasicTypeAST(TYPE_BASIC_STRING),false),
        new BinaryExprAST("<",new VariableExprAST("a"),new VariableExprAST("b")),
    });

    ASTDispatcher dispatcher;
    ast->accept(dispatcher);
}

int RUN_TEST(){
    init_code_generator();
    init_basic_type();

    CodeCollector::begin_section("main");
    TEST_vardecl();
    // TEST_arraydecl();
    // TEST_arrayuse();
    // TEST_arrayofarray();
    // TEST_arrayofarrayuse();
    // TEST_while();
    // TEST_funcdef();
    // TEST_funccall();
    // TEST_struct();
    TEST_arrinstruct();
    // TEST_pointer();
    // TEST_case1();
    // TEST_fibonacci();

    // TEST_typeequal();
    // TEST_typeassignerror();
    // TEST_leftvarassignerror();
    // TEST_undefinedval();
    // TEST_nomember();
    // TEST_opbetweendiffobj();

    CodeCollector::end_section(PLACE_END);
    
    CodeCollector::rearrange_section("struct", 0);
    CodeCollector::rearrange_section("pre_array", 0);
    CodeCollector::rearrange_section("pre_struct", 0);
    CodeCollector::rearrange_section("global_define", 0);
    CodeCollector::rearrange_section("prelude", 0);

    ofstream codeOut("out.cpp");
    CodeCollector::output(codeOut);

    return 0;
}