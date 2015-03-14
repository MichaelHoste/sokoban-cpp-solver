/*----------------------------------------------*/
/*================== ISokoban ==================*/
/*----------------------------------------------*/
/* Version : 1.0								*/
/* Author : Michaël Hoste						*/
/* Copyright : See LICENSE file for details   	*/
/*----------------------------------------------*/

#include "../../../include/Solver/BotA/BotA_ChainedList.h"

/* ------------*/
/* Constructor */
/* ------------*/
BotA_ChainedList::BotA_ChainedList():
	ChainedList()
{

}

/* -----------*/
/* Destructor */
/* -----------*/

/* -------*/
/* Others */
/* -------*/
ListNode* BotA_ChainedList::createListNode(TreeNode* treeNode)
{
	return new BotA_ListNode2(treeNode);
}
