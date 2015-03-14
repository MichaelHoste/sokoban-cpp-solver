/*----------------------------------------------*/
/*================== ISokoban ==================*/
/*----------------------------------------------*/
/* Version : 1.0								*/
/* Author : Michaël Hoste						*/
/* Copyright : See LICENSE file for details   	*/
/*----------------------------------------------*/
/**
 * @brief Binary representation of positions in a level.
 *
 * Can be used to represent boxes zone, pusher move zone, deadlocks zone or
 * goals zone.
 *
 * Zone is stocked in a unsigned integer tab (unsigned int = 2 or 4 bits
 * depending of the CPU). Each "usefull" cell of the level
 * (see zoneToLevelPos from solver) corresponds to 1 bit. If this bit is "1"
 * then there is a "box/possible move/deadlock/goal" on this position
 * depending of the wanted action.
 *
 * Boxes zone representation : 1 if there is a box in this zone position, 0 if
 * not
 * Goals zone representation : 1 if there is a goal in this zone position, 0 if
 * not
 * Deadlock zone representation : 1 if there is a deadlock when a box is in this
 * zone position, 0 if not
 * Pusher move zone representation : 1 if pusher can move on this position OR
 * if this is a box position just next a place where pusher can move, 0 if not
 *
 */
/*----------------------------------------------*/

#ifndef ZONE_H_
#define ZONE_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../Base.h"
#include "../Level.h"

class Zone
{
protected:
	unsigned int* zone; /**< binary representation of the zone */
	int length; /**< Number of positions in the zone */

public:
	static const int PROCESS_BOXES	  = 0;
	static const int PROCESS_PUSHER	  = 1;
	static const int PROCESS_DEADLOCK = 2;
	static const int PROCESS_GOAL 	  = 3;

	static const int USE_OR  = 4;
	static const int USE_AND = 5;
	static const int USE_XOR = 6;
	static const int USE_NOT = 7;

	/**
	 * Constructor for a zone copied from an existing zone
	 * @param zone zone we want to copy
	 */
	Zone(const Zone* zone);

	/**
	 * Constructor to create a zone from a opened file
	 * @param file handle to opened file
	 * @param numberOfPositions number of needed positions to store this zone
	 */
	Zone(FILE* file, int numberOfPositions);

	/**
	 * Constructor to create a zone with a certain number of free positions
	 * @param numberOfPositions number of needed positions to store this zone
	 */
	Zone(int numberOfPositions);

	/**
	 * Constructor for a zone based on a binary operation on others zones
	 * @param zone1 first zone in binary operation
	 * @param zone2 second zone in binary operation
	 * @param operation Kind of binary operation you want to make :
	 * Zone::USE_OR new zone will be the result of an OR operation between
	 * zone1 and zone2
	 * Zone::USE_AND new zone will be the result of an AND operation between
	 * zone1 and zone2
	 * Zone::USE_XOR new zone will be the result of an XOR operation between
	 * zone1 and zone2
	 * Zone::USE NOT new zone will be the result of an NOT operation apply on
	 * zone1. zone2 is not used in this case and can be NULL.
	 */
	Zone(const Zone* zone1, const Zone* zone2, int operation);

	/**
	 * Constructor for a zone based on a level state
	 * @param level original level we want to make zones from it
	 * @param levelToZonePos link between real positions and zone positions
	 * @param zoneToLevelPos link between zone positions and real positions
	 * in the level
	 * @param length Number of cells in zoneToLevelPos (number of bits needed)
	 * @param operation Kind of zone we want to make :
	 * Zone::PROCESS_BOXES if we want to save positions of boxes in the zone
	 * Zone::PROCESS_PUSHER if we want to save possibilities of move of the
	 * pusher
	 * Zone::PROCESS_DEADLOCK if we want to save every places where a deadlock
	 * appears if a box is on it.
	 * Zone::PROCESS_GOAL if we want to save every goal places
	 */
	Zone(const Level* level, const int* levelToZonePos,
			const int* zoneToLevelPos, const int length, const int operation);

	/**
	 * Constructor for pusher zone based on a boxes zone and a starting position
	 * @param boxes Boxes zone of the level we want to make pusher zone
	 * @param startPos Starting position of the pusher (level representation)
	 * @param levelToZonePos Link between real positions and zone positions
	 * @param colsNumber Number of cols in level
	 */
	Zone(const Zone* boxes, const int startPos,	const int* levelToZonePos,
			int colsNumber);

