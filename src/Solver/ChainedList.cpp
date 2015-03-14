/*----------------------------------------------*/
/*================== ISokoban ==================*/
/*----------------------------------------------*/
/* Version : 1.0								*/
/* Author : Michaël Hoste						*/
/* Copyright : See LICENSE file for details   	*/
/*----------------------------------------------*/

#include "../../include/Solver/ChainedList.h"

/* ------------*/
/* Constructor */
/* ------------*/
ChainedList::ChainedList():
	firstItem(NULL),
	lastItem(NULL),
	length(0)
{

}

/* -----------*/
/* Destructor */
/* -----------*/
ChainedList::~ChainedList()
{
	ListNode* cur = firstItem;
	while(cur != NULL)
	{
		ListNode* temp = cur->getNext();
		delete cur;
		cur = temp;
	}
}

/* -------*/
/* Others */
/* -------*/
void ChainedList::addFirstItem(TreeNode* item)
{
	if(item)
	{
		// Initialize new item
		ListNode* newItem = createListNode(item);
		newItem->setPrev(NULL);
		newItem->setNext(this->firstItem);

		// Readapt startlist if not NULL
		if(this->firstItem != NULL)
			this->firstItem->setPrev(newItem);

		// Replace startList with new item
		this->firstItem = newItem;

		this->length++;

		// firstItem = lastItem if only 1 item in the list
		if(length == 1)
			this->lastItem = newItem;
	}
	else
		printf("ChainedList :: TreeNode to add is NULL.\n");
}

void ChainedList::addLastItem(TreeNode* item)
{
	if(item)
	{
		// Initialize new item
		ListNode* newItem = createListNode(item);
		newItem->setNext(NULL);
		newItem->setPrev(this->lastItem);

		// Readapt endList
		if(this->lastItem != NULL)
			this->lastItem->setNext(newItem);

		// Replace endList with new item
		this->lastItem = newItem;

		this->length++;

		// firstItem = lastItem if only 1 item in the list
		if(length == 1)
			this->firstItem = newItem;
	}
	else
		printf("ChainedList :: TreeNode to add is NULL.\n");
}

void ChainedList::deleteFirstItem(void)
{
	if(length > 0)
	{
		// Save second pointer
		ListNode* temp = this->firstItem->getNext();

		// Delete first pointer
		delete this->firstItem;

		// Assign second pointer to firstItem pointer
		this->firstItem = temp;

		this->length--;

		// If length == 0, then both pointers must be egals to NULL
		if(length == 0)
			this->lastItem = NULL;
	}
}

void ChainedList::deleteLastItem(void)
{
	if(length > 0)
	{
		// Save pointer before last one
		ListNode* temp = this->lastItem->getPrev();

		// Delete last pointer
		delete this->lastItem;

		// Assign pointer before last to lastItem
		this->lastItem = temp;

		this->length--;

		// If length == 1, then both pointers must be egals to NULL
		if(length == 0)
			this->firstItem = NULL;
	}
}

ListNode* ChainedList::createListNode(TreeNode* treeNode)
{
	return new ListNode(treeNode);
}
