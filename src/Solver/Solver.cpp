/*----------------------------------------------*/
/*================== ISokoban ==================*/
/*----------------------------------------------*/
/* Version : 1.0								*/
/* Author : Michaël Hoste						*/
/* Copyright : See LICENSE file for details   	*/
/*----------------------------------------------*/

#include "../../include/Solver/Solver.h"
#include "../../include/Solver/TreeNode.h"
#include "../../include/Solver/HashTable.h"
#include "../../include/Solver/ChainedList.h"
#include "../../include/Solver/Node.h"
#include "../../include/Solver/Stats.h"
#include "../../include/Solver/Deadlock.h"

/* ------------*/
/* Constructor */
/* ------------*/
Solver::Solver(Base* base, Level* level, int maxNodeNumber, int maxRamSize,
		int openTableSize, int closeTableSize, int deadlockedBoxesSearch,
		bool onlyPushNumber) :
	base(base),
	level(level),
	stats(NULL),
	processingTreeRoot(NULL),
	openTable(NULL),
	closeTable(NULL),
	closeNodeList(NULL),
	deadlockEngine(NULL),
	goalZone(NULL),
	goalsPositions(NULL),
	zoneToLevelPos(NULL),
	zoneToLevelPosLength(0),
	levelToZonePos(NULL),
	nodeNumber(0),
	maxNodeNumber(maxNodeNumber),
	ramSize(0),
	maxRamSize(maxRamSize),
	deadlockedBoxesSearch(deadlockedBoxesSearch),
	onlyPushNumber(onlyPushNumber),
	solved(false),
	stopped(false)
{
	OPENTABLE_SIZE = openTableSize;
	CLOSETABLE_SIZE = closeTableSize;
}

/* -----------*/
/* Destructor */
/* -----------*/
Solver::~Solver()
{
//	WE DO NOT DELETE LEVEL
//	if(level)
//		delete level;

	if(stats)
		delete stats;
	if(processingTreeRoot)
		delete processingTreeRoot;
	if(openTable)
		delete openTable;
	if(closeTable)
		delete closeTable;
	if(closeNodeList)
		delete closeNodeList;
	if(deadlockEngine)
		delete deadlockEngine;
	if(goalZone)
		delete goalZone;
	if(goalsPositions)
		free(goalsPositions);
	if(levelToZonePos)
		free(levelToZonePos);
	if(zoneToLevelPos)
		free(zoneToLevelPos);
}

/* -------*/
/* Others */
/* -------*/
void Solver::resolve()
{
	initialize();

	// If initialized first node was not solution or deadlocked
	if(!solved && maxNodeNumber > 0)
		// Start the search in the tree
		process();
}

void Solver::computeSteps(int** pusherSteps, int** boxesSteps) const
{
	if(stats != NULL)
	{
		// Get solution path
		Path* solutionPath = NULL;
		if(stats->getSolutionPath())
			solutionPath = stats->getSolutionPath();
		else
		{
			TreeNode* solutionTreeNode = stats->getTreeNode();
			solutionPath = solutionTreeNode->getPath();
		}

		// Apply each move of the solution and save useful informations
		const char* solutionCharPath = solutionPath->getMoves();
		int pusherStepsNumber = 1;
		int boxesStepsNumber = 1;
		int nMoves = solutionPath->getNMoves();
		Level* tempLevel = new Level(base, level);
		for(int i=0;i<nMoves;i++)
		{
			char dir = solutionCharPath[i];
			tempLevel->move(dir);

			int pusherM = tempLevel->getPusherPosM();
			int pusherN = tempLevel->getPusherPosN();
			int boxM = pusherM;
			int boxN = pusherN;

			switch(dir)
			{
				case 'R': boxN++;break;
				case 'L': boxN--;break;
				case 'D': boxM++;break;
				case 'U': boxM--;break;
				default : boxN=boxM=-1;break;
			}

			// Add pusherStep number
			if(pusherSteps[pusherM][pusherN] == -1)
			{
				pusherSteps[pusherM][pusherN] = pusherStepsNumber;
				pusherStepsNumber++;
			}

			// add boxesStep number
			if(boxM != -1 && boxN != -1 && boxesSteps[boxM][boxN] == -1)
			{
				boxesSteps[boxM][boxN] = boxesStepsNumber;
				boxesStepsNumber++;
			}
		}

		if(!stats->getSolutionPath())
			delete solutionPath;
		delete tempLevel;
	}

//	printf("pusherSteps\n");
//	for(int i=0;i<nRows;i++)
//	{
//		for(int j=0;j<nCols;j++)
//		{
//			printf("%3d ", pusherSteps[i][j]);
//		}
//		printf("\n");
//	}
//	printf("\n");
//
//	printf("boxesSteps\n");
//	for(int i=0;i<nRows;i++)
//	{
//		for(int j=0;j<nCols;j++)
//		{
//			printf("%3d ", boxesSteps[i][j]);
//		}
//		printf("\n");
//	}
//	printf("\n");
}

