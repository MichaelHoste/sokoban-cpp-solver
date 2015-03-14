/*----------------------------------------------*/
/*================== ISokoban ==================*/
/*----------------------------------------------*/
/* Version : 1.0								*/
/* Author : Michaël Hoste						*/
/* Copyright : See LICENSE file for details   	*/
/*----------------------------------------------*/

#include "../../../include/Solver/BotDFS/BotDFS.h"

/* ------------*/
/* Constructor */
/* ------------*/
BotDFS::BotDFS(Base* base, Level* level, int maxNodeNumber, int maxRamSize,
		int openTableSize, int closeTableSize, int deadlockedBoxesSearch)
	: Solver(base, level, maxNodeNumber, maxRamSize, openTableSize,
			closeTableSize, deadlockedBoxesSearch)
{

}

/* -----------*/
/* Destructor */
/* -----------*/

/* -------*/
/* Others */
/* -------*/
void BotDFS::addTreeNodeToCloseList(TreeNode* treeNode)
{
	closeNodeList->addFirstItem(treeNode);
}
