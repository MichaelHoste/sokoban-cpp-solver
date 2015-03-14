/*----------------------------------------------*/
/*================== ISokoban ==================*/
/*----------------------------------------------*/
/* Version : 1.0								*/
/* Author : Michaël Hoste						*/
/* Copyright : See LICENSE file for details   	*/
/*----------------------------------------------*/

#include "../../../include/Solver/BotA/BotA_Child.h"
#include "../../../include/Solver/Node.h"

Child::Child(Node* child, int childCost):
	child(child),
	childCost(childCost)
{

};

Child::~Child()
{

};
