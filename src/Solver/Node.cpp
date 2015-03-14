/*----------------------------------------------*/
/*================== ISokoban ==================*/
/*----------------------------------------------*/
/* Version : 1.0								*/
/* Author : Michaël Hoste						*/
/* Copyright : See LICENSE file for details   	*/
/*----------------------------------------------*/

#include "../../include/Solver/Node.h"

/* ------------*/
/* Constructor */
/* ------------*/
Node::Node(const Solver* solver):
	solver(solver),
	pusherZone(NULL),
	boxesZone(NULL)
{
// We don't create those automatically, we assign zone after creation with
// setPusherZone and setBoxesZone
}

Node::Node(const Solver* solver, Zone* pusherZone, Zone* boxesZone):
	solver(solver),
	pusherZone(pusherZone),
	boxesZone(boxesZone)
{

}

Node::Node(const Node* otherNode):
	solver(otherNode->getSolver()),
	pusherZone(new Zone(otherNode->getPusherZone())),
	boxesZone(new Zone(otherNode->getBoxesZone()))
{

}

/* -----------*/
/* Destructor */
/* -----------*/
Node::~Node()
{
	if(pusherZone)
		delete pusherZone;
	if(boxesZone)
		delete boxesZone;
}

/* -------*/
/* Others */
/* -------*/
bool Node::isEgal(Node* otherNode) const
{
	int nCells = boxesZone->getNumberCell();

	Zone* otherBZone = otherNode->getBoxesZone();
	Zone* otherPZone = otherNode->getPusherZone();

	for(int i=0;i<nCells;i++)
	{
		if(otherBZone->getZoneTab()[i] != boxesZone->getZoneTab()[i]
		|| otherPZone->getZoneTab()[i] != pusherZone->getZoneTab()[i])
			return false;
	}

	return true;
}

bool Node::isSolution(void) const
{
	return boxesZone->isIncludedIn(solver->getGoalZone());
}

