/*----------------------------------------------*/
/*================== ISokoban ==================*/
/*----------------------------------------------*/
/* Version : 1.0								*/
/* Author : Michaël Hoste						*/
/* Copyright : See LICENSE file for details   	*/
/*----------------------------------------------*/

#include "../../../include/Solver/BotBFS/BotBFS.h"

/* ------------*/
/* Constructor */
/* ------------*/
BotBFS::BotBFS(Base* base, Level* level, int maxNodeNumber, int maxRamSize,
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
void BotBFS::addTreeNodeToCloseList(TreeNode* treeNode)
{
	closeNodeList->addLastItem(treeNode);
}
