/*----------------------------------------------*/
/*================== ISokoban ==================*/
/*----------------------------------------------*/
/* Version : 1.0								*/
/* Author : Michaël Hoste						*/
/* Copyright : See LICENSE file for details   	*/
/*----------------------------------------------*/
/**
 * @brief IDA* algorithm based on best pushes number.
 */
/*----------------------------------------------*/

#ifndef BOTIDA_H_
#define BOTIDA_H_

#include <stdio.h>
#include <stdlib.h>
#include "../BotIDA/BotIDA.h"
#include "../BotBestPushesS/BotBestPushesS.h"

class BotIDA
{
protected:
	Base* base; /**< Main class of the game */
	Level* level; /**< Initial level we want to solve */
	int nodeNumber; /**< Number of explored nodes */
	int maxNodeNumber; /**< Max number of explored nodes */
	int maxRamSize; /**< Max allowed ram size */
	/** number of boxes to use when testing every
	 * possible positions of deadlock */
	int deadlockedBoxesSearch;
	bool onlyPushNumber; /**< Only get push number but not the path */
	bool quickSearch; /**< Test penalties of every nodes */
	Stats* stats; /**< Stats of this solving */

public:
	/** Name of this solver */
	int OPENTABLE_SIZE; /**< Size of hashtable */
	int CLOSETABLE_SIZE; /**< Size of hashtable */
	virtual const char* SOLVER_NAME(){return "BotIDA";}

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
	 * @param onlyPushNumber only keep number of pushes in Stats object.
	 * don't generate solution Path (CPU saving for deadlocks and penalties)
	 * @param quickSearch don't test penalties of every nodes before adding
	 * it on the tree
	 */
	BotIDA(Base* base, Level* level, int maxNodeNumber,	int maxRamSize,
			int openTableSize = 870967, int closeTableSize = 51827,
			int deadlockedBoxesSearch = 2, bool onlyPushNumber = false,
			bool quickSearch = false);

	/**
	 * Destructor
	 */
	virtual ~BotIDA();

	/* --------*/
	/* Getters */
	/* --------*/
	/** @Return Main class of the game */
	inline Base* getBase(void) const { return base; }
	/** @Return Level assigned to solver */
	inline const Level* getLevel(void) const { return level; }
	/** @Return nodeNumber */
	inline const int getNodeNumber(void) const { return nodeNumber; }
	/** @Return maximum Node Number */
	inline const int getMaxNodeNumber(void) const { return maxNodeNumber; }
	/** @Return maximum ram size */
	inline const int getMaxRamSize(void) const { return maxRamSize; }
	/** @Return stats of this level */
	inline const Stats* getStats(void) const { return stats; }

	/* -------*/
	/* Others */
	/* -------*/
	/**
	 * Start to resolve with parameters given to constructor
	 */
	void resolve(void);

protected:
	/**
	 * Test if message say solution is not found because not enough ram
	 * or nodeNumber limited
	 * @param message message of previous solving
	 * @return true if message say "not limited"
	 */
	bool notLimited(char* message) const;

	/**
	 * Test if message say solution is found or max limit reached
	 * @param message message of previous solving
	 * @return true if message say solution is not found and limit not reached
	 */
	bool notFinished(char* message) const;

	/**
	 * Test if message say solution is found
	 * @param message message of previous solving
	 * @return true if message say solution is found
	 */
	bool solutionFound(char* message) const;

	/**
	 * Save solution of this solver iteration
	 * @param solver solver with solution
	 */
	void saveSolution(Solver* solver);

	/**
	 * Test if a file named "IDASolution.dat" exists in the level repertory
	 * @return true if a file already exists (so, level is solved),
	 * false if not
	 */
	bool alreadySolved();

	/**
	 * Initialize correct iteration cost of IDA* from a file or directly
	 * generated.
	 * @return initial cost
	 */
	int initInitialCost();

	/**
	 * Generate correct iteration cost of IDA*
	 * @return initial cost
	 */
	int createInitialCost();

	/**
	 * Load correct iteration cost of IDA* from a file
	 * @return initial cost
	 */
	int loadInitialCost(char* fileName);

	/**
	 * Save last iteration cost of IDA* to a file
	 * @param cost cost
	 */
	void saveInitialCost(int cost);

};

#endif /*BOTIDA_H_*/
