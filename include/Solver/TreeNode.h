/*----------------------------------------------*/
/*================== ISokoban ==================*/
/*----------------------------------------------*/
/* Version : 1.0								*/
/* Author : Michaël Hoste						*/
/* Copyright : See LICENSE file for details   	*/
/*----------------------------------------------*/
/**
 * @brief Improved node representation for trees
 *
 * This is the kind of nodes to be used with trees
 */
/*----------------------------------------------*/

#ifndef TREENODE_H_
#define TREENODE_H_

#include <stdlib.h>
#include <string.h>
#include "../Base.h"
#include "../Level.h"
#include "Node.h"
#include "Zone.h"
#include "../Path.h"

class TreeNode
{
protected:
	Node* node; /**< content node of this item in the tree */
	TreeNode* parent; /**< parent ListNode of the tree */
	TreeNode** children; /**< List of children of the tree */
	int childrenNumber; /**< Number of children in the tree */

public:
	/**
	 * Constructor for empty node
	 * @param node Node to be stocked on this item
	 */
	TreeNode(Node* node);

	/**
	 * Destructor
	 */
	~TreeNode();

	/* --------*/
	/* Getters */
	/* --------*/
	/** @Return Content node */
	inline Node* getNode(void) const { return node; }
	/** @Return parent TreeNode */
	inline TreeNode* getParent(void) const { return parent; }
	/** @Return tab of children TreeNodes */
	inline TreeNode** getChildren(void) const { return children; }
	/** @Return number of children of this TreeNode */
	inline int getChildrenNumber(void) const { return childrenNumber; }

	/* --------*/
	/* Setters */
	/* --------*/
	/** Assign parent treeNode */
	inline void setParent(TreeNode* parent) { this->parent=parent; }
	/** Assign tab of children TreeNodes */
	inline void setChildren(TreeNode** children) { this->children=children; }
	/** Set children numbers */
	inline void setChildrenNumber(int number) { this->childrenNumber=number; }

	/* -------*/
	/* Others */
	/* -------*/
	/**
	 * Add a child node to this node
	 * @param newChild New child Node to add to this node
	 */
	void addChild(TreeNode* newChild);

	/**
	 * Get the "pre" pusher position of this node (cell just before he pushes
	 * a box)
	 * @return "pre" pusher position (level representation) of this node.
	 * return -1 if no pusher preposition (no parent node)
	 */
	int getPusherPrePosition(void) const;

	/**
	 * Get the pusher position of this node (just after he pushes a box)
	 * @return pusher position (level representation) of this node.
	 */
	int getPusherPosition(void) const;

	/**
	 * Get the pushed box pre-position (position of box before it moves)
	 * @return pushed box pre-position (level representation) of this node.
	 * return -1 if no pushed box (no parent node)
	 */
	int getPushedBoxPrePosition(void) const;

	/**
	 * Get the pushed box post-position (position of box after it moves)
	 * @return pushed box post-position (level representation) of this node.
	 * return -1 if no pushed box (no parent node)
	 */
	int getPushedBoxPostPosition(void) const;

	/**
	 * Return a list of all successive positions of pusher from start treeNode
	 * to this node
	 * @return list of all successive positions (level representation) of
	 * pusher from start treeNode to this node and terminated by "-1"
	 */
	int* listOfPusherPositions(void);

	/**
	 * Return path from root of the tree to this treeNode.
	 * @return List of moves (Path) the pusher must do to get this state of
	 * level from the root node
	 */
	Path* getPath(void);

	/**
	 * Compute total num of boxes of this branch for each cell
	 * @param statsTab must be NULL (usefull to iterative call)
	 * @return tab of length cells (where length is number of positions where
	 * pusher can move) with in each cell total number of box at this position
	 * in this branch.
	 */
	int* computeTreeBoxStats(int* statsTab = NULL) const;

protected:

};

#endif /*LISTNODE_H_*/
