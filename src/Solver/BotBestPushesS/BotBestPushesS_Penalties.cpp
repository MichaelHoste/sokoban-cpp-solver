/*----------------------------------------------*/
/*================== ISokoban ==================*/
/*----------------------------------------------*/
/* Version : 1.0								*/
/* Author : Michaël Hoste						*/
/* Copyright : See LICENSE file for details   	*/
/*----------------------------------------------*/

#include "../../../include/Solver/BotBestPushesS/BotBestPushesS_Penalties.h"

/* ------------*/
/* Constructor */
/* ------------*/
BotBestPushesS_Penalties::BotBestPushesS_Penalties(Solver* solver):
	base(solver->getBase()),
	solver(solver),
	pNodes(NULL),
	pValues(NULL),
	pLength(0),
	pListLengths(NULL),
	penaltiesTestTab(NULL),
	penaltiesTestTabLength(0),
	emptyLevel(NULL),
	level(solver->getLevel()),
	goalZone(solver->getGoalZone()),
	zoneToLevelPos(solver->getZoneToLevelPos()),
	zoneToLevelPosLength(solver->getZoneToLevelPosLength()),
	levelToZonePos(solver->getLevelToZonePos())
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

	// Init of penalties zone list (list of sub-zones with multiple boxes that
	// create penalties not catchable by traditionnal methods)
	// We use same number of boxes used for sub-zones than the one used for
	// deadlocked sub-zones
	initPenaltiesList(solver->getDeadlockedBoxesSearch());
}

/* -----------*/
/* Destructor */
/* -----------*/
BotBestPushesS_Penalties::~BotBestPushesS_Penalties()
{
	if(pNodes)
	{
		for(int i=0;i<pLength;i++)
		{
			for(int j=0;j<pListLengths[i];j++)
			{
				delete(pNodes[i][j]);
			}
			free(pNodes[i]);
			free(pValues[i]);
		}
		free(pNodes);
		free(pValues);
		free(pListLengths);
	}
	if(penaltiesTestTab)
		free(penaltiesTestTab);
	if(emptyLevel)
		delete emptyLevel;
}

/* -------*/
/* Others */
/* -------*/
int BotBestPushesS_Penalties::getPenaltyOfTreeNode(TreeNode* treeNode) const
{
	int total = 0;

	// Initialize cell id of next node to be tested for each boxNumber
	int* actualCellId = (int*)malloc(pLength*sizeof(int));
	for(int i=0;i<pLength;i++)
		actualCellId[i] = 0;

	// we create a markedzone with all boxes of treenode
	Zone* boxesZone = treeNode->getNode()->getBoxesZone();
	Zone* pusherZone = treeNode->getNode()->getPusherZone();
	Zone* markedZone = new Zone(boxesZone);

	// Compute total penalty for all boxes
	int maximumPenalty = getMaxPenalty(actualCellId);
	while(maximumPenalty > 0 && total != INT_MAX)
	{
		// For each numOfBoxes penalties (smallest to biggest)
		for(int i=0;i<pLength && total != INT_MAX;i++)
		{
			// Test only nodes of maximumPenalties
			for(int j = actualCellId[i];
				j<pListLengths[i] && pValues[i][j] == maximumPenalty
				&& total != INT_MAX; j++)
			{
				Zone* pPusherZone = pNodes[i][j]->getPusherZone();
				Zone* pBoxesZone = pNodes[i][j]->getBoxesZone();

				// If penalized boxes are all included in markedZone and
				// treenode pusherZone is included in penalized pusherzone
				if(    pBoxesZone->isIncludedIn(markedZone)
				    && pusherZone->isIncludedIn(pPusherZone))
				{
					if(pValues[i][j] == INT_MAX)
						total = INT_MAX;
					else
						total += pValues[i][j];

					// we remove those boxes from markedZone
					markedZone->applyMinusWith(pBoxesZone);
				}
				actualCellId[i]++;
			}
		}
		maximumPenalty = getMaxPenalty(actualCellId);
	}

	delete markedZone;
	free(actualCellId);

	return total;
}

