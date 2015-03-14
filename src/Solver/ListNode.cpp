/*----------------------------------------------*/
/*================== ISokoban ==================*/
/*----------------------------------------------*/
/* Version : 1.0								*/
/* Author : Michaël Hoste						*/
/* Copyright : See LICENSE file for details   	*/
/*----------------------------------------------*/

#include "../../include/Solver/ListNode.h"

/* ------------*/
/* Constructor */
/* ------------*/
ListNode::ListNode(TreeNode* treeNode):
	treeNode(treeNode),
	next(NULL),
	prev(NULL)
{

}

/* -----------*/
/* Destructor */
/* -----------*/
ListNode::~ListNode()
{
	if(prev != NULL)
		prev->setNext(next);

	if(next != NULL)
		next->setPrev(prev);
}

/* -------*/
/* Others */
/* -------*/
void ListNode::addBefore(ListNode* listNode)
{
	if(listNode != NULL)
	{
		// link prev with new node...
		if(this->prev)
			this->prev->setNext(listNode);
		// ...and new node with prev
		listNode->setPrev(this->prev);

		// link this node with new node
		this->prev = listNode;
		// and new node with this node
		listNode->setNext(this);
	}
	else
		printf("ListNode :: listNode to be added is NULL\n");
}

void ListNode::addAfter(ListNode* listNode)
{
	if(listNode != NULL)
	{
		// link next with new node...
		if(this->next)
			this->next->setPrev(listNode);
		// ...and new node with next
		listNode->setNext(this->next);

		// link this node with new node
		this->next = listNode;
		// and new node with this node
		listNode->setPrev(this);
	}
	else
		printf("ListNode :: listNode to be added is NULL\n");
}
