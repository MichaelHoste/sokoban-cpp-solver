/*----------------------------------------------*/
/*================== ISokoban ==================*/
/*----------------------------------------------*/
/* Version : 1.0								*/
/* Author : Michaël Hoste						*/
/* Copyright : See LICENSE file for details   	*/
/*----------------------------------------------*/
/**
 * @brief Main class used to access usefull items
 */
/*----------------------------------------------*/

#ifndef BASE_H_
#define BASE_H_

#include <stdio.h>
#include <stdlib.h>

class Pack;
class Level;
class Display;
class Data;

class Base
{
protected:
	Pack* pack; /**< Active levels pack */
	Level* level; /**< Active level */
//	Timer* timer; /**< To manage real time */
	Data* data; /**< Data relative to game context */
	Display* displayGame; /**< To draw the game */
	Display* displayMenu; /**< To draw the menu */
	Display* actualDisplay; /**< Actual drawed display */

public:
	/**
	 * Constructor
	 */
	Base(int argc, char * const * const argv);

	/**
	 * Destructor
	 */
	~Base();

	/* --------*/
	/* Getters */
	/* --------*/
	/** @Return pointer to active pack of levels */
	inline Pack* getPack(void) const { return pack; }
	/** @Return pointer to active level */
	inline Level* getLevel(void) const { return level; }
	/** @Return pointer to timer */
//	inline Timer* getTimer(void) const { return timer; }
	/** @Return pointer to data */
	inline Data* getData(void) const { return data; }
	/** @Return pointer to display of game */
	inline Display* getDisplayGame(void) const { return displayGame; }
	/** @Return pointer to display of menu */
	inline Display* getDisplayMenu(void) const { return displayMenu; }
	/** @Return pointer to actual display */
	inline Display* getActualDisplay(void) const { return actualDisplay; }

	/* --------*/
	/* Setters */
	/* --------*/
//	/** @param pack new active pack to assign in the Base */
//	inline void setPack(Pack* pack) { this->pack = pack; }
//	/** @param level new active level to assign in the Base */
//	inline void setLevel(Level* level) { this->level = level; }
//	/** @param timer new timer to assign in the Base */
//	inline void setTimer(Timer* timer) { this->timer = timer; }
//	/** @param display new display of game to assign in the Base */
//	inline void setDisplayGame(Display* display) { this->displayGame = display; }
//	/** @param display new display of menu to assign in the Base */
//	inline void setDisplayMenu(Display* display) { this->displayMenu = display; }
//	/** @param display new actual display to assign in the Base */
//	inline void setActualDisplay(Display* display) { this->actualDisplay = display; }

};

#endif  /*BASE_H_*/
