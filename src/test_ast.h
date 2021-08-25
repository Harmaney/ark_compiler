#pragma once
#include "data.h"
#include "gen.h"
#include "codegen.h"
#include <vector>
#include <fstream>

using namespace std;

void TEST_vardecl() {
    auto ast = new VariableDeclAST(new VariableExprAST("a"),
                                   new BasicTypeAST(TYPE_BASIC_INT),false,false);

    cout<<code_gen_work(ast);
}

void TEST_arraydecl() {
    auto ast = new VariableDeclAST(
        new VariableExprAST("a"),
        new ArrayTypeDeclAST(new BasicTypeAST(TYPE_BASIC_INT), new NumberExprAST(1), new NumberExprAST(2)),false,false);

    cout<<code_gen_work(ast);
}

void TEST_arrayofarray() {
    BlockAST *ast = new BlockAST({new VariableDeclAST(
        new VariableExprAST("a"),
        new ArrayTypeDeclAST(new ArrayTypeDeclAST(new BasicTypeAST(TYPE_BASIC_INT),new NumberExprAST(1),new NumberExprAST(10)), new NumberExprAST(1), new NumberExprAST(100)),false,false)});

    cout<<code_gen_work(ast);
}

void TEST_arrayuse() {
    auto ast = new BlockAST({
        new VariableDeclAST(
            new VariableExprAST("a"),
            new ArrayTypeDeclAST(new BasicTypeAST(TYPE_BASIC_INT), new NumberExprAST(1), new NumberExprAST(2)),
            false,false
        ),
        new VariableDeclAST(
            new VariableExprAST("i"),
            new BasicTypeAST(TYPE_BASIC_INT),
            false,false
        ),
        new BinaryExprAST(
            ":=",
            new VariableExprAST("i"),
            new BinaryExprAST(
                "[]",
                new VariableExprAST("a"),
                new NumberExprAST(1)
            )
        ),
    });

    cout<<code_gen_work(ast);
}

