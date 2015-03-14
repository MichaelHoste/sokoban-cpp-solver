/*----------------------------------------------*/
/*================== ISokoban ==================*/
/*----------------------------------------------*/
/* Version : 1.0								*/
/* Author : Michaël Hoste						*/
/* Copyright : See LICENSE file for details   	*/
/*----------------------------------------------*/

#include "../include/Level.h"

/* ------------*/
/* Constructor */
/* ------------*/
Level::Level(Base* base):
	base(base),
	id(0),
	grid(NULL),
	boxesNumber(0),
	goalsNumber(0),
	rowsNumber(0),
	colsNumber(0),
	pusherPosM(0),
	pusherPosN(0),
	xCenter(0),
	yCenter(0),
	won(false)
{
	name[0]='\0';
	packName[0]='\0';

	myBestMoves = new Path(base);
	myBestPushes = new Path(base);
	worldBestMoves = new Path(base);
	worldBestPushes = new Path(base);
	actualPath = new Path(base);
}

Level::Level(Base* base, const Level* source):
	base(base)
{
	copyLevel(source);
}

Level::Level(Base* base, xmlNodePtr levelNode, xmlNodePtr pathNode, int id,
		const char* packName):
	base(base),
	id(0),
	grid(NULL),
	boxesNumber(0),
	goalsNumber(0),
	rowsNumber(0),
	colsNumber(0),
	pusherPosM(0),
	pusherPosN(0),
	xCenter(0),
	yCenter(0),
	won(false),
	myBestMoves(NULL),
	myBestPushes(NULL),
	worldBestMoves(NULL),
	worldBestPushes(NULL),
	actualPath(NULL)
{
	name[0]='\0';
	strcpy(this->packName, packName);

	// Load level and best paths (solutions) from nodes situated in xml files
	xmlLoad(levelNode, pathNode, id);
}

/* -----------*/
/* Destructor */
/* -----------*/
Level::~Level()
{
	if(grid)
		free(grid);
	if(myBestMoves)
		delete myBestMoves;
	if(myBestPushes)
		delete myBestPushes;
	if(worldBestMoves)
		delete worldBestMoves;
	if(worldBestPushes)
		delete worldBestPushes;
	if(actualPath)
		delete actualPath;
}

/* -------*/
/* Others */
/* -------*/
char Level::readPos(const int m, const int n) const
{
	if(m<rowsNumber && n<colsNumber && m>=0 && n>=0)
		return grid[colsNumber*m+n];
	else return 'E';
}

char Level::readPos(const int pos) const
{
	if(pos >= 0 && pos < rowsNumber*colsNumber)
		return grid[pos];
	else return 'E';
}

void Level::writePos(const int m, const int n, const char letter)
{
	if(m<rowsNumber && n<colsNumber && m>=0 && n>=0)
		grid[colsNumber*m+n]=letter;
	else
		printf("Level :: invalid grid position : m = %d and n = %d\n", m, n);
}

void Level::writePos(const int pos, const char letter)
{
	if(pos >= 0 && pos < rowsNumber*colsNumber)
		grid[pos]=letter;
	else
	{
		printf("Level :: invalid grid position : pos = %d\n", pos);
		exit(1);
	}
}

bool Level::pusherCanMove(const char direction) const
{
	char mouv1=' ';
	char mouv2=' ';
	int m = pusherPosM;
	int n = pusherPosN;

	/* Following of the direction, test 2 cells */
	if(direction=='u')
	{
		mouv1=readPos(m-1, n);
		mouv2=readPos(m-2, n);
	}
	else if(direction=='d')
	{
		mouv1=readPos(m+1, n);
		mouv2=readPos(m+2, n);
	}
	else if(direction=='l')
	{
		mouv1=readPos(m, n-1);
		mouv2=readPos(m, n-2);
	}
	else if(direction=='r')
	{
		mouv1=readPos(m, n+1);
		mouv2=readPos(m, n+2);
	}

	/* If (there is a wall) OR (two boxes or one box and a wall) */
	if( mouv1 == '#' ||
	  (   (mouv1 == '*' || mouv1 == '$')
	   && (mouv2 == '*' || mouv2 == '$' || mouv2 == '#')))
	{
		return false;
	}
	else return true;
}