bool BotBestPushesS_Penalties::penaltiesListTest(int* tab, int tabLength,
		int* nodeLimit, int pusherLevelPos)
{
	int startNodeLimit, solverActualNodes = 0;
	if(nodeLimit == NULL)
		startNodeLimit = INT_MAX;
	else
		startNodeLimit = (*nodeLimit);

	bool endOfSearch = false;
	bool backTo0 = false;
	// 1. Create a level with specific boxes positions
	Level* testLevel = new Level(base, emptyLevel);

	for(int i=0;i<tabLength;i++)
	{
		int levelPos = zoneToLevelPos[tab[i]];
		if(testLevel->readPos(levelPos) == '.')
			testLevel->writePos(levelPos, '*');
		else
			testLevel->writePos(levelPos, '$');
	}
	testLevel->setBoxesNumber(tabLength);
	int nCols = testLevel->getColsNumber();

	// 2. Create a markedZone just with boxes positions
	Zone* markedZone = new Zone(testLevel, levelToZonePos, zoneToLevelPos,
			zoneToLevelPosLength, Zone::PROCESS_BOXES);

	// 3. For each unmarked cell of markedzone
	for(int i=0;i<markedZone->getLength();i++)
	{
		Zone* boxesZone = new Zone(testLevel, levelToZonePos, zoneToLevelPos,
				zoneToLevelPosLength, Zone::PROCESS_BOXES);

		if(!markedZone->readPos(i) || pusherLevelPos != -1)
		{
			// 3.1. Put pusher at this place in the level
			int levelPos;
			if(pusherLevelPos != -1)
				levelPos = pusherLevelPos;
			else
				levelPos = zoneToLevelPos[i];

			if(testLevel->readPos(levelPos) == '.')
				testLevel->writePos(levelPos, '+');
			else
				testLevel->writePos(levelPos, '@');
			testLevel->setPusherPosM(levelPos/nCols);
			testLevel->setPusherPosN(levelPos%nCols);

			// 3.2. Create a pusherZone with this level
			Zone* pusherZone = new Zone(testLevel, levelToZonePos,
				  zoneToLevelPos, zoneToLevelPosLength, Zone::PROCESS_PUSHER);

			// 3.3. Mark markedZone with pusherZone
			markedZone->applyOrWith(pusherZone);

			// 3.4. Create a treenode with boxesZone and pusherZone
			Node* testNode = new Node(solver);
			testNode->setBoxesZone(boxesZone);
			testNode->setPusherZone(pusherZone);
			TreeNode* testTreeNode = new TreeNode(testNode);

			// 3.5. Test created treenode
			const Deadlock* engine = solver->getDeadlockEngine();
			bool dCorner = engine->deadlockedCorner(testTreeNode);
			bool dZone = engine->deadlockedZone(testTreeNode, -1);
			bool deadlocked = dCorner || dZone;

			for(int j=0;j<tabLength && !deadlocked;j++)
				deadlocked = engine->deadlockedLastMove(testTreeNode,
										zoneToLevelPos[tab[j]]);

			// If a penalty on this level is not useless
			if(!deadlocked && !isUselessPenalty(testLevel))
			{
				// 3.6.Test BotBestPushesS solver on this created treenode
				int openTable;
				switch(tabLength)
				{
					case 1 : openTable = 53;break;
					case 2 : openTable = 503;break;
					case 3 : openTable = 3003;break;
					case 4 : openTable = 13003;break;
					default : openTable = 48743;break;
				}

				int limitSize = solver->getMaxRamSize();
				int prereq = tabLength - 1;
				if(solver->getDeadlockedBoxesSearch() < prereq)
					prereq = solver->getDeadlockedBoxesSearch();
				Solver* newSolver = new BotBestPushesS(base, testLevel, INT_MAX,
						limitSize, 5, 5, -1, prereq, true);
				int estimatedCost = ((BotA*)newSolver)->getInitialCost();
				delete newSolver;

				// If solving is possible
				if(estimatedCost != INT_MAX)
				{
					Solver* newSolver = new BotBestPushesS(base, testLevel,
							startNodeLimit, limitSize, openTable, openTable/2,
							estimatedCost, prereq, true);
//					BotIDA* newSolver = new BotIDA(base, testLevel, startNodeLimit,
//							limitSize, openTable, openTable/2, prereq, true);
					newSolver->resolve();
					char* message = (char*)malloc(255*sizeof(char));
					const Stats* stats = newSolver->getStats();
					solverActualNodes = newSolver->getNodeNumber();
					message = strcpy(message, stats->getMessage());

					// If solving is not possible with this cost (and so,
					// there exists a penalty)
					if(notResolved(message))
					{
						// Test real value of estimation (test each
						// combinaison of goals : time consuming but
						// keep optimality)
						int numOfBoxes = tabLength;
						int actualPen = getPenaltyOfTreeNode(testTreeNode);
						bool quick;
						if(numOfBoxes <= 2)
							quick = false;
						else
							quick = true;

						int mult;
						if(startNodeLimit == INT_MAX)
							mult = INT_MAX;
						else mult = startNodeLimit*10;
						int newPen = validNode(testNode, numOfBoxes, prereq,
								actualPen, INT_MAX, quick, mult);
						if(newPen > actualPen)
						{
							Node* newNode = new Node(testNode);
							addToPNodes(newNode, newPen,
									numOfBoxes);

							testNode->print();
							printf("penalty : %d\n", newPen);

							// Test of each penalty with n-1 boxes
							testLessBoxesPenalties(tab, tabLength,
									startNodeLimit, pusherLevelPos);
						}
						else if(newPen == -1)
						{
							backTo0 = true;
						}

						savePenaltiesList();
					}
					else if(ramLimitReached(message))
						endOfSearch = true;

					free(message);
					delete newSolver;
				}
			}

			delete testTreeNode;

			// 3.7. Clear temporary indications
			if(testLevel->readPos(levelPos) == '+')
				testLevel->writePos(levelPos, '.');
			else
				testLevel->writePos(levelPos, 's');
		}
		else
			delete boxesZone;

		if(pusherLevelPos != -1)
			break;
	}

	if(nodeLimit != NULL && !backTo0)
		(*nodeLimit) -= solverActualNodes;
	else if(nodeLimit != NULL && backTo0)
		(*nodeLimit) = 1;

	delete testLevel;
	delete markedZone;

	return endOfSearch;
}

