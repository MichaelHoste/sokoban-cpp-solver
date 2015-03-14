/*----------------------------------------------*/
/*================== ISokoban ==================*/
/*----------------------------------------------*/
/* Version : 1.0								*/
/* Author : Michaël Hoste						*/
/* Copyright : See LICENSE file for details   	*/
/*----------------------------------------------*/

#include "../../../include/Solver/BotBestPushesS/BotBestPushesS.h"
#include "../../../include/Solver/BotBestPushesS/BotBestPushesS_Penalties.h"

//TODO Bad include but it's working, try to do it in another way
#include "BotBestPushesS_Matrix.cpp"

/* ------------*/
/* Constructor */
/* ------------*/
BotBestPushesS::BotBestPushesS(Base* base, Level* level, int maxNodeNumber,
		int maxRamSize, int openTableSize, int closeTableSize, int costLimit,
		int deadlockedBoxesSearch, bool onlyPushNumber, bool quickSearch)
	: BotA(base, level, maxNodeNumber, maxRamSize, openTableSize,
			closeTableSize, costLimit, deadlockedBoxesSearch, onlyPushNumber,
			quickSearch),
	  costTable(NULL),
	  penaltiesEngine(NULL)

{
	// Create empty level (without boxes or pusher)
	emptyLevel = new Level(base, this->level);
	int nCols = emptyLevel->getColsNumber();
	int nRows = emptyLevel->getRowsNumber();
	for(int i=0;i<nRows*nCols;i++)
	{
		char c = emptyLevel->readPos(i);
		if(c == '$' || c == '@')
			emptyLevel->writePos(i, 's');
		else if(c == '*' || c == '+')
			emptyLevel->writePos(i, '.');
	}
	emptyLevel->setBoxesNumber(0);
}

/* -----------*/
/* Destructor */
/* -----------*/
BotBestPushesS::~BotBestPushesS()
{
	for(int i=0;i<zoneToLevelPosLength;i++)
		free(costTable[i]);
	free(costTable);
	delete penaltiesEngine;
	if(emptyLevel)
		delete emptyLevel;
}

/* -------*/
/* Others */
/* -------*/
void BotBestPushesS::initialize(void)
{
	// Create translation tables between zone and level
	initZoneToLevelPos();
	initLevelToZonePos();

	// Initialize fixed deadlock and goals Zones
	initGoalZone();
	initDeadlockEngine();

	// Initialize hash table
	initHashTable();

	// Initialize waiting node list
	initCloseNodeList();

	// Initialize cost table
	costTable = initCostTable();

	// Initialize penalties
	penaltiesEngine = new BotBestPushesS_Penalties(this);

	// Test deductive positions (if exists and asked)
	if(!onlyPushNumber)
		processDeductive();

	// Init starting TreeNode
	initStartingNode();
}

TreeNode* BotBestPushesS::createTreeNode(Node* node, TreeNode* parentTreeNode,
		int pushCost)
{
	TreeNode* newTreeNode = new BotA_TreeNode(node);

	if(parentTreeNode)
		parentTreeNode->addChild(newTreeNode);

	int costG = g(newTreeNode, pushCost);
	int costH = h(newTreeNode);
	int costF;
	if(costH == INT_MAX)
		costF = INT_MAX;
	else
		costF = costG + costH;
	((BotA_TreeNode*)newTreeNode)->setG(costG);
	((BotA_TreeNode*)newTreeNode)->setF(costF);

	// If search need to be more accurate
	if(!onlyPushNumber && costF <= costLimit &&
			!deadlockEngine->deadlockedNode(newTreeNode))
	{
		// If root treenode, we test every boxes for penalties and deadlocks
		if(!quickSearch && parentTreeNode == NULL)
		{
			int length = node->getBoxesZone()->getLength();
			for(int i=0;i<length;i++)
			{
				if(node->getBoxesZone()->readPos(i) == 1)
					analyzeNode(node, zoneToLevelPos[i], 5000);
			}
		}
		// If not, we only test last pushed box if a zone was created
		else if(!quickSearch && parentTreeNode != NULL)
		{
			int lastBox = newTreeNode->getPushedBoxPostPosition();
			analyzeNode(node, lastBox, 2000+(nodeNumber/100));
		}

		if(quickSearch && nodeNumber%100000 == 0)
		{
			int length = node->getBoxesZone()->getLength();
			for(int i=0;i<length;i++)
			{
				if(node->getBoxesZone()->readPos(i) == 1)
					analyzeNode(node, zoneToLevelPos[i], 2000);
			}
		}

		// re-compute cost to take advantage of new analyze
		costG = g(newTreeNode, pushCost);
		costH = h(newTreeNode);
		if(costH == INT_MAX)
			costF = INT_MAX;
		else
			costF = costG + costH;
		((BotA_TreeNode*)newTreeNode)->setG(costG);
		((BotA_TreeNode*)newTreeNode)->setF(costF);
	}

	return newTreeNode;
}

