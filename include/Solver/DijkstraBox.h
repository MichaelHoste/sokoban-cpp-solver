/*----------------------------------------------*/
/*================== ISokoban ==================*/
/*----------------------------------------------*/
/* Version : 1.0								*/
/* Author : Michaël Hoste						*/
/* Copyright : See LICENSE file for details   	*/
/*----------------------------------------------*/
/**
 * @brief Dijkstra algorithm used to see what goals a box can reach and with
 * how many pushes it could.
 *
 * This is a special representation of dijkstra because we want to keep
 * possible positions of the pusher after each push. Each box position is
 * represented by 4 nodes : left, up, right and down. Value in each node
 * means "I can push this box in there by pushing it from left/up/right/down
 * with a total of VALUE pushes"
 */
/*----------------------------------------------*/

#ifndef DIJKSTRABOX_
#define DIJKSTRABOX_

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include "Solver.h"
#include "Node.h"

class CellList
{
public:
	int pos;
	int dir;
	int value;
	CellList* next;
	CellList(int pos, int dir, int value, CellList* next):
		pos(pos),
		dir(dir),
		value(value),
		next(next)
	{}
	~CellList()
	{
		if(next)
			delete next;
	}
};

class DijkstraBox
{
protected:
	/**
	 * actual dijkstra cost Table : getPos(x,y,VAR) is value of a cell where
	 * VAR is RIGHT, LEFT, UP or DOWN and (x,y) is position of a cell of
	 * this level.
	 */
	int* dTable;
	const Solver* solver; /**< Solver w're using right now */
	Node* node; /**< current node we want to apply dijkstra to */
	int boxPos; /**< current box position we want to test */
	int nCols; /**< number of columns in this level */

	CellList* firstCell; /**< First cell of waiting cells chainedList */
	CellList* lastCell; /**< Last cell of waiting cells chainedList */

public:
	static const int LEFT  = 0; /**< index of left node */
	static const int RIGHT = 1; /**< index of right node */
	static const int UP    = 2; /**< index of up node */
	static const int DOWN  = 3; /**< index of down node */

	/**
	 * Constructor of a new table where we'll apply dijkstra later
	 * This constructor will let you use a node where a specific
	 * starting pusherZone is defined
	 * @param solver Solver we're using right now
	 * @param node current node we want to test
	 * @param boxPos box position (level representation) we want to move
	 * to goals
	 */
	DijkstraBox(const Solver* solver, Node* node, int boxPos);

	/**
	 * Destructor of the table
	 */
	virtual ~DijkstraBox();

	/* --------*/
	/* Getters */
	/* --------*/
	/** @Return Solver we're using right now */
	inline const Solver* getSolver(void) const { return solver; }

	/* -------*/
	/* Others */
	/* -------*/
	/**
	 * Return value of a position in the table
	 * @param x x position in level
	 * @param y y position in level
	 * @param dir must be RIGHT/LEFT/UP/DOWN
	 * @return value of a position in the table
	 */
	inline int getPos(int x, int y, int dir) const
	{
		return dTable[x*nCols*4 + y*4 + dir];
	}

	/**
	 * Return value of a position in the table
	 * @param pos pos position in this level
	 * @param dir must be RIGHT/LEFT/UP/DOWN
	 * @return value of a position in the table
	 */
	inline int getPos(int pos, int dir) const
	{
		return dTable[pos*4 + dir];
	}

	/**
	 * Assign value of a position in the table
	 * @param x x position in level
	 * @param y y position in level
	 * @param dir must be RIGHT/LEFT/UP/DOWN
	 */
	inline void setPos(int x, int y, int dir, int value) const
	{
		dTable[x*nCols*4 + y*4 + dir] = value;
	}

	/**
	 * Assign value of a position in the table
	 * @param pos pos position in this level
	 * @param dir must be RIGHT/LEFT/UP/DOWN
	 * @return value of a position in the table
	 */
	inline void setPos(int pos, int dir, int value) const
	{
		dTable[pos*4 + dir] = value;
	}

	/**
	 * compute dijkstra and return value of every goal positions
	 * @param pLastPushPosition allocated pointer with numOfGoals positions
	 * that will be filled with positions of pusher when making last push
	 * of each goal
	 * @return tab of values of goals positions (level representation)
	 */
	int* resolveGoals(int* pLastPushPosition);

	/**
	 * compute dijkstra and return value of every positions
	 * @return tab of values of every positions
	 */
	int* resolvePositions(void);

	/**
	 * BE CAREFULL : use this function after resolveGoals or resolvePositions
	 * Get a list filled with pusher moves used to push box from first pos
	 * to "position".
	 * ex : "RurDDluRR"
	 * @param position Level position of  box destination
	 * @return list of all moves to push a box from start position to
	 * "position"
	 */
	char* listMovesToPosition(int position);

protected:
	/**
	 * recursive call of dijkstra
	 * @param posBox position of box in level
	 * @param dir dir must be RIGHT/LEFT/UP/DOWN
	 * @param value value we want to assign at this position
	 */
	void dijkstraRec(int posBox, int dir, int value);

	/**
	 * Debug function to print actual level state
	 */
	void print();

	/**
	 * BE CAREFULL : use this function after resolveGoals or resolvePositions
	 * Get a list filled with successive box positions used when moving box
	 * from start position to "position".
	 * @param position Level position of box destination
	 * @return list of all position of box to push it from start position to
	 * "position"
	 */
	int* listBoxPosToPosition(int position);

	/**
	 * BE CAREFULL : use this function after resolveGoals or resolvePositions
	 * From à list of Box Positions, get all pre-pusher positions.
	 * A pre-pusher position is a position of pusher just before he pushes
	 * a box
	 * @param list of box positions (use listBoxPosToPosition for that)
	 * @param listLength length of list
	 * @return list of all pusher pre-positions
	 */
	int* listPusherPrepos(int* list, int listLength);

	/**
	 * Add a move into a list of moves
	 * @param list of previous moves
	 * @param move move to add (l,r,u,d or L,R,U,D)
	 * @return new list of moves
	 */
	char* addMoveToList(char* list, char move);

	/**
	 * return minimal value
	 * @param a first value
	 * @param b second value
	 * @param c third value
	 * @param d fourth value
	 * @return minimal value
	 */
	int min(int a, int b, int c, int d);
};


#endif /*DIJKSTRABOX_*/