void TEST_arrayofarrayuse() {
    auto ast = new BlockAST({
        new VariableDeclAST(
            new VariableExprAST("a"),
            new ArrayTypeDeclAST(new ArrayTypeDeclAST(new BasicTypeAST(TYPE_BASIC_INT),new NumberExprAST(1),new NumberExprAST(10)), 
            new NumberExprAST(1),
            new NumberExprAST(100)),
            false,false
        ),
        new VariableDeclAST(
            new VariableExprAST("i"),
            new BasicTypeAST(TYPE_BASIC_INT),
            false,false
        ),
        new BinaryExprAST(
            ":=",
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
            ":=",
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

    cout<<code_gen_work(ast);
}

void TEST_struct() {
    BlockAST *block_ast=new BlockAST(
        {
            new StructDeclAST(
                "Test",
                {
                    new ParameterDeclAST(
                        new VariableExprAST("a"),
                        new BasicTypeAST(TYPE_BASIC_INT),
                        false,false
                    )
                }
            ),
            new VariableDeclAST(
                new VariableExprAST("test"),
                new BasicTypeAST("Test"),
                false,false
            ),
            new VariableDeclAST(
                new VariableExprAST("p"),
                new BasicTypeAST(TYPE_BASIC_INT),
                false,false
            ),
            new BinaryExprAST(
                ":=",
                new VariableExprAST("p"),
                new BinaryExprAST(
                    ".",
                    new VariableExprAST("test"),
                    new StringExprAST("a")
                )
            )
        }
    );

    GlobalAST *ast=new GlobalAST({},{},{},block_ast);

    cout<<code_gen_work(ast);
}

void TEST_arrinstruct(){
    BlockAST *ast=new BlockAST({
        new StructDeclAST(
            "SP",
            {
                new ParameterDeclAST(
                    new VariableExprAST("arr1"),
                    new ArrayTypeDeclAST(new BasicTypeAST(TYPE_BASIC_INT),new NumberExprAST(0),new NumberExprAST(10)),
                    false,false
                ),
            }
        ),
        new VariableDeclAST(
            new VariableExprAST("sp"),
            new ArrayTypeDeclAST(new BasicTypeAST("SP"),new NumberExprAST(0),new NumberExprAST(10)),
            false,false
        ),
        new BinaryExprAST(
            ":=",
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

    cout<<code_gen_work(ast);
}

void TEST_while() {
    BlockAST *env = new BlockAST({});
    env->exprs.push_back(new VariableDeclAST(new VariableExprAST("i"),
                                             new BasicTypeAST(TYPE_BASIC_INT),false,false));

    BlockAST *while_block = new BlockAST({});

    while_block->exprs.push_back(new VariableDeclAST(
        new VariableExprAST("domjudge"), new BasicTypeAST(TYPE_BASIC_INT),false,false));
    WhileStatementAST *while_ast = new WhileStatementAST(
        new BinaryExprAST("<", new VariableExprAST("i"), new NumberExprAST(5)),
        while_block);

    env->exprs.push_back(while_ast);

    cout<<code_gen_work(env);
}

void TEST_for() {
    BlockAST *env = new BlockAST({});
    env->exprs.push_back(new VariableDeclAST(new VariableExprAST("i"),
                                             new BasicTypeAST(TYPE_BASIC_INT),false,false));

    BlockAST *for_block = new BlockAST({});

    for_block->exprs.push_back(new VariableDeclAST(
        new VariableExprAST("domjudge"), new BasicTypeAST(TYPE_BASIC_DOUBLE),false,false));
    ForStatementAST *for_ast =
        new ForStatementAST(new VariableExprAST("i"), new NumberExprAST(1),
                            new NumberExprAST(5), for_block);

    env->exprs.push_back(for_ast);

    code_gen_work(env);
}

void TEST_funcdef(){
    auto ast=new FunctionAST(
        new FunctionSignatureAST(
            "tryy",
            {
                new ParameterDeclAST(
                    new VariableExprAST("a"),
                    new BasicTypeAST(TYPE_BASIC_INT),
                    true,false
                ),
            },
            new BasicTypeAST(TYPE_BASIC_INT)
        ),
        {},
        new BlockAST({
            new BinaryExprAST(
                ":=",
                new VariableExprAST("a"),
                new NumberExprAST(1)
            )
        })
    );

    cout<<code_gen_work(ast);
}

void TEST_funccall() {
    BlockAST *block = new BlockAST({
        new FunctionAST(
            new FunctionSignatureAST(
                "tryy",
                {
                    new ParameterDeclAST(
                        new VariableExprAST("a"),
                        new BasicTypeAST(TYPE_BASIC_INT),
                        true,false
                    ),
                },
                new BasicTypeAST(TYPE_BASIC_INT)
            ),
            {},
            new BlockAST({
                new BinaryExprAST(
                    ":=",
                    new VariableExprAST("a"),
                    new NumberExprAST(1)
                )
            })
        ),
        new VariableDeclAST(
            new VariableExprAST("a"),
            new BasicTypeAST(TYPE_BASIC_INT),
            false,
            false
        ),
        new BinaryExprAST(
            ":=",
            new VariableExprAST("a"),
            new NumberExprAST(123)
        ),
        new CallExprAST(
            "tryy",
            {
                new VariableExprAST("a")
            }
        )
    });

    GlobalAST *global = new GlobalAST({}, {},{},block);

    cout<<code_gen_work(global);
}

void TEST_pointer() {
    BlockAST *ast = new BlockAST(
        {new VariableDeclAST(new VariableExprAST("a"),
                             new BasicTypeAST(TYPE_BASIC_INT),false,false),
         new BinaryExprAST("=", new VariableExprAST("a"), new NumberExprAST(1)),
         new VariableDeclAST(
             new VariableExprAST("p"),
             new PointerTypeDeclAST(new BasicTypeAST(TYPE_BASIC_INT)),false,false),
         new BinaryExprAST("=", new VariableExprAST("a"),
                           new UnaryExprAST("*", new VariableExprAST("p")))});

    code_gen_work(ast);
}

void TEST_case1() {
    GlobalAST *global = new GlobalAST({
        new VariableDeclAST(
            new VariableExprAST("i"),
            new BasicTypeAST(TYPE_BASIC_INT),
            false,false
        )},
        {},
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

    code_gen_work(global);
}

void TEST_fibonacci(){
    GlobalAST *global = new GlobalAST(
        {
            new VariableDeclAST(
                new VariableExprAST("i"),
                new BasicTypeAST(TYPE_BASIC_INT),
                false,false
            ),
            new VariableDeclAST(
                new VariableExprAST("a"),
                new BasicTypeAST(TYPE_BASIC_INT),
                false,false
            ),
            new VariableDeclAST(
                new VariableExprAST("b"),
                new BasicTypeAST(TYPE_BASIC_INT),
                false,false
            ),
            new VariableDeclAST(
                new VariableExprAST("c"),
                new BasicTypeAST(TYPE_BASIC_INT),
                false,false
            ),
        },
        {},
        {
            new FunctionAST(
                new FunctionSignatureAST(
                    "add",
                    {
                        new ParameterDeclAST (new VariableExprAST("a"),new BasicTypeAST(TYPE_BASIC_INT),false,false),
                        new ParameterDeclAST(new VariableExprAST("b"),new BasicTypeAST(TYPE_BASIC_INT),false,false),
                        new ParameterDeclAST(new VariableExprAST("c"),new BasicTypeAST(TYPE_BASIC_INT),true,false),
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

    code_gen_work(global);
}

////////////////////////////////////////////////////////////

void TEST_typeequal(){
    auto *ast=new BlockAST({
        new VariableDeclAST(new VariableExprAST("a"),new BasicTypeAST(TYPE_BASIC_INT),false,false),
        new BinaryExprAST("=",new VariableExprAST("a"),new NumberExprAST(1)),
    });

    code_gen_work(ast);
}

void TEST_typeassignerror(){
    auto *ast=new BlockAST({
        new VariableDeclAST(new VariableExprAST("a"),new BasicTypeAST(TYPE_BASIC_INT),false,false),
        new BinaryExprAST("=",new VariableExprAST("a"),new NumberExprAST(1.1)),
    });

    code_gen_work(ast);
}

void TEST_leftvarassignerror(){
    auto *ast=new BlockAST({
        new VariableDeclAST(new VariableExprAST("a"),new BasicTypeAST(TYPE_BASIC_INT),false,false),
        new BinaryExprAST("=",new NumberExprAST(1),new VariableExprAST("a")),
    });

    code_gen_work(ast);
}

void TEST_undefinedval(){
    auto *ast=new BlockAST({
        new VariableDeclAST(new VariableExprAST("a"),new BasicTypeAST(TYPE_BASIC_INT),false,false),
        new BinaryExprAST("=",new VariableExprAST("b"),new NumberExprAST(1)),
    });

    code_gen_work(ast);
}

void TEST_nomember(){
    auto *ast=new BlockAST({
        new StructDeclAST(
            "st",
            {
                new ParameterDeclAST(new VariableExprAST("a"),new BasicTypeAST(TYPE_BASIC_INT),false,false)
            }
        ),
        new VariableDeclAST(new VariableExprAST("a"),new BasicTypeAST("st"),false,false),
        new BinaryExprAST("=",new BinaryExprAST(".",new VariableExprAST("a"),new StringExprAST("a")),new NumberExprAST(1)),
    });

    code_gen_work(ast);
}

void TEST_opbetweendiffobj(){
    auto *ast=new BlockAST({
        new VariableDeclAST(new VariableExprAST("a"),new BasicTypeAST(TYPE_BASIC_INT),false,false),
        new VariableDeclAST(new VariableExprAST("b"),new BasicTypeAST(TYPE_BASIC_STRING),false,false),
        new BinaryExprAST("<",new VariableExprAST("a"),new VariableExprAST("b")),
    });

    code_gen_work(ast);
}

int RUN_TEST(){
    // TEST_vardecl();
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

    return 0;
}