	/**
	 * Destructor
	 */
	~Zone();

	/* --------*/
	/* Getters */
	/* --------*/
	/** @Return Number of positions in the zone */
	inline int getLength(void) const { return length; }
	/** @Return Binary representation of the zone */
	inline const unsigned int* getZoneTab(void) const { return zone; }

	/* --------*/
	/* Setters */
	/* --------*/

	/* -------*/
	/* Others */
	/* -------*/
	/**
	 * Calculate number of cells usefull to represent this zone
	 */
	inline int getNumberCell(void) const
	{
		int cellSize = 8*sizeof(unsigned int);
		int rest = length%cellSize;
		if(rest > 0)
			return 1+length/cellSize;
		else
			return length/cellSize;
	}

	/**
	 * Print a zone in integer and binary mode
	 */
	void print(void) const;

	/**
	 * Print a zone in its level form
	 * @param level original level corresponding to the zone
	 * @param levelToZonePos Translation table between level and zone
	 */
	void print(const Level* level, const int* levelToZonePos) const;

	/**
	 * Read position "pos" of the zone and return 1 or 0 depending of the bit
	 * state at this position
	 * @param pos Position we want to read
	 * 0 <= pos < NumberCell()*sizeof(unsigned int)
	 * @return bit value or -1 if invalid pos
	 */
	inline int readPos(const int pos) const
	{
		int cellSize = (8*sizeof(unsigned int));

		// Cell of tab where the bit is (starting with 0)
		int bitCell = pos/cellSize;
		// Position of the bit in tab[cell] (starting with 0)
		int bitPos = pos%cellSize;

		if(bitCell < getNumberCell() && bitCell >= 0)
			return (this->zone[bitCell] >> (cellSize-1-bitPos)) & 1;
		else
		{
			printf("Zone :: readPos error (pos < 0 or pos > number of bits)\n");
			return -1;
		}
	}

	/**
	 * Set binary number '1' in the position "pos" of the zone
	 * @param pos Position where we want to set 1
	 * 0 <= pos < NumberCell()*sizeof(unsigned int)
	 */
	void write1ToPos(const int pos) const;

	/**
	 * Set binary number '0' in the position "pos" of the zone
	 * @param pos Position where we want to set 1
	 * 0 <= pos < NumberCell()*sizeof(unsigned int)
	 */
	void write0ToPos(const int pos) const;

	/**
	 * Test if this zone is egal to another zone
	 * @param otherZone Other zone we want to compare to this Zone object
	 * WARNING : 2 zones must have same lengths !
	 * @return true if zone is the same, false if not
	 */
	bool isEgal(const Zone* otherZone) const;

	/**
	 * Test if a zone is full of 0  : every bits (and so cells) are egal to 0
	 * @return true if zone is full of 0
	 */
	bool isFullOf0() const;

	/**
	 * Test if a zone is full of 1  : every bits are egal to 1
	 * @return true if zone is full of 1
	 */
	bool isFullOf1() const;

	/**
	 * Test if a zone is included in another zone
	 * @param anotherZone zone where this zone is included or not
	 * @return true if it's included, false if not
	 */
	bool isIncludedIn(const Zone* anotherZone) const;

	/**
	 * Return number of 1 in this zone
	 * @return number of 1 in this zone
	 */
	int getNumberOf1() const;

	/**
	 * Make a zone representation resulting of a binary operation OR between
	 * this zone and the other zone
	 * @param otherZone Zone to make binary operation with
	 * @return resulting zone (this zone that's actually modified)
	 */
	Zone* applyOrWith(Zone* otherZone);

	/**
	 * Make a zone representation resulting of a binary operation AND between
	 * this zone and the other zone
	 * @param otherZone Zone to make binary operation with
	 * @return resulting zone (this zone that's actually modified)
	 */
	Zone* applyAndWith(Zone* otherZone);

	/**
	 * Make a zone representation resulting of a binary operation XOR between
	 * this zone and the other zone
	 * @param otherZone Zone to make binary operation with
	 * @return resulting zone (this zone that's actually modified)
	 */
	Zone* applyXorWith(Zone* otherZone);

	/**
	 * Make a zone representation resulting of a binary operation XOR between
	 * this zone and the other zone
	 * @return resulting zone (this zone that's actually modified)
	 */
	Zone* applyNot();

