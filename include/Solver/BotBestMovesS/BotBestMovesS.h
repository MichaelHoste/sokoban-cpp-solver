/*----------------------------------------------*/
/*================== ISokoban ==================*/
/*----------------------------------------------*/
/* Version : 1.0								*/
/* Author : Michaël Hoste						*/
/* Copyright : See LICENSE file for details   	*/
/*----------------------------------------------*/
/**
 * @brief A* algorithm based on moves number of the pusher.
 * Nodes with less moves are explored first.
 * WARNING : this algorithm doesn't return best moves solution of this level
 * but only a good solution. This is because a state representation of the
 * level doesn't keep exact pusher position but only a pusher zone.
 * This solver will work efficiently (but with a lot of extra work) only if we
 * make level representation with boxes position (zone) and exact pusher
 * position (integer).
 */
/*----------------------------------------------*/

#ifndef BOTBESTMOVESS_H_
#define BOTBESTMOVESS_H_

#include <stdio.h>
#include <stdlib.h>
#include "../BotA/BotA.h"

class BotBestMovesS : public BotA
{
public:
	/** Name of this solver */
    virtual const char* SOLVER_NAME(){return "BotBestMovesS";}

	/**
	 * Constructor for a solver of a level
	 * @param base main class of the game
	 * @param level Level to be used
	 * @param maxNodeNumber Limit number of nodes to explore
	 * @param maxRamSize Limit max ram size to allocate
	 * @param openTableSize size of Open Table to allocate (hashtable)
	 * @param closeTableSize size of Close Table to allocate (hashtable)
	 * @param limit of accepted f(x). if costLimit = -1, costLimit = +infinity
	 * @param deadlockedBoxesSearch number of boxes to use when testing every
	 * possible positions of deadlock
	 */
	BotBestMovesS(Base* base, Level* level, int maxNodeNumber, int maxRamSize,
			int openTableSize, int closeTableSize, int costLimit,
			int deadlockedBoxesSearch = 2);

	/**
	 * Cost function to evaluate the cost of a treenode with its position in the
	 * tree (parent of treenode must be assigned before calling this function)
	 * this cost function will compute number of moves from the start until
	 * this node. So we can search min moves number first
	 * WARNING : not finished yet, to be really efficient, h(x) must be definied
	 */
	virtual int f(TreeNode* treeNode) const;

	virtual int g(TreeNode* treeNode) const;

	virtual int h(TreeNode* treeNode) const;
};

#endif /*BOTBESTMOVESS_H_*/
