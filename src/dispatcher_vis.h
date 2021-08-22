#include "gen.h"

class VisDispatcher : public ADispatcher {
   private:
    VMContext *context;

   public:
    VisDispatcher(VMContext *context) : context(context) {}

    std::any gen_global(GlobalAST *ast) override;

    std::any gen_array_type_decl(ArrayTypeDeclAST *ast) override;
    std::any gen_basic_type(BasicTypeAST *ast) override;
    std::any gen_pointer_type_decl(PointerTypeDeclAST *ast) override;
    std::any gen_type_def(TypeDefAST *ast) override;

    std::any gen_number_expr(NumberExprAST *ast) override;
    std::any gen_string_expr(StringExprAST *ast) override;
    std::any gen_char_expr(CharExprAST *ast) override;
    std::any gen_variable_expr(VariableExprAST *ast) override;
    std::any gen_return(ReturnAST *ast) override;
    std::any gen_unary_expr(UnaryExprAST *ast) override;
    std::any gen_binary_expr(BinaryExprAST *ast) override;
    std::any gen_call_expr(CallExprAST *ast) override;

    std::any gen_if_statement(IfStatementAST *ast) override;
    std::any gen_while_statement(WhileStatementAST *ast) override;
    std::any gen_for_statement(ForStatementAST *ast) override;

    std::any gen_function(FunctionAST *ast) override;
    std::any gen_function_signature(FunctionSignatureAST *ast) override;

    std::any gen_struct(StructDeclAST *ast) override;

    std::any gen_block(BlockAST *ast) override;

    std::any gen_variable_decl(VariableDeclAST *ast) override;
};