#include "ExpressionNode.hpp"

ExpressionNode::ExpressionNode(shared_ptr<Node> _parent, NodeKind _kind) : StatementNode(_parent, _kind) {}
shared_ptr<ClassRecord> getReturnType() {
    return nullptr;
}
ExpressionNode::~ExpressionNode() {}