	/**
	 * Make a zone representation resulting of a MINUS binary operation between
	 * this zone and the other zone
	 * @return resulting zone (this zone that's actually modified)
	 */
	Zone* applyMinusWith(Zone* otherZone);

	/**
	 * Save this zone directly into a file
	 */
	void saveInFile(FILE* file);

protected:
	/**
	 * Make a zone representation of boxes positions in a level
	 * @param level Level we want to make zone
	 * @param zoneToLevelPos Link between zone positions and real positions
	 * in the level
	 */
	void makeBoxesZone(const Level* level, const int* zoneToLevelPos);

	/**
	 * Make a zone representation of pusher possible moves in a level
	 * @param level Level we want to make zone
	 * @param levelToZonePos Link between level positions and zone positions
	 * @param zoneToLevelPos Link between zone positions and real positions
	 * in the level
	 */
	void makePusherZone(const Level* level, const int* levelToZonePos,
			const int* zoneToLevelPos);

	/**
	 * Recursively make a zone representation of pusher possible moves in a
	 * level
	 * @param boxes Zone representation of boxes in this level
	 * @param startPos Starting position (level representation) of the pusher
	 * @param levelToZonePos Link between level positions and zone positions
	 * @param colsNumber Number of cols in level
	 */
	void recursiveMakePusherZone(const Zone* boxes, const int startPos,
			const int* levelToZonePos, const int colsNumber);

	/**
	 * Make a zone representation of places where deadlocks appear if a box
	 * is on it
	 * @param level Level we want to make zone
	 * @param levelToZonePos Link between level positions and zone positions
	 * @param zoneToLevelPos Link between zone positions and real positions
	 * in the level
	 */
	void makeDeadlockZone(const Level* level, const int* levelToZonePos,
			const int* zoneToLevelPos);

	/**
	 * This function add corner deadlocks to deadlock zone. This happened when
	 * there is a box in a corner made by walls
	 * @param level Level we want to make zone
	 * @param zoneToLevelPos Link between zone positions and real positions
	 * in the level
	 */
	void makeCornerDeadlockZone(const Level* level, const int* zoneToLevelPos);

	/**
	 * This function add line deadlocks to deadlock zone. This happened when
	 * there is a box next a wall and no way to remove it (escape move)
	 * You must apply this function AFTER the use of function
	 * makeCornerDeadlockZone because we need corner positions.
	 * @param level Level we want to make zone
	 * @param levelToZonePos Link between level positions and zone positions
	 * @param zoneToLevelPos Link between zone positions and real positions
	 * in the level
	 */
	void makeLineDeadlockZone(const Level* level, const int* levelToZonePos,
			const int* zoneToLevelPos);

	/**
	 * Make a zone representation of goal places
	 * @param level Level we want to make zone
	 * @param zoneToLevelPos Link between zone positions and real positions
	 * in the level
	 */
	void makeGoalZone(const Level* level, const int* zoneToLevelPos);

	/**
	 * Make a zone representation resulting of a binary operation OR
	 * @param zone1 first zone in binary operation
	 * @param zone2 second zone in binary operation
	 */
	void makeORZone(const Zone* zone1, const Zone* zone2);

	/**
	 * Make a zone representation resulting of a binary operation AND
	 * @param zone1 first zone in binary operation
	 * @param zone2 second zone in binary operation
	 */
	void makeANDZone(const Zone* zone1, const Zone* zone2);

	/**
	 * Make a zone representation resulting of a binary operation XOR
	 * @param zone1 first zone in binary operation
	 * @param zone2 second zone in binary operation
	 */
	void makeXORZone(const Zone* zone1, const Zone* zone2);

	/**
	 * Make a zone representation resulting of a binary operation NOT
	 * @param zone1 first zone in binary operation
	 */
	void makeNOTZone(const Zone* zone1);

	/**
	 * Copy other zone in this object. Used by constructor.
	 * @param source zone to be copied in this object.
	 */
	void copyZone(const Zone* source);

	/**
	 * return true if position is in a corner of level (corner with 2 walls)
	 * @param level Level we want to test
	 * @param pos Position we want to test in the level
	 * @return true if pos is in a corner of level, return false if not
	 */
	bool isInCorner(const Level* level, const int pos) const;
};

#endif /*ZONE_H_*/
