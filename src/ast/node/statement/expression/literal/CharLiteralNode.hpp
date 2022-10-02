#pragma once
#include <Defs.hpp>
#include <ast/node/statement/expression/ExpressionNode.hpp>

class CharLiteralNode : public ExpressionNode {
    string str;

    CharLiteralNode(string _str, shared_ptr<Node> _parent);

    shared_ptr<ClassRecord> getReturnType() ;
};