void BotBestPushesS::analyzeNode(const Node* node, int boxLevelPos, int maxNodes)
{
	// new node to be modified
	Node* newNode = new Node(node);
	newNode->getBoxesZone()->write0ToPos(levelToZonePos[boxLevelPos]);

	// Init position of first box
	int* boxPos = (int*)malloc(sizeof(int));
	int boxPosLength = 1;
	boxPos[0] = levelToZonePos[boxLevelPos];

	bool usefulBoxes = true;
	bool overCost = false;
	int numOfNodes = maxNodes;

	// Init pusher's position
	Level* testLevel = new Level(base, emptyLevel);
	int nRows = level->getRowsNumber();
	int nCols = level->getColsNumber();
	int length = node->getBoxesZone()->getLength();
	int lPos = -1;
	for(int i=0;i<length;i++)
	{
		if(    node->getPusherZone()->readPos(i) == 1
			&& node->getBoxesZone()->readPos(i) == 0
			&& ( boxLevelPos == zoneToLevelPos[i] + 1
			  || boxLevelPos == zoneToLevelPos[i] - 1
			  || boxLevelPos == zoneToLevelPos[i] + nCols
			  || boxLevelPos == zoneToLevelPos[i] - nCols))
		{
			lPos = zoneToLevelPos[i];
			if(testLevel->readPos(lPos) == 's')
				testLevel->writePos(zoneToLevelPos[i], '@');
			else if(testLevel->readPos(lPos) == '.')
				testLevel->writePos(zoneToLevelPos[i], '+');

			testLevel->setPusherPosM(lPos/nCols);
			testLevel->setPusherPosN(lPos%nCols);

			break;
		}
	}

	// loop on the useful boxes
	while(usefulBoxes && !overCost && numOfNodes > 0)
	{
		// Creation of level
		int pos = zoneToLevelPos[boxPos[boxPosLength-1]];
		if(testLevel->readPos(pos) == 's')
			testLevel->writePos(pos, '$');
		else if(testLevel->readPos(pos) == '.')
			testLevel->writePos(pos, '*');
		testLevel->setBoxesNumber(boxPosLength);

		// Test deadlock and penalties of current box positions
		if(numOfNodes > 1)
			penaltiesEngine->penaltiesListTest(boxPos, boxPosLength,
					&numOfNodes, lPos);

		// Initialize pusherSteps and boxesSteps variables
		int** pusherSteps = (int**)malloc(nRows*sizeof(int*));
		int** boxesSteps  = (int**)malloc(nRows*sizeof(int*));
		for(int i=0;i<nRows;i++)
		{
			pusherSteps[i] = (int*)malloc(nCols*sizeof(int));
			boxesSteps[i]  = (int*)malloc(nCols*sizeof(int));
		}
		for(int i=0;i<nRows;i++)
			for(int j=0;j<nCols;j++)
				pusherSteps[i][j] = boxesSteps[i][j] = -1;

		if(numOfNodes < 0)
			numOfNodes = 1;

		// Use of solver to detect actual best pushes to solution
		Solver* newSolver = new BotBestPushesS(base, testLevel,
				numOfNodes, ramSize, 503, 251, costLimit,
				deadlockedBoxesSearch, true);
		newSolver->resolve();

		char* message = (char*)malloc(255*sizeof(char));
		const Stats* stats = newSolver->getStats();
		if(stats)
			message = strcpy(message, stats->getMessage());

		// If resolved, we get useful positions
		if(!notResolved(message) && !ramLimitReached(message) && stats != NULL)
		{
			numOfNodes -= newSolver->getNodeNumber();
			newSolver->computeSteps(pusherSteps, boxesSteps);

			int nextBox = findNextBoxToTest(newNode, pusherSteps, boxesSteps);
			if(nextBox != -1)
			{
				newNode->getBoxesZone()->write0ToPos(levelToZonePos[nextBox]);
				boxPosLength++;
				boxPos = (int*)realloc(boxPos, boxPosLength*sizeof(int));
				boxPos[boxPosLength-1] = levelToZonePos[nextBox];
			}
			else
				usefulBoxes = false;
		}
		else
			overCost = true;

		delete newSolver;
		free(message);

		// Delete pusherSteps and boxesSteps variables
		for(int i=0;i<nRows;i++)
		{
			free(pusherSteps[i]);
			free(boxesSteps[i]);
		}
		free(pusherSteps);
		free(boxesSteps);
	}

	delete testLevel;
	free(boxPos);
	delete newNode;
}

