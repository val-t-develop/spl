/*  SPL - Simple Programming Language compiler
 *  Copyright (C) 2022  Valentyn Tymchyshyn
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or any
 *  later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

/*
 *  Valentyn Tymchyshyn (val.t.develop) (val.t.develo@gmail.com)
 *
 *  Node of access.
 */

#include "AccessNode.hpp"

AccessNode::AccessNode(shared_ptr<Node> _parent)
    : ExpressionNode(_parent, NodeKind::ACCESS_NODE) {}

shared_ptr<ClassRecord> AccessNode::getReturnType() {
    if (isExpression()) {
        return static_pointer_cast<ExpressionNode>(access[access.size() - 1])
            ->getReturnType();
    } else {
        return nullptr;
    }
}