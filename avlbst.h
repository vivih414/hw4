#ifndef AVLBST_H
#define AVLBST_H

#include <iostream>
#include <exception>
#include <cstdlib>
#include <cstdint>
#include <algorithm>
#include "bst.h"

struct KeyError { };

/**
* A special kind of node for an AVL tree, which adds the balance as a data member, plus
* other additional helper functions. You do NOT need to implement any functionality or
* add additional data members or helper functions.
*/
template <typename Key, typename Value>
class AVLNode : public Node<Key, Value>
{
public:
    // Constructor/destructor.
    AVLNode(const Key& key, const Value& value, AVLNode<Key, Value>* parent);
    virtual ~AVLNode();

    // Getter/setter for the node's height.
    int8_t getBalance () const;
    void setBalance (int8_t balance);
    void updateBalance(int8_t diff);

    // Getters for parent, left, and right. These need to be redefined since they
    // return pointers to AVLNodes - not plain Nodes. See the Node class in bst.h
    // for more information.
    virtual AVLNode<Key, Value>* getParent() const override;
    virtual AVLNode<Key, Value>* getLeft() const override;
    virtual AVLNode<Key, Value>* getRight() const override;

protected:
    int8_t balance_;    // effectively a signed char
};

/*
  -------------------------------------------------
  Begin implementations for the AVLNode class.
  -------------------------------------------------
*/

/**
* An explicit constructor to initialize the elements by calling the base class constructor
*/
template<class Key, class Value>
AVLNode<Key, Value>::AVLNode(const Key& key, const Value& value, AVLNode<Key, Value> *parent) :
    Node<Key, Value>(key, value, parent), balance_(0)
{

}

/**
* A destructor which does nothing.
*/
template<class Key, class Value>
AVLNode<Key, Value>::~AVLNode()
{

}

/**
* A getter for the balance of a AVLNode.
*/
template<class Key, class Value>
int8_t AVLNode<Key, Value>::getBalance() const
{
    return balance_;
}

/**
* A setter for the balance of a AVLNode.
*/
template<class Key, class Value>
void AVLNode<Key, Value>::setBalance(int8_t balance)
{
    balance_ = balance;
}

/**
* Adds diff to the balance of a AVLNode.
*/
template<class Key, class Value>
void AVLNode<Key, Value>::updateBalance(int8_t diff)
{
    balance_ += diff;
}

/**
* An overridden function for getting the parent since a static_cast is necessary to make sure
* that our node is a AVLNode.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getParent() const
{
    return static_cast<AVLNode<Key, Value>*>(this->parent_);
}

/**
* Overridden for the same reasons as above.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getLeft() const
{
    return static_cast<AVLNode<Key, Value>*>(this->left_);
}

/**
* Overridden for the same reasons as above.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getRight() const
{
    return static_cast<AVLNode<Key, Value>*>(this->right_);
}


/*
  -----------------------------------------------
  End implementations for the AVLNode class.
  -----------------------------------------------
*/


template <class Key, class Value>
class AVLTree : public BinarySearchTree<Key, Value>
{
public:
    virtual void insert (const std::pair<const Key, Value> &new_item); // TODO
    virtual void remove(const Key& key);  // TODO
protected:
    virtual void nodeSwap( AVLNode<Key,Value>* n1, AVLNode<Key,Value>* n2);

    // Add helper functions here
		void rebalanced(AVLNode<Key, Value>* node);
		void leftR(AVLNode<Key, Value>* node);
		void rightR(AVLNode<Key, Value>* node);
};

/*
 * Recall: If key is already in the tree, you should 
 * overwrite the current value with the updated value.
 */
template<class Key, class Value>
void AVLTree<Key, Value>::insert (const std::pair<const Key, Value> &new_item)
{
    // TODO
		AVLNode<Key, Value>* root = static_cast<AVLNode<Key, Value>*>(this->root_);
		AVLNode<Key, Value>* parent = nullptr;
		AVLNode<Key, Value>* current = root;
		
		while(current != nullptr)
		{
			parent = current;
			if(new_item.first < current->getKey())
			{
				current = current->getLeft();
			}
			else if(new_item.first > current->getKey())
			{
				current = current->getRight();
			}
			else
			{
				current->setValue(new_item.second);
				return;
			}
		}


		AVLNode<Key, Value>* newNode = new AVLNode<Key, Value>(new_item.first, new_item.second, parent);
	
		if(parent == nullptr)
		{
			this->root_ = newNode;
		}
		else if(new_item.first < parent->getKey())
		{
			parent->left_ = newNode;
		}
		else
		{
			parent->right_ = newNode;
		}

		AVLNode<Key, Value>* z = newNode;
    AVLNode<Key, Value>* p = parent;

    while (p != nullptr)
    {
        if (z == p->getLeft()) p->updateBalance(-1);
        else p->updateBalance(1);

        if (p->getBalance() == 0) break;
        if (std::abs(p->getBalance()) == 2)
        {
            rebalanced(p);
            break;
        }

        z = p;
        p = p->getParent();
    }
}

