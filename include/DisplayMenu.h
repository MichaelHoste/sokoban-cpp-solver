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

#ifndef DISPLAYMENU_H_
#define DISPLAYMENU_H_

#include <stdio.h>
#include <stdlib.h>
#include "Base.h"

class DisplayMenu
{
protected:
	Base* base; /**< Main class of the game */

public:
	/**
	 * Constructor
	 */
	DisplayMenu(Base* base);

	/**
	 * Destructor
	 */
	~DisplayMenu();

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

#endif /*DISPLAYMENU_H_*/
