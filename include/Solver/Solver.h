/*----------------------------------------------*/
/*================== ISokoban ==================*/
/*----------------------------------------------*/
/* Version : 1.0								*/
/* Author : Michaël Hoste						*/
/* Copyright : See LICENSE file for details   	*/
/*----------------------------------------------*/
/**
 * @brief Main abstract class of sokoban solver
 */
/*----------------------------------------------*/

#ifndef SOLVER_H_
#define SOLVER_H_

#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "../Base.h"
#include "../Level.h"
#include "Zone.h"

class TreeNode;
class HashTable;
class ChainedList;
class Node;
class Stats;
class Deadlock;

class Solver
{
protected:
	Base* base; /**< Main class of the game */
	const Level* level; /**< Initial level we want to solve */
	Stats* stats; /**< Statistic about this solver at the end of work */
	TreeNode* processingTreeRoot; /**< Root of the tree we build when solving */
	HashTable* openTable; /**< hashTable where all openened nodes are keeped */
	HashTable* closeTable; /**< hashTable where all closed nodes are keeped */
	ChainedList* closeNodeList; /**< found nodes but not processed yet */
	Deadlock* deadlockEngine; /**< Management object for all deadlocks */
	Zone* goalZone; /**< Zone representation of all goals in this level */
	int* goalsPositions; /**< zone positions of every goals */
	int* zoneToLevelPos; /**< translation table between new positions in zone and old positions in level */
	int zoneToLevelPosLength; /**< Length of translation table */
	int* levelToZonePos; /**< translation table between old positions in level and new positions in zone */
	int nodeNumber; /**< Number of explored nodes */
	int maxNodeNumber; /**< Max number of explored nodes */
	int ramSize; /**< Actual used ram size */
	int maxRamSize; /**< Max allowed ram size */
	int deadlockedBoxesSearch; /**< number of boxes to use when testing every possible positions of deadlock */
	bool onlyPushNumber; /**< only keep number of pushes in Stats object */
	bool solved; /**< True if solver found a solution or test everything without finding one */
	bool stopped; /**< stopped is true if limit (ram or nodes) reached */

public:
	int OPENTABLE_SIZE; /**< Size of hashtable */
	int CLOSETABLE_SIZE; /**< Size of hashtable */
	static const int PRINT_EVERY_X_LEVELS_QUICK = 1000; /**< print each x levels (quick search) */
	static const int PRINT_EVERY_X_LEVELS_SLOW = 20; /**< print each x levels (slow search) */
    virtual const char* SOLVER_NAME() = 0; /**< Name of this solver */

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
	Solver(Base* base, Level* level, int maxNodeNumber, int maxRamSize,
			int openTableSize = 870967, int closeTableSize = 51827,
			int deadlockedBoxesSearch = 2, bool onlyPushNumber = false);

	/**
	 * Destructor
	 */
	virtual ~Solver();

	/* --------*/
	/* Getters */
	/* --------*/
	/** @Return Main class of the game */
	inline Base* getBase(void) const { return base; }
	/** @Return Level assigned to solver */
	inline const Level* getLevel(void) const { return level; }
	/** @Return Stats of this solver */
	inline const Stats* getStats(void) const { return stats; }
	/** @Return Tree we build when solving */
	inline const TreeNode* getProcessingTreeRoot(void) const
	{ return processingTreeRoot; }
	/** @Return Hash table where all opened nodes are keeped */
	inline HashTable* getOpenTable(void) const
	{ return openTable; }
	/** @Return Hash table where all closed nodes are keeped */
	inline HashTable* getCloseTable(void) const
	{ return closeTable; }
	/** @Return founded nodes but not processed yet */
	inline ChainedList* getCloseNodeList(void) const
	{ return closeNodeList; }
	/** @Return Management of deadlock positions */
	inline const Deadlock* getDeadlockEngine(void) const { return deadlockEngine; }
	/** @Return Binary representation of the goals */
	inline const Zone* getGoalZone(void) const { return goalZone; }
	/** @Return goal positions of every goals */
	inline const int* getGoalsPositions(void) const { return goalsPositions; }
	/** @Return Translation table between zone and level */
	inline const int* getZoneToLevelPos(void) const { return zoneToLevelPos; }
	/** @Return Translation table length */
	inline const int getZoneToLevelPosLength(void) const { return zoneToLevelPosLength; }
	/** @Return Translation table between level and zone */
	inline const int* getLevelToZonePos(void) const { return levelToZonePos; }
	/** @Return nodeNumber */
	inline const int getNodeNumber(void) const { return nodeNumber; }
	/** @Return maximum Node Number */
	inline const int getMaxNodeNumber(void) const { return maxNodeNumber; }
	/** @Return maximum ram size */
	inline const int getMaxRamSize(void) const { return maxRamSize; }
	/** @Return maximum ram size */
	inline const int getDeadlockedBoxesSearch(void) const { return deadlockedBoxesSearch; }
	/** @Return value of solved (true if solved or impossible, false if not) */
	inline const int getSolved(void) const { return solved; }

