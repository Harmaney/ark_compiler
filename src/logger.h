#ifndef LOGGER_HPP
#define LOGGER_HPP

static int LOG_LEVEL = 4;

// FATAL    ERROR   WARN    INFO    DEBUG   TRACE
// 0        1       2       3       4       5

#define TRACE(a)          \
    if (5 <= LOG_LEVEL) { \
        a                 \
    }
#define DEBUG(a)          \
    if (4 <= LOG_LEVEL) { \
        a                 \
    }
#define INFO(a)           \
    if (3 <= LOG_LEVEL) { \
        a                 \
    }
#define WARN(a)           \
    if (2 <= LOG_LEVEL) { \
        a                 \
    }
#define ERROR(a)          \
    if (1 <= LOG_LEVEL) { \
        a                 \
    }
#define FATAL(a)          \
    if (0 <= LOG_LEVEL) { \
        a                 \
    }

#include <fstream>

#include "data.h"
#include "json.hpp"
#include "parser.h"

using Json = nlohmann::json;

extern std::ofstream WALK_AST;

template <typename... T>
Json Serialize(AST* ptr) {
    Json ret;
    ret.push_back(uint64_t(any_cast<AST*>(ptr)));
    return ret;
}

template <typename... T>
Json Serialize(std::any ptr, const T&... arg) {
    Json ret;
    ret.push_back(uint64_t(any_cast<AST*>(ptr)));
    for (auto i : Serialize(arg...)) ret.push_back(i);
    return ret;
}

template <typename... T>
Json Serialize(const vector<std::any>& vec) {
    Json ret;
    for (auto ptr : vec) ret.push_back(uint64_t(any_cast<AST*>(ptr)));
    return ret;
}

template <typename... T>
Json Serialize(const vector<std::any>& vec, const T&... arg) {
    Json ret;
    for (auto ptr : vec) ret.push_back(uint64_t(any_cast<AST*>(ptr)));
    for (auto i : Serialize(arg...)) ret.push_back(i);
    return ret;
}

Json ASTNodeInfo(GlobalAST* node) { return {{"name", "GlobalAST"}, {"son", Serialize(node->vars, node->functions, node->mainBlock)}}; }

Json ASTNodeInfo(UnaryExprAST* node) {}

Json ASTNodeInfo(VariableDeclAST* node) {}

Json ASTNodeInfo(VariableExprAST* node) {}

