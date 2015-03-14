/*----------------------------------------------*/
/*================== ISokoban ==================*/
/*----------------------------------------------*/
/* Version : 1.0								*/
/* Author : Michaël Hoste						*/
/* Copyright : See LICENSE file for details   	*/
/*----------------------------------------------*/
/**
 * @brief A specific version of ChainedList to use specific listNodes with
 * heapStackCell values (only used with closeNodeList)
 */
/*----------------------------------------------*/

#ifndef BOTA_CHAINEDLIST_H_
#define BOTA_CHAINEDLIST_H_

#include <stdio.h>
#include <stdlib.h>
#include "../ChainedList.h"
#include "BotA_ListNode2.h"
#include "BotA_TreeNode.h"

class BotA_ChainedList : public ChainedList
{
protected:


public:
	/**
	 * Constructor of an empty chained list
	 */
	BotA_ChainedList();

	/**
	 * Destructor
	 */

	/* --------*/
	/* Getters */
	/* --------*/

	/* -------*/
	/* Others */
	/* -------*/
protected:
	/**
	 * Create an ListNode containing a treeNode
	 * @param treeNode TreeNode to store in this ListNode
	 * @return New ListNode filled with the treeNode argument
	 */
	virtual ListNode* createListNode(TreeNode* treeNode);
};

#endif /*BOTA_CHAINEDLIST_H_*/
