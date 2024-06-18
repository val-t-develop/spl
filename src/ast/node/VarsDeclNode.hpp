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
 *  Node of variables declaration.
 */

#pragma once
#include <ast/node/VarDeclNode.hpp>

class VarsDeclNode : public StatementNode {
  public:
    vector<shared_ptr<VarDeclNode>> decls = vector<shared_ptr<VarDeclNode>>();

    VarsDeclNode(vector<shared_ptr<VarDeclNode>> _decls,
                 shared_ptr<Node> _parent);
};
