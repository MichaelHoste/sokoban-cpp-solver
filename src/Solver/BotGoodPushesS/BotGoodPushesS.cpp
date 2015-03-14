/*----------------------------------------------*/
/*================== ISokoban ==================*/
/*----------------------------------------------*/
/* Version : 1.0								*/
/* Author : Michaël Hoste						*/
/* Copyright : See LICENSE file for details   	*/
/*----------------------------------------------*/

#include "../../../include/Solver/BotGoodPushesS/BotGoodPushesS.h"

/* ------------*/
/* Constructor */
/* ------------*/
BotGoodPushesS::BotGoodPushesS(Base* base, Level* level, int maxNodeNumber,
		int maxRamSize, int openTableSize, int closeTableSize,
		int deadlockedBoxesSearch, bool onlyPushNumber)
	: BotBestPushesS(base, level, maxNodeNumber, maxRamSize, openTableSize,
			closeTableSize, -1, deadlockedBoxesSearch, onlyPushNumber)
{

}

/* -----------*/
/* Destructor */
/* -----------*/

/* -------*/
/* Others */
/* -------*/
int BotGoodPushesS::g(TreeNode* treeNode, int pushCost) const
{
	return 0;
}
