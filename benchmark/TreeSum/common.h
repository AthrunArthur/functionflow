#include <cmath>
typedef float Value;

struct TreeNode {
    //! Pointer to left subtree
    TreeNode* left; 
    //! Pointer to right subtree
    TreeNode* right;
    //! Number of nodes in this subtree, including this node.
    long node_count;
    //! Value associated with the node.
    Value value;
};

Value SerialSumTree( TreeNode* root );
Value ParaSumTree(TreeNode * root);

extern int threshold;

