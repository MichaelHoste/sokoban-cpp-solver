/*----------------------------------------------*/
/*================== ISokoban ==================*/
/*----------------------------------------------*/
/* Version : 1.0								*/
/* Author : Michaël Hoste						*/
/* Copyright : See LICENSE file for details   	*/
/*----------------------------------------------*/

#include "../../../include/Solver/BotBestMovesS/BotBestMovesS.h"

/* ------------*/
/* Constructor */
/* ------------*/
BotBestMovesS::BotBestMovesS(Base* base, Level* level, int maxNodeNumber,
		int maxRamSize, int openTableSize, int closeTableSize, int costLimit,
		int deadlockedBoxesSearch)
	: BotA(base, level, maxNodeNumber, maxRamSize, openTableSize,
			closeTableSize, costLimit, deadlockedBoxesSearch)
{

}

/* -----------*/
/* Destructor */
/* -----------*/

/* -------*/
/* Others */
/* -------*/
int BotBestMovesS::f(TreeNode* treeNode) const
{
	return h(treeNode)+g(treeNode);
}

int BotBestMovesS::g(TreeNode* treeNode) const
{
	if(treeNode->getParent() == NULL)
		return 0;
	else
	{
		TreeNode* parentNode = treeNode->getParent();

		// we get cost of parent
		int value = ((BotA_TreeNode*)parentNode)->getG();

		// We get positions from pusher in parentnode and this node
		int from = parentNode->getPusherPosition();
		int toprepos = treeNode->getPusherPrePosition();

		// we add number of moves from last push: moves before push + push itself
		value += parentNode->getNode()->countMovesFromAToB(from, toprepos) + 1;

		return value;
	}
}

int BotBestMovesS::h(TreeNode* treeNode) const
{
	return 0;
}