int Level::move(const char direction)
{
	int m_1, m_2, n_1, n_2, action=1;
	int m = pusherPosM;
	int n = pusherPosN;
	int state;
	char dir = direction;

	// accept upper and lower dir
	if(dir == 'U')
		dir = 'u';
	else if(dir == 'D')
		dir = 'd';
	else if(dir == 'L')
		dir = 'l';
	else if(dir == 'R')
		dir = 'r';

	/* Following of the direction, test 2 cells */
	if(dir=='u' && pusherCanMove('u'))
	{
		m_1=m-1;
		m_2=m-2;
		n_1=n_2=n;
		pusherPosM--;
	}
	else if(dir=='d' && pusherCanMove('d'))
	{
		m_1=m+1;
		m_2=m+2;
		n_1=n_2=n;
		pusherPosM++;
	}
	else if(dir=='l' && pusherCanMove('l'))
	{
		n_1=n-1;
		n_2=n-2;
		m_1=m_2=m;
		pusherPosN--;
	}
	else if(dir=='r' && pusherCanMove('r'))
	{
		n_1=n+1;
		n_2=n+2;
		m_1=m_2=m;
		pusherPosN++;
	}
	else
	{
		action=0;
		state = MOUV_NONE;
	}

	/* Move accepted */
	if(action==1)
	{
		state = MOUV_SOKOBAN;

		/* Test on cell (m,n) */
		if(readPos(m, n) == '+')
			writePos(m, n, '.');
		else
			writePos(m, n, 's');

		/* Test on cell (m_2,n_2) */
		if(readPos(m_1, n_1)=='$' || readPos(m_1, n_1)=='*')
		{
			if(readPos(m_2, n_2)=='.')
				writePos(m_2, n_2, '*');
			else
				writePos(m_2, n_2, '$');

			/* Save push */
			actualPath->addPush(dir);
			state = MOUV_BOX;
		}

		/* Test on cell (m_1, n_1) */
		if(readPos(m_1, n_1)=='.' || readPos(m_1, n_1)=='*')
			writePos(m_1, n_1, '+');
		else
			writePos(m_1, n_1, '@');

		/* Save move */
		if(state != MOUV_BOX)
			actualPath->addMove(dir);
	}

	return state;
}

int Level::eraseMove(void)
{
	int m_1, m_2, n_1, n_2, action=1, maj=0;
	int m = pusherPosM;
	int n = pusherPosN;
	char direction = actualPath->getLastMove();
	int state = Level::MOUV_SOKOBAN;

	if(direction=='U' || direction=='D' || direction=='L' || direction=='R')
	{
		maj=1;
		state = Level::MOUV_BOX;
	}

	/* Following of the direction, test 2 cells */
	if(direction=='u' || direction=='U')
	{
		m_1=m-1;
		m_2=m+1;
		n_1=n_2=n;
		pusherPosM++;
	}
	else if(direction=='d'  || direction=='D')
	{
		m_1=m+1;
		m_2=m-1;
		n_1=n_2=n;
		pusherPosM--;
	}
	else if(direction=='l' || direction=='L')
	{
		n_1=n-1;
		n_2=n+1;
		m_1=m_2=m;
		pusherPosN++;
	}
	else if(direction=='r' || direction=='R')
	{
		n_1=n+1;
		n_2=n-1;
		m_1=m_2=m;
		pusherPosN--;
	}
	else
	{
		action = 0;
		state = Level::MOUV_NONE;
	}

	if(action==1)
	{
		/* Test of cell (m_2,n_2) */
		if(readPos(m_2, n_2) == '.')
			writePos(m_2, n_2, '+');
		else
			writePos(m_2, n_2, '@');

		/* Test of cell (m_1, n_1) */
		if(readPos(m_1, n_1)=='*' && maj==1)
			writePos(m_1, n_1, '.');
		else if(readPos(m_1, n_1)=='$' && maj==1)
			writePos(m_1, n_1, 's');

		/* Test of cell (m,n) */
		if(readPos(m, n)=='+' && maj==0)
			writePos(m, n, '.');
		else if(readPos(m, n)=='@' && maj==0)
			writePos(m, n, 's');
		else if(readPos(m, n)=='+' && maj==1)
			writePos(m, n, '*');
		else if(readPos(m, n)=='@' && maj==1)
			writePos(m, n, '$');
	}

	actualPath->deleteLastMove();

	return state;
}

bool Level::isWon(void) const
{
	for(int i=0;i<rowsNumber*colsNumber;i++)
		if(grid[i]=='$')
			return false;

	return true;
}

bool Level::isSolution(Path* path) const
{
	bool solution;

	// Copy of the actual state of this level
	Level* testLevel = new Level(base, this);

	// Get the pointer (without any modifications on it, only readings)
	const char * tabPath = path->getMoves();

	// Use path to move in this level copy
	for(int i=0;i<path->getNMoves();i++)
		testLevel->move(tabPath[i]);

	solution = testLevel->isWon();

	delete testLevel;

	return solution;
}

void Level::print(const char* fileName) const
{
	if(fileName)
		printInFile(fileName);
	else printf("Level :: invalid fileName");
}

void Level::print(void) const
{
	printInFile(NULL);
}