int BotBestPushesS::findNextBoxToTest(const Node* node, int** pusherSteps,
		int** boxesSteps)
{
	int nRows = level->getRowsNumber();
	int nCols = level->getColsNumber();
	int foundBox = -1;
	int bestCost = INT_MAX;

	// Test of boxesSteps to find new boxes to add
	for(int i=0;i<nRows;i++)
	{
		for(int j=0;j<nCols;j++)
		{
			// If position is used, beter cost than before and got a box
			// in the node
			if(boxesSteps[i][j] != -1 && boxesSteps[i][j] < bestCost
				&& node->getBoxesZone()->readPos(levelToZonePos[i*nCols+j]) == 1)
			{
				bestCost = boxesSteps[i][j];
				foundBox = i*nCols+j;
			}
		}
	}

	// if no more boxes in boxesSteps, we'll try pusherSteps
	if(foundBox == -1)
	{
		for(int i=0;i<nRows;i++)
		{
			for(int j=0;j<nCols;j++)
			{
				// If position is used, beter cost than before and got a box
				// in the node
				if(pusherSteps[i][j] != -1 && pusherSteps[i][j] < bestCost
					&& node->getBoxesZone()->readPos(levelToZonePos[i*nCols+j]) == 1)
				{
					bestCost = pusherSteps[i][j];
					foundBox = i*nCols+j;
				}
			}
		}
	}

	return foundBox;
}

bool BotBestPushesS::ramLimitReached(char* message) const
{
	return
		(strcmp(message,
				"Impossible to solve : Ram Limit or Max Nodes Limit reached")
					== 0);
}

bool BotBestPushesS::notResolved(char* message) const
{
	return(
		(strcmp(message,
				"Impossible to solve : No more nodes in waiting list")
					== 0)
	||  (strcmp(message,
				"Impossible to solve : first node has no child")
					== 0));
}

void BotBestPushesS::processDeductive()
{
	// Load file
	const char* packName = level->getPackName();
	int levelId = level->getId();
	char** file = (char**)malloc(6*sizeof(char*));
	for(int i=0;i<5;i++)
	{
		file[i] = (char*)malloc(300*sizeof(char));
		file[i][0] = '\0';
	}
	strcpy(file[0], "data");
	strcpy(file[1], "solver");
	sprintf(file[2], "%s", packName);
	sprintf(file[3], "%d", levelId);
	strcpy(file[4], "deductivePositions.dat");
	file[5] = NULL;
	char* fileName = Util::generateRep(file);

	if(Util::isThisFileExists(fileName))
	{
		FILE* ffile = fopen(fileName, "r");

		int dedNumber = 0;
		int** ded = (int**)malloc(1*sizeof(int*));
		ded[0] = NULL;
		while(!feof(ffile))
		{
			dedNumber++;
			ded = (int**)realloc(ded, (dedNumber+1)*sizeof(int*));
			ded[dedNumber] = NULL;

			int nCells;
			fscanf(ffile, "%d\n", &nCells);
			ded[dedNumber-1] = (int*)malloc((nCells+1)*sizeof(int));
			ded[dedNumber-1][nCells] = -1;

			for(int i=0;i<nCells;i++)
				fscanf(ffile, "%d", &(ded[dedNumber-1][i]));

			fscanf(ffile, "\n");
		}
		fclose(ffile);

		int count = 250;
		for(int i=0;i<dedNumber;i++)
		{
			int numOfBoxes = 0;
			while(ded[i][numOfBoxes] != -1)
				numOfBoxes++;

//			printf("deadlock deductive test : ");
//			for(int j=0;j<numOfBoxes;j++)
//				printf("%d | ", ded[i][j]);
//			printf("\n");
//
//			deadlockEngine->deadlockListTest(ded[i], numOfBoxes);

			printf("penalty deductive test : ");
			for(int j=0;j<numOfBoxes;j++)
				printf("%d | ", ded[i][j]);
			printf("\n");

			penaltiesEngine->penaltiesListTest(ded[i], numOfBoxes);

			count--;
			if(count == 0)
			{
				FILE* ffile = fopen(fileName, "w");
				for(int j=i;j<dedNumber;j++)
				{
					int numOfCells = 0;
					while(ded[j][numOfCells] != -1)
						numOfCells++;

					fprintf(ffile, "%d\n", numOfCells);
					for(int k=0;k<numOfCells;k++)
						fprintf(ffile, "%d ", ded[j][k]);
					fprintf(ffile, "\n");
				}
				fclose(ffile);
				count = 250;
			}
		}

		for(int i=0;i<dedNumber;i++)
			free(ded[i]);
		free(ded);
	}

	for(int i=0;i<5;i++)
		free(file[i]);
	free(file);
	free(fileName);
}

