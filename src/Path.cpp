/*----------------------------------------------*/
/*================== ISokoban ==================*/
/*----------------------------------------------*/
/* Version : 1.0								*/
/* Author : Michaël Hoste						*/
/* Copyright : See LICENSE file for details   	*/
/*----------------------------------------------*/

#include "../include/Path.h"

/* ------------*/
/* Constructor */
/* ------------*/
Path::Path(Base* base) :
	base(base),
	nPushes(0),
	nMoves(0)
{
	moves = (char*)malloc(sizeof(char));
	moves[0]='\0';
}

Path::Path(Base* base, const char* path, const int mode) :
	base(base),
	nPushes(0),
	nMoves(0)
{
	moves = (char*)malloc(sizeof(char));
	moves[0]='\0';

	if(mode == Path::COMPRESSED)
	{
		char* uncompressedPath;
		uncompressedPath = Path::uncompressPath(path);

		// We fill in the path of this object
		int length = strlen(uncompressedPath);
		for(int i=0;i<length;i++)
			addDisplacement(uncompressedPath[i]);

		free(uncompressedPath);
	}
	else if(mode == Path::UNCOMPRESSED)
	{
		// We create the path of this object
		int length = strlen(path);
		for(int i=0;i<length;i++)
			addDisplacement(path[i]);
	}
}

Path::Path(Base* base, const Path* source):
	base(base),
	nPushes(source->getNPushes()),
	nMoves(source->getNMoves())
{
	this->moves = (char*)malloc((source->getNMoves()+1)*sizeof(char));
	strcpy(this->moves, source->getMoves());
}

/* -----------*/
/* Destructor */
/* -----------*/
Path::~Path()
{
	free(moves);
}

/* -------*/
/* Others */
/* -------*/
void Path::addMove(const char direction)
{
	if(isValidDirection(direction))
	{
		nMoves++;
		moves = (char*)realloc(moves,(nMoves+1)*sizeof(char));
		moves[nMoves-1]=Util::toLower(direction);
		moves[nMoves]='\0';
	}
	else
		printf("Path :: invalid direction : %c\n", direction);
}

void Path::addPush(const char direction)
{
	if(isValidDirection(direction))
	{
		nMoves++;
		nPushes++;
		moves = (char*)realloc(moves,(nMoves+1)*sizeof(char));
		moves[nMoves-1]=Util::toUpper(direction);
		moves[nMoves]='\0';
	}
	else
		printf("Path :: invalid direction : %c\n", direction);
}

void Path::addDisplacement(const char direction)
{
	if(isValidDirection(direction))
	{
		if(direction>='A' && direction<='Z')
			nPushes++;
		nMoves++;
		moves = (char*)realloc(moves,(nMoves+1)*sizeof(char));
		moves[nMoves-1]=direction;
		moves[nMoves]='\0';
	}
	else
		printf("Path :: invalid direction : %c\n", direction);
}

void Path::deleteLastMove(void)
{
	if(nMoves>0)
	{
		char lastmove = moves[nMoves-1];
		if(lastmove>='A' && lastmove<='Z')
			nPushes--;
		nMoves--;
		moves = (char*)realloc(moves,(nMoves+1)*sizeof(char));
		moves[nMoves] = '\0';
	}
}

void Path::print(void) const
{
	printf("Path :: %s\n", moves);
}

char* Path::compressPath(const char * uncompressedPath)
{
	int cpt_c=1;
	char tabi;
	int i=0,j,k;
	int c_chiffres;
	char * compressedPath;
	int length;

	compressedPath=(char*)malloc(1*sizeof(char));
	compressedPath[0]='\0';

	/* We loop on each cell of the uncompressed path */
	length = strlen(uncompressedPath);
	while(i<length)
	{
		j=0;
		tabi=uncompressedPath[i];
		/* while it's the same displacement and we're not in the end */
		while(tabi==uncompressedPath[i] && i<length)
		{
			/* (Identical moves)++ */
			j++;
			i++;
		}

		/* If there is only one move in this direction */
		if(j==1)
		{
			cpt_c++;
			compressedPath=(char*)realloc(compressedPath, cpt_c*sizeof(char));
		}
		/* If there are many moves in one direction */
		else if(j>=2)
		{
			c_chiffres=Util::numberLength(j);

			cpt_c=cpt_c+c_chiffres+1;
			compressedPath=(char*)realloc(compressedPath, cpt_c*sizeof(char));

			for(k=0;k<c_chiffres;k++)
				compressedPath[cpt_c-2-c_chiffres+k]=Util::nDecimal(j, k);
		}

		/* In every case, add direction and '\0' next to it */
		compressedPath[cpt_c-2]=tabi;
		compressedPath[cpt_c-1]='\0';
	}

	return compressedPath;
}

char* Path::uncompressPath(const char * compressedPath)
{
	int i=0, j=0, nbr;
	char buffer[10];
	int cpt=1;
	char * uncompressedPath;

	// Alloc empty cell
	uncompressedPath=(char*)malloc(1*sizeof(char));
	uncompressedPath[0]='\0';

	// While we're not in the end of compressed path
	while(compressedPath[i]!='\0')
	{
		buffer[0]='1';
		buffer[1]='\0';

		// we put decimal in buffer
		while(Util::isDecimal(compressedPath[i]))
		{
			buffer[j]=compressedPath[i];
			buffer[j+1]='\0';
			j++;
			i++;
		}

		// we get decimal on untextual mode
		nbr=atoi(buffer);

		for(j=0;j<nbr;j++)
		{
			cpt++;
			uncompressedPath=(char*)realloc(uncompressedPath, cpt*sizeof(char));
			uncompressedPath[cpt-2]=compressedPath[i];
			uncompressedPath[cpt-1]='\0';
		}

		i++;
		j=0;
	}

	return uncompressedPath;
}

inline bool Path::isValidDirection(const char direction)
{
	if(direction=='u' || direction=='d' || direction=='l' || direction=='r'
	|| direction=='U' || direction=='D' || direction=='L' || direction=='R')
		return true;
	else return false;
}
