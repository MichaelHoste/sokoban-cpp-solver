/*----------------------------------------------*/
/*================== ISokoban ==================*/
/*----------------------------------------------*/
/* Version : 1.0								*/
/* Author : Michaël Hoste						*/
/* Copyright : See LICENSE file for details   	*/
/*----------------------------------------------*/
/**
 * @brief Management of Lists made of strings
 *
 * Lists are ordred by alphabetical order and don't contain 2 same strings
 */
/*----------------------------------------------*/

#ifndef STRINGLIST_
#define STRINGLIST_

#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include "Base.h"
#include "Util.h"

class StringList
{
protected:
	Base* base; /**< Main class of the game */
	char** list; /**< Tab of strings. Final string is a "." */
	int length; /**< Length of the string list */

public:
	/**
	 * Constructor for a empty list
	 * @param base main class of the game
	 */
	StringList(Base* base);

	/**
	 * Constructor for a list based on a tab of strings
	 * @param base main class of the game
	 * @param list List of string. Last string of list must be "."
	 */
	StringList(Base* base, const char** list);

	/**
	 * Constructor for a list copied from another list
	 * @param base main class of the game
	 * @param list Another list of string
	 */
	StringList(Base* base, const StringList* list);

	/**
	 * Constructor for a list of files contained in a directory
	 * @param base main class of the game
	 * @param repertory repertory we want to list
	 */
	StringList(Base* base, const char* directory);

	/**
	 * Destructor
	 */
	~StringList();

	/* --------*/
	/* Getters */
	/* --------*/
	/** @Return Id of this level */
	inline char* const * const getList(void) const { return list; }
	/** @Return Name of this level */
	inline int getLength(void) const { return length; }

	/* --------*/
	/* Setters */
	/* --------*/


	/* -------*/
	/* Others */
	/* -------*/

	/**
	 * Add a element to the list. If this element already exist in the list,
	 * it will not be added
	 * @param string element to add to the list
	 */
	void addString(const char* string);

	/**
	 * Remove a element (string) from this list
	 * @param string element to remove from the list
	 */
	void removeString(const char* string);

	/**
	 * Get a element from the list
	 * @param id Number of the element to get (starting with 0)
	 * @return string number "id" from the list
	 */
	const char* getString(int id);

	/**
	 * Is this string present in this list ? Case is not used. So "hello" and
	 * "HeLlO" are the same
	 * @param string string to test
	 * @return true if present, false if not
	 */
	bool isPresent(const char* string) const;

	/**
	 * Print list on standard output
	 */
	void print(void) const;

	/**
	 * Get position of a string on the list
	 * @param string string we want to find position
	 * @return position of the string or -1 if not present
	 */
	int getStringId(const char* string) const;

protected:
	/**
	 * List every files in the repertory "rep"
	 * @param directory Repertory where we want to list files
	 * @return tab of string where last string is "."
	 */
	char** fileListing(const char* directory) const;

	/**
	 * Insert tab of string in this object
	 * @param list tab of string terminated with string "."
	 */
	void insertList(char const* const* list);

	/**
	 * Order this list of string alphabetically
	 */
	void orderList(void);
};

#endif /*STRINGLIST_*/
