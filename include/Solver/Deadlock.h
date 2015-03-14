/*----------------------------------------------*/
/*================== ISokoban ==================*/
/*----------------------------------------------*/
/* Version : 1.0								*/
/* Author : Michaël Hoste						*/
/* Copyright : See LICENSE file for details   	*/
/*----------------------------------------------*/
/**
 * @brief Management of all kinds of deadlock is centalized in this class
 */
/*----------------------------------------------*/

#ifndef DEADLOCK_H_
#define DEADLOCK_H_

#include <stdio.h>
#include <stdlib.h>
#include "Solver.h"
#include "Node.h"
#include "TreeNode.h"

class BotGoodPushesS;

class ZoneList
{
public:
	Zone* zone;
	ZoneList* next;
	ZoneList(Zone* zone, ZoneList* next):
		zone(zone),
		next(next)
	{}
	~ZoneList()
	{
		delete zone;
		if(next)
			delete next;
	}
};

class Deadlock
{
protected:
	Base* base; /**< Main class of the game */
	Solver* solver; /**< deadlock object belang to this solver */
	Zone* deadlockZone; /**< Zone representation of all single positions of deadlock */
	Node** deadlockNodeList; /**< List of node representations of all multiple positions of deadlock */
	int deadlockNodeListLength; /**< number of nodes in deadlockNodeList */
	int* deadlockTestTab; /**< Tab of current boxes positions in deadlock test */
	int deadlockTestTabLength; /**< Length of deadlockTestTab */
	Level* emptyLevel; /**< Level with no boxes or pusher */

	const Level* level; /**< Initial level we want to solve */
	const Zone* goalZone; /**< Zone representation of all goals in this level */
	const int* zoneToLevelPos; /**< translation table between new positions in zone and old positions in level */
	const int zoneToLevelPosLength; /**< Length of translation table */
	const int* levelToZonePos; /**< translation table between old positions in level and new positions in zone */


public:
	/** Maximum junctions of simple zones to assemblate before testing them
	 * (-1 = +inf) : zone made by 2 junctions, 3 junctions, ... */
	static const int DEADLOCKEDZONE_MAXLEVELOFJUNCTIONS = -1;
	/** Maximum number of zones to create by assembling junctions of zones) */
	static const int DEADLOCKEDZONE_MAXZONESTOCREATE = 10000;
	/** number of recursive iterations used to test a blocked zone */
	static const int BLOCKEDZONE_DEEPNESS = 10;

	/**
	 * Constructor
	 */
	Deadlock(Solver* solver);

	/**
	 * Destructor
	 */
	~Deadlock();

	/* --------*/
	/* Getters */
	/* --------*/
	/** @Return Assigned solver */
	inline Solver* getSolver(void) const { return solver; }
	/** @Return Deadlock Zone */
	inline Zone* getDeadlockZone(void) const { return deadlockZone; }
	/** @Return list of dedlocked nodes */
	inline Node** getDeadlockNodeList(void) const { return deadlockNodeList; }
	/** @Return level with no boxes or pusher */
	inline Level* getEmptyLevel(void) const { return emptyLevel; }

	/* --------*/
	/* Setters */
	/* --------*/


	/* -------*/
	/* Others */
	/* -------*/

	/**
	 * Test if a TreeNode is a deadlocked node
	 * @param treenode treenode to be tested
	 * @return true if this node is a deadlock, false if not
	 */
	bool deadlockedNode(TreeNode* treenode) const;

	/**
	 * Test if last move made a square deadlock or a Z deadlock
	 * square deadlock :
	 * ######
	 * #    #
	 * # O# #
	 * # OO #
	 * #    #
	 * ######
	 * Z deadlock :
	 * #####   #####   ######   ######
	 * #   #   #   #   # #  #   #  # #
	 * ##O #   # O##   # OO #   # OO #
	 * # O##   ##O #   #  # #   # #  #
	 * #   #   #   #   ######   ######
	 * #####   #####
	 * @param treenode treenode to be tested
	 * @param boxPos level representation of box position we want to test. If -2
	 * we'll test last moved box
	 * @return true if this node is a deadlock, false if not
	 */
	bool deadlockedLastMove(const TreeNode* treenode, int posBox=-2) const;

	/**
	 * Test if a box is on a deadlock cell (corner or deadlock lines)
	 * @param treenode treenode to be tested
	 * @return true if this node is a deadlock, false if not
	 */
	bool deadlockedCorner(const TreeNode* treenode) const;

