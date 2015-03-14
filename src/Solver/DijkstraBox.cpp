/*----------------------------------------------*/
/*================== ISokoban ==================*/
/*----------------------------------------------*/
/* Version : 1.0								*/
/* Author : Michaël Hoste						*/
/* Copyright : See LICENSE file for details   	*/
/*----------------------------------------------*/

#include "../../include/Solver/DijkstraBox.h"

/* ------------*/
/* Constructor */
/* ------------*/
DijkstraBox::DijkstraBox(const Solver* solver, Node* node, int boxPos):
	solver(solver),
	node(node),
	boxPos(boxPos),
	nCols(solver->getLevel()->getColsNumber()),
	firstCell(NULL),
	lastCell(NULL)
{
	const Level* level = solver->getLevel();
	int nRows = level->getRowsNumber();
	const int* lToZPos = solver->getLevelToZonePos();

	int n = nCols*nRows*4;
	dTable = (int*)malloc(n*sizeof(int));

	Zone* boxes = node->getBoxesZone();

	for(int i=0;i<n;i+=4)
	{
		if(lToZPos[i/4] != -1 && boxes->readPos(lToZPos[i/4]) != 1)
			dTable[i] = dTable[i+1] = dTable[i+2] = dTable[i+3] = INT_MAX;
		else
			dTable[i] = dTable[i+1] = dTable[i+2] = dTable[i+3] = -1;
	}

	// boxPos can move
	dTable[boxPos*4] = dTable[boxPos*4+1] = dTable[boxPos*4+2] = dTable[boxPos*4+3] = INT_MAX;

	// Init waiting cell chainedlist
	firstCell = new CellList(boxPos, LEFT, 0, NULL);
	lastCell = firstCell;
	firstCell = new CellList(boxPos, RIGHT, 0, firstCell);
	firstCell = new CellList(boxPos, UP, 0, firstCell);
	firstCell = new CellList(boxPos, DOWN, 0, firstCell);
}

/* -----------*/
/* Destructor */
/* -----------*/
DijkstraBox::~DijkstraBox()
{
	free(dTable);
}

/* -------*/
/* Others */
/* -------*/
int* DijkstraBox::resolveGoals(int* pLastPushPosition)
{
	CellList* cur = firstCell;
	while(cur != NULL)
	{
		dijkstraRec(cur->pos, cur->dir, cur->value);
		CellList* temp = cur;
		cur = cur->next;
		temp->next = NULL;
		delete temp;
	}

	const int* zToLPos = solver->getZoneToLevelPos();
	int goalsNumber = solver->getLevel()->getGoalsNumber();
	int* goalsValues = (int*)malloc(goalsNumber*sizeof(int));
//	pLastPushPosition = (int*)realloc(pLastPushPosition,
//			goalsNumber*sizeof(int));

	const Zone* goals = solver->getGoalZone();
	int length = goals->getLength();
	int k=0;
	for(int i=0;i<length;i++)
	{
		if(goals->readPos(i) == 1)
		{
			// Keep lowest value of goal
			goalsValues[k] = INT_MAX;
			int pos = zToLPos[i];
			for(int j=0;j<4;j++)
			{
				if(getPos(pos, j) < goalsValues[k])
				{
					goalsValues[k] = getPos(pos, j);
					int newPos = -1;
					switch(j)
					{
						case RIGHT: newPos = pos + 1; break;
						case LEFT : newPos = pos - 1; break;
						case UP   : newPos = pos - nCols; break;
						case DOWN : newPos = pos + nCols; break;
						default   : break;
					}
					pLastPushPosition[k] = newPos;
				}
			}
			k++;
		}
	}

	return goalsValues;
}

int* DijkstraBox::resolvePositions(void)
{
	CellList* cur = firstCell;
	while(cur != NULL)
	{
		dijkstraRec(cur->pos, cur->dir, cur->value);
		CellList* temp = cur;
		cur = cur->next;
		temp->next = NULL;
		delete temp;
	}

	const int* zToLPos = solver->getZoneToLevelPos();
	const int zToLPosLength = solver->getZoneToLevelPosLength();
	int* posValues = (int*)malloc(zToLPosLength*sizeof(int));

	for(int i=0;i<zToLPosLength;i++)
	{
		// Keep lowest value of goal
		posValues[i] = INT_MAX;
		int pos = zToLPos[i];
		for(int j=0;j<4;j++)
		{
			if(getPos(pos, j) < posValues[i])
				posValues[i] = getPos(pos, j);
		}
	}

	return posValues;
}

