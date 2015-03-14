/*----------------------------------------------*/
/*================== ISokoban ==================*/
/*----------------------------------------------*/
/* Version : 1.0								*/
/* Author : Michaël Hoste						*/
/* Copyright : See LICENSE file for details   	*/
/*----------------------------------------------*/
/**
 * @brief Utility class only with static method
 */
/*----------------------------------------------*/

#ifndef UTIL_H_
#define UTIL_H_

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>

class Util
{
protected:

public:
	/* -------*/
	/* Others */
	/* -------*/
	/**
	 * Fonction used to cut and clean text and make it ready to draw
	 * @param text text to be cut
	 * @param length max length of the new text
	 * @return text cut to (max) length chars and cleaned
	 */
	static char* cutText(const char* text, const int length);

	/**
	 * Capitalize one char
	 * @param letter char to capitalize
	 * @return capitalized char
	 */
	static char toUpper(const char letter);

	/**
	 * Capitalize one char
	 * @param letter char to UnCapitalize
	 * @return uncapitalized char
	 */
	static char toLower(const char letter);

	/**
	 * Uncapitalize one string
	 * @param string string to UnCapitalize
	 * @return uncapitalized string
	 */
	static char* stringToLower(const char* string);

	/**
	 * Number of decimals in this number
	 * @param number number to search in
	 * @return number of of decimals
	 */
	static int numberLength(const int number);

	/**
	 * Take the Nst decimal in the number
	 * @param number number to search in
	 * @param n number of decimal to return
	 * @return caracter of the Nst decimal of number
	 */
	static char nDecimal(const int number, const int n);

	/**
	 * Is it a decimal (between 0 and 9)
	 * @param a caracter to test
	 * @return true if "a" is a decimal. false if not.
	 */
	static bool isDecimal(const char a);

	/**
	 * Generate a one line fileName from a list of repertory
	 * Ex : if reps is "file"|"name"|"michael.pdf", then a string with
	 * "file/name/michael.pdf" will be created.
	 * @param rep repertories to test. Ends with NUL
	 */
	static char* generateRep(char** reps);

	/**
	 * Create a local repertory made by a list of strings.
	 * Ex : if reps is "file"|"name"|"michael", then a repertory
	 * "file/name/michael/" will be created
	 * @param rep repertories to test. Ends with NULL
	 */
	static void createRep(char** reps);

	/**
	 * Is this file exists ?
	 * @param file file to test
	 * @return true if it exists, false if not
	 */
	static bool isThisFileExists(char* file);

	/**
	 * Is this repertory exists ?
	 * @param rep repertory to test
	 * @return true if it exists, false if not
	 */
	static bool isThisRepExists(char* rep);

};

#endif /*UTIL_H_*/
