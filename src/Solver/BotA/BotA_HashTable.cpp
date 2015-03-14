/*----------------------------------------------*/
/*================== ISokoban ==================*/
/*----------------------------------------------*/
/* Version : 1.0								*/
/* Author : Michaël Hoste						*/
/* Copyright : See LICENSE file for details   	*/
/*----------------------------------------------*/

#include "../../../include/Solver/BotA/BotA_HashTable.h"

/* ------------*/
/* Constructor */
/* ------------*/
BotA_HashTable::BotA_HashTable(int length):
	HashTable(length)
{
	for(int i=0;i<this->length;i++)
	{
		delete table[i];
		table[i] = new BotA_ChainedList();
	}
}

/* -----------*/
/* Destructor */
/* -----------*/

/* -------*/
/* Others */
/* -------*/
bool BotA_HashTable::isPresent(const Node* node) const
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

int BotA_HashTable::getHeapStackCell(const Node* node)
{
	int pos = h(node);
	ListNode * cur = table[pos]->getFirstItem();
	while(cur != NULL)
	{
		if(node->isEgal(cur->getNode()))
			return ((BotA_ListNode2*)cur)->getHeapStackCell();
		cur = cur->getNext();
	}

	return -1;

//	int length = node->getSolver()->getCloseNodeList()->getLength();
//	for(int i=0;i<length;i++)
//	{
//		if(node->isEgal(((BotA_HeapStack*)node->getSolver()->getCloseNodeList())->getTreeNodeTab()[i]->getNode()))
//			return i;
//	}
//	return -1;
}

bool BotA_HashTable::changeHeapStackCell(const Node* node, int newHeapStackCell)
{
	int pos = h(node);
	ListNode * cur = table[pos]->getFirstItem();
	while(cur != NULL)
	{
		if(node->isEgal(cur->getNode()))
		{
			((BotA_ListNode2*)cur)->setHeapStackCell(newHeapStackCell);
			return true;
		}
		cur = cur->getNext();
	}

	return false;
}