/*
 * Recall: The writeup specifies that if a node has 2 children you
 * should swap with the predecessor and then remove.
 */
template<class Key, class Value>
void AVLTree<Key, Value>:: remove(const Key& key)
{
    // TODO
		AVLNode<Key, Value>* node = static_cast<AVLNode<Key, Value>*>(this->internalFind(key));

		if(node == nullptr)
		{
			return;
		}
		if(node->getLeft() && node->getRight())
		{
			AVLNode<Key, Value>* pre = static_cast<AVLNode<Key, Value>*>(this->predecessor(node));
			nodeSwap(node, pre);
		}

		AVLNode<Key, Value>* child;
		if(node->getLeft())
		{
			child = node->getLeft();
		}
		else
		{
			child = node->getRight();
		}

		AVLNode<Key, Value>* parent = node->getParent();

		if(child != nullptr)
		{
			child->parent_ = parent;
		}
		if(parent == nullptr)
		{
			this->root_ = child;
		}
		else if(node == parent->getLeft())
		{
			parent->left_ = child;
		}
		else
		{
			parent->right_ = child;
		}

		delete node;
		rebalanced(parent);
}

template<class Key, class Value>
void AVLTree<Key, Value>::rebalanced(AVLNode<Key, Value>* node)
{
	 if (!node) return;

    int balance = node->getBalance();

    if (balance == -2)
    {
        AVLNode<Key, Value>* left = node->getLeft();
        if (left && left->getBalance() <= 0)
        {
            rightR(node);
        }
        else if (left)
        {
            leftR(left);
            rightR(node);
        }
    }
    else if (balance == 2)
    {
        AVLNode<Key, Value>* right = node->getRight();
        if (right && right->getBalance() >= 0)
        {
          leftR(node);
        }
        else if (right)
        {
          rightR(right);
          leftR(node);
        }
    }
}

template<class Key, class Value>
void AVLTree<Key, Value>::leftR(AVLNode<Key, Value>* node)
{
	AVLNode<Key, Value>* rightChild = node->getRight();
  AVLNode<Key, Value>* rightLeft = rightChild->getLeft();

  node->right_ = rightLeft;
  if (rightLeft)
	{
		rightLeft->parent_ = node;
	}

  rightChild->left_ = node;
	rightChild->parent_ = node->getParent();

  if(!node->getParent())
	{ 
		this->root_ = rightChild;
	}
  else if(node == node->getParent()->getLeft())
	{
	 node->getParent()->left_ = rightChild;
	}
  else
	{
		node->getParent()->right_ = rightChild;
	}

  node->parent_ = rightChild;

  int oldBalance = rightChild->getBalance();
  if (oldBalance == 0) {
        node->setBalance(1);
        rightChild->setBalance(-1);
  } 
	else 
	{
		node->setBalance(0);
    rightChild->setBalance(0);
  }
}

template<class Key, class Value>
void AVLTree<Key, Value>::rightR(AVLNode<Key, Value>* node)
{
	AVLNode<Key, Value>* leftChild = node->getLeft();
  AVLNode<Key, Value>* leftRight = leftChild->getRight();

  node->left_ = leftRight;
  if(leftRight) 
	{
		leftRight->parent_ = node;
	}

  leftChild->right_ = node;
  leftChild->parent_ = node->getParent();

  if(!node->getParent()) 
	{
		this->root_ = leftChild;
	}
  else if(node == node->getParent()->getLeft())
	{
		node->getParent()->left_ = leftChild;
	}
  else
	{
		node->getParent()->right_ = leftChild;
	}

  node->parent_ = leftChild;

  int oldBalance = leftChild->getBalance();
  if (oldBalance == 0) 
	{
    node->setBalance(-1);
    leftChild->setBalance(1);
  } 
	else 
	{
    node->setBalance(0);
    leftChild->setBalance(0);
  }
}

template<class Key, class Value>
void AVLTree<Key, Value>::nodeSwap( AVLNode<Key,Value>* n1, AVLNode<Key,Value>* n2)
{
    BinarySearchTree<Key, Value>::nodeSwap(n1, n2);
    int8_t tempB = n1->getBalance();
    n1->setBalance(n2->getBalance());
    n2->setBalance(tempB);
}


#endif