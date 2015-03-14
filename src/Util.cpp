/*----------------------------------------------*/
/*================== ISokoban ==================*/
/*----------------------------------------------*/
/* Version : 1.0								*/
/* Author : Michaël Hoste						*/
/* Copyright : See LICENSE file for details   	*/
/*----------------------------------------------*/

#include "../include/Util.h"

/* -------*/
/* Others */
/* -------*/
char* Util::cutText(const char* text, const int length)
{
	int i, j=0;
	char* newText;

	newText=(char*)malloc(2*sizeof(char));

	// Delete starting empty chars
	while(text[j]==' ' || text[j]=='\t' || text[j]=='\n')
		j++;

	for(i=j;i<length && text[i] != '\0';i++)
	{
		newText=(char*)realloc(newText, (i-j+2)*sizeof(char));
		if(text[i]=='\n')
			newText[i-j]=' ';
		else
			newText[i-j]=text[i];
	}
	newText[i-j]='\0';

	return newText;
}

char Util::toUpper(const char letter)
{
	if(letter>='a' && letter<='z')
		return (letter + ('A' - 'a'));
	else return letter;
}

char Util::toLower(const char letter)
{
	if(letter>='A' && letter<='Z')
		return (letter + ('a' - 'A'));
	else return letter;
}

char* Util::stringToLower(const char* string)
{
	int i;
	char* chaine2;
	int num = strlen(string);

	chaine2 = (char*)malloc((num+1)*sizeof(char));

	for(i=0;i<num;i++)
	{
		if(string[i]>='A' && string[i]<='Z')
			chaine2[i] = string[i] + ('a' - 'A');
		else chaine2[i] = string[i];
	}

	chaine2[i]='\0';

	return chaine2;
}

int Util::numberLength(const int number)
{
	char buffer[10];
	sprintf(buffer, "%d", number);

	return strlen(buffer);
}

char Util::nDecimal(const int number, const int n)
{
	char buffer[10];
	sprintf(buffer, "%d", number);

	if(n<=9)
		return buffer[n];
	else return '0';
}

bool Util::isDecimal(const char a)
{
	if(a>='0' && a<='9') return true;
	else return false;
}

char* Util::generateRep(char** reps)
{
	int length = 0;

	// Length of bigger folder name
	int i=0;
	while(reps[i] != NULL)
	{
		length += strlen(reps[i])+2;
		i++;
	}
	length += 1;

	char* repertory = (char*)malloc(length*sizeof(char));
	repertory[0] = '\0';

	// For each
	i=0;
	while(reps[i] != NULL)
	{
		if(i != 0)
		{
#ifdef SLINUX
			strcat(repertory, "/");
#endif
#ifdef SWIN32
			strcat(repertory, "\\");
#endif
		}

		strcat(repertory, reps[i]);

		i++;
	}

	return repertory;
}

void Util::createRep(char** reps)
{
	int length = 0;

	// Length of bigger folder name
	int i=0;
	while(reps[i] != NULL)
	{
		length += strlen(reps[i])+2;
		i++;
	}
	length += 1;

	char* repertory = (char*)malloc(length*sizeof(char));
	repertory[0] = '\0';

	// For each
	i=0;
	while(reps[i] != NULL)
	{
		if(i != 0)
		{
#ifdef SLINUX
			strcat(repertory, "/");
#endif
#ifdef SWIN32
			strcat(repertory, "\\");
#endif
		}

		strcat(repertory, reps[i]);

		if(!isThisRepExists(repertory))
		{
			char* makeRepertory = (char*)malloc((length+10)*sizeof(char));
			makeRepertory[0] = '\0';
			strcpy(makeRepertory, "mkdir ");
			strcat(makeRepertory, repertory);
			system(makeRepertory);
			free(makeRepertory);
		}

		i++;
	}

	free(repertory);
}

bool Util::isThisFileExists(char* file)
{
	FILE* ffile = fopen(file, "r");

	if(ffile == NULL)
		return false;
	else
	{
		fclose(ffile);
		return true;
	}
}

bool Util::isThisRepExists(char* rep)
{
	DIR* rrep = opendir(rep);

	if(rrep == NULL)
		return false;
	else
	{
		closedir(rrep);
		return true;
	}
}

