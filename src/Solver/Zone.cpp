/*----------------------------------------------*/
/*================== ISokoban ==================*/
/*----------------------------------------------*/
/* Version : 1.0								*/
/* Author : Michaël Hoste						*/
/* Copyright : See LICENSE file for details   	*/
/*----------------------------------------------*/

#include "../../include/Solver/Zone.h"

/* ------------*/
/* Constructor */
/* ------------*/
Zone::Zone(const Zone* zone):
	zone(NULL),
	length(0)
{
	copyZone(zone);
}

Zone::Zone(FILE* file, int numberOfPositions):
	zone(NULL),
	length(0)
{
	this->length = numberOfPositions;
	int intSize = sizeof(unsigned int);
	int nCells = getNumberCell();
	this->zone = (unsigned int*)malloc(nCells*intSize);

	for(int i=0;i<nCells;i++)
		fscanf(file, "%u ", &this->zone[i]);
	fscanf(file, "\n");
}

Zone::Zone(int numberOfPositions):
	zone(NULL),
	length(0)
{
	this->length = numberOfPositions;
	int intSize = sizeof(unsigned int);
	int nCells = getNumberCell();
	this->zone = (unsigned int*)malloc(nCells*intSize);

	for(int i=0;i<nCells;i++)
		this->zone[i] = 0;
}

Zone::Zone(const Zone* zone1, const Zone* zone2, int operation):
	zone(NULL),
	length(zone1->getLength())
{
	if(operation == USE_OR)
		makeORZone(zone1, zone2);
	else if(operation == USE_AND)
		makeANDZone(zone1, zone2);
	else if(operation == USE_XOR)
		makeXORZone(zone1, zone2);
	else if(operation == USE_NOT)
		makeNOTZone(zone1);
}

Zone::Zone(const Level* level, const int* levelToZonePos,
		const int* zoneToLevelPos, int length, int operation):
	zone(NULL),
	length(length)
{
	if(operation == PROCESS_BOXES)
		makeBoxesZone(level, zoneToLevelPos);
	else if(operation == PROCESS_PUSHER)
		makePusherZone(level, levelToZonePos, zoneToLevelPos);
	else if(operation == PROCESS_DEADLOCK)
		makeDeadlockZone(level, levelToZonePos, zoneToLevelPos);
	else if(operation == PROCESS_GOAL)
		makeGoalZone(level, zoneToLevelPos);
}

Zone::Zone(const Zone* boxes, const int startPos, const int* levelToZonePos,
		int colsNumber):
	zone(NULL),
	length(boxes->getLength())
{
	int intSize = sizeof(unsigned int);
	int nCells = getNumberCell();
	this->zone = (unsigned int*)malloc(nCells*intSize);

	for(int i=0;i<nCells;i++)
		this->zone[i]=(unsigned int)0;

	// Recursive call
	recursiveMakePusherZone(boxes, startPos, levelToZonePos, colsNumber);
}

/* -----------*/
/* Destructor */
/* -----------*/
Zone::~Zone()
{
	if(zone)
		free(zone);
}

/* -------*/
/* Others */
/* -------*/
void Zone::print(void) const
{
	int nCells = getNumberCell();
	int intSize = sizeof(unsigned int);

	for(int i=0;i<nCells;i++)
	{
		printf("cell %d : %u en binaire : ", i, this->zone[i]);
		for(int j = (8*intSize)-1;j >= 0;j--)
		{
			printf("%d", (this->zone[i] >> j ) & 1);
		}
		printf("\n");
	}
}

void Zone::print(const Level* level, const int* levelToZonePos) const
{
	Level* copy = new Level(NULL, level);
	int m = copy->getRowsNumber();
	int n = copy->getColsNumber();
	for(int i=0;i<m*n;i++)
	{
		int c = copy->readPos(i);
		if(c == '$' || c == '@')
			copy->writePos(i, ' ');
		else if(c == '*' || c == '+')
			copy->writePos(i, '.');

		if(levelToZonePos[i] != -1)
		{
			if(this->readPos(levelToZonePos[i]) == 1)
				copy->writePos(i, 'O');
		}
	}

	/* Double loop used to print every symbols of this level */
	for(int j=0;j<m;j++)
	{
		for(int k=n*j;k<n*(j+1);k++)
		{
			/* We change 's' in ' ' for better lisibility */
			if(copy->readPos(k) == 's')
				printf(" ");
			else
				printf("%c", copy->readPos(k));
		}
		printf("\n");
	}

	delete copy;
}