void BotBestPushesS_Penalties::testLessBoxesPenalties(int* tab, int tabLength,
		int limit, int pusherLevelPos)
{
	int* newTab = (int*)malloc((tabLength-1)*sizeof(int));

	// we delete one box from the list
	for(int i=0;i<tabLength;i++)
	{
		// We create a list without this box
		int k=0;
		for(int j=0;j<tabLength;j++)
		{
			if(j != i)
			{
				newTab[k] = tab[j];
				k++;
			}
		}
		// Test of this new tab
		int newLimit = limit;
		penaltiesListTest(newTab, tabLength-1, &newLimit, pusherLevelPos);
	}

	free(newTab);
}

int BotBestPushesS_Penalties::getMaxPenalty(int* actualCellId) const
{
	int max = 0;
	for(int i=0;i<pLength;i++)
	{
		if(actualCellId[i] < pListLengths[i])
			if(pValues[i][actualCellId[i]] > max)
				max = pValues[i][actualCellId[i]];
	}

	return max;
}

void BotBestPushesS_Penalties::addToPNodes(Node* newPenalizedNode, int penalty,
		int numOfBoxes)
{
	// Initialization
	if(pLength == 0)
	{
		pNodes = (Node***)malloc(sizeof(Node**));
		pValues = (int**)malloc(sizeof(int*));
		pListLengths = (int*)malloc(sizeof(int));
		pListLengths[0] = 0;
	}

	// If more boxes than cells, reallocation
	for(int i=pLength;i<numOfBoxes;i++)
	{
		pNodes = (Node***)realloc(pNodes, (i+1)*sizeof(Node**));
		pValues = (int**)realloc(pValues, (i+1)*sizeof(int*));

		pNodes[i] = (Node**)malloc(sizeof(Node*));
		pValues[i] = (int*)malloc(sizeof(int));

		pListLengths = (int*)realloc(pListLengths, (i+1)*sizeof(int));
		pListLengths[i] = 0;

		pLength = i+1;
	}

	int cell = numOfBoxes-1;
	pListLengths[cell]++;
	int size = pListLengths[cell];

	pNodes[cell] = (Node**)realloc(pNodes[cell], size*sizeof(Node*));
	pNodes[cell][size-1] = newPenalizedNode;

	pValues[cell] = (int*)realloc(pValues[cell], size*sizeof(int));
	pValues[cell][size-1] = penalty;

	orderList(cell);
}