int BotBestPushesS::f(TreeNode* treeNode, int pushCost) const
{
	int resH = h(treeNode);
	int resG = g(treeNode, pushCost);
	if(resH == INT_MAX || resG == INT_MAX)
		return INT_MAX;
	else
		return resH + resG;
}

int BotBestPushesS::g(TreeNode* treeNode, int pushCost) const
{
	if(treeNode->getParent() == NULL)
		return 0;
	else
	{
		TreeNode* parentNode = treeNode->getParent();

		// we get cost of parent
		int value = ((BotA_TreeNode*)parentNode)->getG();

		// We return cost of parent + cost of this node (always 1 push by node)
		if(pushCost == 0)
			return value+1;
		else
			return value+pushCost;
	}
}

int BotBestPushesS::h(TreeNode* treeNode) const
{
	int sum = 0;

	// We start by getting boxes positions (zone representation) in treeNode
	int* boxesPositions = (int*)malloc(level->getBoxesNumber()*sizeof(int));
	int* goalsPositions = (int*)malloc(level->getGoalsNumber()*sizeof(int));
	Zone* boxesZone = treeNode->getNode()->getBoxesZone();
	int numb = 0;
	int numg = 0;
	for(int i=0;i<zoneToLevelPosLength;i++)
	{
		if(boxesZone->readPos(i) == 1)
		{
			boxesPositions[numb] = i;
			numb++;
		}
		if(goalZone->readPos(i) == 1)
		{
			goalsPositions[numg] = i;
			numg++;
		}
	}

	BotBestPushesS_Matrix<double>* assignMatrix =
		createAssignationMatrix(boxesPositions, goalsPositions);

	// Apply Munkres algorithm to matrix.
	// lowerCostIndex[0] is best goal for first box, lowerCostIndex[1] is best
	// goal for second box, ...
	BotBestPushesS_Munkres m;
	int* lowerCostIndex = m.solve(*assignMatrix);

	for(int i=0;i<level->getBoxesNumber();i++)
	{
		int j = lowerCostIndex[i];
		int a = costTable[boxesPositions[i]][goalsPositions[j]];
		if(a == INT_MAX)
		{
			sum = INT_MAX;
			break;
		}
		sum += a;
	}

	delete assignMatrix;
	free(lowerCostIndex);
	free(boxesPositions);
	free(goalsPositions);

	int penalty = penaltiesEngine->getPenaltyOfTreeNode(treeNode);

//	printf("sum %d, penalty %d\n", sum, penalty);

	if(sum == INT_MAX || penalty == INT_MAX)
		return INT_MAX;
	else
		return sum+penalty;
}

int** BotBestPushesS::initCostTable()
{
	const char* packName = level->getPackName();
	int levelId = level->getId();

	char** file = (char**)malloc(6*sizeof(char*));
	for(int i=0;i<5;i++)
	{
		file[i] = (char*)malloc(300*sizeof(char));
		file[i][0] = '\0';
	}
	strcpy(file[0], "data");
	strcpy(file[1], "solver");
	sprintf(file[2], "%s", packName);
	sprintf(file[3], "%d", levelId);
	strcpy(file[4], "costTable.dat");
	file[5] = NULL;

	char* fileLine = Util::generateRep(file);

	int** cTable;
	if(Util::isThisFileExists(fileLine))
		cTable = loadCostTable(fileLine);
	else
		cTable = createCostTable();

	for(int i=0;i<5;i++)
		free(file[i]);
	free(file);
	free(fileLine);

	return cTable;
}

