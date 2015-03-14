/*----------------------------------------------*/
/*================== ISokoban ==================*/
/*----------------------------------------------*/
/* Version : 1.0								*/
/* Author : Michaël Hoste						*/
/* Copyright : See LICENSE file for details   	*/
/*----------------------------------------------*/
/**
 * @brief Main Double Chained List of ListNodes class.
 *
 * This class contains informations about number of items in the list and
 * first and last ListNodes.
 */
/*----------------------------------------------*/

#ifndef CHAINEDLIST_H_
#define CHAINEDLIST_H_

#include <stdio.h>
#include <stdlib.h>
#include "ListNode.h"
#include "TreeNode.h"

class ChainedList
{
protected:
	ListNode* firstItem; /**< first item pointer of the list */
	ListNode* lastItem; /**< last item pointer of the list */
	int length; /**< length of the list */

public:
	/**
	 * Constructor of an empty chained list
	 */
	ChainedList();

	/**
	 * Destructor
	 */
	virtual ~ChainedList();

	/* --------*/
	/* Getters */
	/* --------*/
	/** @Return start item of the list */
	virtual inline ListNode* getFirstItem(void) const { return firstItem; }
	/** @Return last item of the list */
	inline ListNode* getLastItem(void) const { return lastItem; }
	/** @Return length of the list */
	inline int getLength(void) const { return length; }

	/* --------*/
	/* Setters */
	/* --------*/
	/** @Set start item of the list */
	void setFirstItem(ListNode* item) { firstItem=item; }
	/** @Set last item of the list */
	void setLastItem(ListNode* item) { lastItem=item; }
	/** @Set length of the list */
	void setLength(int length) { this->length=length; }

	/* -------*/
	/* Others */
	/* -------*/
	/**
	 * Add an item to the end of the chainedlist
	 * @param item Item to add
	 */
	virtual void addFirstItem(TreeNode* item);

	/**
	 * Add an item to the end of the chainedlist
	 * @param item Item to add
	 */
	virtual void addLastItem(TreeNode* item);

	/**
	 * Delete first item of the list
	 */
	virtual void deleteFirstItem(void);

	/**
	 * Delete last item of the list
	 */
	virtual void deleteLastItem(void);

protected:
	/**
	 * Create an ListNode containing a treeNode
	 * @param treeNode TreeNode to store in this ListNode
	 * @return New ListNode filled with the treeNode argument
	 */
	virtual ListNode* createListNode(TreeNode* treeNode);
};

#endif /*CHAINEDLIST_H_*/