struct ParserOutputer {
    Json j;
    /*
      address :
      type: 'pst' / 'ast'

    */
    ParserOutputer() : j(Json::array()) {}
    void AddNode(GrammarTreeNode* node) {
        using namespace NodeProperties;
        Json nj;
        if (node->type == "S") {  // S -> ProgramStruct
            ;
        } else if (node->type == "ProgramStruct") {
            nj["ast"] = ASTNodeInfo(cast<GlobalAST*>(node->prop));
        } else if (node->type == "ProgramHead") {
            ;
        } else if (node->type == "ProgramBody") {  // ProgramBody -> Component CompoundStatement @
            ;
        } else if (node->type == "Component") {
            if (node->son.empty()) {
                // Do nothing.
            } else if (node->son[0]->raw == "const") {  // Component -> const ConstDeclaration ; Component
                abort();
            } else if (node->son[0]->raw == "type") {  // Component -> type TypeDeclaration ; Component
                /*prop = cast<Component>(node->son[3]->prop);
                prop["type"].push_back(node->son[1]->prop);  /// fixme*/
            } else if (node->son[0]->raw == "var") {  // Component -> var VarDeclaration ; Component
                /*prop = cast<Component>(node->son[3]->prop);
                for (auto i : cast<VarDeclaration>(node->son[1]->prop)) prop["var"].push_back(i);*/
            } else if (node->son[0]->type == "Subprogram") {  // Component -> Subprogram ; Component
                /*prop = cast<Component>(node->son[2]->prop);
                prop["function"].insert(prop["function"].begin(), node->son[0]->prop);*/
            }
        } else if (node->type == "IDList") {
            if (node->son.size() == 1) {  // IDList -> ID
                ;
            } else {  // IDList -> IDList , ID
                ;
            }
        } else if (node->type == "ConstDeclaration") {  // fixme

        } else if (node->type == "ConstValue") {
            if (node->son[0]->raw == "+") {  // ConstValue -> + Num
                ASTNodeInfo(std::any_cast<UnaryExprAST*>(node->prop));
            } else if (node->son[0]->raw == "-") {  // ConstValue -> - Num
                ASTNodeInfo(std::any_cast<UnaryExprAST*>(node->prop));
            } else if (node->son.size() == 1) {  // ConstValue -> Num
                ;
            }
        } else if (node->type == "TypeDeclaration") {  // fixme
            ;
        } else if (node->type == "ActualType") {  // fixme
            ;
        } else if (node->type == "VarDeclaration") {
            for (auto vars : std::any_cast<std::vector<std::any>>(node->prop)) {
                auto p = std::any_cast<VariableDeclAST*>(vars);
                ASTNodeInfo(p->sig);
                ASTNodeInfo(p);
            }
        } else if (node->type == "Type") {
            Type prop;
            if (node->son[0]->raw == "array") {  // Type -> array [ Period ] of BasicType
                prop = cast<BasicType>(node->son[5]->prop);
                auto range = cast<Period>(node->son[2]->prop);
                reverse(range.begin(), range.end());
                for (auto [L, R] : range) {
                    prop = new ArrayTypeDeclAST(prop, L, R);
                }
                
            } else if (node->son[0]->type == "BasicType") {  // Type -> BasicType
                prop = cast<BasicType>(node->son[0]->prop);
            } else {  // Type -> ^ BasicType
                prop = new PointerTypeDeclAST(cast<BasicType>(node->son[1]->prop));
            }
            node->prop = prop;
        } else if (node->type == "BasicType") {
            BasicType prop = new BasicTypeAST(node->son[0]->raw);
            node->prop = prop;
        } else if (node->type == "Period") {
            Period prop;
            if (node->son.size() == 3) {  // Period -> Digits .. Digits
                prop = Period{make_pair(cast<Digits>(node->son[0]->prop), cast<Digits>(node->son[2]->prop))};
            } else {  // Period -> Period , Digits .. Digits
                prop = cast<Period>(node->son[0]->prop);
                prop.emplace_back(cast<Digits>(node->son[2]->prop), cast<Digits>(node->son[4]->prop));
            }
            node->prop = prop;
        } else if (node->type == "Subprogram") {  // Subprogram -> SubprogramHead ; SubprogramBody
            auto [parameter, body] = cast<SubprogramBody>(node->son[2]->prop);
            Subprogram prop = new FunctionAST(cast<SubprogramHead>(node->son[0]->prop), parameter, body);
            node->prop = prop;
        } else if (node->type == "SubprogramHead") {
            SubprogramHead prop;
            if (node->son.size() == 3) {  // SubprogramHead -> procedure ID FormalParameter
                prop = new FunctionSignatureAST(node->son[1]->raw, cast<FormalParameter>(node->son[2]->prop), new BasicTypeAST("void"));
            } else {  // SubprogramHead -> function ID FormalParameter : BasicType
                prop = new FunctionSignatureAST(node->son[1]->raw, cast<FormalParameter>(node->son[2]->prop), cast<BasicType>(node->son[4]->prop));
            }
            node->prop = prop;
        } else if (node->type == "FormalParameter") {
            FormalParameter prop;
            if (node->son.size() <= 2) {
                prop = FormalParameter();
            } else {  // FormalParameter -> ( ParameterList )
                prop = cast<ParameterList>(node->son[1]->prop);
            }
            node->prop = prop;
        } else if (node->type == "ParameterList") {
            ParameterList prop;
            if (node->son.size() == 1) {  // ParameterList -> Parameter
                prop = cast<ParameterList>(node->son[0]->prop);
            } else {  // ParameterList -> ParameterList ; Parameter
                prop = cast<ParameterList>(node->son[0]->prop);
                for (auto i : cast<Parameter>(node->son[2]->prop)) prop.push_back(i);
            }
            node->prop = prop;
        } else if (node->type == "Parameter") {
            // Parameter -> VarParameter
            // Parameter -> ValueParameter
            node->prop = cast<Parameter>(node->son[0]->prop);
        } else if (node->type == "VarParameter") {
            // VarParameter -> var ValueParameter
            VarParameter prop = cast<ValueParameter>(node->son[1]->prop);
            for (auto& ptr : prop) ptr->isRef = true;
            node->prop = prop;
        } else if (node->type == "ValueParameter") {
            // ValueParameter -> IDList : BasicType
            ValueParameter prop;
            std::vector<VariableDeclAST*> list;
            for (std::string i : cast<IDList>(node->son[0]->prop)) {
                prop.push_back(new VariableDeclAST(new VariableExprAST(i), cast<BasicType>(node->son[2]->prop), false));
            }
            node->prop = prop;
        } else if (node->type == "SubprogramBody") {
            // SubprogramBody -> SubComponent CompoundStatement
            node->prop = SubprogramBody(cast<SubComponent>(node->son[0]->prop), cast<CompoundStatement>(node->son[1]->prop));
        } else if (node->type == "SubComponent") {
            SubComponent prop;
            if (node->son.size() == 0)
                node->prop = {};
            else {
                // SubComponent -> const ConstDeclaration ; SubComponent
                // SubComponent -> var VarDeclaration ; SubComponent
                prop = cast<SubComponent>(node->son[3]->prop);
                if (node->son[0]->raw == "const") {
                    abort();
                } else {  // fixme
                    for (auto i : cast<VarDeclaration>(node->son[1]->prop)) prop.insert(prop.begin(), i);
                }
            }
            node->prop = prop;
        } else if (node->type == "CompoundStatement") {
            // CompoundStatement -> begin StatementList end
            node->prop = new BlockAST(cast<StatementList>(node->son[1]->prop));
        } else if (node->type == "StatementList") {
            StatementList prop;
            if (node->son.size() == 1) {  // StatementList -> Statement
                auto statement = cast<Statement>(node->son[0]->prop);
                if (!statement)
                    prop = StatementList{};
                else
                    prop = StatementList{statement};
            } else {  // StatementList -> StatementList ; Statement
                prop = cast<StatementList>(node->son[0]->prop);
                auto statement = cast<Statement>(node->son[2]->prop);
                if (statement) prop.push_back(statement);
            }
            node->prop = prop;
        } else if (node->type == "Statement") {
            Statement prop;
            if (node->son.empty()) {
                prop = nullptr;
            } else if (node->son[0]->type == "ComposedVariable") {
                // Statement -> ComposedVariable assignOP Expression
                prop = new BinaryExprAST(node->son[1]->raw, cast<ExprAST*>(node->son[0]->prop), cast<ExprAST*>(node->son[2]->prop));
            } else if (node->son[0]->type == "ProcedureCall(ARG)") {
                prop = cast<Statement>(node->son[0]->prop);
            } else if (node->son[0]->raw == "if") {
                // Statement -> if Expression then CompoundStatement ElsePart @
                prop = new IfStatementAST(cast<Expression>(node->son[1]->prop), cast<CompoundStatement>(node->son[3]->prop), cast<ElsePart>(node->son[4]->prop));
            } else if (node->son[0]->raw == "for") {
                // Statement -> for ID assignOP Expression to Expression do CompoundStatement
                prop = new ForStatementAST(new VariableExprAST(node->son[1]->raw), cast<Expression>(node->son[3]->prop), cast<Expression>(node->son[5]->prop), cast<CompoundStatement>(node->son[7]->prop));
            } else if (node->son[0]->raw == "while") {
                // Statement -> while Expression do CompoundStatement
                prop = new WhileStatementAST(cast<ExprAST*>(node->son[1]->prop), cast<CompoundStatement>(node->son[3]->prop));
            }
            node->prop = prop;
        } else if (node->type == "Variable") {  // Variable -> ID IDVarpart
            Variable prop = new VariableExprAST(node->son[0]->raw);
            auto part = cast<IDVarpart>(node->son[1]->prop);
            for (auto expr : part) {
                prop = new BinaryExprAST("[]", prop, expr);
            }
            node->prop = prop;
        } else if (node->type == "IDVarpart") {
            IDVarpart prop;
            if (node->son.size() == 0)
                prop = IDVarpart();
            else  // IDVarpart -> [ ExpressionList ]
                prop = cast<IDVarpart>(node->son[1]->prop);
            node->prop = prop;
        } else if (node->type == "ProcedureCall(ARG)") {
            ProcedureCall prop;
            if (node->son.size() == 4) {  // ProcedureCall(ARG) -> ID ( ExpressionList )
                prop = new CallExprAST(node->son[0]->raw, cast<ExpressionList>(node->son[2]->prop));
            } else {  // ProcedureCall(ARG) -> ID ( )
                prop = new CallExprAST(node->son[0]->raw, {});
            }
            node->prop = prop;
        } else if (node->type == "ElsePart") {
            ElsePart prop;
            if (node->son.size() > 0) {  // ElsePart -> else CompoundStatement
                prop = cast<ElsePart>(node->son[1]->prop);
            } else {
                prop = new BlockAST({});
            }
            node->prop = prop;
        } else if (node->type == "ExpressionList") {
            ExpressionList prop;
            if (node->son.size() == 1) {  // ExpressionList -> Expression
                prop = ExpressionList{cast<Expression>(node->son[0]->prop)};
            } else {  // ExpressionList -> ExpressionList , Expression
                prop = cast<ExpressionList>(node->son[0]->prop);
                prop.push_back(cast<Expression>(node->son[2]->prop));
            }
            node->prop = prop;
        } else if (node->type == "Expression") {
            /*
                Expression -> SimpleExpression relOP SimpleExpression
                Expression -> SimpleExpression = SimpleExpression
                Expression -> stringVal
                Expression -> SimpleExpression
                Expression -> Expression ^
            */
            Expression prop = nullptr;
            if (node->son.size() == 3) {
                prop = new BinaryExprAST(node->son[1]->raw, cast<ExprAST*>(node->son[0]->prop), cast<ExprAST*>(node->son[2]->prop));
            } else if (node->son.size() == 2) {
                prop = new UnaryExprAST("^", cast<ExprAST*>(node->son[0]->prop));
            } else {
                if (node->son[0]->parserSymbol == "stringVal") {
                    prop = new StringExprAST(node->son[0]->raw);
                } else {  // Expression -> SimpleExpression
                    prop = cast<Expression>(node->son[0]->prop);
                }
            }
            node->prop = prop;
        } else if (node->type == "SimpleExpression") {
            SimpleExpression prop;
            if (node->son.size() == 1)  // SimpleExpression -> Term
                prop = cast<SimpleExpression>(node->son[0]->prop);
            else {  // SimpleExpression -> SimpleExpression addOP Term
                prop = new BinaryExprAST(cast<std::string>(node->son[1]->prop), cast<ExprAST*>(node->son[0]->prop), cast<ExprAST*>(node->son[2]->prop));
            }
            node->prop = prop;
        } else if (node->type == "Term") {
            Term prop = nullptr;
            if (node->son.size() == 1)  // Term -> Factor
                prop = cast<Term>(node->son[0]->prop);
            else {  // Term -> Term mulOP Factor
                prop = new BinaryExprAST(node->son[1]->raw, cast<ExprAST*>(node->son[0]->prop), cast<ExprAST*>(node->son[2]->prop));
            }
            node->prop = prop;
        } else if (node->type == "Factor") {
            Factor prop = nullptr;
            if (node->son[0]->type == "Num") {  // Factor -> Num
                prop = cast<Factor>(node->son[0]->prop);
            } else if (node->son[0]->type == "ComposedVariable") {  // Factor -> ComposedVariable
                prop = cast<Factor>(node->son[0]->prop);
            } else if (node->son[1]->type == "Num") {
                // Factor -> + Num
                // Factor -> - Num
                prop = new UnaryExprAST(node->son[0]->raw, cast<ExprAST*>(node->son[1]->prop));
            } else if (node->son[1]->type == "Factor") {
                // Factor -> not Factor
                // Factor -> uminus Factor
                prop = new UnaryExprAST(node->son[0]->raw, cast<ExprAST*>(node->son[1]->prop));
            } else
                abort();
            node->prop = prop;
        } else if (node->type == "ComposedVariable") {
            ComposedVariable prop = nullptr;
            if (node->son[0]->type == "Variable") {
                prop = cast<ComposedVariable>(node->son[0]->prop);
            } else if (node->son[0]->type == "ProcedureCall(ARG)") {
                prop = cast<ComposedVariable>(node->son[0]->prop);
            } else if (node->son[1]->type == "Expression") {  // ( Expression )
                prop = cast<ComposedVariable>(node->son[1]->prop);
            } else {
                prop = new BinaryExprAST(".", cast<ExprAST*>(node->son[0]->prop), new StringExprAST(node->son[2]->raw));
            }
            node->prop = prop;
        } else if (node->type == "Num") {
            Num prop;
            if (node->son[0]->parserSymbol == "intVal") {  // Num -> intVal
                prop = new NumberExprAST(std::stoi(node->son[0]->raw));
            } else if (node->son[0]->parserSymbol == "realVal") {
                prop = new NumberExprAST(std::stod(node->son[0]->raw));
            } else
                abort();
            node->prop = prop;
        } else if (node->type == "Digits") {
            node->prop = new NumberExprAST(std::stoi(node->son[0]->raw));
            assert(node->prop.type() == typeid(Digits));
        } else if (node->type == "addOP") {
            node->prop = node->son[0]->raw;
            assert(node->son[0]->raw == "+" || node->son[0]->raw == "-" || node->son[0]->raw == "or");
            assert(node->prop.type() == typeid(addOP));
        }
        j.push_back(nj);
    }
};

extern ParserOutputer parserOutputer;

#endif