int** BotBestPushesS::createCostTable()
{
	int m = level->getRowsNumber();
	int n = level->getColsNumber();

	int zoneLength = 0;
	int* zonePos = (int*)malloc(sizeof(int));
	for(int i=0;i<m*n;i++)
	{
		char c = level->readPos(i);
		// Compute number of position in zone representation of level
		if(c != '#' && c != ' ')
		{
			zoneLength++;
			zonePos=(int*)realloc(zonePos, zoneLength*sizeof(int));
			zonePos[zoneLength-1] = i;
		}
	}

	// number of zone positions
	int** cTable;
	cTable = (int**)malloc(zoneLength*sizeof(int*));
	for(int i=0;i<zoneLength;i++)
		cTable[i] = (int*)malloc(zoneLength*sizeof(int));

	// Store values on table
	for(int i=0;i<zoneLength;i++)
	{
		// Start to get 4 neighbours cell of this box position
		//(level representation)
		int boxPos = zonePos[i];
		int* lNei = (int*)malloc(4*sizeof(int));
		lNei[0] = boxPos - 1; // Left
		lNei[1] = boxPos + 1; // Right
		lNei[2] = boxPos - level->getColsNumber(); // Up
		lNei[3] = boxPos + level->getColsNumber(); // Down

		// Create boxesZone with only 1box
		Zone* newBoxesZone = new Zone(goalZone->getLength());
		newBoxesZone->write1ToPos(levelToZonePos[zonePos[i]]);

		// Create node with complete pusherZone
		Zone* pusherZone = new Zone(goalZone->getLength());
		pusherZone->applyNot();
		Node* newNode = new Node(this, pusherZone, newBoxesZone);

		// Compute solutions
		DijkstraBox* dBox = new DijkstraBox(this, newNode, boxPos);
		int* sol = dBox->resolvePositions();
		delete dBox;

		for(int j=0;j<zoneLength;j++)
			cTable[i][j] = sol[j];

		printf("costTable Initialization [%.0f%%]\n",
				100/(float)zoneLength*i);

		free(sol);
		free(lNei);
		delete newNode;
	}

	free(zonePos);

	saveCostTable(cTable, zoneLength);

	return cTable;
}

int** BotBestPushesS::loadCostTable(char* fileName)
{
	int zoneLength;
	FILE* file = fopen(fileName, "r");

	fscanf(file, "%d\n", &zoneLength);

	int** cTable;
	cTable = (int**)malloc(zoneLength*sizeof(int*));
	for(int i=0;i<zoneLength;i++)
		cTable[i] = (int*)malloc(zoneLength*sizeof(int));

	for(int i=0;i<zoneLength;i++)
	{
		for(int j=0;j<zoneLength;j++)
		{
			fscanf(file, "%d ", &cTable[i][j]);
		}
		fscanf(file, "\n");
	}

	fclose(file);

	return cTable;
}

void BotBestPushesS::saveCostTable(int** cTable, int zoneLength)
{
	const char* packName = level->getPackName();
	int levelId = level->getId();

	char** file = (char**)malloc(6*sizeof(char*));
	for(int i=0;i<4;i++)
	{
		file[i] = (char*)malloc(300*sizeof(char));
		file[i][0] = '\0';
	}
	strcpy(file[0], "data");
	strcpy(file[1], "solver");
	strcpy(file[2], packName);
	sprintf(file[3], "%d", levelId);
	file[4] = NULL;
	file[5] = NULL;

	Util::createRep(file);

	file[4] = (char*)malloc(20*sizeof(char));
	file[4][0] = '\0';
	strcpy(file[4], "costTable.dat");
	char* fileLine = Util::generateRep(file);

	FILE* ffile = NULL;
	ffile = fopen(fileLine, "w");

	fprintf(ffile, "%d\n", zoneLength);
	for(int i=0;i<zoneLength;i++)
	{
		for(int j=0;j<zoneLength;j++)
		{
			fprintf(ffile, "%d ", cTable[i][j]);
		}
		fprintf(ffile, "\n");
	}
	fclose(ffile);

	for(int i=0;i<5;i++)
		free(file[i]);
	free(file);
	free(fileLine);
}

BotBestPushesS_Matrix<double>*
BotBestPushesS::createAssignationMatrix(const int* boxesPositions,
		const int* goalsPositions) const
{
	int numOfBoxes = level->getBoxesNumber();
	int numOfGoals = level->getGoalsNumber();
	BotBestPushesS_Matrix<double>* assignMatrix =
		new BotBestPushesS_Matrix<double>(numOfGoals, numOfGoals);

	// Create assignTable
	for(int i=0;i<numOfBoxes;i++)
		for(int j=0;j<numOfGoals;j++)
			(*assignMatrix)(i,j)
				= costTable[boxesPositions[i]][goalsPositions[j]];

	// If there are less boxes than goals, we fill the rest with 999999
	// (big factice value so it's used after all others box). we need a
	// square matrix
	for(int i=numOfBoxes;i<numOfGoals;i++)
		for(int j=0;j<numOfGoals;j++)
			(*assignMatrix)(i,j) = 999999;

	return assignMatrix;
}