	/* --------*/
	/* Setters */
	/* --------*/

	/* -------*/
	/* Others */
	/* -------*/
	/**
	 * Start to resolve with parameters given to constructor
	 */
	virtual void resolve(void);

	/**
	 * Compute order of successive positions of the pusher.
	 * Compute order of successive positions of boxes.
	 * @param pusherSteps tab of nRows*nCols (M*N of level) initialized
	 * with every positions to -1
	 * @param boxesSteps tab of nRows*nCols (M*N of level) initialized
	 * with every positions to -1
	 * @return pusherSteps every position of the pusher are indicated with
	 * a number of priority
	 * @return boxesSteps every position of the boxes are indicated with
	 * a number of priority
	 */
	void computeSteps(int** pusherSteps, int** boxesSteps) const;

	/**
	 * Return size in octets of the actual state of this solver
	 * @return size in octets of the actual state of this solver
	 */
	virtual int getSize(void);

protected:
	/**
	 * Initialization of every structure usefull to resolve a level
	 * (hashtable, level translation tables, deadlock, tree, node list, ...)
	 */
	virtual void initialize(void);

	/**
	 * Try to resolve this level with a fixed max number of nodes or a fixed
	 * max number of ram size to allocate. (first to be reached)
	 */
	virtual void process();

	/**
	 * Get first treeNode from closeNodeList and delete it from the list
	 * @return first treeNode from closeNodeList
	 */
	virtual TreeNode* firstTreeNode(void);

	/**
	 * Add a new TreeNode to the waiting list at the right position.
	 */
	virtual void addTreeNodeToCloseList(TreeNode* treeNode) = 0;

	/**
	 * Get list of children from a treeNode and move this treenode from
	 * close list to open list
	 * @return list of children (terminated by NULL)
	 */
	virtual Node** findChildren(TreeNode* treeNode);

	/**
	 * look if a node was already searched in the past
	 * @param node to test
	 * @return true if node is already searched (open or close node)
	 */
	virtual bool isAlreadySearched(Node* node);

 	/**
 	 * Find every usefull position in the level and make a translation table
 	 * between new positions in zone and old positions in level
 	 */
	virtual void initZoneToLevelPos(void);

 	/**
 	 * Make a translation table between positions in level and positions in
 	 * zones. table[pos] will return position of related bit in zone if it
 	 * exists or -1 if no related position in zone (wall or outside cell).
 	 */
	virtual void initLevelToZonePos(void);

	/**
	 * Initialize goal zone of level we want to resolve.
	 */
	virtual void initGoalZone(void);

	/**
	 * Initialize deadlock engine of level we want to resolve.
	 */
	virtual void initDeadlockEngine(void);

	/**
	 * Initialize empty hashtable
	 */
	virtual void initHashTable(void);

	/**
	 * Initialize empty waiting node list
	 */
	virtual void initCloseNodeList(void);

	/**
	 * Initialize starting node of processing tree and put it into
	 * closeNodeList and hashTable.
	 * If starting node is deadlocked or solution, then this->solved = true
	 */
	virtual void initStartingNode(void);

	/**
	 * Create a TreeNode from a node and its parent treeNode in the search tree
	 * (if he got one, else NULL)
	 * @param node Node we want to put in the new TreeNode
	 * @param parentTreeNode Parent TreeNode we want to attach to new TreeNode
	 * @param pushCost number of pushes from its parent
	 * (usefull for macro-pushes. Can be anything if not A*)
	 * @return a new TreeNode that contains param "Node"
	 */
	virtual TreeNode* createTreeNode(Node* node, TreeNode* parentTreeNode,
			int pushCost);

	/**
	 * Test if a treeNode is a solution node and create Stats object.
	 * @param treeNode treenode we want to test
	 * @return true if this treenode is a solution, false if not
	 */
	virtual bool testSolution(TreeNode* treeNode);

	/**
	 * Test if a treenode is a solution treenode.
	 * @param treeNode TreeNode to tested
	 * @return true if this node is solution (all boxes on goals), false if not
	 */
	virtual bool solutionNode(const TreeNode* treeNode) const;

	/**
	 * During solving, print usefull infos in the console
	 * @param treeNode treenode to draw
	 */
	virtual void printInfos(TreeNode* treeNode);
};

#endif /*SOLVER_H_*/
