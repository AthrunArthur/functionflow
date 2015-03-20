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
    int c = 1;
    if(root->left){
      l([&x, root](){ x = ParaSumTree(root->left);});
      c++;
    }
    if(root->right){
      r([&y, root](){ y = ParaSumTree(root->right);});
      c++;
    }

    result = result + (l&&r).then([&x, &y]()->Value{return x+y;});
    return sqrt(sqrt(result/c)*10)*10;
  }
}
