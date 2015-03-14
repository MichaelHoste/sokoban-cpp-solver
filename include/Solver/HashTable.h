/*----------------------------------------------*/
/*================== ISokoban ==================*/
/*----------------------------------------------*/
/* Version : 1.0								*/
/* Author : Michaël Hoste						*/
/* Copyright : See LICENSE file for details   	*/
/*----------------------------------------------*/
/**
 * @brief Hash Table Class to store all level states
 *
 * This hash table is a tab of N ChainedList of TreeNode (reuse of the
 * processing tree pointer).
 * when adding a TreeNode, the "H" function is used to make a position in the
 * tab from the node.
 */
/*----------------------------------------------*/

#ifndef HASHTABLE_H_
#define HASHTABLE_H_

#include <stdio.h>
#include <stdlib.h>
#include "../Base.h"
#include "ChainedList.h"
#include "Zone.h"

class HashTable
{
protected:
	ChainedList** table; /**< Table of hashing */
	int length; /**< Length of hashtable : number of cells */
	int itemNumber; /**< Number of items in the table */

public:
	/**
	 * Constructor
	 */
	HashTable(int length);

	/**
	 * Destructor
	 */
	virtual ~HashTable();

	/* --------*/
	/* Getters */
	/* --------*/
	/** @Return table of "length" chainedlist */
	inline ChainedList * const * const getTable(void) const { return table; }
	/** @Return length of table */
	inline int getLength(void) const { return length; }
	/** @Return number of items stocked in the hashtable */
	inline int getItemNumber(void) const { return itemNumber; }

	/* --------*/
	/* Setters */
	/* --------*/

	/* -------*/
	/* Others */
	/* -------*/
	/**
	 * Add an item into the table but don't test if it is already present
	 * @param treeNode treeNode pointer we want to add to the hashTable
	 */
	void addItem(TreeNode* treeNode);

	/**
	 * Delete an item from the table (if this item if several times in the table,
	 * we delete it only one time). It delete only listNode but pointer to
	 * treeNode stay in the tree
	 * @param node Node we want to delete from the table
	 * @return true if something was deleted, false if not
	 */
	bool removeItem(const Node* node);

	/**
	 * Test if an Node is already present into the hashTable
	 * @param node Node we want to test
	 * @return true if the Node is present, false if not
	 */
	virtual bool isPresent(const Node* node) const;

	/**
	 * Get a treenode pointer in hashtable from a node
	 * @param node Node we want use to find treenode
	 * @return treenode pointer to found node or NULL if node doesn't exist in
	 * hashtable
	 */
	TreeNode* getTreeNodeFromNode(const Node* node);

protected:
	/**
	 * Hashing function that use a node to compute an index number for the tab
	 * @param node Node we want to compute
	 * @return index number for the tab
	 */
	int h(const Node* node) const;
};

#endif /*HASHTABLE_H_*/
