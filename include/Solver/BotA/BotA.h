/*----------------------------------------------*/
/*================== ISokoban ==================*/
/*----------------------------------------------*/
/* Version : 1.0								*/
/* Author : Michaël Hoste						*/
/* Copyright : See LICENSE file for details   	*/
/*----------------------------------------------*/
/**
 * @brief A implementation of A* algorithm without specific function
 * f(x) = g(x) + h(x) (this class must be derived)
 */
/*----------------------------------------------*/

#ifndef BOTA_H_
#define BOTA_H_

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <limits.h>
#include "../Solver.h"
#include "BotA_HeapStack.h"
#include "BotA_TreeNode.h"
#include "BotA_ListNode.h"
#include "BotA_HashTable.h"
#include "BotA_ListNode2.h"
#include "BotA_ChainedList.h"
#include "BotA_Child.h"
#include "../Deadlock.h"
#include "../Stats.h"
#include "../HashTable.h"

class BotA : public Solver
{
protected:
	int costLimit; /**< limit of accepted f(x) */
	/**
	 * minimum value of rejected f(x) for next iteration of IDA*.
	 * Maybe costLimit+1 but somethimes it's costLimit+2 or more
	 */
	int minReject;
	bool quickSearch; /**< Test penalties of every nodes */

public:
    virtual const char* SOLVER_NAME() = 0; /**< Name of this solver */

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
	BotA(Base* base, Level* level, int maxNodeNumber, int maxRamSize,
			int openTableSize, int closeTableSize, int costLimit,
			int deadlockedBoxesSearch = 2, bool onlyPushNumber = false,
			bool quickSearch = false);

	/* --------*/
	/* Getters */
	/* --------*/
	/** @Return minReject */
	inline int getMinReject(void) const { return minReject; }
	/** @Return limit of accepted f(x) */
	inline int getCostLimit(void) const { return costLimit; }
	/** @Return quick search */
	inline int getQuickSearch(void) const { return quickSearch; }

	/**
	 * Return size in octets of the actual state of this solver
	 * @return size in octets of the actual state of this solver
	 */
	virtual int getSize(void);

	/**
	 * Get cost of first treeNode
	 * @return cost of first treeNode
	 */
	int getInitialCost();

protected:
	/**
	 * Try to resolve this level with a fixed max number of nodes or a fixed
	 * max number of ram size to allocate. (first to be reached)
	 */
	virtual void process();

	/**
	 * If ram limit or node limit are reached, we save a list of sub-states
	 * of boxes where cells are most used by boxes in the tree.
	 * This list of sub-states will be tested before next solving of same level
	 * @param number of "top" cell positions we want to keep. If we take 15, we
	 * will have list of every sub-states of 1,2,3,4,5 and 6 boxes for 15 most
	 * used positions
	 */
	void saveMostUsedPositions(int numberOfPos);

	/**
	 * Test if a treenode is a dead branch (treenode and all children are in
	 * openTable) or not
	 * @param treeNode node to be tested
	 * @return true if treenode is a dead branch, false if not
	 */
	bool isDeadTreeNode(TreeNode* treeNode);

	/**
	 * delete a dead treenode from the tree and all its children. Also delete
	 * this node from openTable.
	 * @param treeNode node to be deleted
	 */
	void deleteDeadTreeNode(TreeNode* treeNode);

	/**
	 * If this node is already searched, we look at the cost of stocked
	 * treeNode and move what it need to be moved to keep the lowest cost
	 * @param treeNode treenode we want to look if it's new or beter than
	 * anything existing
	 * @param counter counter of children of actual treeNode
	 * @return true if it's already searched and doesn't need to be added.
	 * false if it's not already searched and we need to add it.
	 */
	bool workOnAlreadySearched(TreeNode* treeNode, int* counter);

	/**
	 * Get first treeNode from closeNodeList and delete it from the list
	 * @return first treeNode from closeNodeList
	 */
	TreeNode* firstTreeNode(void);