int Solver::getSize(void)
{
	int sum = 0;
	int nNumber = openTable->getItemNumber()+closeTable->getItemNumber();

	// hashtable and treeNode (treenodes in hashtable or tree are the same)
	sum+=((openTable->getLength()+closeTable->getLength())*sizeof(ChainedList));
	sum+=(nNumber*sizeof(ListNode));
	sum+=(nNumber*sizeof(TreeNode));
	sum+=(nNumber*sizeof(Node));
	sum+=(2*nNumber*sizeof(Zone));
	sum+=(2*goalZone->getNumberCell()*sizeof(unsigned int));
	sum+=(nNumber*sizeof(TreeNode**));
	sum+=(nNumber*sizeof(TreeNode*));

	// closenodelist (not in the tree yet but children of nodes in the tree)
	int nodeListLength = closeNodeList->getLength();
	sum+=(nodeListLength*sizeof(ListNode)); // ListNodes

	// Heap-administration blocks size : admin data to manage each heap block
	sum+=60*nNumber;

	// Start size (levels, ...)
	sum+=14000000;

	return sum;
}

void Solver::initialize(void)
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

	// Init starting TreeNode
	initStartingNode();
}

void Solver::process()
{
	int i;
	TreeNode* treeNode = NULL;
	Node** children = NULL;
	bool deadlockednode;
	bool alreadysearched;
	bool stopped = false; // stopped is true if limit reached

	// Main loop (we stop after a limited number of iterations)
	while(!solved)
	{
		nodeNumber++;

		// Find next treeNode and its children to compute them
		treeNode = firstTreeNode();

		// Test solution and make stats
		if(!solved)
			solved = testSolution(treeNode);

		// Find every children of this node
		if(!solved)
		{
			children = findChildren(treeNode);
			printInfos(treeNode);
		}

		// We test every found children
		i=0;
		while(!solved && children != NULL && children[i] != NULL)
		{
			// Create a treeNode and attach it to its parent treeNode
			TreeNode* childrenI = createTreeNode(children[i], treeNode, 0);

			// If node already present in open or close hashtable
			alreadysearched = isAlreadySearched(children[i]);

			if(!alreadysearched)
				deadlockednode = deadlockEngine->deadlockedNode(childrenI);

			// If not deadlocked and not already present in the hashTable
			if(!alreadysearched && !deadlockednode)
			{
				// If stop condition, we continue close list until it's empty
				if(!stopped)
				{
					addTreeNodeToCloseList(childrenI);
					closeTable->addItem(childrenI);
				}
			}
			else
				delete childrenI;

			i++;
		}

		if(!solved && children != NULL)
			free(children);
	}

	if(nodeNumber>maxNodeNumber || getSize()>maxRamSize*1048576)
		stopped = true;
}

TreeNode* Solver::firstTreeNode(void)
{
	TreeNode* treeNode = NULL;

	// Get next treeNode from the list
	if(closeNodeList->getFirstItem() != NULL)
	{
		treeNode = closeNodeList->getFirstItem()->getTreeNode();

		// Delete this treeNode from the list
		closeNodeList->deleteFirstItem();

		// Use this treeNode to get every children states
		return treeNode;
	}
	else
	{
		// No more nodes, impossible to resolve
		solved = true;
		stats = new Stats(base, this, NULL, NULL, -1,
				"Impossible to solve : No more nodes in waiting list");

		return NULL;
	}
}

Node** Solver::findChildren(TreeNode* treeNode)
{
	if(treeNode)
	{
		closeTable->removeItem(treeNode->getNode());
		openTable->addItem(treeNode);
		return treeNode->getNode()->findChildren();
	}
	else
		return NULL;
}

bool Solver::isAlreadySearched(Node* node)
{
	return
	(openTable->isPresent(node) || closeTable->isPresent(node));
}

