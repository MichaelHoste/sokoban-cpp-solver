/*----------------------------------------------*/
/*================== ISokoban ==================*/
/*----------------------------------------------*/
/* Version : 1.0								*/
/* Author : Michaël Hoste						*/
/* Copyright : See LICENSE file for details   	*/
/*----------------------------------------------*/

#include "../include/Base.h"

#include "../include/Level.h"
#include "../include/Pack.h"
#include "../include/Display.h"
#include "../include/Data.h"

/* ------------*/
/* Constructor */
/* ------------*/
Base::Base(int argc, char * const * const argv)
{
	data = new Data(this, argc, argv);

	pack = new Pack(this);
	level = new Level(this);
//	timer = new Timer();
	displayGame = new Display();
	displayMenu = new Display();
	actualDisplay = new Display();
}

/* -----------*/
/* Destructor */
/* -----------*/
Base::~Base()
{
	if(pack)
		delete pack;
	if(level)
		delete level;
//	if(timer)
//		delete timer;
	if(data)
		delete data;
	if(displayGame)
		delete displayGame;
	if(displayMenu)
		delete displayMenu;
	if(actualDisplay)
		delete actualDisplay;
}
