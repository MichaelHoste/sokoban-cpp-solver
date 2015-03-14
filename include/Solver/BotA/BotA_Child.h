/*----------------------------------------------*/
/*================== ISokoban ==================*/
/*----------------------------------------------*/
/* Version : 1.0								*/
/* Author : Michaël Hoste						*/
/* Copyright : See LICENSE file for details   	*/
/*----------------------------------------------*/
/**
 * @brief More a usefull structure than a class
 */
/*----------------------------------------------*/

#ifndef BOTA_CHILD_H_
#define BOTA_CHILD_H_

class Node;

class Child
{
public :
	Node* child;
	int childCost;

	Child(Node* child, int childCost);
	~Child();
};

#endif /*BOTA_CHILD_H_*/
