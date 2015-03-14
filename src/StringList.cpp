/*----------------------------------------------*/
/*================== ISokoban ==================*/
/*----------------------------------------------*/
/* Version : 1.0								*/
/* Author : Michaël Hoste						*/
/* Copyright : See LICENSE file for details   	*/
/*----------------------------------------------*/

#include "../include/StringList.h"

/* ------------*/
/* Constructor */
/* ------------*/
StringList::StringList(Base* base):
	base(base),
	length(0)
{
	this->list = (char**)malloc(sizeof(char*));
	this->list[0]=(char*)malloc(2*sizeof(char));

	this->list[0][0]='.';
	this->list[0][1]='\0';
}

StringList::StringList(Base* base, const char** list):
	base(base),
	length(0)
{
	insertList(list);
}

StringList::StringList(Base* base, const StringList* list):
	base(base)
{
	insertList(list->getList());
}

StringList::StringList(Base* base, const char* directory):
	base(base)
{
	int i;
	char ** fileList = fileListing(directory);
	insertList(fileList);

	for(i=0;strcmp(fileList[i], ".")!=0;i++)
	{
		free(fileList[i]);
	}

	free(fileList[i]);
	free(fileList);
}

/* -----------*/
/* Destructor */
/* -----------*/
StringList::~StringList()
{
	for(int i=0;i<=this->length;i++)
		free(this->list[i]);
	free(this->list);
}

/* -------*/
/* Others */
/* -------*/
void StringList::addString(const char* string)
{
	if(!isPresent(string))
	{
		this->length++;
		// Ajout d'une zone de texte à la fin
		this->list = (char**)realloc(this->list, (length+1)*sizeof(char*));
		// Réallocation du "." en une chaine de la taille de string
		this->list[length-1] =
			(char*)realloc(this->list[length-1],(strlen(string)+1)*sizeof(char));
		// On copie la chaine dans son emplacement
		strcpy(this->list[length-1], string);
		// On alloue la place pour le "."
		this->list[length] = (char*)malloc(2*sizeof(char));
		// On place le "."
		strcpy(this->list[length], ".");
		orderList();
	}
}

void StringList::removeString(const char* string)
{
	int i;
	int j=0;
	int value;
	bool founded = false;
	char * stringtemp = Util::stringToLower(string);
	char * stringtemp2;

	for(i=0;i<length;i++)
	{
		stringtemp2 = Util::stringToLower(list[i]);
		value = strcmp(stringtemp, stringtemp2);
		free(stringtemp2);

		// We exceed the position
		if(value < 0)
		{
			break;
		}
		// String match
		else if(value==0)
		{
			founded = true;
			this->length--;
			j=i;
			break;
		}
	}

	free(stringtemp);

	// move every elements on the left a step to the right
	if(founded)
	{
		free(this->list[j]);
		list[j] = list[j+1];
		for(i=j+1;strcmp(list[i],".")!=0;i++)
		{
			list[i] = list[i+1];
		}
		this->list = (char**)realloc(this->list, i*sizeof(char*));
	}
}

const char* StringList::getString(int id)
{
	return list[id];
}

bool StringList::isPresent(const char* string) const
{
	int i;
	int value;
	char* stringtemp = Util::stringToLower(string);
	char* stringtemp2;

	for(i=0;i<length;i++)
	{
		stringtemp2 = Util::stringToLower(list[i]);
		value = strcmp(stringtemp, stringtemp2);
		free(stringtemp2);

		// We exceed the position
		if(value < 0)
		{
			free(stringtemp);
			return false;
		}
		// String match
		else if(value==0)
		{
			free(stringtemp);
			return true;
		}
	}

	free(stringtemp);
	return false;
}

void StringList::print(void) const
{
	for(int i=0;i<this->length;i++)
	{
		printf("%s\n", list[i]);
	}
}

int StringList::getStringId(const char* string) const
{
	if(string == NULL)
		return -1;

	char* stringtemp = Util::stringToLower(string);
	char* stringtemp2;

	int value;
	for(int i=0;i<this->length;i++)
	{
		stringtemp2 = Util::stringToLower(list[i]);
		value = strcmp(stringtemp, stringtemp2);
		free(stringtemp2);
		if(value < 0)
		{
			free(stringtemp);
			return -1;
		}
		else if(value == 0)
		{
			free(stringtemp);
			return i;
		}
	}
	free(stringtemp);
	return -1;
}

char** StringList::fileListing(const char* directory) const
{
	struct dirent *lecture;
	DIR *rep;
	char ** liste_fichiers_xml;

	int compteur=1;

	liste_fichiers_xml=(char**)malloc(sizeof(char*));

	rep = opendir(directory);

	while ((lecture = readdir(rep)))
	{
		// We do not keep hidden files
		if(lecture->d_name[0] != '.')
		{
			liste_fichiers_xml=(char**)realloc(liste_fichiers_xml,
											   compteur*sizeof(char*));
			int taille=strlen(lecture->d_name)+1;
			liste_fichiers_xml[compteur-1]=(char*)malloc(taille*sizeof(char));
			strcpy(liste_fichiers_xml[compteur-1], lecture->d_name);
			compteur++;
		}
	}
	closedir(rep);

	liste_fichiers_xml=(char**)realloc(liste_fichiers_xml, compteur*sizeof(char*));
	liste_fichiers_xml[compteur-1]=(char*)malloc(2*sizeof(char));

	strcpy(liste_fichiers_xml[compteur-1], ".");

	return liste_fichiers_xml;
}

void StringList::insertList(char const* const* list)
{
	int i;
	this->list = (char**)malloc(sizeof(char*));

	for(i=0;strcmp(list[i], ".")!=0;i++)
	{
		this->list=(char**)realloc(this->list, (i+1)*sizeof(char*));
		this->list[i]=(char*)malloc((strlen(list[i])+1)*sizeof(char));
		strcpy(this->list[i], list[i]);
	}

	this->list=(char**)realloc(this->list, (i+1)*sizeof(char*));
	this->list[i]=(char*)malloc(2*sizeof(char));

	this->list[i][0]='.';
	this->list[i][1]='\0';

	this->length = i;

	orderList();
}

void StringList::orderList(void)
{
	int i=0;
	char* temp;
	int echange=1;
	char *temp2, *temp3;

	// If list is empty or with only 1 string, we do nothing
	if(strcmp(list[0], ".") != 0  && strcmp(list[1], ".") != 0)
	{
		while(echange == 1)
		{
			echange = 0;
			while(strcmp(list[i+1], ".") != 0) // Fin de la liste
			{
				temp2 = Util::stringToLower(list[i]);
				temp3 = Util::stringToLower(list[i+1]);
				if(strcmp(temp2, temp3) > 0)
				{
					temp=list[i];
					list[i]=list[i+1];
					list[i+1]=temp;
					echange = 1;
				}
				i++;
				free(temp2);
				free(temp3);
			}
			i=0;
		}
	}
}