void Zone::write1ToPos(const int pos) const
{
	if(pos < 0)
		printf("Zone :: writePos error (pos<0)\n");
	else
	{
		int cellSize = sizeof(unsigned int)*8;

		// Cell of tab where the bit is (starting with 0)
		int bitCell = pos/cellSize;
		// Position of the bit in tab[cell] (starting with 0)
		int bitPos = pos%cellSize;

		if(bitCell < getNumberCell() && bitCell >= 0)
		{
			zone[bitCell] |= (unsigned int)1<<cellSize-1-bitPos;
		}
	}
}

void Zone::write0ToPos(const int pos) const
{
	if(pos < 0)
		printf("Zone :: writePos error (pos<0)\n");
	else
	{
		int cellSize = sizeof(unsigned int)*8;

		// Cell of tab where the bit is (starting with 0)
		int bitCell = pos/cellSize;
		// Position of the bit in tab[cell] (starting with 0)
		int bitPos = pos%cellSize;

		if(bitCell < getNumberCell())
		{
			zone[bitCell] &= ~((unsigned int)1<<cellSize-1-bitPos);
		}
	}
}

bool Zone::isEgal(const Zone* otherZone) const
{
	int nCells = getNumberCell();
	for(int i=0;i<nCells;i++)
	{
		if(otherZone->getZoneTab()[i] != zone[i])
			return false;
	}

	return true;
}

bool Zone::isFullOf0() const
{
	int nCells = getNumberCell();
	for(int i=0;i<nCells;i++)
	{
		if(zone[i] != (unsigned int)0)
			return false;
	}

	return true;
}

bool Zone::isFullOf1() const
{
	int nCells = getNumberCell();
	for(int i=0;i<nCells;i++)
	{
		if(~zone[i] != (unsigned int)0)
			return false;
	}

	return true;
}

bool Zone::isIncludedIn(const Zone* anotherZone) const
{
	// If node's boxes are all included in markedZone
	Zone* junction = new Zone(this, anotherZone, Zone::USE_AND);
	bool included = junction->isEgal(this);
	delete junction;

	return included;
}

int Zone::getNumberOf1() const
{
	int total = 0;
	for(int i=0;i<length;i++)
	{
		if(readPos(i) == 1)
			total++;
	}

	return total;
}

Zone* Zone::applyOrWith(Zone* otherZone)
{
	int nCells = getNumberCell();

	for(int i=0;i<nCells;i++)
		this->zone[i] = zone[i] | otherZone->getZoneTab()[i];

	return this;
}

Zone* Zone::applyAndWith(Zone* otherZone)
{
	int nCells = getNumberCell();

	for(int i=0;i<nCells;i++)
		this->zone[i] = zone[i] & otherZone->getZoneTab()[i];

	return this;
}

Zone* Zone::applyXorWith(Zone* otherZone)
{
	int nCells = getNumberCell();

	for(int i=0;i<nCells;i++)
		this->zone[i] = zone[i] ^ otherZone->getZoneTab()[i];

	return this;
}

Zone* Zone::applyNot()
{
	int nCells = getNumberCell();

	for(int i=0;i<nCells;i++)
		this->zone[i] = ~zone[i];

	return this;
}

Zone* Zone::applyMinusWith(Zone* otherZone)
{
	for(int i=0;i<length;i++)
	{
		if(otherZone->readPos(i) == 1)
			write0ToPos(i);
	}

	return this;
}

void Zone::saveInFile(FILE* file)
{
	int nCells = getNumberCell();
	for(int k=0;k<nCells;k++)
		fprintf(file, "%u ", this->zone[k]);
	fprintf(file, "\n");
}

void Zone::makeBoxesZone(const Level* level, const int* zoneToLevelPos)
{
	int intSize = sizeof(unsigned int);
	int pos;
	char cell;
	int nCells = getNumberCell();
	this->zone = (unsigned int*)malloc(nCells*intSize);

	for(int i=0;i<nCells;i++)
		this->zone[i]=(unsigned int)0;

	// For each cell
	for(int k=0;k<nCells;k++)
	{
		// For each bit
		for(int i=0 ; i<8*intSize ; i++)
		{
			pos = k*8*intSize+i;

			if(pos<length)
				cell = level->readPos(zoneToLevelPos[pos]);
			else
				cell = 'E';

			// If box, add 1 at position
			if(cell == '*' || cell == '$')
				write1ToPos(pos);
			// else, keep 0 at position
		}
	}
}

