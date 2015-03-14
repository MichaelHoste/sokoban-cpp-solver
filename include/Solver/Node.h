/*----------------------------------------------*/
/*================== ISokoban ==================*/
/*----------------------------------------------*/
/* Version : 1.0								*/
/* Author : Michaël Hoste						*/
/* Copyright : See LICENSE file for details   	*/
/*----------------------------------------------*/
/**
 * @brief Lowest representation of a node. Only contain pusher zone and boxes
 * zone.
 *
 * This is the minimum representation of a level state. But this state cannot
 * be used without the original level
 */
/*----------------------------------------------*/

#ifndef NODE_H_
#define NODE_H_

#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include "Solver.h"
#include "DijkstraBox.h"
#include "BotA/BotA_Child.h"

class Node
{
protected:
	const Solver* solver; /**< Assigned solver */
	Zone* pusherZone; /**< Move zone of the pusher */
	Zone* boxesZone; /**< Position of boxes */

	/**
	 * Usefull class to store informations on macro-pushes
	 */
	class Macro
	{
	public :
		Macro(){};
		~Macro(){};
		int boxOldPosition;
		int boxNewPosition;
		int pusherNewPosition;
		int numberOfPushes;
		int costOfGoal;
	};

public:
	/**
	 * Constructor for a empty node assigned to a solver
	 * @param solver Assigned solver
	 */
	Node(const Solver* solver);

	/**
	 * Constructor for a node with defined pusherZone and boxesZone
	 * @param solver Assigned solver
	 * @param pusherZone zone where pusher can move
	 * @param boxesZone zone of boxes positions
	 */
	Node(const Solver* solver, Zone* pusherZone, Zone* boxesZone);

	/**
	 * Constructor for a node that's copy of another node
	 * @param otherNode other node we want to copy
	 */
	Node(const Node* otherNode);

	/**
	 * Destructor
	 */
	~Node();

	/* --------*/
	/* Getters */
	/* --------*/
	/** @Return Assigned solver */
	inline const Solver* getSolver(void) const { return solver; }
	/** @Return Zone of possible pusher move */
	inline Zone* getPusherZone(void) const { return pusherZone; }
	/** @Return Zone of boxes */
	inline Zone* getBoxesZone(void) const { return boxesZone; }

	/* --------*/
	/* Setters */
	/* --------*/
	/** @Return Assign pusher zone */
	inline void setPusherZone(Zone* pusherZone) { this->pusherZone=pusherZone; }
	/** @Return Assign boxes zone */
	inline void setBoxesZone(Zone* boxesZone) { this->boxesZone=boxesZone; }

	/* -------*/
	/* Others */
	/* -------*/
	/**
	 * Test if this node is egal to another node
	 * @param otherNode Other node we want to compare to this Node object
	 * WARNING : 2 nodes must provide from the same level !
	 * @return true if node is the same, false if not
	 */
	bool isEgal(Node* otherNode) const;

	/**
	 * Test if a node is a solution node for the associed level
	 * @return true if this node is solution, false if not
	 */
	bool isSolution(void) const;

	/**
	 * Use this node to find all successor nodes. A successor node exists if
	 * it can be reached from the original node just by pushing a box.
	 * @return list of all successor nodes. Terminated by a NULL.
	 */
	Node** findChildren(void) const;

	/**
	 * Use this node to find all macro successor nodes. A macro successor node
	 * exists if we can push a box on a goal directly, even if you have to do
	 * many pushes of the same box to make it.
	 * @param nodeList list of direct successor previously found with
	 * findChildren.
	 * @return modified nodeList added with all macro pushes.
	 * Terminated by a NULL.
	 * @return tab of number of pushes for all macro-pushes.
	 */
	Child** findMacroChildren(Node** nodeList);

	/**
	 * This function use this node (box positions) to make the shortest way
	 * between position A and position B in the level and return list of moves
	 * used to do it. Be carreful, no box can be pushed on the way from A to B
	 * @param posA starting position (level representation)
	 * @param posB ending position (level representation)
	 * @return list of moves to go from posA to posB terminated by '\0'
	 */
	char* listMovesFromAToB(const int posA, const int posB) const;

	/**
	 * This function use this node (box positions) to make the shortest way
	 * between position A and position B in the level and return number of moves
	 * used to do it. Be carreful, no box can be pushed on the way from A to B
	 * @param posA starting position (level representation)
	 * @param posB ending position (level representation)
	 * @return number of moves to go from posA to posB.
	 */
	int countMovesFromAToB(const int posA, const int posB) const;

	/**
	 * Print node (level) state in the console
	 */
	void print(void) const;

protected:
	/**
	 * Recursive function based on Dijkstra algorithm. It fill a tab in with
	 * every informations about min distance between a cell and start pos.
	 * @param levelTab tab of values (init with -1 for walls and big numbers
	 * for every other cells)
	 * @param startPos start position to compute (level representation)
	 * @param colsNumber number of cols in this level
	 * @return levelTab filled with every informations about min distances
	 * between a cell and start pos
	 */
	void dijkstraRec(int* levelTab, const int startPos,
			const int colsNumber) const;

	/**
	 * Get cost of every goals for this node. A cost of a goal is the number of
	 * boxes or walls just next this goal. cost go from 0 to 4. 4 is a top
	 * priority goal
	 * @param node node to be tested
	 * @return tab of cost of each goal
	 */
	int* computeCostOfGoals(Node* node);
};

#endif /*NODE_H_*/
