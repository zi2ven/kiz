/**
 * @file ast.hpp
 * @brief 抽象语法树（AST）核心定义
 *  
 * @author azhz1107cat
 * @date 2025-10-25
 */

#pragma once
#include <memory>
#include <string>
#include <vector>

namespace kiz {

enum class AstType {
    // 表达式类型（对应 Expression 子类）
    NilExpr, BoolExpr,
    StringExpr, NumberExpr, ListExpr, IdentifierExpr,
    BinaryExpr, UnaryExpr,
    CallExpr,
    GetMemberExpr, GetItemExpr,
    FuncDeclExpr, DictDeclExpr,

    // 语句类型（对应 Statement 子类）
    AssignStmt, NonlocalAssignStmt, GlobalAssignStmt,
    SetMemberStmt, SetItemStmt,
    BlockStmt, IfStmt, WhileStmt,
    ReturnStmt, ImportStmt,
    NullStmt, ExprStmt,
    BreakStmt, NextStmt
};

// AST 基类
struct ASTNode {
    util::PositionInfo pos{};
    AstType ast_type = AstType::NullStmt;
    
    virtual ~ASTNode() = default;
};

// 符号类型信息
struct TypeInfo {
    std::string type_name;
    std::vector<std::unique_ptr<TypeInfo>> subs;
    explicit TypeInfo(std::string tn, std::vector<std::unique_ptr<TypeInfo>> s = {})
        : type_name(std::move(tn)), subs(std::move(s)) {}
};


// 表达式基类
struct Expression :  ASTNode {
    std::unique_ptr<TypeInfo> type_info;
};

// 语句基类
struct Statement :  ASTNode {};

// 字符串字面量
struct StringExpr final :  Expression {
    std::string value;
    explicit StringExpr(const util::PositionInfo& pos, std::string v)
        : pos(pos), value(std::move(v)) {
        this->ast_type = AstType::StringExpr;
        this->type_info = std::make_unique<TypeInfo>("string");
    }
};

// 数字字面量
struct NumberExpr final :  Expression {
    std::string value;
    explicit NumberExpr(const util::PositionInfo& pos, std::string v)
        : pos(pos), value(std::move(v)) {
        this->ast_type = AstType::NumberExpr;
        this->type_info = std::make_unique<TypeInfo>("number");
    }
};

// 空值字面量
struct NilExpr final : Expression {
    explicit NilExpr(const util::PositionInfo& pos)
        : pos(pos) {
        this->ast_type = AstType::NilExpr;
    }
};

// 布尔值字面量
struct BoolExpr final : Expression {
    bool val;
    explicit BoolExpr(const util::PositionInfo& pos, bool v) : pos(pos), val(v) {
        this->ast_type = AstType::BoolExpr;
    }
};

// 数组字面量
struct ListExpr final :  Expression {
    std::vector<std::unique_ptr<Expression>> elements;
    explicit ListExpr(const util::PositionInfo& pos, std::vector<std::unique_ptr<Expression>> elems)
        : pos(pos), elements(std::move(elems)) {
        this->ast_type = AstType::ListExpr;
        this->type_info = std::make_unique<TypeInfo>("list");
    }
};

// 标识符
struct IdentifierExpr final :  Expression {
    std::string name;
    explicit IdentifierExpr(const util::PositionInfo& pos, std::string n)
        : pos(pos), name(std::move(n)) {
        this->ast_type = AstType::IdentifierExpr;
        this->type_info = std::make_unique<TypeInfo>("identifier");
    }
};

// 二元运算
struct BinaryExpr final :  Expression {
    std::string op;
    std::unique_ptr<Expression> left, right;
    BinaryExpr(const util::PositionInfo& pos, std::string o, std::unique_ptr<Expression> l, std::unique_ptr<Expression> r)
        : pos(pos), op(std::move(o)), left(std::move(l)), right(std::move(r)) {
        this->ast_type = AstType::BinaryExpr;
        this->type_info = std::make_unique<TypeInfo>("binary_expr");
    }
};

// 一元运算
struct UnaryExpr final :  Expression {
    std::string op;
    std::unique_ptr<Expression> operand;
    UnaryExpr(const util::PositionInfo& pos, std::string o, std::unique_ptr<Expression> e)
        : pos(pos), op(std::move(o)), operand(std::move(e)) {
        this->ast_type = AstType::UnaryExpr;
        this->type_info = std::make_unique<TypeInfo>("unary_expr");
    }
};

// 赋值
struct AssignStmt final :  Statement {
    std::string name;
    std::unique_ptr<Expression> expr;
    AssignStmt(const util::PositionInfo& pos, std::string n, std::unique_ptr<Expression> e)
        : pos(pos), name(std::move(n)), expr(std::move(e)) {
        this->ast_type = AstType::AssignStmt;
    }
};

// nonlocal赋值
struct NonlocalAssignStmt final :  Statement {
    std::string name;
    std::unique_ptr<Expression> expr;
    NonlocalAssignStmt(const util::PositionInfo& pos, std::string n, std::unique_ptr<Expression> e)
        : pos(pos), name(std::move(n)), expr(std::move(e)) {
        this->ast_type = AstType::NonlocalAssignStmt;
    }
};

// global赋值
struct GlobalAssignStmt final :  Statement {
    std::string name;
    std::unique_ptr<Expression> expr;
    GlobalAssignStmt(const util::PositionInfo& pos, std::string n, std::unique_ptr<Expression> e)
        : pos(pos), name(std::move(n)), expr(std::move(e)) {
        this->ast_type = AstType::GlobalAssignStmt;
    }
};

// 复合语句块
struct BlockStmt final :  Statement {
    std::vector<std::unique_ptr<Statement>> statements{};
    explicit BlockStmt(const util::PositionInfo& pos, std::vector<std::unique_ptr<Statement>> s)
        : pos(pos), statements(std::move(s)) {
        this->ast_type = AstType::BlockStmt;
    }
};

// if 语句
struct IfStmt final :  Statement {
    std::unique_ptr<Expression> condition;
    std::unique_ptr<BlockStmt> thenBlock;
    std::unique_ptr<BlockStmt> elseBlock;
    IfStmt(const util::PositionInfo& pos, std::unique_ptr<Expression> cond, std::unique_ptr<BlockStmt> thenB, std::unique_ptr<BlockStmt> elseB)
        : pos(pos), condition(std::move(cond)), thenBlock(std::move(thenB)), elseBlock(std::move(elseB)) {
        this->ast_type = AstType::IfStmt;
    }
};

// while 语句
struct WhileStmt final :  Statement {
    std::unique_ptr<Expression> condition;
    std::unique_ptr<BlockStmt> body;
    WhileStmt(const util::PositionInfo& pos, std::unique_ptr<Expression> cond, std::unique_ptr<BlockStmt> b)
        : pos(pos), condition(std::move(cond)), body(std::move(b)) {
        this->ast_type = AstType::WhileStmt;
    }
};

// 设置成员
struct SetMemberStmt final :  Statement {
    std::unique_ptr<Expression> g_mem;
    std::unique_ptr<Expression> val;
    SetMemberStmt(const util::PositionInfo& pos, std::unique_ptr<Expression> g_mem, std::unique_ptr<Expression> val)
        : pos(pos), g_mem(std::move(g_mem)), val(std::move(val)) {
        this->ast_type = AstType::SetMemberStmt;
    }
};

// 函数调用
struct CallExpr final :  Expression {
    std::unique_ptr<Expression> callee;
    std::vector<std::unique_ptr<Expression>> args;
    CallExpr(const util::PositionInfo& pos, std::unique_ptr<Expression> c, std::vector<std::unique_ptr<Expression>> a)
        : pos(pos), callee(std::move(c)), args(std::move(a)) {
        this->ast_type = AstType::CallExpr;
        this->type_info = std::make_unique<TypeInfo>("call_expr");
    }
};

// 获取成员
struct GetMemberExpr final :  Expression {
    std::unique_ptr<Expression> father;
    std::unique_ptr<IdentifierExpr> child;
    GetMemberExpr(const util::PositionInfo& pos, std::unique_ptr<Expression> f, std::unique_ptr<IdentifierExpr> c)
        : pos(pos), father(std::move(f)), child(std::move(c)) {
        this->ast_type = AstType::GetMemberExpr;
        this->type_info = std::make_unique<TypeInfo>("get_member_expr");
    }
};

// 获取项
struct GetItemExpr final :  Expression {
    std::unique_ptr<Expression> father;
    std::vector<std::unique_ptr<Expression>> params;
    GetItemExpr(const util::PositionInfo& pos, std::unique_ptr<Expression> f, std::vector<std::unique_ptr<Expression>> p)
        : pos(pos), father(std::move(f)), params(std::move(p)) {
        this->ast_type = AstType::GetItemExpr;
        this->type_info = std::make_unique<TypeInfo>("get_item_expr");
    }
};

// 声明匿名函数
struct FnDeclExpr final :  Expression {
    std::string name;
    std::vector<std::string> params;
    std::unique_ptr<BlockStmt> body;
    FnDeclExpr(const util::PositionInfo& pos, std::string n, std::vector<std::string> p, std::unique_ptr<BlockStmt> b)
        : pos(pos), name(std::move(n)), params(std::move(p)), body(std::move(b)) {
        this->ast_type = AstType::FuncDeclExpr;
        this->type_info = std::make_unique<TypeInfo>("function");
    }
};

// 声明字典
struct DictDeclExpr final :  Expression {
    std::string name;
    std::vector<std::pair<std::string, std::unique_ptr<Expression>>> init_list;
    DictDeclExpr(const util::PositionInfo& pos, std::string n, std::vector<std::pair<std::string, std::unique_ptr<Expression>>> i)
        : pos(pos), name(std::move(n)), init_list(std::move(i)) {
        this->ast_type = AstType::DictDeclExpr;
        this->type_info = std::make_unique<TypeInfo>("dict");
    }
};

// return 语句
struct ReturnStmt final :  Statement {
    std::unique_ptr<Expression> expr;
    explicit ReturnStmt(const util::PositionInfo& pos, std::unique_ptr<Expression> e)
        : pos(pos), expr(std::move(e)) {
        this->ast_type = AstType::ReturnStmt;
    }
};

// import 语句
struct ImportStmt final :  Statement {
    std::string path;
    explicit ImportStmt(const util::PositionInfo& pos, std::string p)
        : pos(pos), path(std::move(p)) {
        this->ast_type = AstType::ImportStmt;
    }
};

// 空语句
struct NullStmt final :  Statement {
    explicit NullStmt(const util::PositionInfo& pos)
        : pos(pos) {
        this->ast_type = AstType::NullStmt;
    }
};

// break 语句
struct BreakStmt final :  Statement {
    explicit BreakStmt(const util::PositionInfo& pos)
        : pos(pos) {
        this->ast_type = AstType::BreakStmt;
    }
};

// continue 语句
struct NextStmt final :  Statement {
    explicit NextStmt(const util::PositionInfo& pos)
        : pos(pos) {
        this->ast_type = AstType::NextStmt;
    }
};

// 表达式语句
struct ExprStmt final :  Statement {
    std::unique_ptr<Expression> expr;
    explicit ExprStmt(const util::PositionInfo& pos, std::unique_ptr<Expression> e)
        : pos(pos), expr(std::move(e)) {
        this->ast_type = AstType::ExprStmt;
    }
};

} // namespace kiz