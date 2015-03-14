/*----------------------------------------------*/
/*================== ISokoban ==================*/
/*----------------------------------------------*/
/* Version : 1.0								*/
/* Author : Michaël Hoste						*/
/* Copyright : See LICENSE file for details   	*/
/*----------------------------------------------*/
/**
 * @brief Class usefull to represent a way to move in a level.
 *
 * Pushes are represented by upper case and moves by lower case
 * In files, this is writed in COMPRESSED mode, but in this class, it's
 * UNCOMPRESSED for rapidity and flexibility.
 * In compressed mode, when many moves in one direction, letters are prefixed
 * by numbers.
 *
 * example uncompressed mode : rrrllUUUUUruLLLdlluRRRRdr
 *
 * example compressed mode : 3r2l5Uru3Ld2lu4Rdr
 */
/*----------------------------------------------*/

#ifndef PATH_H_
#define PATH_H_

#include <stdlib.h>
#include <string.h>
#include "Base.h"
#include "Util.h"

class Path
{
protected:
	Base* base; /**< Main class of the game */
	int nPushes; /**< Number of pushes */
	int nMoves; /**< Number of moves */
	char* moves; /**< UNCOMPRESSED List of moves */

public:
	static const int COMPRESSED = 0;
	static const int UNCOMPRESSED = 1;

	/**
	 * Constructor for a empty path
	 * @param base main class of the game
	 */
	Path(Base* base);

	/**
	 * Constructor with use of a compressed or uncompressed path to start
	 * @param base main class of the game
	 * @param path Compressed or uncompressed path we use to create this object.
	 * 		  	   It's copied and not directly used.
	 * @param mode Path::COMPRESSED or Path::UNCOMPRESSED.
	 * 			   It depends of "path" style.
	 */
	Path(Base* base, const char* path, const int mode);

	/**
	 * Constructor to copy another path in this object
	 * @param base main class of the game
	 * @param source Another path to be copied in this object.
	 * 		  	     It's copied and not directly used.
	 */
	Path(Base* base, const Path* source);

	/**
	 * Destructor
	 */
	~Path();

	/* --------*/
	/* Getters */
	/* --------*/
	/** @Return number of pushes */
	inline int getNPushes(void) const { return nPushes; }
	/** @Return number of moves */
	inline int getNMoves(void) const { return nMoves; }
	/** @Return list of moves */
	inline const char* getMoves(void) const { return moves; }
	/**
	 * @Return direction of last move (l,r,u,d,L,R,U,D)
	 *          or 'E' if last move does'nt exist
	 */
	inline char getLastMove(void) const
	{
		if(nMoves>0) return moves[nMoves-1];
		else return 'E';
	}

	/* --------*/
	/* Setters */
	/* --------*/


	/* -------*/
	/* Others */
	/* -------*/
	/**
	 * Add move in a direction
	 * @param direction direction where the pusher move
	 */
	void addMove(const char direction);

	/**
	 * Add push in a direction
	 * @param direction direction where pusher push a box
	 */
	void addPush(const char direction);

	/**
	 * Add displacement in a direction.
	 * push or move is automatically assigned following of the case of direction
	 * @param direction direction where pusher push a box
	 */
	void addDisplacement(const char direction);

	/**
	 * Delete last move
	 */
	void deleteLastMove(void);

	/**
	 * Print path of this object
	 */
	void print(void) const;

	/**
	 * Compress a path (see description of Path class)
	 * @param path uncompressed path
	 * @return compressed path
	 */
	static char* compressPath(const char * path);

	/**
	 * Uncompress a path (see description of Path class)
	 * @param path compressed path
	 * @return uncompressed path
	 */
	static char* uncompressPath(const char * path);

protected:
	/**
	 * Is a direction valid (u,d,r,l,U,D,R,L)
	 * @param direction direction to test
	 * @return true if valid, false if not
	 */
	static bool isValidDirection(const char direction);
};

#endif /*PATH_H_*/