Node** Node::findChildren(void) const
{
	// Position of neighbours of processed cell
	int pos, posLeft, posRight, posUp, posDown;

	// Neighbours zones to be assigned if not NULL at the end of the loop
	Zone *leftMovePusherZone, *leftMoveBoxesZone;
	Zone *rightMovePusherZone, *rightMoveBoxesZone;
	Zone *upMovePusherZone, *upMoveBoxesZone;
	Zone *downMovePusherZone, *downMoveBoxesZone;

	// Initialization of children list
	int nChildren = 0;
	Node** children = (Node**)malloc((nChildren + 1)*sizeof(Node*));
	children[nChildren] = NULL;

	const int* LtoZPos = solver->getLevelToZonePos();
	const int* ZtoLPos = solver->getZoneToLevelPos();
	const int levelColsNumber = solver->getLevel()->getColsNumber();

	// Create a zone with every boxes the pusher can reach
	Zone* accessibleBoxes = new Zone(pusherZone, boxesZone, Zone::USE_AND);
	int length = accessibleBoxes->getLength();

	// For each bit
	for(int i=0;i<length;i++)
	{
		// If there is a box in this position that can be reached
		if(accessibleBoxes->readPos(i) == 1)
		{
			leftMovePusherZone = leftMoveBoxesZone = NULL;
			rightMovePusherZone = rightMoveBoxesZone = NULL;
			upMovePusherZone = upMoveBoxesZone = NULL;
			downMovePusherZone = downMoveBoxesZone = NULL;

			pos = ZtoLPos[i];
			// Positions in "level" representation
			posLeft = LtoZPos[pos-1];
			posRight = LtoZPos[pos+1];
			posUp = LtoZPos[pos-levelColsNumber];
			posDown = LtoZPos[pos+levelColsNumber];

			// Test where you can push it (if you can)
			if(posLeft != -1 && posRight != -1)
			{
				// If you can push the box from left to right
				if( pusherZone->readPos(posLeft)
				&& !boxesZone->readPos(posLeft)
				&& !boxesZone->readPos(posRight))
				{
					// Create corresponding new Zones
					rightMoveBoxesZone = new Zone(this->boxesZone);
					rightMoveBoxesZone->write1ToPos(posRight);
					rightMoveBoxesZone->write0ToPos(i);

					rightMovePusherZone = new Zone(rightMoveBoxesZone, pos,
							LtoZPos, levelColsNumber);

					// Add children to the list
					nChildren++;
					children = (Node**)realloc(children,
											   (nChildren + 1)*sizeof(Node*));
					children[nChildren] = NULL;
					children[nChildren-1] = new Node(solver);
					children[nChildren-1]->setBoxesZone(rightMoveBoxesZone);
					children[nChildren-1]->setPusherZone(rightMovePusherZone);
				}
				// If you can push the box from right to left
				if( pusherZone->readPos(posRight)
				&& !boxesZone->readPos(posRight)
				&& !boxesZone->readPos(posLeft))
				{
					// Create corresponding new Zones
					leftMoveBoxesZone = new Zone(this->boxesZone);
					leftMoveBoxesZone->write1ToPos(posLeft);
					leftMoveBoxesZone->write0ToPos(i);

					leftMovePusherZone = new Zone(leftMoveBoxesZone, pos,
							LtoZPos, levelColsNumber);

					// Add children to the list
					nChildren++;
					children = (Node**)realloc(children,
											   (nChildren + 1)*sizeof(Node*));
					children[nChildren] = NULL;
					children[nChildren-1] = new Node(solver);
					children[nChildren-1]->setBoxesZone(leftMoveBoxesZone);
					children[nChildren-1]->setPusherZone(leftMovePusherZone);
				}
			}
			if(posUp != -1 && posDown != -1)
			{
				// If you can push the box from up to down
				if( pusherZone->readPos(posUp)
				&& !boxesZone->readPos(posUp)
				&& !boxesZone->readPos(posDown))
				{
					// Create corresponding new Zones
					downMoveBoxesZone = new Zone(this->boxesZone);
					downMoveBoxesZone->write1ToPos(posDown);
					downMoveBoxesZone->write0ToPos(i);

					downMovePusherZone = new Zone(downMoveBoxesZone, pos,
							LtoZPos, levelColsNumber);

					// Add children to the list
					nChildren++;
					children = (Node**)realloc(children,
											   (nChildren + 1)*sizeof(Node*));
					children[nChildren] = NULL;
					children[nChildren-1] = new Node(solver);
					children[nChildren-1]->setBoxesZone(downMoveBoxesZone);
					children[nChildren-1]->setPusherZone(downMovePusherZone);
				}
				// If you can push the box from down to up
				if( pusherZone->readPos(posDown)
				&& !boxesZone->readPos(posDown)
				&& !boxesZone->readPos(posUp))
				{
					// Create corresponding new Zones
					upMoveBoxesZone = new Zone(this->boxesZone);
					upMoveBoxesZone->write1ToPos(posUp);
					upMoveBoxesZone->write0ToPos(i);

					upMovePusherZone = new Zone(upMoveBoxesZone, pos,
							LtoZPos, levelColsNumber);

					// Add children to the list
					nChildren++;
					children = (Node**)realloc(children,
											   (nChildren + 1)*sizeof(Node*));
					children[nChildren] = NULL;
					children[nChildren-1] = new Node(solver);
					children[nChildren-1]->setBoxesZone(upMoveBoxesZone);
					children[nChildren-1]->setPusherZone(upMovePusherZone);
				}
			}
		}
	}

	delete accessibleBoxes;

	return children;
}

