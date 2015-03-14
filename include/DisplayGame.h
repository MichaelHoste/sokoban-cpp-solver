/*----------------------------------------------*/
/*================== ISokoban ==================*/
/*----------------------------------------------*/
/* Version : 1.0								*/
/* Author : Michaël Hoste						*/
/* Copyright : See LICENSE file for details   	*/
/*----------------------------------------------*/
/**
 * @brief
 */
/*----------------------------------------------*/

#ifndef DISPLAYGAME_H_
#define DISPLAYGAME_H_

#include <stdio.h>
#include <stdlib.h>
#include "Base.h"

class DisplayGame
{
protected:
	Base* base; /**< Main class of the game */

public:
	/**
	 * Constructor
	 */
	DisplayGame(Base* base);

	/**
	 * Destructor
	 */
	~DisplayGame();

	/* --------*/
	/* Getters */
	/* --------*/

	/* --------*/
	/* Setters */
	/* --------*/


	/* -------*/
	/* Others */
	/* -------*/

protected:

};

#endif /*DISPLAYGAME_H_*/