void BotBestPushesS_Penalties::orderList(int index) const
{
	if(index+1 > pLength)
	{
		printf("BotBestPushesS_Penalties :: ");
		printf("index > length of nodes list\n");
		exit(1);
	}

	int i=0;
	Node* nodeTemp;
	int penTemp;
	int change=1;

	// If list is empty, we do nothing
	if(pListLengths[index] > 1)
	{
		while(change == 1)
		{
			change = 0;
			while(i+1 < pListLengths[index]) // end of list
			{
				if(pValues[index][i+1] > pValues[index][i])
				{
					penTemp = pValues[index][i];
					nodeTemp = pNodes[index][i];

					pValues[index][i] = pValues[index][i+1];
					pNodes[index][i] = pNodes[index][i+1];

					pValues[index][i+1] = penTemp;
					pNodes[index][i+1] = nodeTemp;

					change = 1;
				}
				i++;
			}
			i=0;
		}
	}
}

void BotBestPushesS_Penalties::initPenaltiesList(int numberOfBoxes)
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
	strcpy(file[4], "penaltiesNodeList.dat");
	file[5] = NULL;

	char* fileLine = Util::generateRep(file);

	// We don't create deadlock list with BotGoodPushesS (cycle)
	if(Util::isThisFileExists(fileLine))
	{
		// Load existing list and complete until numberOfBoxes reached
		loadPenaltiesList(fileLine, numberOfBoxes);
		if(numberOfBoxes != 0 && numberOfBoxes >= penaltiesTestTabLength)
			createPenaltiesList(numberOfBoxes);
	}
	else
		createPenaltiesList(numberOfBoxes);

	for(int i=0;i<5;i++)
		free(file[i]);
	free(file);
	free(fileLine);
}