	/**
	 * Get list of children from a treeNode and move this treenode from
	 * close list to open list. This function take macro children too.
	 * (children with many pushes to put a box on a goal in one step)
	 * @param treeNode treeNode we want to find children
	 * @return list of children (terminated by NULL) with list of cost
	 * (number of push) of every children.
	 * When it's not a macro push, it's always 1.
	 */
	Child** findPonderedChildren(TreeNode* treeNode);

	/**
	 * Add a new TreeNode to the waiting list at the right position.
	 * New Treenode is added TO THE CORRECT PLACE DEPENDING OF ITS COST
	 * (best-first search)
	 */
	virtual void addTreeNodeToCloseList(TreeNode* treeNode);

	/**
	 * Initialize empty hashtables
	 */
	virtual void initHashTable(void);

	/**
	 * Initialize empty waiting node list
	 */
	virtual void initCloseNodeList(void);

	/**
	 * Create a TreeNode from a node and its parent treeNode in the search tree
	 * (if he got one, else NULL) and assign a cost at it
	 * @param node Node we want to put in the new TreeNode
	 * @param parentTreeNode Parent TreeNode we want to attach to new TreeNode
	 * @param pushCost number of pushes from its parent
	 * (usefull for macro-pushes)
	 * @return a new TreeNode that contains param "Node" and a definied cost
	 */
	virtual TreeNode* createTreeNode(Node* node, TreeNode* parentTreeNode,
			int pushCost);

	/**
	 * Move a treenode in the tree by split it from its parent and attach it to a
	 * new parent
	 * @param treeNode treenode we want to move in the tree
	 * @param newParent new parent for this treenode
	 */
	void moveTreeNode(TreeNode* treeNode, TreeNode* newParent);

	/**
	 * Cost function to evaluate the cost of a treenode with its position in the
	 * tree (parent of treenode must be assigned before calling this function)
	 * @param treeNode to be computed
	 * @param pushCost number of pushes from its parent
	 * (usefull for macro-pushes)
	 * @return cost value of this node
	 */
	virtual int f(TreeNode* treeNode, int pushCost) const = 0;

	/**
	 * Cost function to evaluate the cost of getting to this treeNode from the
	 * initial treenode (parent of treenode must be assigned before calling this
	 * function)
	 * @param treeNode to be computed
	 * @param pushCost number of pushes from its parent
	 * (usefull for macro-pushes)
	 * @return cost value from start to this node
	 */
	virtual int g(TreeNode* treeNode, int pushCost) const = 0;

	/**
	 * Cost function to evaluate the cost of admissibly estimated distance to a
	 * goal treenode from this treenode (parent of treenode must be assigned
	 * before calling this function)
	 */
	virtual int h(TreeNode* treeNode) const = 0;

	/**
	 * Test if a treeNode is a solution node and create Stats object.
	 * @param treeNode treenode we want to test
	 * @return true if this treenode is a solution, false if not
	 */
	virtual bool testSolution(TreeNode* treeNode);

	/**
	 * Test if a treenode is a solution treenode (based on h(x)=0 value)
	 * @param treeNode TreeNode to tested
	 * @return true if this node is solution (h(x) = 0), false if not
	 */
	virtual bool solutionNode(const TreeNode* treeNode) const;

	/**
	 * Find most used positions (boxes on them). Positions with goals or
	 * without goals.
	 * @param number number of most used positions we want to find
	 * @param withGoals true if we want to take care of goals, false if not
	 * @return tab of "number" cells filled with most used positions.
	 * Be carefull, this tab can be ended with several -1 "number" is bigger
	 * than usefull cells
	 */
	int* mostUsedPositions(int number, bool withGoals);

	/**
	 * find max cell of a tab, keep it, and replace it with -1
	 * @param tab tab of values
	 * @param tabLength length of tab
	 * @param withGoals true if we want to take care of goals, false if not
	 * @return max cell of the tab
	 */
	int findMaxCellAndReplaceValue(int* tab, int tabLength, bool withGoals);

//	int computeTree(TreeNode* treeNode) const;

	/**
	 * During solving, print usefull infos in the console
	 * @param treeNode treenode to draw
	 */
	virtual void printInfos(TreeNode* treeNode);
};

#endif /*BOTA_H_*/