void Solver::initZoneToLevelPos(void)
{
	int length = 0;
	this->zoneToLevelPos = (int*)malloc(1*sizeof(int));

	int m = this->level->getRowsNumber();
	int n = this->level->getColsNumber();

	for(int i=0;i<m*n;i++)
	{
		char pos = this->level->readPos(i);

		if(pos != '#' && pos != ' ')
		{
			length++;
			zoneToLevelPos=(int*)realloc(zoneToLevelPos, length*sizeof(int));
			zoneToLevelPos[length-1] = i;
		}
	}

	this->zoneToLevelPosLength = length;
}

void Solver::initLevelToZonePos(void)
{
	int m = level->getRowsNumber();
	int n = level->getColsNumber();
	this->levelToZonePos = (int*)malloc(m*n*sizeof(int));

	for(int i=0;i<m*n;i++)
		this->levelToZonePos[i]=-1;

	for(int i=0;i<zoneToLevelPosLength;i++)
		this->levelToZonePos[zoneToLevelPos[i]]=i;
}

void Solver::initGoalZone(void)
{
	goalZone = new Zone(level, levelToZonePos, zoneToLevelPos,
			zoneToLevelPosLength, Zone::PROCESS_GOAL);

	int goalsNumber = level->getGoalsNumber();
	goalsPositions = (int*)malloc(goalsNumber*sizeof(int));

	int length = goalZone->getLength();
	int k=0;
	for(int i=0;i<length;i++)
	{
		if(goalZone->readPos(i) == 1)
		{
			goalsPositions[k] = i;
			k++;
		}
	}
}

void Solver::initDeadlockEngine(void)
{
	deadlockEngine = new Deadlock(this);
}

void Solver::initHashTable(void)
{
	openTable = new HashTable(OPENTABLE_SIZE);
	closeTable = new HashTable(CLOSETABLE_SIZE);
}

void Solver::initCloseNodeList(void)
{
	closeNodeList = new ChainedList();
}

TreeNode* Solver::createTreeNode(Node* node, TreeNode* parentTreeNode,
		int pushCost)
{
	TreeNode* newTreeNode = new TreeNode(node);
	if(parentTreeNode)
		parentTreeNode->addChild(newTreeNode);

	return newTreeNode;
}

void Solver::initStartingNode(void)
{
	// Initialization of starting Node
	Zone* startBoxesZone = new Zone(level, levelToZonePos,
			zoneToLevelPos,	zoneToLevelPosLength, Zone::PROCESS_BOXES);
	Zone* startPusherZone = new Zone(level, levelToZonePos,
			zoneToLevelPos,	zoneToLevelPosLength, Zone::PROCESS_PUSHER);
	Node* startNode = new Node(this);
	startNode->setBoxesZone(startBoxesZone);
	startNode->setPusherZone(startPusherZone);

	// Initialize processing tree (root)
	processingTreeRoot = createTreeNode(startNode, NULL, 0);

	if(!solutionNode(processingTreeRoot) &&
	   !deadlockEngine->deadlockedNode(processingTreeRoot))
	{
		// Add this node in the hashtable
		closeTable->addItem(processingTreeRoot);

		// Add root item in the waiting node list
		addTreeNodeToCloseList(processingTreeRoot);
	}
	else
	{
		Path* path;
		if(onlyPushNumber)
			path = NULL;
		else
			path = new Path(base);

		solved = true;
		if(solutionNode(processingTreeRoot))
			stats = new Stats(base, this, processingTreeRoot, path, 0,
				"Solution found : first node is solution");
		else
			stats = new Stats(base, this, NULL, NULL, -1,
				"Impossible to solve : first node has no child");
	}
}

bool Solver::testSolution(TreeNode* treeNode)
{
	bool solutionnode;
	if(treeNode)
		solutionnode = solutionNode(treeNode);
	else
		return true;

	if(solutionnode)
	{
		Path* path = treeNode->getPath();

		stats = new Stats(base, this, treeNode, path, path->getNPushes(),
				"Solution found");
		return true;
	}
	else
		return false;
}

bool Solver::solutionNode(const TreeNode* treeNode) const
{
	// Test if node is solution or not
	if(treeNode->getNode()->isSolution())
		return true;
	else
		return false;
}

void Solver::printInfos(TreeNode* treeNode)
{
	if(nodeNumber%PRINT_EVERY_X_LEVELS_QUICK == 0)
	{
		printf("\nInfos :\n");
		printf("-------\n");

		printf("NodeNumber : %d\n", nodeNumber);
		printf("CloseList : %d\n", closeNodeList->getLength());
		printf("RamSize : %3.2f Mo\n\n", ((float)getSize())/(1024*1024));

		treeNode->getNode()->print();
		printf("\n--------------------------------------\n");
	}
}
