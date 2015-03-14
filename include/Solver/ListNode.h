/*----------------------------------------------*/
/*================== ISokoban ==================*/
/*----------------------------------------------*/
/* Version : 1.0								*/
/* Author : Michaël Hoste						*/
/* Copyright : See LICENSE file for details   	*/
/*----------------------------------------------*/
/**
 * @brief Node representation for chained lists.
 *
 * This ListNode contain a TreeNode with all information about its parent
 * TreeNode in the processing tree.
 */
/*----------------------------------------------*/

#ifndef LISTNODE_H_
#define LISTNODE_H_

#include <stdlib.h>
#include <string.h>
#include "../Base.h"
#include "Node.h"
#include "TreeNode.h"

class ListNode
{
protected:
	TreeNode* treeNode; /**< TreeNode stocked on this item of the list */
	ListNode* next; /**< Next ListNode of the list */
	ListNode* prev;/**< Previous ListNode of the list */

public:
	/**
	 * Constructor for empty node
	 * @param treeNode treeNode to be stocked on this item
	 */
	ListNode(TreeNode* treeNode);

	/**
	 * Destructor
	 * No destruction of TreeNode because it will be deleted in it's tree.
	 * If treeNode don't belong to a tree anymore when deleting this listNode,
	 * then delete it by hand.
	 */
	virtual ~ListNode();

	/* --------*/
	/* Getters */
	/* --------*/
	/** @Return Content TreeNode with informations about position in the
	 *  processing tree */
	inline TreeNode* getTreeNode(void) const { return treeNode; }
	/** @Return next ListNode */
	inline ListNode* getNext(void) const { return next; }
	/** @Return previous ListNode */
	inline ListNode* getPrev(void) const { return prev; }
	/** @Return Content Node (only light representation of level) */
	inline Node* getNode(void) const { return treeNode->getNode(); }

	/* --------*/
	/* Setters */
	/* --------*/
	/** Assign next ListNode */
	inline void setNext(ListNode* next) { this->next=next; }
	/** Assign previous ListNode */
	inline void setPrev(ListNode* prev) { this->prev=prev; }

	/* -------*/
	/* Others */
	/* -------*/
	/**
	 * Add a listNode just before this node
	 * @param node node to add before this node
	 */
	void addBefore(ListNode* listNode);

	/**
	 * Add a listNode just before this node
	 * @param node node to add after this node
	 */
	void addAfter(ListNode* listNode);
};

#endif /*LISTNODE_H_*/
