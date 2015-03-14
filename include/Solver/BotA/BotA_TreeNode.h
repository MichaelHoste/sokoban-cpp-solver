/*----------------------------------------------*/
/*================== ISokoban ==================*/
/*----------------------------------------------*/
/* Version : 1.0								*/
/* Author : Michaël Hoste						*/
/* Copyright : See LICENSE file for details   	*/
/*----------------------------------------------*/
/**
 * @brief A version of TreeNode with "f" and "g" getters where f(x)=g(x)+h(x).
 * It's used to make the A* solver
 */
/*----------------------------------------------*/

#ifndef BOTA_TREENODE_H_
#define BOTA_TREENODE_H_

#include <stdlib.h>
#include <string.h>
#include "../Node.h"
#include "../TreeNode.h"
#include "../HashTable.h"

class BotA_HashTable;
class BotA_HeapStack;

class BotA_TreeNode : public TreeNode
{
protected:
	int f; /**< f(x) */
	int g; /**< g(x) */

public:
	/**
	 * Constructor for new node with cost
	 * @param node Node to be stocked on this item
	 * @param f f(x)
	 * @param g g(x)
	 */
	BotA_TreeNode(Node* node, const int f, const int g);

	/**
	 * Constructor for new node with f and g initialized to -1
	 * @param node Node to be stocked on this item
	 */
	BotA_TreeNode(Node* node);

	/* --------*/
	/* Getters */
	/* --------*/
	/** @Return f(x) value */
	inline int getF(void) const { return f; }
	/** @Return g(x) value */
	inline int getG(void) const { return g; }
	/** @Return h(x) value */
	inline int getH(void) const { return f-g; }

	/* --------*/
	/* Setters */
	/* --------*/
	/** @Return Assign f(x) value */
	inline void setF(int f) { this->f=f; }
	/** @Return Assign g(x) value */
	inline void setG(int g) { this->g=g; }

	/**
	 * Decrease a value from G variable (and f variable because g is in f) of this
	 * node and every descendant he got
	 * @param value Value we want to decrease G variable of this node and all its
	 * children
	 */
	void decreaseValue(int value);

protected:

};

#endif /*BOTA_TREENODE_H_*/