void BotBestPushesS_Penalties::createPenaltiesList(int numberOfBoxes)
{
	bool endOfSearch = false;

	// Initialisation of tab of boxes positions
	bool needInit = false;
	if(!penaltiesTestTab)
	{
		needInit = true;
		penaltiesTestTab = (int*)malloc(sizeof(int));
		penaltiesTestTabLength = 1;
	}

	int counter = 0;

	int boxesNumber = level->getBoxesNumber();
	if(numberOfBoxes > boxesNumber)
		numberOfBoxes = boxesNumber;

	// test penalties for 1, 2, ... boxes
	int i;
	for(i=penaltiesTestTabLength-1;i<numberOfBoxes && !endOfSearch;i++)
	{
		if(needInit)
		{
			penaltiesTestTab = (int*)realloc(penaltiesTestTab,
											(i+1)*sizeof(int));
			penaltiesTestTabLength = i+1;

			// tab[0] = 0, tab[1] = 1, ... first box=first pos, 2e box=2e pos
			for(int j=0;j<penaltiesTestTabLength;j++)
				penaltiesTestTab[j]=j;
		}
		needInit=true;

		// while every boxes are not in last positions
		bool endWhile = false;
		while(!endWhile && !endOfSearch)
		{
			printf("penalty test : ");
			for(int j=0;j<penaltiesTestTabLength;j++)
				printf("%d | ", penaltiesTestTab[j]);
			printf("\n");

			endOfSearch = penaltiesListTest(penaltiesTestTab,
					penaltiesTestTabLength);

			if(!lastPenaltiesListTest(goalZone->getLength(), penaltiesTestTab,
					penaltiesTestTabLength))
			{
				increasePenaltiesListTest(goalZone->getLength(),
						&penaltiesTestTab, &penaltiesTestTabLength);
			}
			else
				endWhile=true;


			counter++;
			if(counter >= 250)
			{
				counter = 0;
				savePenaltiesList();
			}
		}

		if(endOfSearch)
		{
			printf("BotBestPushesS_Penalties :: ");
			printf("ram limit reached for penalties test\n");
			exit(1);
		}
	}

	// Save start of next iteration in file
	if(i == numberOfBoxes)
	{
		penaltiesTestTab = (int*)realloc(penaltiesTestTab, (i+1)*sizeof(int));
		penaltiesTestTabLength = i+1;

		// tab[0] = 0, tab[1] = 1, ... first box=first pos, 2e box=2e pos
		for(int j=0;j<penaltiesTestTabLength;j++)
			penaltiesTestTab[j]=j;

		savePenaltiesList();
	}
}

void BotBestPushesS_Penalties::savePenaltiesList()
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

	file[4] = (char*)malloc(25*sizeof(char));
	file[4][0] = '\0';
	strcpy(file[4], "penaltiesNodeList.dat");
	char* fileLine = Util::generateRep(file);

	FILE* ffile = NULL;
	ffile = fopen(fileLine, "w");

	fprintf(ffile, "%d\n", penaltiesTestTabLength);
	for(int i=0;i<penaltiesTestTabLength;i++)
		fprintf(ffile, "%d ", penaltiesTestTab[i]);
	fprintf(ffile, "\n");

	// Get Number of penalized nodes
	int pNodesNumber = 0;
	for(int i=0;i<pLength;i++)
		pNodesNumber += pListLengths[i];

	fprintf(ffile, "%d\n", pNodesNumber);

	// Save each node
	for(int i=0;i<pLength;i++)
	{
		for(int j=0;j<pListLengths[i];j++)
		{
			pNodes[i][j]->getBoxesZone()->saveInFile(ffile);
			pNodes[i][j]->getPusherZone()->saveInFile(ffile);
			fprintf(ffile, "%d\n", pValues[i][j]);
		}
	}

	fclose(ffile);

	for(int i=0;i<5;i++)
		free(file[i]);
	free(file);
	free(fileLine);
}

void BotBestPushesS_Penalties::loadPenaltiesList(char* fileName,
		int numberOfBoxes)
{
	FILE* file = fopen(fileName, "r");

	fscanf(file, "%d\n", &penaltiesTestTabLength);

	penaltiesTestTab = (int*)malloc(penaltiesTestTabLength*sizeof(int));

	for(int i=0;i<penaltiesTestTabLength;i++)
		fscanf(file, "%d ", &penaltiesTestTab[i]);
	fscanf(file, "\n");

	int numberOfNodes;
	fscanf(file, "%d\n", &numberOfNodes);

	// If number of boxes we want to load is 0, we don't load anything
	// If number of boxes != 0, we load any information we have (even if
	// there are more boxes informations)
	if(numberOfBoxes != 0)
	{
		for(int i=0;i<numberOfNodes;i++)
		{
			Zone* boxes = new Zone(file, zoneToLevelPosLength);
			Zone* pusher = new Zone(file, zoneToLevelPosLength);
			int value;
			fscanf(file, "%d\n", &value);
			int numOfBoxes = boxes->getNumberOf1();
			Node* newNode = new Node(solver, pusher, boxes);
			addToPNodes(newNode, value, numOfBoxes);
		}
	}

	fclose(file);
}