Child** Node::findMacroChildren(Node** nodeList)
{
	// Get some informations on nodes already present into the list
	// and initialize their gCost to 1 (no macro : simple push)
	int nodeListLength = 0;
	while(nodeList != NULL && nodeList[nodeListLength] != NULL)
		nodeListLength++;

	Child** children = (Child**)malloc((nodeListLength+1)*sizeof(Child*));
	for(int i=0;i<nodeListLength;i++)
		children[i] = new Child(nodeList[i], 0);
	children[nodeListLength] = NULL;
	free(nodeList);

	Macro** macroList = (Macro**)malloc(sizeof(Macro*));
	int macroListLength = 0;

	const int* zToLPos = solver->getZoneToLevelPos();
	const int* lToZPos = solver->getLevelToZonePos();
	int nCols = solver->getLevel()->getColsNumber();
	int numOfGoals = solver->getLevel()->getGoalsNumber();

	// Get cost of every goals for this node. A cost of a goal is the number of
	// boxes or walls just next this goal. cost go from 0 to 4. 4 is a top
	// priority goal
	int* costOfGoals = computeCostOfGoals(this);

	// Create a zone with every boxes the pusher can reach
	Zone* accessibleBoxes = new Zone(pusherZone, boxesZone, Zone::USE_AND);

	// Test each accessible box of this node and save
	// every possible macro informations
	int length = accessibleBoxes->getLength();
	for(int i=0;i<length;i++)
	{
		if(accessibleBoxes->readPos(i) == 1)
		{
			DijkstraBox* dBox = new DijkstraBox(solver, this, zToLPos[i]);
			int* lastPositions = (int*)malloc(numOfGoals*sizeof(int));
			int* numOfPushes = dBox->resolveGoals(lastPositions);

			for(int j=0;j<numOfGoals;j++)
			{
				// If goal can be reached by the box and box is not on goal
				if(  numOfPushes[j] != INT_MAX && numOfPushes[j] != -1
				  && i != solver->getGoalsPositions()[j])
				{
					// Save information on this macro on macroList
					macroListLength++;
					int l = macroListLength;
					macroList = (Macro**)realloc(macroList,
							l*sizeof(Macro*));
					macroList[l-1] = new Macro();
					macroList[l-1]->boxOldPosition = i;
					macroList[l-1]->boxNewPosition =
											solver->getGoalsPositions()[j];
					macroList[l-1]->pusherNewPosition = lastPositions[j];
					macroList[l-1]->numberOfPushes = numOfPushes[j];
					macroList[l-1]->costOfGoal = costOfGoals[j];
				}
			}

			delete dBox;
			free(numOfPushes);
			free(lastPositions);
		}
	}

	delete accessibleBoxes;

	// Now we have all macro informations we want
	// We choose to keep only nodes with box on goals with maximum costs.
	int maxGoalCost = 0;
	for(int i=0;i<macroListLength;i++)
	{
		if(macroList[i]->costOfGoal > maxGoalCost)
			maxGoalCost = macroList[i]->costOfGoal;
	}

	for(int i=0;i<macroListLength;i++)
	{
		if(macroList[i]->costOfGoal == maxGoalCost)
		{
			// Generation of new node
			Zone* newBoxesZone = new Zone(this->getBoxesZone());
			newBoxesZone->write0ToPos(macroList[i]->boxOldPosition);
			newBoxesZone->write1ToPos(macroList[i]->boxNewPosition);
			Zone* newPusherZone = new Zone(newBoxesZone,
					macroList[i]->pusherNewPosition, lToZPos, nCols);
			Node* newNode = new Node(solver, newPusherZone, newBoxesZone);

			// Save new node
			nodeListLength++;
			children = (Child**)realloc(children,
									  (nodeListLength+1)*sizeof(Child*));
			children[nodeListLength-1] = new Child(newNode,
					macroList[i]->numberOfPushes);
			children[nodeListLength] = NULL;
		}
	}

	for(int i=0;i<macroListLength;i++)
		delete macroList[i];
	free(macroList);
	free(costOfGoals);

	return children;
}

