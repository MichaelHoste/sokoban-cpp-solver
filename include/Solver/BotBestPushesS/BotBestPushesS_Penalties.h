/*----------------------------------------------*/
/*================== ISokoban ==================*/
/*----------------------------------------------*/
/* Version : 1.0								*/
/* Author : Michaël Hoste						*/
/* Copyright : See LICENSE file for details   	*/
/*----------------------------------------------*/
/**
 * @brief Management of cost penalties. Try to detect every sub-states
 * that causes a penalty to the initial cost.
 */
/*----------------------------------------------*/

#ifndef BOTBESTPUSHESS_PENALTIES_H_
#define BOTBESTPUSHESS_PENALTIES_H_

#include <stdio.h>
#include <stdlib.h>
#include "../Solver.h"
#include "../BotBestPushesS/BotBestPushesS.h"
#include "../Node.h"
#include "../TreeNode.h"

class BotBestPushesS_Penalties
{
protected:
	Base* base; /**< Main class of the game */
	Solver* solver; /**< deadlock object belang to this solver */

	/**
	 * List of node representations of penalties sub-nodes. Sorted by number
	 * of boxes by sub-nodes. pNodes[i][j] is the (j)th sub-state with (i+1)
	 * boxes */
	Node*** pNodes;
	/** penalties values of sub-nodes pValues[i][j] is the (j)th sub-state
	 * with (i+1) boxes */
	int** pValues;
	int pLength; /**< Length of pNodes */
	int* pListLengths; /**< length of each list of pNodes and pValues */

	int* penaltiesTestTab; /**< Tab of current boxes positions in penalties test */
	int penaltiesTestTabLength; /**< Length of deadlockTestTab */
	Level* emptyLevel; /**< Level with no boxes or pusher */

	const Level* level; /**< Initial level we want to solve */
	const Zone* goalZone; /**< Zone representation of all goals in this level */
	const int* zoneToLevelPos; /**< translation table between new positions in zone and old positions in level */
	const int zoneToLevelPosLength; /**< Length of translation table */
	const int* levelToZonePos; /**< translation table between old positions in level and new positions in zone */


public:
	/**
	 * Constructor
	 */
	BotBestPushesS_Penalties(Solver* solver);

	/**
	 * Destructor
	 */
	~BotBestPushesS_Penalties();

	/* --------*/
	/* Getters */
	/* --------*/
	/** @Return Assigned solver */
	inline Solver* getSolver(void) const { return solver; }
	/** @Return list of penalties sub-states */
	inline Node*** getPenaltiesNodeList(void) const { return pNodes; }
	/** @Return level with no boxes or pusher */
	inline Level* getEmptyLevel(void) const { return emptyLevel; }

	/* --------*/
	/* Setters */
	/* --------*/


	/* -------*/
	/* Others */
	/* -------*/
	/**
	 * Get this treenode penalty
	 * @param treenode treenode to be tested
	 * @return penalty of this treenode
	 */
	int getPenaltyOfTreeNode(TreeNode* treenode) const;

	/**
	 * Test the actual sub-node (defined in memory) by creating a temporary
	 * level and testing it with every possible pusherZone.
	 * @param tab tab of cell positions to test
	 * @param tabLength length of tab
	 * @param nodeLimit limit of nodes we will use to test. Will be decrease by
	 * number of really used nodes
	 * @return true if solver exceed max ram value, false if not.
	 */
	bool penaltiesListTest(int* tab, int tabLength, int* nodeLimit = NULL,
			int pusherLevelPos = -1);

protected:
	void testLessBoxesPenalties(int* tab, int tabLength, int limit,
			int pusherLevelPos);

	int getMaxPenalty(int* actualCellId) const;

	/**
	 * Add a node and its penalty to list of penalized nodes
	 * @param newPenalizedNode node to add
	 * @param penalty penalty associed to this node
	 * @param numOfBoxes number of boxes in this node
	 */
	void addToPNodes(Node* newPenalizedNode, int penalty, int numOfBoxes);