void Level::xmlSavePath(FILE* file) const
{
	fprintf(file, "	<Level Id=\"%d\">\n", this->id);
	fprintf(file, "		<MBM mn=\"%d\" mp=\"%d\">%s</MBM>\n",
				  myBestMoves->getNMoves(),
				  myBestMoves->getNPushes(),
				  myBestMoves->compressPath(myBestMoves->getMoves()));
	fprintf(file, "		<MBP mp=\"%d\" mn=\"%d\">%s</MBP>\n",
				  myBestPushes->getNPushes(),
				  myBestPushes->getNMoves(),
				  myBestPushes->compressPath(myBestPushes->getMoves()));
	fprintf(file, "		<WBM mn=\"%d\" mp=\"%d\">%s</WBM>\n",
			      worldBestMoves->getNMoves(),
			      worldBestMoves->getNPushes(),
			      worldBestMoves->compressPath(worldBestMoves->getMoves()));
	fprintf(file, "		<WBP mp=\"%d\" mn=\"%d\">%s</WBP>\n",
			      worldBestPushes->getNPushes(),
			      worldBestPushes->getNMoves(),
			      worldBestPushes->compressPath(worldBestPushes->getMoves()));
	fprintf(file, "	</Level>\n");
}

void Level::initializePusherPos(void)
{
	bool find = false;

	for(int i=0;i<rowsNumber && !find;i++)
	{
		for(int j=0;j<colsNumber && !find;j++)
		{
			if(readPos(i,j) == '@' || readPos(i,j) == '+')
			{
				pusherPosM = i;
				pusherPosN = j;
				find = true;
			}
		}
	}
}

void Level::makeGround(void)
{
	/* Recursively set "inside ground" to 's' starting with pusher position */
	makeGroundRec(pusherPosM, pusherPosN);

	/* Set back modified (by recusively method) symbols to regular symbols */
	for(int i=0;i<rowsNumber*colsNumber;i++)
	{
		if(grid[i]=='p') grid[i]='.';
		else if(grid[i]=='d') grid[i]='$';
		else if(grid[i]=='a') grid[i]='*';
	}
}

void Level::makeGroundRec(const int m, const int n)
{
	char a;
	a=readPos(m, n);

	/* Change of values to "ground" or "visited" */
	if(a == ' ') writePos(m, n, 's');
	else if(a == '.') writePos(m, n, 'p');
	else if(a == '$') writePos(m, n, 'd');
	else if(a == '*') writePos(m, n, 'a');

	/* If non-visited cell, test neighbours cells */
	if(a != '#' && a != 's' && a != 'p' && a != 'd' && a != 'a')
	{
		makeGroundRec(m+1, n);
		makeGroundRec(m-1, n);
		makeGroundRec(m, n+1);
		makeGroundRec(m, n-1);
	}
}

void Level::copyLevel(const Level* source)
{
	this->id = source->getId();
	strcpy(this->name, source->getName());
	strcpy(this->packName, source->getPackName());

	int m = source->getRowsNumber();
	int n = source->getColsNumber();

	if(source->getGrid() != NULL)
	{
		this->grid = (char*)malloc((m*n+1)*sizeof(char));
		strcpy(this->grid, source->getGrid());
	}
	else
		this->grid = NULL;

	this->boxesNumber = source->getBoxesNumber();
	this->goalsNumber = source->getGoalsNumber();
	this->rowsNumber = source->getRowsNumber();
	this->colsNumber = source->getColsNumber();
	this->pusherPosM = source->getPusherPosM();
	this->pusherPosN = source->getPusherPosN();
	this->xCenter = source->getXCenter();
	this->yCenter = source->getYCenter();
	this->won = source->getWon();

	this->myBestMoves = new Path(this->base, source->getMyBestMoves());
	this->myBestPushes = new Path(this->base, source->getMyBestPushes());
	this->worldBestMoves = new Path(this->base, source->getWorldBestMoves());
	this->worldBestPushes = new Path(this->base, source->getWorldBestPushes());
	this->actualPath = new Path(this->base, source->getActualPath());
}

void Level::printInFile(const char* fileName) const
{
	FILE * file;
	int j, k;

	/* If fileName is NULL, standard output is used to print */
	if(fileName!=NULL)
		file=fopen(fileName, "a");
	else file=stdout;

	/* print infos on Level. */
	fprintf(file, "id   : %d\n", id);
	fprintf(file, "name : %s\n", name);
	fprintf(file, "pack name : %s\n", packName);
	fprintf(file, "rows : %d\n", rowsNumber);
	fprintf(file, "cols : %d\n\n", colsNumber);

	/* Double loop used to print every symbols of this level */
	for(j=0;j<rowsNumber;j++)
	{
		for(k=colsNumber*j;k<colsNumber*(j+1);k++)
		{
			/* We change 's' in ' ' for better lisibility */
			if(grid[k] == 's')
				fprintf(file, " ");
			else
				fprintf(file, "%c", grid[k]);
		}
		fprintf(file, "\n");
	}
	fprintf(file, "\n--------------------------\n\n");

	if(fileName != NULL)
		fclose(file);
}

