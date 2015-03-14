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

#ifndef DISPLAY_H_
#define DISPLAY_H_

#include <stdio.h>
#include <stdlib.h>
#include "Base.h"

class Display
{
protected:
	Base* base; /**< Main class of the game */

public:
	/**
	 * Constructor
	 */
	Display();

	/**
	 * Destructor
	 */
	~Display();

	/* --------*/
	/* Getters */
	/* --------*/

	/* --------*/
	/* Setters */
	/* --------*/


	/* -------*/
	/* Others */
	/* -------*/

};

#endif /*DISPLAY_H_*/
