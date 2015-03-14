/*----------------------------------------------*/
/*================== ISokoban ==================*/
/*----------------------------------------------*/
/* Version : 1.0								*/
/* Author : Michaël Hoste						*/
/* Copyright : See LICENSE file for details   	*/
/*----------------------------------------------*/
/**
 * @brief Datas relative to game context
 */
/*----------------------------------------------*/

#ifndef DATA_H_
#define DATA_H_

#include <stdlib.h>
#include <string.h>
#include "Base.h"
#include "StringList.h"

class Data
{
protected:
	Base* base; /**< Main class of the game */
	StringList* packNameList; /**< List of all available packs */
	StringList* playerNameList; /**< List of all available players */
	int selectedPackNameId; /**< Id of selected pack on the list */
	int selectedPlayerNameId; /**< Id of selected player on the list */
	char screenMode; /**< f for fullscreen mode, w for windowed mode */
	int width; /**< Width of window */
	int height; /**< Height of window */
	char* resolution; /**< Resolution on this shape : "widthxheight" */
	char* language; /**< language of the game "fr", "en", ... */
	bool sound; /**< True if you want sound, false if you don't */
	bool music; /**< True if you want music, false if you don't */

public:
	/**
	 * Constructor for game datas.
	 * @param base main class of the game
	 * @param argc number of arguments
	 * @param argv param starting params
	 * These are like this :
	 * ./sokoban "pseudo" -t "widthxheight" "fr" sound music
	 * t : w for windowed mode, f for fullscreen mode
	 * sound : 1=on 0=false
	 * music : 1=on 0=false
	 * example : ./sokoban "michael" -f "800x600" "fr" 1 1
	 */
	Data(Base* base, int argc, char * const * const argv);

	/**
	 * Destructor
	 */
	~Data();

	/* --------*/
	/* Getters */
	/* --------*/

	/** @Return list of pack names */
	inline StringList* getPackNameList(void) const { return packNameList; }
	/** @Return list of player names */
	inline StringList* getPlayerNameList(void) const { return playerNameList; }
	/** @Return id of selected pack in the list */
	inline int getSelectedPackNameId(void) const { return selectedPackNameId; }
	/** @Return id of selected player in the list */
	inline int getSelectedPlayerNameId(void) const { return selectedPlayerNameId; }
	/** @Return Screen mode (windowed : w, fullscreen : f) */
	inline char getScreenMode(void) const { return screenMode; }
	/** @Return width of screen */
	inline int getWidth(void) const { return width; }
	/** @Return height of screen */
	inline int getHeight(void) const { return height; }
	/** @Return resolution of type "widthxheight" */
	inline const char* getResolution(void) const { return resolution; }
	/** @Return language ("fr", "en",..) */
	inline const char* getLanguage(void) const { return language; }
	/** @Return true if sound is on, false if sound is off */
	inline bool getSound(void) const { return sound; }
	/** @Return true if music is on, false if music is off */
	inline bool getMusic(void) const { return music; }

	/* --------*/
	/* Setters */
	/* --------*/

	/* -------*/
	/* Others */
	/* -------*/

};

#endif /*DATA_H_*/
