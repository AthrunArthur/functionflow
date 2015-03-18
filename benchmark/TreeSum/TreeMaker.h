/*
    Copyright 2005-2014 Intel Corporation.  All Rights Reserved.

    This file is part of Threading Building Blocks. Threading Building Blocks is free software;
    you can redistribute it and/or modify it under the terms of the GNU General Public License
    version 2  as  published  by  the  Free Software Foundation.  Threading Building Blocks is
    distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the
    implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
    See  the GNU General Public License for more details.   You should have received a copy of
    the  GNU General Public License along with Threading Building Blocks; if not, write to the
    Free Software Foundation, Inc.,  51 Franklin St,  Fifth Floor,  Boston,  MA 02110-1301 USA

    As a special exception,  you may use this file  as part of a free software library without
    restriction.  Specifically,  if other files instantiate templates  or use macros or inline
    functions from this file, or you compile this file and link it with other files to produce
    an executable,  this file does not by itself cause the resulting executable to be covered
    by the GNU General Public License. This exception does not however invalidate any other
    reasons why the executable file might be covered by the GNU General Public License.
*/

#ifndef TREE_MAKER_H_
#define TREE_MAKER_H_


static double Pi = 3.14159265358979;

class TreeMaker {
public:
    static TreeNode* allocate_node() {
        return  new TreeNode;
    }

    static TreeNode* do_in_one_thread( long number_of_nodes ) {
        if( number_of_nodes==0 ) {
            return NULL;
        } else {
            TreeNode* n = allocate_node();
            n->node_count = number_of_nodes;
            n->value = Value(Pi*number_of_nodes);
            --number_of_nodes;
            n->left  = do_in_one_thread( number_of_nodes/2 ); 
            n->right = do_in_one_thread( number_of_nodes - number_of_nodes/2 );
            return n;
        }
    }

    static TreeNode* create( long number_of_nodes) {
        TreeNode* root = allocate_node();
        root->node_count = number_of_nodes;
        root->value = Value(Pi*number_of_nodes);
        --number_of_nodes;

        root->left  = do_in_one_thread( number_of_nodes/2 );

        root->right = do_in_one_thread( number_of_nodes - number_of_nodes/2 );

        return root;
    }
};

#endif // TREE_MAKER_H_
