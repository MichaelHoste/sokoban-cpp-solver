/*----------------------------------------------*/
/*================== ISokoban ==================*/
/*----------------------------------------------*/
/* Version : 1.0								*/
/* Author : Michaël Hoste						*/
/* Copyright : See LICENSE file for details   	*/
/*----------------------------------------------*/
/**
 * @brief class usefull to store statistical informations and results of the
 * work of a specific solver
 */
/*----------------------------------------------*/

#ifndef STATS_H_
#define STATS_H_

#include <stdio.h>
#include <stdlib.h>
#include "../Base.h"
#include "Solver.h"
#include "HashTable.h"
#include "BotA/BotA.h"

class BotBestPushesS;
class Solver;

class Stats
{
protected:
	Base* base; /**< Main class of the game */
	Solver* solver; /**< Solver pointer */
	TreeNode* treeNode; /**< solution treeNode */
	bool solved; /**< true if solver gets a solution (or a impossibility) */
	Path* solutionPath; /**< Solution path of this level */
	/**
	 * direct indication of num of pushes (no need to go through solutionPath
	 */
	int numOfPushes;
	char* message; /**< Message to be printed with stats */

public:
	/**
	 * Constructor of a stats object
	 * @param base Main class
	 * @param solver solver we want to link these stats
	 * @param treeNode solution TreeNode
	 * @param solutionPath Solution we founded (NULL if no solution founded)
	 * @param numOfPushes Num of pushes to go to solution
	 * @param message Message to be printed with stats
	 */
	Stats(Base* base, Solver* solver, TreeNode* treeNode, Path* solutionPath,
			int numOfPushes, char* message);

	/**
	 * Constructor to copy another stats object
	 * @param otherStats other stats object to copy in this object
	 */
	Stats(const Stats* otherStats);

	/**
	 * Destructor
	 */
	~Stats();

	/* --------*/
	/* Getters */
	/* --------*/
	/** @Return base object */
	inline Base* getBase(void) const { return base; }
	/** @Return solver */
	inline Solver* getSolver(void) const { return solver; }
	/** @Return true if solved, false if not */
	inline bool getSolved(void) const { return solved; }
	/** @Return solution treeNode */
	inline TreeNode* getTreeNode(void) const { return treeNode; }
	/** @Return solution path */
	inline Path* getSolutionPath(void) const { return solutionPath; }
	/** @Return solution path */
	inline int getNumOfPushes(void) const { return numOfPushes; }
	/** @Return message */
	inline char* getMessage(void) const { return message; }

	/* --------*/
	/* Setters */
	/* --------*/

	/* -------*/
	/* Others */
	/* -------*/
	/**
	 * Print stats on the screen (standard output)
	 */
	void print(void) const;

	/**
	 * Print stats on a file
	 * @param fileName Name of file we want to print in
	 */
	void print(const char* fileName) const;

	/**
	 * Create a tab with informations about repartition of h (hashing function)
	 * tab[0] means number of chained list with 0 items, tab[1] means number
	 * of chained list with 1 item, ... tab[n+1] = -1
	 * @return tab with "h" repartition informations. tab[n+1] = -1.
	 */
	int* hashingRepartitionStats() const;

protected:

	/**
	 * Print stats in text mode in a file or in the shell.
	 * @param fileName name of the file we want to print in. If NULL, then
	 *        it will be print on screen (standard output)
	 */
	void printInFile(const char* fileName) const;

};

#endif /*STATS_H_*/
