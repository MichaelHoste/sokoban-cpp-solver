/*----------------------------------------------*/
/*================== ISokoban ==================*/
/*----------------------------------------------*/
/* Version : 1.0								*/
/* Author : Michaël Hoste						*/
/* Copyright : See LICENSE file for details   	*/
/*----------------------------------------------*/
/**
 * @brief A* search algorithm based on estimated remaining pushes number of
 * the pusher. (f(x) = h(x))
 * Nodes with less pushes are explored first.
 * this algorithm DOESN'T return best pushes solution of this level but only
 * a good solution.
 */
/*----------------------------------------------*/

#ifndef BOTGOODPUSHESS_H_
#define BOTGOODPUSHESS_H_

#include <stdio.h>
#include <stdlib.h>
#include "../BotA/BotA.h"
#include "../BotBestPushesS/BotBestPushesS.h"

class BotGoodPushesS : public BotBestPushesS
{
protected:

public:
	/** Name of this solver */
    virtual const char* SOLVER_NAME(){return "BotGoodPushesS";}

	/**
	 * Constructor for a solver of a level
	 * @param base main class of the game
	 * @param level Level to be used
	 * @param maxNodeNumber Limit number of nodes to explore
	 * @param maxRamSize Limit max ram size to allocate
	 * @param openTableSize size of Open Table to allocate (hashtable)
	 * @param closeTableSize size of Close Table to allocate (hashtable)
	 * @param deadlockedBoxesSearch number of boxes to use when testing every
	 * possible positions of deadlock
	 * @param onlyPushNumber only keep number of pushes in Stats object.
	 * don't generate solution Path (CPU saving for deadlocks and penalties)
	 */
	BotGoodPushesS(Base* base, Level* level, int maxNodeNumber,	int maxRamSize,
			int openTableSize = 870967, int closeTableSize = 51827,
			int deadlockedBoxesSearch = 2, bool onlyPushNumber = false);

	/* --------*/
	/* Getters */
	/* --------*/
	/**
	 * In this solver, we use f(x)=h(x) and not f(x)=g(x)+h(x) so g(x) must
	 * be egal to 0.
	 * @param treeNode to be computed
	 * @param pushCost number of pushes from its parent
	 * (usefull for macro-pushes)
	 * @return always 0
	 */
	virtual int g(TreeNode* treeNode, int pushCost) const;
};

#endif /*BOTGOODPUSHESS_H_*/