	/**
	 * Sort one nodes list in order of penalty of each node
	 * @param index index of pNodes (and pValues) cells we want to sort
	 */
	void orderList(int index) const;

	/**
	 * Initialize list of penalized sub-nodes in memory. Load it from file if
	 * already generated and generate (and store) it if not.
	 * @param numberOfBoxes number of boxes to put in a level before testing.
	 * If every sub-node of "n" boxes is generated, it means every penalties
	 * of "n" boxes will be stopped.
	 */
	void initPenaltiesList(int numberOfBoxes);

	/**
	 * Generate list of penalized sub-nodes in memory and save it in a file.
	 * @param numberOfBoxes number of boxes to put in a level before testing.
	 * If every sub-node of "n" boxes is generated, it means every penalties
	 * of "n" boxes will be stopped.
	 */
	void createPenaltiesList(int numberOfBoxes);

	/**
	 * Save list of penalized sub-nodes from memory to a file
	 */
	void savePenaltiesList();

	/**
	 * Load list of penalized sub-nodes from a file to memory
	 * @param file file we want to read from
	 * @param numberOfBoxes maximum number of boxes in nodes we want to load.
	 */
	void loadPenaltiesList(char* fileName, int numberOfBoxes);

	/**
	 * To test every sub-nodes, we start by testing every boxes positions with
	 * only a box. Then we test every boxes positions with 2 boxes, and 3, ...
	 * We need to increase positions of boxes in a tab.
	 * With 1 box : 1,2,3, ...
	 * With 2 boxes : 1,2 | 1,3 | 1,4 | ... | 1,N | 2,3 | 2,4 | ... | N-1,N
	 * ...
	 * This function increase boxes positions with M boxes and N positions in
	 * the level.
	 * @param positions number of positions we want to increase
	 * @param tab pointer to tab with all positions
	 * @param tabLength pointer to length of tab
	 */
	void increasePenaltiesListTest(int positions, int** tab, int* tabLength);

	/**
	 * To test every sub-nodes, we start by testing every boxes positions with
	 * only a box. Then we test every boxes positions with 2 boxes, and 3, ...
	 * We need to increase positions of boxes in a tab.
	 * With 1 box : 1,2,3, ...
	 * With 2 boxes : 1,2 | 1,3 | 1,4 | ... | 1,N | 2,3 | 2,4 | ... | N-1,N
	 * ...
	 * This function tell us when positions are last positions of a specific
	 * number of boxes. (for 1 box : N, for 2 boxes : N-1,N, for 3 boxes :
	 * N-2,N-1,N)
	 * @param positions number of positions we want to increase
	 * @param tab pointer to tab with all positions
	 * @param tabLength pointer to length of tab
	 * @return true if last boxes positions, false if not
	 */
	bool lastPenaltiesListTest(int positions, int* tab,	int tabLength);

	/**
	 * Test if node penalty if valid for each sub-group of goals
	 * If no, this penalty is not a valid penalty because it can break
	 * optimality of solution
	 * @param node node to test
	 * @param numOfBoxes number of boxes in this node
	 * @param prereq penalties search already done (num of boxes tested)
	 * @param penaltyDown penalty already given to this node. New penalty can
	 * only be above this value
	 * @param penaltyUp penalty we want to confirm. Real penalty can only be
	 * under this value
	 * @param quickValid true if valid is done quickly (not test of all
	 * goals configurations). It's quick but it can also be non optimal
	 * @param maxNodes maximum nodes to use to valid
	 * @return valid penalty (if 0, penalty is not valid). if -1, max nodes
	 * is reached during validation
	 */
	int validNode(Node* node, int numOfBoxes, int prereq, int penaltyDown,
			int penaltyUp, bool quickValid = false, int maxNodes = INT_MAX);

	/**
	 * If, in a level, the pusher in surrounded by boxes or walls, then
	 * it's not usefull to test its penalty
	 * @param l level we want to test
	 * @return true if this a penalty test is useless on this level, false
	 * if not
	 */
	bool isUselessPenalty(Level* l);

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
};

#endif /*BOTBESTPUSHESS_PENALTIES_H_*/