void Zone::makePusherZone(const Level* level, const int* levelToZonePos,
		const int* zoneToLevelPos)
{
	int intSize = sizeof(unsigned int);
	int nCells = getNumberCell();
	this->zone = (unsigned int*)malloc(nCells*intSize);

	for(int i=0;i<nCells;i++)
		this->zone[i]=(unsigned int)0;

	Zone* boxes = new Zone(level, levelToZonePos, zoneToLevelPos,
			length,	PROCESS_BOXES);
	int pusherPosM = level->getPusherPosM();
	int pusherPosN = level->getPusherPosN();
	int startPos = pusherPosM*level->getColsNumber() + pusherPosN;

	// Recursive call
	recursiveMakePusherZone(boxes, startPos, levelToZonePos,
			level->getColsNumber());

	delete boxes;
}

void Zone::recursiveMakePusherZone(const Zone* boxes, const int startPos,
		const int* levelToZonePos, const int colsNumber)
{
	int zoneStartPos = levelToZonePos[startPos];
	// If this zone is valid and not checked yet
	if(zoneStartPos != -1 && readPos(zoneStartPos) == 0)
	{
		// We check it
		write1ToPos(zoneStartPos);

		// If there is a box in here, we don't go through it
		if(boxes->readPos(zoneStartPos) == 0)
		{
			// Test of 4 neighbours if they are in the zone
			recursiveMakePusherZone(boxes, startPos+1, levelToZonePos,
				colsNumber);
			recursiveMakePusherZone(boxes, startPos-1, levelToZonePos,
				colsNumber);
			recursiveMakePusherZone(boxes, startPos+colsNumber, levelToZonePos,
				colsNumber);
			recursiveMakePusherZone(boxes, startPos-colsNumber, levelToZonePos,
				colsNumber);
		}
	}
}

void Zone::makeDeadlockZone(const Level* level, const int* levelToZonePos,
		const int* zoneToLevelPos)
{
	int intSize = sizeof(unsigned int);
	int nCells = getNumberCell();
	this->zone = (unsigned int*)malloc(nCells*intSize);

	for(int i=0;i<nCells;i++)
		this->zone[i]=(unsigned int)0;

	makeCornerDeadlockZone(level, zoneToLevelPos);
	makeLineDeadlockZone(level, levelToZonePos, zoneToLevelPos);
}

void Zone::makeCornerDeadlockZone(const Level* level,
		const int* zoneToLevelPos)
{
	// For each cell of the level
	for(int i=0;i<length;i++)
	{
		int levelPos = zoneToLevelPos[i];
		char car = level->readPos(levelPos);

		// Don't deadlock goals
		if(car != '.' && car != '*' && car != '+')
		{
			// If corner, add 1 at position
			if(isInCorner(level, levelPos))
				write1ToPos(i);
			// else, keep 0 at position
		}
	}
}

void Zone::makeLineDeadlockZone(const Level* level,
		const int* levelToZonePos, const int* zoneToLevelPos)
{
	int tempLevelPos, tempZonePos, tempCar;
	int* cornerPos = (int*)malloc(sizeof(int));
	int cornerPosSize = 0;
	int nCols = level->getColsNumber();

	// Store every position of corners
	for(int i=0;i<length;i++)
	{
		if(readPos(i) == 1)
		{
			cornerPos = (int*)realloc(cornerPos,
					(cornerPosSize+1)*sizeof(int));
			cornerPos[cornerPosSize] = i;
			cornerPosSize++;
		}
	}

	// for each corner
	for(int i=0;i<cornerPosSize;i++)
	{
		bool upEscape = false;
		bool downEscape = false;
		bool leftEscape = false;
		bool rightEscape = false;
		int cZonePos = cornerPos[i];
		int cLevelPos = zoneToLevelPos[cZonePos];

		/************************/
		/* Test horizontal line */
		/************************/
		tempLevelPos = cLevelPos;

		tempZonePos = levelToZonePos[tempLevelPos];
		tempCar = level->readPos(tempLevelPos);

		// while cell is...
		while(tempZonePos != -1 				 // ...not wall...
		   && tempCar != '.' && tempCar != '*')  // ...and not goal
		{
			// If the cell just above the actual cell is not wall
			if(levelToZonePos[tempLevelPos-nCols] != -1)
				upEscape = true;
			if(levelToZonePos[tempLevelPos+nCols] != -1)
				downEscape = true;

			if(upEscape && downEscape)
				break;

			tempLevelPos++;
			tempZonePos = levelToZonePos[tempLevelPos];
			tempCar = level->readPos(tempLevelPos);
		}

		// mark horizontal line if up or down is full of walls
		if(tempZonePos == -1)
		{
			tempLevelPos = cLevelPos+1;
			tempZonePos = levelToZonePos[tempLevelPos];
			tempCar = level->readPos(tempLevelPos);
			while(tempZonePos != -1)
			{
				write1ToPos(tempZonePos);
				tempLevelPos++;
				tempZonePos = levelToZonePos[tempLevelPos];
				tempCar = level->readPos(tempLevelPos);
			}
		}

		/**********************/
		/* Test vertical line */
		/**********************/
		tempLevelPos = cLevelPos;

		tempZonePos = levelToZonePos[tempLevelPos];
		tempCar = level->readPos(tempLevelPos);

		// while cell is...
		while(tempZonePos != -1 				 // ...not wall...
		   && tempCar != '.' && tempCar != '*')  // ...and not goal
		{
			// If the cell just above the actual cell is not wall
			if(levelToZonePos[tempLevelPos-1] != -1)
				leftEscape = true;
			if(levelToZonePos[tempLevelPos+1] != -1)
				rightEscape = true;

			if(leftEscape && rightEscape)
				break;

			tempLevelPos+=nCols;
			tempZonePos = levelToZonePos[tempLevelPos];
			tempCar = level->readPos(tempLevelPos);
		}

		// mark horizontal line if up or down is full of walls
		if(tempZonePos == -1)
		{
			tempLevelPos = cLevelPos+nCols;
			tempZonePos = levelToZonePos[tempLevelPos];
			tempCar = level->readPos(tempLevelPos);
			while(tempZonePos != -1)
			{
				write1ToPos(tempZonePos);
				tempLevelPos+=nCols;
				tempZonePos = levelToZonePos[tempLevelPos];
				tempCar = level->readPos(tempLevelPos);
			}
		}
	}

	free(cornerPos);
}

