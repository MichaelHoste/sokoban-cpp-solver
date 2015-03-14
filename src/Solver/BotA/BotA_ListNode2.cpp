/*----------------------------------------------*/
/*================== ISokoban ==================*/
/*----------------------------------------------*/
/* Version : 1.0								*/
/* Author : Michaël Hoste						*/
/* Copyright : See LICENSE file for details   	*/
/*----------------------------------------------*/

#include "../../../include/Solver/BotA/BotA_ListNode2.h"

/* ------------*/
/* Constructor */
/* ------------*/
BotA_ListNode2::BotA_ListNode2(TreeNode* treeNode):
	ListNode(treeNode),
	heapStackCell(-1)
{

}

BotA_ListNode2::BotA_ListNode2(TreeNode* treeNode, int heapStackCell):
	ListNode(treeNode),
	heapStackCell(heapStackCell)
{

}

/* -----------*/
/* Destructor */
/* -----------*/

/* -------*/
/* Others */
/* -------*/

