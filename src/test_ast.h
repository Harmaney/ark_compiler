#pragma once
#include "data.h"
#include "gen.h"
#include "codegen.h"
#include <vector>
#include <fstream>

using namespace std;

void TEST_vardecl() {
    auto ast = new VariableDeclAST(new VariableExprAST("a"),
                                   new BasicTypeAST(TYPE_BASIC_INT));

    ASTDispatcher dispatcher;
    ast->accept(dispatcher);
}

void TEST_arraydecl() {
    auto ast = new VariableDeclAST(
        new VariableExprAST("a"),
        new ArrayTypeDeclAST(new BasicTypeAST(TYPE_BASIC_INT), 1, 2));

    ASTDispatcher dispatcher;
    ast->accept(dispatcher);
}

void TEST_arrayofarray() {
    BlockAST *ast = new BlockAST({new VariableDeclAST(
        new VariableExprAST("a"),
        new ArrayTypeDeclAST(new ArrayTypeDeclAST(new BasicTypeAST(TYPE_BASIC_INT),1,10), 1, 100))});

    ASTDispatcher dispatcher;
    ast->accept(dispatcher);
}

void TEST_arrayuse() {
    auto ast = new BlockAST({
        new VariableDeclAST(
            new VariableExprAST("a"),
            new ArrayTypeDeclAST(new BasicTypeAST(TYPE_BASIC_INT), 1, 2)
        ),
        new VariableDeclAST(
            new VariableExprAST("i"),
            new BasicTypeAST(TYPE_BASIC_INT)
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
            new ArrayTypeDeclAST(new ArrayTypeDeclAST(new BasicTypeAST(TYPE_BASIC_INT),1,10), 
            1,
            100)
        ),
        new VariableDeclAST(
            new VariableExprAST("i"),
            new BasicTypeAST(TYPE_BASIC_INT)
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
    std::vector<VariableDeclAST *> t;
    t.push_back(new VariableDeclAST(new VariableExprAST("a"),
                                    new BasicTypeAST(TYPE_BASIC_INT)));
    StructDeclAST *ast = new StructDeclAST("Test", t);

    ASTDispatcher dispacher;
    ast->accept(dispacher);
}

void TEST_while() {
    BlockAST *env = new BlockAST({});
    env->exprs.push_back(new VariableDeclAST(new VariableExprAST("i"),
                                             new BasicTypeAST(TYPE_BASIC_INT)));

    BlockAST *while_block = new BlockAST({});

    while_block->exprs.push_back(new VariableDeclAST(
        new VariableExprAST("domjudge"), new BasicTypeAST(TYPE_BASIC_INT)));
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
                                             new BasicTypeAST(TYPE_BASIC_INT)));

    BlockAST *for_block = new BlockAST({});

    for_block->exprs.push_back(new VariableDeclAST(
        new VariableExprAST("domjudge"), new BasicTypeAST(TYPE_BASIC_DOUBLE)));
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
                    new VariableExprAST("a",true),
                    new BasicTypeAST(TYPE_BASIC_INT)
                ),
            },
            new BasicTypeAST(TYPE_BASIC_INT)
        ),
        new BlockAST({
            new BinaryExprAST(
                "=",
                new VariableExprAST("a",true),
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
        new VariableExprAST("a"), new BasicTypeAST(TYPE_BASIC_INT));
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
                             new BasicTypeAST(TYPE_BASIC_INT)),
         new BinaryExprAST("=", new VariableExprAST("a"), new NumberExprAST(1)),
         new VariableDeclAST(
             new VariableExprAST("p"),
             new PointerTypeDeclAST(new BasicTypeAST(TYPE_BASIC_INT))),
         new BinaryExprAST("=", new VariableExprAST("a"),
                           new UnaryExprAST("*", new VariableExprAST("p")))});

    ASTDispatcher dispatcher;
    ast->accept(dispatcher);
}

void TEST_case1() {
    GlobalAST *global = new GlobalAST({
        new VariableDeclAST(
            new VariableExprAST("i"),
            new BasicTypeAST(TYPE_BASIC_INT)
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
                new BasicTypeAST(TYPE_BASIC_INT)
            ),
            new VariableDeclAST(
                new VariableExprAST("a"),
                new BasicTypeAST(TYPE_BASIC_INT)
            ),
            new VariableDeclAST(
                new VariableExprAST("b"),
                new BasicTypeAST(TYPE_BASIC_INT)
            ),
            new VariableDeclAST(
                new VariableExprAST("c"),
                new BasicTypeAST(TYPE_BASIC_INT)
            ),
        },
        {},
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
                            new BinaryExprAST(
                                "=",
                                new VariableExprAST("c"),
                                new BinaryExprAST(
                                    "+",
                                    new VariableExprAST("a"),
                                    new VariableExprAST("b")
                                )
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

int RUN_TEST(){
    init_code_generator();
    init_basic_type();

    CodeCollector::begin_section();
    // TEST_vardecl();
    // TEST_arraydecl();
    // TEST_arrayuse();
    // TEST_arrayofarray();
    // TEST_arrayofarrayuse();
    // TEST_while();
    TEST_funcdef();
    // TEST_funccall();
    // TEST_struct();
    // TEST_pointer();
    // TEST_case1();
    // TEST_fibonacci();

    CodeCollector::end_section(PLACE_END);

    CodeCollector::rearrange_section("global_define", 0);
    CodeCollector::rearrange_section("prelude", 0);

    ofstream codeOut("out.cpp");
    CodeCollector::output(codeOut);
}