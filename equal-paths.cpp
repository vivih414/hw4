#ifndef RECCHECK
//if you want to add any #includes like <iostream> you must do them here (before the next endif)

#endif

#include "equal-paths.h"
using namespace std;


// You may add any prototypes of helper functions here
bool equalCheck(Node* root, int depth, int& leaf)
{
	if(root == nullptr)
	{
		return true;
	}
	if(root->left == nullptr && root->right == nullptr)
	{
		if(leaf == -1)
		{
			leaf = depth;
			return true;
		}
		return depth == leaf;
	}

	bool leftSide = equalCheck(root->left, depth+1, leaf);
	bool rightSide = equalCheck(root->right, depth+1, leaf);

	return leftSide && rightSide;
}

bool equalPaths(Node * root)
{
    // Add your code below
		int leaf = -1;

		return equalCheck(root, 0, leaf);
}