char* DijkstraBox::listMovesToPosition(int position)
{
	const int* lToZPos = solver->getLevelToZonePos();

	// Initialization
	char* list = (char*)malloc(sizeof(char));
	list[0] = '\0';

	// position must not be a wall, a fixed box nor a non-accessible cell
	int val = min(getPos(position, 0), getPos(position, 1),
			getPos(position, 2), getPos(position, 3));
	if(val == INT_MAX || val == -1 || val == 0)
		return list;

	// Get list of "val+1" boxes
	int* listBoxPos = listBoxPosToPosition(position);

	// Get list of "val" pusher positions
	int* listPusherPos = listPusherPrepos(listBoxPos, val+1);

//	printf("boxes\n");
//	for(int i=0;i<val+1;i++)
//		printf("%3d ", listBoxPos[i]);
//
//	printf("\npusher\n");
//	for(int i=0;i<val;i++)
//		printf("%3d ", listPusherPos[i]);
//	printf("\n");

	// Initialization of pusher position
	for(int i=0;i<val;i++)
	{
		if(listPusherPos[i] + 1 == listBoxPos[i])
			list = addMoveToList(list, 'R');
		else if(listPusherPos[i] - 1 == listBoxPos[i])
			list = addMoveToList(list, 'L');
		else if(listPusherPos[i] + nCols == listBoxPos[i])
			list = addMoveToList(list, 'D');
		else if(listPusherPos[i] - nCols == listBoxPos[i])
			list = addMoveToList(list, 'U');

		// If no need to replace
		if(i == val-1)
			break;

		// 1. Create node with current position of boxes
		Zone* boxesZone = new Zone(node->getBoxesZone());

		// 2. Delete initial position of box and write current position instead
		boxesZone->write0ToPos(lToZPos[boxPos]);
		boxesZone->write1ToPos(lToZPos[listBoxPos[i+1]]);

		// 3. Create new pusherZone and make a node with it
		Zone* pusherZone = new Zone(boxesZone, listBoxPos[i], lToZPos, nCols);
		Node* newNode = new Node(solver, pusherZone, boxesZone);

		// 4. Find list of moves from old pusher position to new pusher pos
		int posA = listBoxPos[i];
		int posB = listPusherPos[i+1];
		char* moves = newNode->listMovesFromAToB(posA, posB);

		// 5. Add list of moves to list
		int mLength = strlen(moves);
		for(int j=0;j<mLength;j++)
			list = addMoveToList(list, moves[j]);

		free(moves);
		delete newNode;
	}

	free(listBoxPos);
	free(listPusherPos);

	return list;
}

void DijkstraBox::dijkstraRec(int posBox, int dir, int value)
{
	// If pos is not a wall and value is better
	if(getPos(posBox, dir) != -1 && value < getPos(posBox, dir))
	{
		setPos(posBox, dir, value);
//		print();

		Zone* boxesZone = new Zone(node->getBoxesZone());
		const int* lToZPos = solver->getLevelToZonePos();

		// Delete initial position of box and write current position instead
		boxesZone->write0ToPos(lToZPos[boxPos]);
		boxesZone->write1ToPos(lToZPos[posBox]);

		// Neighbours and opposite neighbours of the box (level representation)
		int* lNei = (int*)malloc(4*sizeof(int));
		int* olNei = (int*)malloc(4*sizeof(int));
		lNei[0] = olNei[1] = posBox - 1; // Left
		lNei[1] = olNei[0] = posBox + 1; // Right
		lNei[2] = olNei[3] = posBox - nCols; // Up
		lNei[3] = olNei[2] = posBox + nCols; // Down

		// Neighbours and opposite neighbours of the box (zone representation)
		int* zNei = (int*)malloc(4*sizeof(int));
		int* ozNei = (int*)malloc(4*sizeof(int));
		for(int i=0;i<4;i++)
		{
			zNei[i] = lToZPos[lNei[i]];
			ozNei[i] = lToZPos[olNei[i]];
		}

		// If first iteration, we use pusherzone of node for possible moves
		Zone* reachableZone;
		if(value == 0)
			reachableZone = new Zone(node->getPusherZone());
		else
			reachableZone = new Zone(boxesZone, lNei[dir], lToZPos, nCols);

		// If position if reachable by pusher and is not a box,
		for(int i=0;i<4;i++)
		{
			if(zNei[i] != -1 && reachableZone->readPos(zNei[i])
					&& !boxesZone->readPos(zNei[i]))
			{
				lastCell->next = new CellList(olNei[i], i, value+1, NULL);
				lastCell = lastCell->next;
			}
		}

		delete reachableZone;
		delete boxesZone;
		free(lNei);
		free(olNei);
		free(zNei);
		free(ozNei);
	}
}