void BotBestPushesS_Penalties::increasePenaltiesListTest(int positions,
		int** tab, int* tabLength)
{
	int posIncreased = 0;

	for(int i=(*tabLength)-1;i>=0;i--)
	{
		if((*tab)[i] != positions-(*tabLength)+i)
		{
			(*tab)[i]++;
			posIncreased = i;
			break;
		}
	}

	for(int i=posIncreased+1;i<(*tabLength);i++)
		(*tab)[i] = (*tab)[i-1]+1;
}

bool BotBestPushesS_Penalties::lastPenaltiesListTest(int positions, int* tab,
		int tabLength)
{
	for(int i=tabLength-1;i>=0;i--)
	{
		if(tab[i] != positions-tabLength+i)
			return false;
	}
	return true;
}

int BotBestPushesS_Penalties::validNode(Node* node, int numOfBoxes, int prereq,
		int penaltyDown, int penaltyUp, bool quickValid, int maxNodes)
{
	// Test if this node already exists into nodes list. If it is, it's not
	// coherent because the estimatedCost must be right in all situations
	int k = numOfBoxes-1;
	if(k < pLength)
	{
		for(int i=0;i<pListLengths[k];i++)
		{
			if(pNodes[k][i]->isEgal(node))
				return 0;
		}
	}

	// Initialize test Level : delete all boxes except the one of node to test
	// and delete every goals + put pusher at its right place
	int numOfGoals = emptyLevel->getGoalsNumber();
	int* goalPositions = (int*)malloc(numOfGoals*sizeof(int));
	int goalI = 0;
	Level* testLevel = new Level(base, emptyLevel);
	testLevel->setBoxesNumber(numOfBoxes);
	testLevel->setGoalsNumber(numOfBoxes);
	int nCols = testLevel->getColsNumber();
	bool flagPusher = false;
	for(int i=0;i<zoneToLevelPosLength;i++)
	{
		int levelPos = zoneToLevelPos[i];

		// Initialize tab of goals positions
		char c = testLevel->readPos(levelPos);
		if(c == '+' || c == '.' || c == '*')
		{
			goalPositions[goalI] = levelPos;
			goalI++;
		}

		// Put static boxes in this level
		if(node->getBoxesZone()->readPos(i))
			testLevel->writePos(levelPos, '$');
		// Put 1 (and only 1) static pusher in this level
		else if(flagPusher == false && node->getPusherZone()->readPos(i))
		{
			testLevel->writePos(levelPos, '@');
			testLevel->setPusherPosM(levelPos/nCols);
			testLevel->setPusherPosN(levelPos%nCols);
			flagPusher = true;
		}
		// Rest of level must be blank
		else
			testLevel->writePos(levelPos, 's');
	}

	// Define size in hashtable
	int openTable;
	switch(numOfBoxes)
	{
		case 1 : openTable = 53;break;
		case 2 : openTable = 503;break;
		case 3 : openTable = 3003;break;
		case 4 : openTable = 13003;break;
		default : openTable = 48743;break;
	}

	// initialize tab of goals positions
	int* posTab = (int*)malloc(numOfBoxes*sizeof(int));
	for(int i=0;i<numOfBoxes;i++)
		posTab[i] = i;

	int firstPosGoal = 0;

	// If minimum penalty is not 0 and not last test to make
	int penalty = penaltyUp;
	bool endOfTest = false;
	while(!endOfTest && penalty > penaltyDown)
	{
		printf("penalty confirmation : ");
		if(quickValid)
		{
			for(int j=0;j<numOfBoxes;j++)
				printf("%d | ", firstPosGoal+j);
		}
		else
		{
			for(int j=0;j<numOfBoxes;j++)
				printf("%d | ", posTab[j]);
		}
		printf("\n");

		// Put only goals to test in the level
		for(int i=0;i<numOfBoxes;i++)
		{
			int levelPos;
			if(quickValid)
				levelPos = goalPositions[firstPosGoal+i];
			else
				levelPos = goalPositions[posTab[i]];

			char c = testLevel->readPos(levelPos);
			if(c == '$')
				testLevel->writePos(levelPos, '*');
			else if(c == '@')
				testLevel->writePos(levelPos, '+');
			else
				testLevel->writePos(levelPos, '.');
		}

		// Test this temporary level
		int limitSize = solver->getMaxRamSize();

		// solver with deadlocked nodes but without penalized nodes
		Solver* newSolver = new BotBestPushesS(base, testLevel, INT_MAX,
				limitSize, 5, 5, -1, 0, true);
		int estimatedCost = ((BotA*)newSolver)->getInitialCost();
		delete newSolver;

		if(estimatedCost != INT_MAX)
		{
			 // solver application
			newSolver = new BotBestPushesS(base, testLevel, maxNodes,
					limitSize, openTable, openTable/2, -1, prereq, true);
			newSolver->resolve();
			char* message = (char*)malloc(255*sizeof(char));
			const Stats* stats = newSolver->getStats();
			message = strcpy(message, stats->getMessage());

			// If solving is a success
			if(!notResolved(message) && !ramLimitReached(message))
			{
				int realCost = stats->getNumOfPushes();
				if(realCost - estimatedCost < penalty)
					penalty = realCost - estimatedCost;
			}
			else if(ramLimitReached(message))
			{
				penalty = -1;
				endOfTest = true;
			}

			free(message);
			delete newSolver;
		}

		// Move out temporary goals
		for(int i=0;i<numOfBoxes;i++)
		{
			int levelPos;
			if(quickValid)
				levelPos = goalPositions[firstPosGoal+i];
			else
				levelPos = goalPositions[posTab[i]];

			char c = testLevel->readPos(levelPos);
			if(c == '*')
				testLevel->writePos(levelPos, '$');
			else if(c == '+')
				testLevel->writePos(levelPos, '@');
			else
				testLevel->writePos(levelPos, 's');
		}

		if(quickValid)
		{
			// Don't excess numOfGoals
			firstPosGoal += numOfBoxes;

			if(firstPosGoal == numOfGoals)
				endOfTest = true;
			else if(firstPosGoal + numOfBoxes > numOfGoals)
				firstPosGoal = numOfGoals - numOfBoxes;
		}
		else
		{
			if(lastPenaltiesListTest(numOfGoals, posTab, numOfBoxes))
				endOfTest = true;
			else
				increasePenaltiesListTest(numOfGoals, &posTab, &numOfBoxes);
		}
	}

	free(goalPositions);
	delete testLevel;
	free(posTab);

	return penalty;
}

