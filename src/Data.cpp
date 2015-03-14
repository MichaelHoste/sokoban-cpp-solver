/*----------------------------------------------*/
/*================== ISokoban ==================*/
/*----------------------------------------------*/
/* Version : 1.0								*/
/* Author : Michaël Hoste						*/
/* Copyright : See LICENSE file for details   	*/
/*----------------------------------------------*/

#include "../include/Data.h"

/* ------------*/
/* Constructor */
/* ------------*/
Data::Data(Base* base, int argc, char * const * const argv)
{
	char* playerName=NULL;

	// Default initialisation
	packNameList = new StringList(base, "data/levels");
	packNameList->removeString("SokobanLev.dtd");
	playerNameList = new StringList(base, "data/profiles");
	playerNameList->removeString("SokobanPaths.dtd");

	this->language = (char*)malloc(3*sizeof(char));
	strcpy(this->language, "fr");

	this->screenMode = 'w';
	this->resolution = (char*)malloc(15*sizeof(char));
	strcpy(this->resolution, "640x480");
	this->width = 640;
	this->height = 480;
	this->sound = this->music = 1;

	// No parameter : start launcher
	if(argc == 1)
	{
#ifdef SWIN32
		system("start launcher.exe");
#else
		system("./launcher");
#endif
		exit(0);
	}
	// Pseudo
	if(argc >= 2)
	{
		playerName=(char*)malloc((strlen(argv[1])+1)*sizeof(char));
		strcpy(playerName, argv[1]);
	}
	// screenmode
	if(argc >= 3)
	{
		if(strcmp("-f", argv[2]) == 0)
		{
			screenMode='f';
		}
		else if(strcmp("-w", argv[2]) == 0)
		{
			screenMode='w';
		}
		else
		{
			printf("Data :: invalid screen mode parameter\n");
			exit(1);
		}
	}
	// Resolution
	if(argc >= 4)
	{
		char* temp = (char*)malloc(15*sizeof(char));

		strcpy(this->resolution, argv[3]);

		// Résolution en texte (fullscreen) et nombres (window)
		this->resolution = strcat(this->resolution, ":32");
		int i=0, j=0;
		while(this->resolution[i] != 'x' && this->resolution[i]!='\0')
		{
			temp[j]=this->resolution[i];
			temp[j+1]='\0';
			i++;
			j++;
		}

		if(this->resolution[i]=='\0')
		{
			printf("Data :: invalid screen size parameter\n");
			exit(1);
		}

		this->width = atoi(temp);
		i++;
		j=0;
		while(this->resolution[i] != '\0')
		{
			temp[j]=this->resolution[i];
			temp[j+1]='\0';
			i++;
			j++;
		}
		this->height = atoi(temp);

		free(temp);
	}
	// language ("fr", "en", ...)
	if(argc >= 5)
	{
		if(strlen(argv[4]) <= 2)
			strcpy(this->language, argv[4]);
		else
		{
			printf("Data :: invalid language parameter\n");
			exit(1);
		}
	}
	// sound and music
	if(argc == 7)
	{
		sound = atoi(argv[5]);
		music = atoi(argv[6]);
	}
	// Too much parameters
	if(argc > 7)
	{
		printf("Data :: too much parameters.\n");
		exit(1);
	}

	// Default selected pack = Original.slc but if it doesn't exist, then we
	// use the first founded level pack
	selectedPackNameId = packNameList->getStringId("Original.slc");
	if(selectedPackNameId == -1)
		selectedPackNameId = 0;

	selectedPlayerNameId = playerNameList->getStringId(playerName);
	if(selectedPlayerNameId == -1)
	{
		printf("Data :: invalid player name\n");
		exit(1);
	}
	free(playerName);

}

/* -----------*/
/* Destructor */
/* -----------*/
Data::~Data()
{
	if(packNameList)
		delete packNameList;
	if(playerNameList)
		delete playerNameList;
	if(resolution)
		free(resolution);
	if(language)
		free(language);
}

/* -------*/
/* Others */
/* -------*/
