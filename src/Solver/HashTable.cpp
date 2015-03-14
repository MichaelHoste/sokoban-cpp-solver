/*----------------------------------------------*/
/*================== ISokoban ==================*/
/*----------------------------------------------*/
/* Version : 1.0								*/
/* Author : Michaël Hoste						*/
/* Copyright : See LICENSE file for details   	*/
/*----------------------------------------------*/

#include "../../include/Solver/HashTable.h"

/* ------------*/
/* Constructor */
/* ------------*/
HashTable::HashTable(int length):
	table(NULL),
	length(length),
	itemNumber(0)
{
	table = (ChainedList**)malloc(length*sizeof(ChainedList*));
	for(int i=0;i<length;i++)
		table[i] = new ChainedList();
}

/* -----------*/
/* Destructor */
/* -----------*/
HashTable::~HashTable()
{
	for(int i=0;i<length;i++)
		delete table[i];

	free(table);
}

/* -------*/
/* Others */
/* -------*/
void HashTable::addItem(TreeNode* treeNode)
{
	int pos = h(treeNode->getNode());
	table[pos]->addLastItem(treeNode);
	itemNumber++;
}

bool HashTable::removeItem(const Node* node)
{
	int pos = h(node);
	ListNode * cur = table[pos]->getFirstItem();
	while(cur != NULL)
	{
		if(node->isEgal(cur->getNode()))
		{
			if(cur == table[pos]->getFirstItem())
				table[pos]->setFirstItem(cur->getNext());
			if(cur == table[pos]->getLastItem())
				table[pos]->setLastItem(cur->getPrev());

			delete cur;
			itemNumber--;
			table[pos]->setLength(table[pos]->getLength()-1);

			return true;
		}
		cur = cur->getNext();
	}
	return false;
}

bool HashTable::isPresent(const Node* node) const
{
	int pos = h(node);
	ListNode * cur = table[pos]->getFirstItem();
	while(cur != NULL)
	{
		if(node->isEgal(cur->getNode()))
			return true;
		cur = cur->getNext();
	}

	return false;
}

TreeNode* HashTable::getTreeNodeFromNode(const Node* node)
{
	int pos = h(node);
	ListNode * cur = table[pos]->getFirstItem();
	while(cur != NULL)
	{
		if(node->isEgal(cur->getNode()))
			return cur->getTreeNode();
		cur = cur->getNext();
	}

	return NULL;
}

int HashTable::h(const Node* node) const
{
	int sum = 0;
	Zone* boxes = node->getBoxesZone();
	Zone* pusher = node->getPusherZone();

	// We suppose both zones have the same size
	int cellNumber = boxes->getNumberCell();

	for(int i=0;i<cellNumber;i++)
	{
		sum += boxes->getZoneTab()[i]%length;
		sum += pusher->getZoneTab()[i]%length;
		sum = sum%length;
	}

	return sum;
}
