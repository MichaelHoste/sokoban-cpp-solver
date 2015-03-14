/*----------------------------------------------*/
/*================== ISokoban ==================*/
/*----------------------------------------------*/
/* Version : 1.0								*/
/* Author : Michaël Hoste						*/
/* Copyright : See LICENSE file for details   	*/
/*----------------------------------------------*/
/**
 * @brief A Heap Stack of TreeNodes. It can work like a chainedList to sort
 * TreeNodes in order of cost. Smallest is always root treeNode of stack
 */
/*----------------------------------------------*/

#ifndef BOTA_HEAPSTACK_H_
#define BOTA_HEAPSTACK_H_

#include <stdio.h>
#include <stdlib.h>
#include "../ChainedList.h"
#include "BotA.h"
#include "BotA_ListNode.h"
#include "BotA_TreeNode.h"
#include "BotA_HashTable.h"

class BotA_HeapStack : public ChainedList
{
protected:
	/**
	 * Tab of (closed) treenodes sorted by heap stack (see implementation of
	 * heap stack to know why we use a tab and not a tree).
	 */
	BotA_TreeNode** treeNodeTab;
	int tabLength; /**< Length of the treeNode tab */

public:
	static const int TAB_MEMORY_STEP = 1000; /**< alloc and dealloc step of tab */

	/**
	 * Constructor of an empty chained list
	 */
	BotA_HeapStack();

	/**
	 * Destructor
	 */
	virtual ~BotA_HeapStack();

	/* --------*/
	/* Getters */
	/* --------*/
	/** @Return table of treenodes */
	inline BotA_TreeNode** getTreeNodeTab(void) const { return treeNodeTab; }
	/** @Return length of treenode table (not number of items but number of cells) */
	inline int getTabLength(void) const { return tabLength; }
	/** @Return number of items in table */
	inline int getLength(void) const { return length; }

	/* -------*/
	/* Others */
	/* -------*/
	/**
	 * Add an item into the stack depending of the value of its cost.
	 * @param item TreeNode to add to the stack
	 */
	void addItem(TreeNode* item);

	/**
	 * Delete first item of the stack and
	 */
	virtual void deleteFirstItem(void);

	/**
	 *  @Return root item of the stack in a ListNode for compatibilty with solver
	 */
	virtual ListNode* getFirstItem(void) const;

	/**
	 * Test cost of 2 treenodes position in treeNodeTab
	 * @param cell1 first cell position to test
	 * @param cell2 second cell position to test
	 * @return true if cost of treeNodeTab[cell1] is smaller than cost of
	 * treeNodeTab[cell2]
	 */
	virtual bool isSmaller(int cell1, int cell2) const;

	/**
	 * Re-position a cell of the tab (node in the heap stack) at this right
	 * position depending of its new lesser value than before
	 * @param cell cell we want to re-position depending of its new lesser value
	 */
	void repositionCell(int cell);

	/**
	 * Unused functions with Heap Stack search
	 */
	virtual void addFirstItem(TreeNode* item);
	virtual void addLastItem(TreeNode* item);
	virtual void deleteLastItem(void);
	virtual TreeNode* getLastItem(void);

protected:
	/**
	 * Put a cell of the tab (node in the heap Stack) at its right position
	 * depending of its value (move down the tree if necessary but never up)
	 * @param cell cell we want to move if necessary
	 */
	void minHeapify(int cell);

	/**
	 * Get parent node of a cell of the tab
	 * @param cell cell we want to find parent
	 * @return num of parent cell in tab
	 */
	inline int parent(int cell) { return cell/2; }

	/**
	 * Get left child node of a cell of the tab
	 * @param cell cell we want to find left child node
	 * @return num of left child cell in tab
	 */
	inline int leftChild(int cell) { return 2*cell; }

	/**
	 * Get right child node of a cell of the tab
	 * @param cell cell we want to find right child node
	 * @return num of right child cell in tab
	 */
	inline int rightChild(int cell) { return 2*cell + 1; }
};

#endif /*BOTA_HEAPSTACK_H_*/
