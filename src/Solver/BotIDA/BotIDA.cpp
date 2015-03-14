/*----------------------------------------------*/
/*================== ISokoban ==================*/
/*----------------------------------------------*/
/* Version : 1.0								*/
/* Author : Michaël Hoste						*/
/* Copyright : See LICENSE file for details   	*/
/*----------------------------------------------*/

#include "../../../include/Solver/BotIDA/BotIDA.h"

/* ------------*/
/* Constructor */
/* ------------*/
BotIDA::BotIDA(Base* base, Level* level, int maxNodeNumber, int maxRamSize,
		int openTableSize, int closeTableSize, int deadlockedBoxesSearch,
		bool onlyPushNumber, bool quickSearch):
	base(base),
	level(level),
	nodeNumber(0),
	maxNodeNumber(maxNodeNumber),
	maxRamSize(maxRamSize),
	deadlockedBoxesSearch(deadlockedBoxesSearch),
	onlyPushNumber(onlyPushNumber),
	quickSearch(quickSearch),
	stats(NULL)

{
	OPENTABLE_SIZE = openTableSize;
	CLOSETABLE_SIZE = closeTableSize;
}

/* -----------*/
/* Destructor */
/* -----------*/
BotIDA::~BotIDA()
{
	if(stats)
		delete stats;
}

/* -------*/
/* Others */
/* -------*/
void BotIDA::resolve()
{
	bool solved = alreadySolved();

	// Get initial cost to start with
	int cost = initInitialCost();

	char* message = (char*)malloc(255*sizeof(char));
	message = strcpy(message, "starting...");

	// While solution's not found or limit not reached, we keep trying
	while(!solved && notFinished(message))
	{
		printf("cost : %d\n", cost);
		Solver* solver = new BotBestPushesS(base, level, maxNodeNumber,
				maxRamSize,	OPENTABLE_SIZE, CLOSETABLE_SIZE, cost,
				deadlockedBoxesSearch, onlyPushNumber, quickSearch);
		solver->resolve();

		nodeNumber += solver->getNodeNumber();

		if(!onlyPushNumber)
		{
			solver->getStats()->print();
			solver->getStats()->print("solutions.txt");
		}

		int newCost = ((BotA*)solver)->getMinReject();
		if(newCost <= cost)
		{
			delete solver;
			break;
		}
		else
			cost = newCost;

		message = strcpy(message, solver->getStats()->getMessage());

		bool solutionIsFound = solutionFound(message);

		if(!onlyPushNumber && solutionIsFound)
			saveSolution(solver);
		else if(!onlyPushNumber && notLimited(message))
			saveInitialCost(cost);

		if(solved || !notFinished(message))
			stats = new Stats(solver->getStats());

		delete solver;
	}

	free(message);
}

bool BotIDA::notLimited(char* message) const
{
	return
		( 	strcmp(message,
				"Impossible to solve : Ram Limit or Max Nodes Limit reached")
						!= 0);
}

bool BotIDA::notFinished(char* message) const
{
	return
		(strcmp(message,
				"Impossible to solve : Ram Limit or Max Nodes Limit reached")
					!= 0
	  && strcmp(message, "Solution found : first node is solution") != 0
	  && strcmp(message, "Solution found") != 0);
}

bool BotIDA::solutionFound(char* message) const
{
	return
		(strcmp(message, "Solution found : first node is solution") == 0
	  || strcmp(message, "Solution found") == 0);
}

void BotIDA::saveSolution(Solver* solver)
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
	strcpy(file[2], packName);
	sprintf(file[3], "%d", levelId);
	strcpy(file[4], "IDASolution.dat");
	file[5] = NULL;

	char* fileLine = Util::generateRep(file);

	solver->getStats()->print(fileLine);

	for(int i=0;i<5;i++)
		free(file[i]);
	free(file);
	free(fileLine);
}

bool BotIDA::alreadySolved()
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
	strcpy(file[4], "IDASolution.dat");
	file[5] = NULL;

	char* fileLine = Util::generateRep(file);

	bool alreadySolved;
	if(Util::isThisFileExists(fileLine))
		alreadySolved = true;
	else
		alreadySolved = false;

	for(int i=0;i<5;i++)
		free(file[i]);
	free(file);
	free(fileLine);

	if(alreadySolved == true)
	{
		printf("BotIDA :: Level is already solved, please remove ");
		printf("IDASolution.dat from \"data/solver/...\" folder to solve ");
		printf("it again\n");
	}

	return alreadySolved;
}

int BotIDA::initInitialCost()
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
	strcpy(file[4], "actualIDACost.dat");
	file[5] = NULL;

	char* fileLine = Util::generateRep(file);

	int cost;
	if(Util::isThisFileExists(fileLine) && !onlyPushNumber)
		cost = loadInitialCost(fileLine);
	else
		cost = createInitialCost();

	for(int i=0;i<5;i++)
		free(file[i]);
	free(file);
	free(fileLine);

	return cost;
}

int BotIDA::createInitialCost()
{
	int cost = 0;

	Solver* initSolver = new BotBestPushesS(base, level, 0,
			maxRamSize, 1, 1, -1, deadlockedBoxesSearch, true);
	cost = ((BotA*)initSolver)->getInitialCost();
	delete initSolver;

	if(!onlyPushNumber)
		saveInitialCost(cost);

	return cost;
}

int BotIDA::loadInitialCost(char* fileName)
{
	int cost;

	FILE* file = fopen(fileName, "r");
	fscanf(file, "%d\n", &cost);
	fclose(file);

	return cost;
}

void BotIDA::saveInitialCost(int cost)
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
	strcpy(file[4], "actualIDACost.dat");
	char* fileLine = Util::generateRep(file);

	FILE* ffile = fopen(fileLine, "w");
	fprintf(ffile, "%d\n", cost);
	fclose(ffile);

	for(int i=0;i<5;i++)
		free(file[i]);
	free(file);
	free(fileLine);
}