char* Node::listMovesFromAToB(const int posA, const int posB) const
{
	char* moves = (char*)malloc(sizeof(char));
	moves[0]='\0';

	const int* LToZPos = solver->getLevelToZonePos();

	// 1. Make a tab from this node with "fixed" positions (boxes, walls) to
	//    -1 and others pos to a big number
	int m = solver->getLevel()->getRowsNumber();
	int n = solver->getLevel()->getColsNumber();
	int* tempLevel = (int*)malloc(m*n*sizeof(int));

	for(int i=0;i<m*n;i++)
	{
		char c = solver->getLevel()->readPos(i);
		if(c == ' ' || c == '#')
			tempLevel[i] = -1;
		else
		{
			if(boxesZone->readPos(LToZPos[i]) == 1)
				tempLevel[i] = -1;
			else
				tempLevel[i] = INT_MAX; // big number
		}
	}

	// 2. Fill the tab in with numbers. Numbers represents minimum distance
	//    between respective cell and posA.
	tempLevel[posA] = 0;
	dijkstraRec(tempLevel, posA, n);

	// 3. Start with PosB, find correct neighbours and use them to find a way
	//    until posA. (ex: posB is 8, following cells must be 7,6,5,...)
	int tempPos = posB;
	int tempValue = tempLevel[posB];
	if(tempValue == INT_MAX || tempValue == -1)
	{
		printf("Node :: Moves from A (%d) to B (%d) are not possible\n",
				posA, posB);
		this->print();
		exit(-1);
	}
	moves = (char*)realloc(moves, (tempValue+1)*sizeof(char));
	int movesLength = tempValue+1;
	moves[tempValue]='\0';

	while(tempPos != posA)
	{
		movesLength--;

		// left
		if(tempLevel[tempPos-1] == tempValue-1)
		{
			tempPos = tempPos-1;
			tempValue = tempValue-1;

			moves[movesLength-1]='r';
		}
		// right
		else if(tempLevel[tempPos+1] == tempValue-1)
		{
			tempPos = tempPos+1;
			tempValue = tempValue-1;

			moves[movesLength-1]='l';
		}
		// up
		else if(tempLevel[tempPos-n] == tempValue-1)
		{
			tempPos = tempPos-n;
			tempValue = tempValue-1;

			moves[movesLength-1]='d';
		}
		// down
		else if(tempLevel[tempPos+n] == tempValue-1)
		{
			tempPos = tempPos+n;
			tempValue = tempValue-1;

			moves[movesLength-1]='u';
		}
	}

	free(tempLevel);

	return moves;
}

int Node::countMovesFromAToB(const int posA, const int posB) const
{
	const int* LToZPos = solver->getLevelToZonePos();

	// 1. Make a tab from this node with "fixed" positions (boxes, walls) to
	//    -1 and others pos to a big number
	int m = solver->getLevel()->getRowsNumber();
	int n = solver->getLevel()->getColsNumber();
	int* tempLevel = (int*)malloc(m*n*sizeof(int));

	for(int i=0;i<m*n;i++)
	{
		char c = solver->getLevel()->readPos(i);

		if(c == ' ' || c == '#')
			tempLevel[i] = -1;
		else
		{
			if(boxesZone->readPos(LToZPos[i]) == 1)
				tempLevel[i] = -1;
			else
				tempLevel[i] = INT_MAX; // big number
		}
	}

	// 2. Fill the tab in with numbers. Numbers represents minimum distance
	//    between respective cell and posA.
	tempLevel[posA] = 0;
	dijkstraRec(tempLevel, posA, n);

	int value = tempLevel[posB];

	free(tempLevel);

	return value;
}

void Node::print(void) const
{
	printf("Boxes Positions - Pusher Positions\n");
	printf("--------------------------------------\n");

	const int* levelToZonePos = solver->getLevelToZonePos();

	Level* copyB = new Level(NULL, solver->getLevel());
	int m = copyB->getRowsNumber();
	int n = copyB->getColsNumber();
	for(int i=0;i<m*n;i++)
	{
		int c = copyB->readPos(i);
		if(c == '$' || c == '@')
			copyB->writePos(i, ' ');
		else if(c == '*' || c == '+')
			copyB->writePos(i, '.');

		if(levelToZonePos[i] != -1)
		{
			if(boxesZone->readPos(levelToZonePos[i]) == 1)
				copyB->writePos(i, 'O');
		}
	}

	Level* copyP = new Level(NULL, solver->getLevel());
	for(int i=0;i<m*n;i++)
	{
		int c = copyP->readPos(i);
		if(c == '$' || c == '@')
			copyP->writePos(i, ' ');
		else if(c == '*' || c == '+')
			copyP->writePos(i, '.');

		if(levelToZonePos[i] != -1)
		{
			if(pusherZone->readPos(levelToZonePos[i]) == 1)
				copyP->writePos(i, 'O');
		}
	}

	/* Double loop used to print every symbols of this level */
	for(int j=0;j<m;j++)
	{
		// Boxes
		for(int k=n*j;k<n*(j+1);k++)
		{
			/* We change 's' in ' ' for better lisibility */
			if(copyB->readPos(k) == 's')
				printf(" ");
			else
				printf("%c", copyB->readPos(k));
		}
		printf("    ");
		// Pusher
		for(int k=n*j;k<n*(j+1);k++)
		{
			/* We change 's' in ' ' for better lisibility */
			if(copyP->readPos(k) == 's')
				printf(" ");
			else
				printf("%c", copyP->readPos(k));
		}
		printf("\n");
	}

	delete copyB;
	delete copyP;
}