bool BotBestPushesS_Penalties::isUselessPenalty(Level* l)
{
	int nCols = level->getColsNumber();
	int counter = 0;

	int posM = l->getPusherPosM();
	int posN = l->getPusherPosN();
	int pos = posM*nCols+posN;

	int* lNeig = (int*)malloc(4*sizeof(int));
	lNeig[0] = pos - 1;
	lNeig[1] = pos + 1;
	lNeig[2] = pos - nCols;
	lNeig[3] = pos + nCols;

	counter = 0;
	for(int j=0;j<4;j++)
	{
		// If there is a wall or a box in this neighbour
		char c = l->readPos(lNeig[j]);
		if(c == '#' || c == '$' || c == '*')
			counter++;
	}

	free(lNeig);

	if(counter == 4)
		return true;
	else
		return false;
}

bool BotBestPushesS_Penalties::ramLimitReached(char* message) const
{
	return
		(strcmp(message,
				"Impossible to solve : Ram Limit or Max Nodes Limit reached")
					== 0);
}

bool BotBestPushesS_Penalties::notResolved(char* message) const
{
	return(
		(strcmp(message,
				"Impossible to solve : No more nodes in waiting list")
					== 0)
	||  (strcmp(message,
				"Impossible to solve : first node has no child")
					== 0));
}