void Zone::makeGoalZone(const Level* level, const int* zoneToLevelPos)
{
	int intSize = sizeof(unsigned int);
	int pos;
	char cell;
	int nCells = getNumberCell();
	this->zone = (unsigned int*)malloc(nCells*intSize);

	for(int i=0;i<nCells;i++)
		this->zone[i]=(unsigned int)0;

	for(int k=0;k<nCells;k++)
	{
		// For each bit
		for(int i=0 ; i<8*intSize ; i++)
		{
			pos = k*8*intSize+i;

			if(pos<length)
				cell = level->readPos(zoneToLevelPos[pos]);
			else
				cell = 'E';

			// If goal add 1 at position
			if(cell == '*' || cell == '.' || cell == '+')
				write1ToPos(pos);
			// else, keep 0 at position
		}
	}
}

void Zone::makeORZone(const Zone* zone1, const Zone* zone2)
{
	int intSize = sizeof(unsigned int);
	int nCells = getNumberCell();
	this->zone = (unsigned int*)malloc(nCells*intSize);

	for(int i=0;i<nCells;i++)
	{
		this->zone[i] = zone1->getZoneTab()[i] | zone2->getZoneTab()[i];
	}
}

void Zone::makeANDZone(const Zone* zone1, const Zone* zone2)
{
	int intSize = sizeof(unsigned int);
	int nCells = getNumberCell();
	this->zone = (unsigned int*)malloc(nCells*intSize);

	for(int i=0;i<nCells;i++)
	{
		this->zone[i] = zone1->getZoneTab()[i] & zone2->getZoneTab()[i];
	}
}

void Zone::makeXORZone(const Zone* zone1, const Zone* zone2)
{
	int intSize = sizeof(unsigned int);
	int nCells = getNumberCell();
	this->zone = (unsigned int*)malloc(nCells*intSize);

	for(int i=0;i<nCells;i++)
	{
		this->zone[i] = zone1->getZoneTab()[i] ^ zone2->getZoneTab()[i];
	}
}

void Zone::makeNOTZone(const Zone* zone1)
{
	int intSize = sizeof(unsigned int);
	int nCells = getNumberCell();
	this->zone = (unsigned int*)malloc(nCells*intSize);

	for(int i=0;i<nCells;i++)
	{
		this->zone[i] = ~zone1->getZoneTab()[i];
	}
}

void Zone::copyZone(const Zone* source)
{
	this->length = source->getLength();
	if(length > 0)
	{
		int nCells = getNumberCell();
		this->zone = (unsigned int*)malloc(nCells*sizeof(unsigned int));

		for(int i=0;i<nCells;i++)
		{
			this->zone[i] = source->getZoneTab()[i];
		}
	}
	else
		this->zone = NULL;
}

bool Zone::isInCorner(const Level* level, const int pos) const
{
	char l = level->readPos(pos-1);
	char r = level->readPos(pos+1);
	char u = level->readPos(pos - level->getColsNumber());
	char d = level->readPos(pos + level->getColsNumber());

	if((u == '#' && l == '#') || (u == '#' && r == '#')
	 ||(d == '#' && l == '#') || (d == '#' && r == '#'))
		return true;
	else
		return false;
}