void Level::xmlLoad(xmlNodePtr levelNode, xmlNodePtr pathNode, const int id)
{
	int i;
	char *tempid, *tempid2, *tempheight, *tempwidth;
	this->id = id;

	/**************************** [LEVEL] ****************************/

	/* Copy attribute properties */
	tempid = (char*)xmlGetProp(levelNode, (unsigned char*)"Id");
	tempheight = (char*)xmlGetProp(levelNode, (unsigned char*)"Height");
	tempwidth = (char*)xmlGetProp(levelNode, (unsigned char*)"Width");

	tempid2 = Util::cutText(tempid, 64);
	strcpy(name, tempid2);
	rowsNumber = atoi(tempheight);
	colsNumber = atoi(tempwidth);

	free(tempid);
	free(tempid2);
	free(tempheight);
	free(tempwidth);

	grid=(char*)malloc((rowsNumber*colsNumber+1)*sizeof(char));

	// Fill in with empty cells (empty cells are not explicitely in xml file)
	for(i=0;i<rowsNumber*colsNumber;i++)
		grid[i]=' ';

	i=0;
	// Each line
	levelNode = levelNode->children->next;
	while(levelNode)
	{
		// allocate variable with line
		char* temptext = (char*)xmlNodeGetContent(levelNode);
		strcpy(&(grid[colsNumber*i]), temptext);

		// If temptext is shorter than numberCols, we need to delete extra '\0'
		if(grid[colsNumber*i+strlen(temptext)] == '\0')
			grid[colsNumber*i+strlen(temptext)] = ' ';

		free(temptext);
		i++;
		levelNode = levelNode->next->next;
	}

	grid[rowsNumber*colsNumber] = '\0';

	// Find initial position of pusher
	initializePusherPos();

	// Make ground inside the level
	makeGround();

	// Initialize number of boxes and goals
	boxesNumber = 0;
	goalsNumber = 0;
	for(int i=0;i<rowsNumber;i++)
	{
		for(int j=0;j<colsNumber;j++)
		{
			char pos = readPos(i,j);
			if(pos == '*' || pos == '$')
				boxesNumber++;
			if(pos == '+' || pos == '*' || pos == '.')
				goalsNumber++;
		}
	}

	/**************************** [PATH] ****************************/

	if(pathNode == NULL)
	{
		myBestMoves = new Path(base);
		myBestPushes = new Path(base);
		worldBestMoves = new Path(base);
		worldBestPushes = new Path(base);
		actualPath = new Path(base);
	}
	else
	{
		// we go down
		pathNode = pathNode->children->next;

		// MyBestMoves node
		if(strcmp((char*)pathNode->name, "MBM") == 0)
		{
			char * temppath;
			temppath = (char*)xmlNodeGetContent(pathNode);
			this->myBestMoves = new Path(base, temppath, Path::COMPRESSED);
			free(temppath);
		}
		else
		{
			printf("Level :: <MBM> doesn't exist\n");
			exit(1);
		}

		pathNode = pathNode->next->next;

		// MyBestPushes node
		if(strcmp((char*)pathNode->name, "MBP") == 0)
		{
			char * temppath;
			temppath = (char*)xmlNodeGetContent(pathNode);
			this->myBestPushes = new Path(base, temppath, Path::COMPRESSED);
			free(temppath);
		}
		else
		{
			printf("Level :: <MBP> doesn't exist\n");
			exit(1);
		}

		pathNode = pathNode->next->next;

		// WorldBestMoves node
		if(strcmp((char*)pathNode->name, "WBM") == 0)
		{
			char * temppath;
			temppath = (char*)xmlNodeGetContent(pathNode);
			this->worldBestMoves = new Path(base, temppath, Path::COMPRESSED);
			free(temppath);
		}
		else
		{
			printf("Level :: <WBM> doesn't exist\n");
			exit(1);
		}

		pathNode = pathNode->next->next;

		// WorldBestPushes node
		if(strcmp((char*)pathNode->name, "WBP") == 0)
		{
			char * temppath;
			temppath = (char*)xmlNodeGetContent(pathNode);
			this->worldBestPushes = new Path(base, temppath, Path::COMPRESSED);
			free(temppath);
		}
		else
		{
			printf("Level :: <WBP> doesn't exist\n");
			exit(1);
		}

		this->actualPath = new Path(base);

		if(this->myBestMoves->getNMoves() > 0)
			won = true;
		else won = false;
	}
}
