/*----------------------------------------------*/
/*================== ISokoban ==================*/
/*----------------------------------------------*/
/* Version : 1.0								*/
/* Author : Michaël Hoste						*/
/* Copyright : See LICENSE file for details   	*/
/*----------------------------------------------*/
/**
 * @brief A version of ListNode with "f" and "g" getters where f(x)=g(x)+h(x).
 * It's used to make the A* solver
 */
/*----------------------------------------------*/

#ifndef BOTA_LISTNODE_H_
#define BOTA_LISTNODE_H_

#include <stdlib.h>
#include <string.h>
#include "../Node.h"
#include "../ListNode.h"
#include "BotA_TreeNode.h"

class BotA_ListNode : public ListNode
{
protected:


public:
	/**
	 * Constructor for empty node
	 * @param treeNode treeNode to be stocked on this item
	 */
	BotA_ListNode(TreeNode* treeNode);

	/* --------*/
	/* Getters */
	/* --------*/
	/** @Return f(x) value */
	inline int getF(void) const { return ((BotA_TreeNode*)treeNode)->getF(); }
	/** @Return g(x) value */
	inline int getG(void) const { return ((BotA_TreeNode*)treeNode)->getG(); }
	/** @Return h(x) value */
	inline int getH(void) const
	{ return ((BotA_TreeNode*)treeNode)->getH(); }

protected:

};

#endif /*BOTA_LISTNODE_H_*/