void Node::dijkstraRec(int* levelTab, const int startPos,
		const int colsNumber) const
{
	int dist = levelTab[startPos]+1;
	int posUp = startPos - colsNumber;
	int posDown = startPos + colsNumber;
	int posLeft = startPos - 1;
	int posRight = startPos + 1;
	int cellUp = levelTab[posUp];
	int cellDown = levelTab[posDown];
	int cellLeft = levelTab[posLeft];
	int cellRight = levelTab[posRight];

	// Up
	if(cellUp != -1 && cellUp > dist)
	{
		levelTab[posUp] = dist;
		dijkstraRec(levelTab, posUp, colsNumber);
	}
	// Down
	if(cellDown != -1 && cellDown > dist)
	{
		levelTab[posDown] = dist;
		dijkstraRec(levelTab, posDown, colsNumber);
	}
	// Left
	if(cellLeft != -1 && cellLeft > dist)
	{
		levelTab[posLeft] = dist;
		dijkstraRec(levelTab, posLeft, colsNumber);
	}
	// Right
	if(cellRight != -1 && cellRight > dist)
	{
		levelTab[posRight] = dist;
		dijkstraRec(levelTab, posRight, colsNumber);
	}
}

int* Node::computeCostOfGoals(Node* node)
{
	const int* zToLPos = solver->getZoneToLevelPos();
	const int* lToZPos = solver->getLevelToZonePos();
	int nCols = solver->getLevel()->getColsNumber();

	int numOfGoals = solver->getLevel()->getGoalsNumber();
	int* costOfGoals = (int*)malloc(numOfGoals*sizeof(int));
	for(int i=0;i<numOfGoals;i++)
	{
		costOfGoals[i] = 0;
		int zPos = solver->getGoalsPositions()[i];
		int lPos = zToLPos[zPos];

		// Get neighbours positions of goal
		int* lNeig = (int*)malloc(4*sizeof(int));
		int* zNeig = (int*)malloc(4*sizeof(int));
		lNeig[0] = lPos + 1;
		lNeig[1] = lPos - 1;
		lNeig[2] = lPos + nCols;
		lNeig[3] = lPos - nCols;
		for(int j=0;j<4;j++)
			zNeig[j] = lToZPos[lNeig[j]];

		// Test neighbours positions
		for(int j=0;j<4;j++)
		{
			char cell = solver->getLevel()->readPos(lNeig[j]);
			if(cell == '#')
				costOfGoals[i]++;
			else
			{
				// If there is a box on a neighbour position
				if(node->getBoxesZone()->readPos(zNeig[j]) == 1)
					costOfGoals[i]++;
			}
		}

		// test vertical and horizontal move
		for(int j=0;j<2;j=j+2)
		{
			char cell1 = solver->getLevel()->readPos(lNeig[j]);
			char cell2 = solver->getLevel()->readPos(lNeig[j+1]);
			if( (cell1 == '#' || node->getBoxesZone()->readPos(zNeig[j]) == 1)
			&& (cell2 == '#' || node->getBoxesZone()->readPos(zNeig[j+1]) == 1))
				costOfGoals[i]++;
		}

		free(lNeig);
		free(zNeig);
	}

	return costOfGoals;
}
