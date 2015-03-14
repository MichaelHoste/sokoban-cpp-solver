/*----------------------------------------------*/
/*================== ISokoban ==================*/
/*----------------------------------------------*/
/* Version : 1.0								*/
/* Author : Michaël Hoste						*/
/* Copyright : See LICENSE file for details   	*/
/*----------------------------------------------*/
/**
 * @brief A version of HashTable specific to closeNodeList in heap stack
 * With it, we can get or change heapStackCell value of a specific node in the
 * hashtable
 */
/*----------------------------------------------*/

#ifndef BOTA_HASHTABLE_H_
#define BOTA_HASHTABLE_H_

#include <stdlib.h>
#include <string.h>
#include "../Node.h"
#include "../HashTable.h"
#include "BotA_ListNode2.h"
#include "BotA_ChainedList.h"
#include "BotA_TreeNode.h"
#include "BotA_HeapStack.h"

class BotA_HashTable : public HashTable
{
protected:

public:
	/**
	 * Constructor for new node
	 * @param treeNode treeNode to be stocked on this item
	 */
	BotA_HashTable(int length);

	/* --------*/
	/* Getters */
	/* --------*/
	/**
	 * Test if an Node is already present into the hashTable
	 * @param node Node we want to test
	 * @return value of treenode (f(x)) is present, -1 if not
	 */
	virtual bool isPresent(const Node* node) const;

	/**
	 * Get the heapStackCell value (cell number of node in the heapStack)
	 * from a node in the hashTable
	 * @param node node we want to get value
	 * @return heapStackCell value of the node or -1 if value is not in the
	 * hashTable.
	 */
	int getHeapStackCell(const Node* node);

	/**
	 * change the heapStackCell value (cell number of node in the heapStack)
	 * from a Node in the hashTable
	 * @param node node we want to change value
	 * @param newHeapStackCell new heap stack cell to assign to this node
	 * @return true if value is changed, false if no node like this in the
	 * hashtable
	 */
	bool changeHeapStackCell(const Node* node, const int newHeapStackCell);

protected:

};

#endif /*BOTA_HASHTABLE_H_*/