	/**
	 * Test if any deadlocked zone exists. A deadlocked zone is a zone with
	 * boxes or walls all around and, every box you push will create a deadlock.
	 * @param treenode treenode to be tested
	 * @param maxN Maximum number of zones to join to make the high level zone
	 * to test. If maxN = -1, then we'll use the maximum number of junctions we
	 * could
	 * @return true if this node is a deadlock, false if not
	 */
	bool deadlockedZone(TreeNode* treenode, int maxN) const;

protected:
	/**
	 * Test if there is a simple deadlock on the treenode.
	 * A simple deadlock is a deadlock without recursivity
	 * @param treenode treenode to be tested
	 * @return true if this node is a deadlock, false if not
	 */
	bool simpleDeadlock(TreeNode* treenode) const;

	/**
	 * Test if there is a complex deadlock on the treenode.
	 * A complex deadlock is a deadlock with recursivity
	 * @param treenode treenode to be tested
	 * @return true if this node is a deadlock, false if not
	 */
	bool complexDeadlock(TreeNode* treenode) const;

	/**
	 * Create a tab containing list of all single empty zones except empty
	 * zones with pusher position
	 * @param boxesZone boxes zone of current treeNode
	 * @param pusherZone pusher zone of current treeNode
	 * @return tab with list of empty zones except empty zones with pusher
	 * position (last position of tab is NULL)
	 */
	Zone** createZone1Tab(Zone* boxesZone, Zone* pusherZone) const;

	/**
	 * Create list of empty neighbours cells of a position
	 * [right, left, down, up, down-right, down-left, up-right, up-left]
	 * @param boxesZone boxes positions in this state of level
	 * @param pos position we want to get neighbours
	 * @return tab of 8 cells, -1 when position's a wall or a box, number of
	 * cell when it's a free cell
	 */
	int* createNeighboursTab(Zone* boxesZone, int pos) const;

	/**
	 * Create a chained list with every usefull zone of size 1 and put it on the
	 * first cell of received tab of zone lists
	 * @param neiTab list of empty positions
	 * @param boxesZone list of boxes position of this state
	 * @param pusherZone list of pusher position of this state
	 * @param zoneListTab Tab of chained list of zones (now empty because we
	 * want to fill the first cell here (the one with 1 size zone))
	 * @return modified zoneListTab with first cell of tab filled with a chained
	 * list full of zone of size 1
	 */
	ZoneList** createInitialZoneListTab(int* neiTab, Zone* boxesZone,
			Zone* pusherZone, ZoneList** zoneListTab) const;

	/**
	 * Create modified zoneListTab with "size" number of cell of tab filled
	 * with a chained list full of zones of "size" number of junctions
	 * @param zoneListTab Tab of chained list of zones (size-1 first cells must
	 * be filled to use this function)
	 * @param zoneTab list of every empty zones except the ones with pusher
	 * @param size size of junctions of zones we want to create (must be >=2)
	 * @param maxZoneToTest total number of zones to create. If number reached,
	 * we stop to create new zones because it will be excessive
	 * @return modified zoneListTab with "size" number of cell of tab filled
	 * with a chained list full of zones of "size" number of junctions
	 */
	ZoneList** createIZoneListTab(ZoneList** zoneListTab, Zone** zoneTab,
			int size, int* maxZoneToTest) const;

	/**
	 * Test if a zone is blocked. A zone is blocked when pusher is not on the
	 * zone, zone doesn't have any goal and all boxes of the zone are only
	 * pushable TO the zone AND drive to a deadlock.
	 * @param zone zone to test
	 * @param treenode treenode where the zone is
	 * @return 0 if zone is not blocked, 1 if zone is blocked and lead to a
	 * state where no move is possible at all,  2 if every possible moves are
	 * deadlocked (blocked)
	 */
	bool isBlockedZone(const Zone* zone, TreeNode* treenode, int n) const;

	/**
	 * Test if a box is limited to a zone. Limited to a zone means that box
	 * can only move from or to a box position which is in the zone.
	 * @param boxPos box position (level representation)
	 * @param treenode current analyzed treenode
	 * @param testZone current zone we want to test
	 * @return true if this box is limited, false if not
	 */
	bool isZoneLimitedBox(int boxPos, TreeNode* treenode,
			const Zone* testZone) const;

	/**
	 * A box is zone blocked if we can only push this box in the zone from a
	 * position outside the zone. AND if this box is deadlocked in its new
	 * position
	 * @param boxPos box position (level representation)
	 * @param treenode current analyzed treenode
	 * @param testZone current zone we want to test
	 * @return 0 if box is not blocked, 1 if no possible move at all (blocked),
	 * 2 if every possible moves are deadlocked (blocked)
	 */
	bool isZoneBlockedBox(int boxPos, TreeNode* treenode,
			const Zone* testZone, int n) const;
};

#endif /*DEADLOCK_H_*/
