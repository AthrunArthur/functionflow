#include <ff.h>
#include "../common.h"

Value ParaSumTree(TreeNode * root){
  if(root == NULL)
	return 0;
  if(root->node_count < threshold){
    return SerialSumTree(root);
  }
  else
  {
    Value result = root->value;
    ff::para<> l, r;
    Value x = 0, y = 0;
    //if(root->left)
      l([&x, root](){ x = ParaSumTree(root->left);});
    //if(root->right)
      r([&y, root](){ y = ParaSumTree(root->right);});

    return result + (l&&r).then([&x, &y]()->Value{return x+y;});
  }
}
