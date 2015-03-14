/*----------------------------------------------*/
/*================== ISokoban ==================*/
/*----------------------------------------------*/
/* Version : 1.0								*/
/* Author : Michaël Hoste						*/
/* Copyright : See LICENSE file for details   	*/
/*----------------------------------------------*/
/**
 * @brief Breadth-first search implementation of solver
 */
/*----------------------------------------------*/

#ifndef BOTBFS_H_
#define BOTBFS_H_

#include <stdio.h>
#include <stdlib.h>
#include "../Solver.h"
#include "../ChainedList.h"
#include "../Stats.h"

class BotBFS : public Solver
{
public:
    virtual const char* SOLVER_NAME(){return "BotBFS";} /**< Name of this solver */

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
	 */
	BotBFS(Base* base, Level* level, int maxNodeNumber, int maxRamSize,
			int openTableSize = 870967, int closeTableSize = 51827,
			int deadlockedBoxesSearch = 2);

protected:
	/**
	 * Add a new TreeNode to the waiting list at the right position.
	 * New Treenode is added TO THE END of the waiting list
	 * (breadth-first)
	 */
	virtual void addTreeNodeToCloseList(TreeNode* treeNode);
};

#endif /*BOTBFS_H_*/
