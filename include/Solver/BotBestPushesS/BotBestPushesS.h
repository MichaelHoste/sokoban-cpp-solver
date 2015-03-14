/*----------------------------------------------*/
/*================== ISokoban ==================*/
/*----------------------------------------------*/
/* Version : 1.0								*/
/* Author : Michaël Hoste						*/
/* Copyright : See LICENSE file for details   	*/
/*----------------------------------------------*/
/**
 * @brief A* search algorithm based on pushes number of the pusher
 * Nodes with less pushes are explored first.
 * this algorithm DOES return best pushes solution of this level if used with
 * costLimit = -1
 */
/*----------------------------------------------*/

#ifndef BOTBESTPUSHESS_H_
#define BOTBESTPUSHESS_H_

#include <stdio.h>
#include <stdlib.h>
#include "../BotA/BotA.h"
#include "../DijkstraBox.h"
#include "BotBestPushesS_Matrix.h"
#include "BotBestPushesS_Munkres.h"
#include "../BotBFS/BotBFS.h"
#include "../BotIDA/BotIDA.h"

class BotBestPushesS_Penalties;

class BotBestPushesS : public BotA
{
protected:
	int** costTable; /**< cost table of this level */
	BotBestPushesS_Penalties* penaltiesEngine; /**< cost table of this level */
	Level* emptyLevel; /**< Level without boxes */

public:
	/** Name of this solver */
    virtual const char* SOLVER_NAME(){return "BotBestPushesS";}

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
	BotBestPushesS(Base* base, Level* level, int maxNodeNumber, int maxRamSize,
			int openTableSize, int closeTableSize, int costLimit,
			int deadlockedBoxesSearch = 2, bool onlyPushNumber = false,
			bool quickSearch = false);

	/**
	 * Destructor
	 */
	virtual ~BotBestPushesS();

	/* --------*/
	/* Getters */
	/* --------*/
	/** @Return costTable */
	inline int** getCostTable(void) const { return costTable; }
	/** @Return penaltiesEngine */
	inline BotBestPushesS_Penalties* getPenaltiesEngine(void) const
	{ return penaltiesEngine; }

protected:
	/**
	 * Initialization of every structure usefull to resolve a level
	 * (hashtable, level translation tables, deadlock, tree, node list, ...)
	 * + initialization of cost table
	 */
	virtual void initialize(void);

	/**
	 * Create a TreeNode from a node and its parent treeNode in the search tree
	 * (if he got one, else NULL) and assign a cost at it. This version create
	 * a treeNode after being tested for new deadlock of penalized sub-nodes
	 * @param node Node we want to put in the new TreeNode
	 * @param parentTreeNode Parent TreeNode we want to attach to new TreeNode
	 * @param pushCost number of pushes from its parent
	 * (usefull for macro-pushes)
	 * @return a new TreeNode that contains param "Node" and a definied cost
	 */
	virtual TreeNode* createTreeNode(Node* node, TreeNode* parentTreeNode,
			int pushCost);

	/**
	 * Test a box position in this actual node to see if we can detect
	 * some deadlocks or penalties with other boxes in the way to pushes
	 * this one to a solution state.
	 * This function add automaticly new deadlock and penalties on database
	 * @param node node we want to test
	 * @param boxLevelPos box position (level representation) of box we want
	 * to test
	 * @param maxNodes maximum nodes to use to analyze
	 */
	virtual void analyzeNode(const Node* node, int boxLevelPos, int maxNodes);

	int findNextBoxToTest(const Node* Node, int** pusherSteps,
			int** boxesSteps);

	/**
	 * test a return message from a solver.
	 * @return true if message says "Impossible to solve : Ram Limit or Max
	 * Nodes Limit reached", false if not
	 */
	bool ramLimitReached(char* message) const;

	/**
	 * test a return message from a solver.
	 * @return true if message says "Impossible to solve : No more nodes in
	 * waiting list" or "Impossible to solve : first node has no child",
	 * false if not
	 */
	bool notResolved(char* message) const;

	/**
	 * test all sub-states positions stocked in "deductivePositions.dat"
	 */
	void processDeductive();

	/**
	 * Cost function to evaluate the cost of a treenode with its position in the
	 * tree (parent of treenode must be assigned before calling this function)
	 * this cost function will compute number of pushes from the start until this
	 * node (g(x)) + estimate number of pushes from this node to a solution node
	 * (h(x)).
	 * @param treeNode node to be estimated
	 * @param pushCost number of pushes from its parent
	 * (usefull for macro-pushes)
	 * @return cost value
	 */
	virtual int f(TreeNode* treeNode, int pushCost) const;

	/**
	 * Number of pushes from the start until this node
	 * @param treeNode to be computed
	 * @param pushCost number of pushes from its parent
	 * (usefull for macro-pushes)
	 * @return cost value from start to this node
	 */
	virtual int g(TreeNode* treeNode, int pushCost) const;

	/**
	 * Estimated number of pushes from this node to a solution
	 * @param treeNode to be computed
	 * @return estimated cost value from this node to solution node
	 */
	virtual int h(TreeNode* treeNode) const;

	/**
	 * Create or load a cost Table for this level. A cost table is a  2
	 * dimensions table with, for each zone representation position of level,
	 * the associated min number of pushes to each goal. If a position can't
	 * reach a goal, value is set to INT_MAX
	 * exemple : costTable[4][2] is minimum number of pushes to join a box on
	 * 5th zone position to third goal.
	 * @return costTable of this level
	 */
	int** initCostTable();

	/**
	 * Create cost Table for this level.
	 * exemple : costTable[4][2] is minimum number of pushes to join a box on
	 * 5th zone position to third goal.
	 * @return created costable
	 */
	int** createCostTable();

	/**
	 * Load a cost Table of this level from a file.
	 * exemple : costTable[4][2] is minimum number of pushes to join a box on
	 * 5th zone position to third goal.
	 * @param file from where we want to load costTable
	 * @return loaded costable
	 */
	int** loadCostTable(char* file);

	/**
	 * Save costTable in a appropriated file
	 * @param cTable cost table we want to save
	 * @param zoneLength zoneLength of this level
	 */
	void saveCostTable(int** cTable, int zoneLength);

	/**
	 * Create an assignation matrix for this treeNode.
	 * An assignation matrix is a 2 dimensions square matrix with boxes positions
	 * in rows and goals positions in cols. Intersection is number of
	 * pushes used to move a box in a goal without the other boxes in the way.
	 * @param boxesPositions boxes positions of this current treeNode (zone
	 * representation)
	 * @param goalsPositions goals positions of this current level (zone
	 * representation)
	 * @return assignation matrix where matrix(i, j) is number of pushes to move
	 * box i to goal j.
	 */
	BotBestPushesS_Matrix<double>*
	createAssignationMatrix(const int* boxesPositions,
			const int* goalsPositions) const;
};

#endif /*BOTBESTPUSHESS_H_*/
