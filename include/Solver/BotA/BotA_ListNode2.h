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
 * WARNING : this second version of ListNode is dedicated to close hashtable
 * and use one more variable : "heapStackCell".
 * heapStackCell is the number of cell treeNode of this Listnode is currently in
 */
/*----------------------------------------------*/

#ifndef BOTA_LISTNODE2_H_
#define BOTA_LISTNODE2_H_

#include <stdlib.h>
#include <string.h>
#include "../Node.h"
#include "../ListNode.h"
#include "BotA_TreeNode.h"

class BotA_ListNode2 : public ListNode
{
protected:
	int heapStackCell; /**< Number of cell treeNode is currently in */

public:
	/**
	 * Constructor for new node
	 * @param treeNode treeNode to be stocked on this item
	 */
	BotA_ListNode2(TreeNode* treeNode);

	/**
	 * Constructor for new node
	 * @param treeNode treeNode to be stocked on this item
	 * @param heapStackCell Number of cell this treeNode is on heap stack
	 */
	BotA_ListNode2(TreeNode* treeNode, int heapStackCell);

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
	/** @Return heapStackCell value */
	inline int getHeapStackCell(void) const
	{ return heapStackCell; }

	/* --------*/
	/* Setters */
	/* --------*/
	/** Set heapStackCell value */
	inline void setHeapStackCell(int heapStackCell)
	{ this->heapStackCell = heapStackCell; }

protected:

};

#endif /*BOTA_LISTNODE2_H_*/