void DijkstraBox::print()
{
	const Level* level = solver->getLevel();
	int nRows = level->getRowsNumber();

	for(int i=0;i<nRows;i++)
	{
		for(int j=0;j<nCols;j++)
		{
			int val = getPos(i*nCols+j, 0);

			if(val == INT_MAX)
				val = -99;
			printf("%3d", val);
		}
		printf("\n");
	}

	printf("\n");
	printf("\n");
	printf("\n");

	for(int i=0;i<nRows;i++)
	{
		for(int j=0;j<nCols;j++)
		{
			int val = getPos(i*nCols+j, 1);

			if(val == INT_MAX)
				val = -99;
			printf("%3d", val);
		}
		printf("\n");
	}

	printf("\n");
	printf("\n");
	printf("\n");

	for(int i=0;i<nRows;i++)
	{
		for(int j=0;j<nCols;j++)
		{
			int val = getPos(i*nCols+j, 2);

			if(val == INT_MAX)
				val = -99;
			printf("%3d", val);
		}
		printf("\n");
	}

	printf("\n");
	printf("\n");
	printf("\n");

	for(int i=0;i<nRows;i++)
	{
		for(int j=0;j<nCols;j++)
		{
			int val = getPos(i*nCols+j, 3);

			if(val == INT_MAX)
				val = -99;
			printf("%3d", val);
		}
		printf("\n");
	}

	printf("\n");
	printf("\n");
	printf("\n");
}

int* DijkstraBox::listBoxPosToPosition(int position)
{
	int actualPos = position;
	int actualPosCost = min(getPos(position, 0), getPos(position, 1),
				getPos(position, 2), getPos(position, 3));
	int* list = (int*)malloc((actualPosCost+1)*sizeof(int));
	list[actualPosCost] = actualPos;

	while(actualPosCost != 0)
	{
		int* canPushFrom = (int*)malloc(4*sizeof(int));
		canPushFrom[LEFT]  = getPos(actualPos, LEFT);
		canPushFrom[RIGHT] = getPos(actualPos, RIGHT);
		canPushFrom[UP]    = getPos(actualPos, UP);
		canPushFrom[DOWN]  = getPos(actualPos, DOWN);

		int* lNeig = (int*)malloc(4*sizeof(int));
		lNeig[0] = actualPos - 1;
		lNeig[1] = actualPos + 1;
		lNeig[2] = actualPos - nCols;
		lNeig[3] = actualPos + nCols;

		// For each neighbours of actual position
		for(int i=0;i<4;i++)
		{
			// If box can be pushed from this neighbour position
			if(canPushFrom[i] == actualPosCost)
			{
				// If one cell of this neighbour is egal to new cost
				int newCost = actualPosCost - 1;
				if(   getPos(lNeig[i], LEFT)  == newCost
				   || getPos(lNeig[i], RIGHT) == newCost
				   || getPos(lNeig[i], UP)    == newCost
				   || getPos(lNeig[i], DOWN)  == newCost)
				{
					actualPos = lNeig[i];
					actualPosCost--;
					list[actualPosCost] = actualPos;
					break;
				}
			}
		}

		free(lNeig);
		free(canPushFrom);
	}

	return list;
}

int* DijkstraBox::listPusherPrepos(int* list, int listLength)
{
	int* pusherList = (int*)malloc((listLength-1)*sizeof(int));

	for(int i=0;i<listLength-1;i++)
	{
		if(list[i]+1 == list[i+1])
			pusherList[i] = list[i]-1;
		else if(list[i]-1 == list[i+1])
			pusherList[i] = list[i]+1;
		else if(list[i]+nCols == list[i+1])
			pusherList[i] = list[i]-nCols;
		else if(list[i]-nCols == list[i+1])
			pusherList[i] = list[i]+nCols;
	}

	return pusherList;
}

char* DijkstraBox::addMoveToList(char* list, char move)
{
	int length = strlen(list);
	list = (char*)realloc(list, (length+2)*sizeof(char));
	list[length] = move;
	list[length+1] = '\0';

	return list;
}

int DijkstraBox::min(int a, int b, int c, int d)
{
	int min = a;
	if(b < min)
		min = b;
	if(c < min)
		min = c;
	if(d < min)
		min = d;

	return min;
}
