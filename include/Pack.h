/*----------------------------------------------*/
/*================== ISokoban ==================*/
/*----------------------------------------------*/
/* Version : 1.0							    */
/* Author : Michaël Hoste					    */
/* Copyright : See LICENSE file for details     */
/*----------------------------------------------*/
/**
 * @brief Class used to store a (xml) pack of levels
 *
 * Scores (Paths) associed with these levels are loaded from an external
 * (xml too) file and are relative to loggued player (param to constructor)
 */
/*----------------------------------------------*/
/* TODO
 * Enregistrer les meilleures scores (world) à part
 */
/*----------------------------------------------*/

#ifndef PACK_H_
#define PACK_H_

#include <stdlib.h>
#include <string.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <sys/types.h>
#include <dirent.h>
#include "Base.h"
#include "Level.h"

class Pack
{
protected:
	Base* base; /**< Main class of the game */
	char fileName[33]; /**< Name of loaded file */
	char player[33]; /**< Name of player who load pack */
	char title[65]; /**< title of pack */
	char description[513]; /**< Description of pack */
	char copyright[65]; /**< Copyright of pack */
	char email[65]; /**< email of creator of pack */
	char url[129]; /**< url of pack */
	int maxRowsNumber; /**< maximum number of rows */
	int maxColsNumber; /**< maximum number of cols */
	Level** levelList; /**< List of n objects of "Level*" type */
	int n; /**< Number of levels in the pack */
	int nSuccess; /**< Number of levels successfully complete by loggued player */

public:
 	/**
 	 * Constructor for a empty pack
 	 * @param base main class of the game
 	 */
 	Pack(Base* base);

 	/**
 	 * Constructor for a pack from xml files. One for levels, one for
 	 * paths (solutions/high scores)
 	 * @param base main class of the game
 	 * @param xmlPackFile name of the xml file containing a pack of levels
 	 *                    situated in data/levels/
 	 * @param player name of loggued player (to know paths to load)
 	 */
 	Pack(Base* base, const char* xmlPackFile, const char* player);

 	/**
 	 * Destructor
 	 */
 	~Pack();

 	/* --------*/
 	/* Getters */
 	/* --------*/
	/** @Return name of loaded xml file */
	inline const char* getFileName(void) const { return fileName; }
	/** @Return title of the pack */
	inline const char* getTitle(void) const { return title; }
	/** @Return Description of pack */
	inline const char* getDescription(void) const { return description; }
	/** @Return Copyright of pack */
	inline const char* getCopyright(void) const { return copyright; }
	/** @Return Email of pack */
	inline const char* getEmail(void) const { return email; }
	/** @Return Url of pack */
	inline const char* getUrl(void) const { return url; }
	/** @Return max RowsNumber of all loaded levels */
	inline int getMaxRowsNumber(void) const { return maxRowsNumber; }
	/** @Return max ColsNumber of all loaded levels */
	inline int getMaxColsNumber(void) const { return maxColsNumber; }
	/** @Return tab of "n" Level pointers */
	inline Level* const * const getLevelList(void) const { return levelList; }
	/** @Return Number of levels in this pack */
	inline int getN(void) const { return n; }
	/** @Return Number of successfull levels in this pack */
	inline int getNSuccess(void) const { return nSuccess; }

 	/* --------*/
 	/* Setters */
 	/* --------*/

 	/* -------*/
 	/* Others */
 	/* -------*/
	/**
	 * Print pack of levels in text mode in a file.
	 * @param fileName name of the file we want to print in.
	 */
	void print(const char* fileName) const;

	/**
	 * Print pack of levels in standard output
	 */
	void print(void) const;

	/**
	 * Save every paths of every levels in data/profiles/"player"/"pack".sol
	 */
	void xmlSavePath(void) const;

protected:
	/**
	 * Open a XML File containing a pack of levels and initialise this object
	 * with levels in it
	 * @param xmlPackFile XML file (with extension) we want to load in path
	 * 					  data/levels/
	 * @param player Player we want to load score associated with these levels
	 */
	void xmlLoad(const char* xmlPackFile, const char* player);

	/**
	 * Create a tree based on a pack xml file and gets nodes related with
	 * levels
	 * @param xmlPackFile XML file (with extension) we want to load in path
	 * 					  data/levels/
	 * @return tree Tree opened (we return for suppression after work on nodes)
	 * @return list of level nodes (terminated with a NULL node) in xml file
	 */
	xmlNodePtr* xmlLoadLevels(xmlDocPtr* tree, const char* xmlPackFile);

	/**
	 * Create a tree based on a player xml file and gets nodes related with
	 * paths
	 * @param player Player we want to load score associated with these levels
	 * @param xmlPackFile XML file (with extension) we want to load in path
	 * 					  data/levels/
	 * @return tree Tree opened (we return for suppression after work on nodes)
	 * @return list of path nodes (terminated with a NULL node) in xml file
	 */
	xmlNodePtr* xmlLoadPath(xmlDocPtr* tree, const char* player,
							const char* xmlPackFile);

	/**
	 * Print pack of levels in text mode in a file or in the shell.
	 * @param fileName name of the file we want to print in. If NULL, then
	 *                 it will be print on screen (standard output)
	 */
	void printInFile(const char* fileName) const;
};

#endif /*PACK_H_*/
