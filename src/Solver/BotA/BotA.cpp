/*----------------------------------------------*/
/*================== ISokoban ==================*/
/*----------------------------------------------*/
/* Version : 1.0								*/
/* Author : Michaël Hoste						*/
/* Copyright : See LICENSE file for details   	*/
/*----------------------------------------------*/

#include "../../../include/Solver/BotA/BotA.h"

/* ------------*/
/* Constructor */
/* ------------*/
BotA::BotA(Base* base, Level* level, int maxNodeNumber, int maxRamSize,
		int openTableSize, int closeTableSize, int costLimit,
		int deadlockedBoxesSearch, bool onlyPushNumber, bool quickSearch) :
	Solver(base, level, maxNodeNumber, maxRamSize, openTableSize,
			closeTableSize, deadlockedBoxesSearch, onlyPushNumber),
	minReject(INT_MAX),
	quickSearch(quickSearch)
{
	if(costLimit >= 0)
		this->costLimit = costLimit;
	else
		this->costLimit = INT_MAX;
}

/* -----------*/
/* Destructor */
/* -----------*/

/* -------*/
/* Others */
/* -------*/
int BotA::getSize(void)
{
#ifdef SLINUX
	system("ps aux | grep sokoban | grep -v grep > cmd.txt");
	FILE* file;
	file = fopen("cmd.txt", "r");
	int ram;
	char* tmp1 = (char*)malloc(50*sizeof(char));
	char* tmp2 = (char*)malloc(50*sizeof(char));
	char* tmp3 = (char*)malloc(50*sizeof(char));
	fscanf(file, "%s %d %s %s %d %d", tmp1, &ram, tmp2, tmp3, &ram, &ram);
	free(tmp1);
	free(tmp2);
	free(tmp3);
	fclose(file);
	ramSize = ram*1024;
	return ram*1024;
#endif
#ifdef SWIN32
	system("tasklist /FI \"IMAGENAME eq sokoban.exe\" /NH > cmd.txt");
	FILE* file;
	file = fopen("cmd.txt", "r");
	int tmpd;
	char* tmp1 = (char*)malloc(50*sizeof(char));
	char* tmp2 = (char*)malloc(50*sizeof(char));
	char* sizeString = (char*)malloc(50*sizeof(char));
	fscanf(file, "%s %d %s %d %s", tmp1, &tmpd, tmp2, &tmpd, sizeString);
	free(tmp1);
	free(tmp2);
	fclose(file);
	double ram = strtod(sizeString, NULL);
	free(sizeString);
	int ramInt = (int)ram*1024*1024;
	return ramInt;
#endif
}

int BotA::getInitialCost()
{
	initialize();

	// Initialization of starting Node
	Zone* startBoxesZone = new Zone(level, levelToZonePos,
			zoneToLevelPos,	zoneToLevelPosLength, Zone::PROCESS_BOXES);
	Zone* startPusherZone = new Zone(level, levelToZonePos,
			zoneToLevelPos,	zoneToLevelPosLength, Zone::PROCESS_PUSHER);
	Node* startNode = new Node(this);
	startNode->setBoxesZone(startBoxesZone);
	startNode->setPusherZone(startPusherZone);

	// Initialize processing tree (root)
	TreeNode* startTreeNode = createTreeNode(startNode, NULL, 0);

	int firstCost = ((BotA_TreeNode*)startTreeNode)->getF();

	delete startTreeNode;
	free(zoneToLevelPos);
	free(levelToZonePos);
	this->zoneToLevelPos = (int*)malloc(sizeof(int));
	this->levelToZonePos = (int*)malloc(sizeof(int));

	return firstCost;
}

void BotA::process()
{
	int i;
	TreeNode* treeNode = NULL;
	Child** children = NULL;
	bool deadlockednode;
	bool alreadysearched;

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
			children = findPonderedChildren(treeNode);
			printInfos(treeNode);
		}

		// We test every children found
		int counter = 0;
		i=0;

		while(!solved && children != NULL && children[i] != NULL)
		{
			// Create a treeNode and attach it to its parent treeNode
			TreeNode* childrenI = createTreeNode(children[i]->child, treeNode,
					children[i]->childCost);

			// If node already present in open or close hashtable
			alreadysearched = workOnAlreadySearched(childrenI, &counter);

			if(!alreadysearched)
				deadlockednode = deadlockEngine->deadlockedNode(childrenI);

			// If not deadlocked and not already present in the hashTable
			if(!alreadysearched && !deadlockednode)
			{
				// If stop condition, we continue close list until it's empty
				if(!stopped)
				{
					closeTable->addItem(childrenI);
					addTreeNodeToCloseList(childrenI);
				}
				counter++;
			}
			else
				delete childrenI;
			delete children[i];

			i++;
		}

		if(!solved && children != NULL)
			free(children);

		if(!solved && counter == 0 && isDeadTreeNode(treeNode))
			deleteDeadTreeNode(treeNode);

		if(nodeNumber>maxNodeNumber || ramSize>maxRamSize*1048576)
		{
			if(costLimit == INT_MAX && !onlyPushNumber)
				stopped = true;
			else
			{
				if(!onlyPushNumber && nodeNumber >= 50000)
					saveMostUsedPositions(15);

				solved = true;
				stats = new Stats(base, this, NULL, NULL, -1,
				"Impossible to solve : Ram Limit or Max Nodes Limit reached");
			}
		}
	}
}

void BotA::saveMostUsedPositions(int numberOfPos)
{
	// Create directory and file
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
	strcpy(file[4], "deductivePositions.dat");
	char* fileLine = Util::generateRep(file);

	FILE* ffile = fopen(fileLine, "w");

	int nBoxes = level->getBoxesNumber();
	int* mostUsedwG  = mostUsedPositions(numberOfPos, true);
	int* mostUsedwoG = mostUsedPositions(numberOfPos, false);

	// Generation of 1 box situations
	if(numberOfPos >= 1 && nBoxes >= 1 && deadlockedBoxesSearch < 1)
	{
		for(int i=0;i<numberOfPos;i++)
		{
			fprintf(ffile, "%d\n%d\n", 1, mostUsedwG[i]);
			fprintf(ffile, "%d\n%d\n", 1, mostUsedwoG[i]);
		}
	}

	// Generation of 2 boxes situations
	if(numberOfPos >= 2 && nBoxes >= 2 && deadlockedBoxesSearch < 2)
	{
		for(int i=0;i<numberOfPos-1;i++){
			for(int j=i+1;j<numberOfPos;j++)
			{
				fprintf(ffile, "%d\n%d %d\n", 2, mostUsedwG[i], mostUsedwG[j]);
				fprintf(ffile, "%d\n%d %d\n", 2, mostUsedwoG[i], mostUsedwoG[j]);
			}}
	}

	// Generation of 3 boxes situations
	if(numberOfPos >= 3 && nBoxes >= 3 && deadlockedBoxesSearch < 3)
	{
		for(int i=0;i<numberOfPos-2;i++){
			for(int j=i+1;j<numberOfPos-1;j++){
				for(int k=j+1;k<numberOfPos;k++)
				{
					fprintf(ffile, "%d\n%d %d %d\n", 3, mostUsedwG[i],
							mostUsedwG[j],  mostUsedwG[k]);
					fprintf(ffile, "%d\n%d %d %d\n", 3, mostUsedwoG[i],
							mostUsedwoG[j], mostUsedwoG[k]);
				}}}
	}

	// Generation of 4 boxes situations
	if(numberOfPos >= 4 && nBoxes >= 4 && deadlockedBoxesSearch < 4)
	{
		for(int i=0;i<numberOfPos-3;i++){
			for(int j=i+1;j<numberOfPos-2;j++){
				for(int k=j+1;k<numberOfPos-1;k++){
					for(int l=k+1;l<numberOfPos;l++)
					{
						fprintf(ffile, "%d\n%d %d %d %d\n", 4, mostUsedwG[i],
								mostUsedwG[j],  mostUsedwG[k], mostUsedwG[l]);
						fprintf(ffile, "%d\n%d %d %d %d\n", 4, mostUsedwoG[i],
								mostUsedwoG[j], mostUsedwoG[k], mostUsedwoG[l]);
					}}}}
	}

	// Generation of 5 boxes situations
	if(numberOfPos >= 5 && nBoxes >= 5 && deadlockedBoxesSearch < 5)
	{
		for(int i=0;i<numberOfPos-4;i++){
			for(int j=i+1;j<numberOfPos-3;j++){
				for(int k=j+1;k<numberOfPos-2;k++){
					for(int l=k+1;l<numberOfPos-1;l++){
						for(int m=l+1;m<numberOfPos;m++)
						{
							fprintf(ffile, "%d\n%d %d %d %d %d\n", 5,
									mostUsedwG[i], mostUsedwG[j],
									mostUsedwG[k], mostUsedwG[l],
									mostUsedwG[m]);
							fprintf(ffile, "%d\n%d %d %d %d %d\n", 5,
									mostUsedwoG[i], mostUsedwoG[j],
									mostUsedwoG[k], mostUsedwoG[l],
									mostUsedwoG[m]);
						}}}}}
	}

	// Generation of 6 boxes situations
	if(numberOfPos >= 6 && nBoxes >= 6 && deadlockedBoxesSearch < 6)
	{
		for(int i=0;i<numberOfPos-5;i++){
			for(int j=i+1;j<numberOfPos-4;j++){
				for(int k=j+1;k<numberOfPos-3;k++){
					for(int l=k+1;l<numberOfPos-2;l++){
						for(int m=l+1;m<numberOfPos-1;m++){
							for(int n=m+1;n<numberOfPos;n++)
							{
								fprintf(ffile, "%d\n%d %d %d %d %d %d\n", 6,
										mostUsedwG[i], mostUsedwG[j],
										mostUsedwG[k], mostUsedwG[l],
										mostUsedwG[m], mostUsedwG[n]);
								fprintf(ffile, "%d\n%d %d %d %d %d %d\n", 6,
										mostUsedwoG[i], mostUsedwoG[j],
										mostUsedwoG[k], mostUsedwoG[l],
										mostUsedwoG[m], mostUsedwoG[n]);
							}}}}}}
	}

	free(mostUsedwG);
	free(mostUsedwoG);

	fclose(ffile);

	for(int i=0;i<5;i++)
		free(file[i]);
	free(file);
	free(fileLine);
}

bool BotA::isDeadTreeNode(TreeNode* treeNode)
{
	// If this treeNode is not treated yet, this treeNode branch is alive
	bool thisOne = closeTable->isPresent(treeNode->getNode());
	if(thisOne)
		return false;
	else
	{
		// Recursive test of its children
		TreeNode** children = treeNode->getChildren();
		int childrenNumber = treeNode->getChildrenNumber();

		for(int i=0;i<childrenNumber;i++)
		{
			if(!isDeadTreeNode(children[i]))
				return false;
		}
		return true;
	}
}

void BotA::deleteDeadTreeNode(TreeNode* treeNode)
{
	TreeNode** children = treeNode->getChildren();
	int childrenNumber = treeNode->getChildrenNumber();

	// recursive call for each children of treeNode
	for(int i=0;i<childrenNumber;i++)
		deleteDeadTreeNode(children[i]);

	// Delete corresponding ListNode from openTable
	openTable->removeItem(treeNode->getNode());
	if(treeNode != processingTreeRoot)
		delete treeNode;
}

bool BotA::workOnAlreadySearched(TreeNode* treeNode, int* counter)
{
	bool alreadysearched = false;
	Node* node = treeNode->getNode();
	int newValue = ((BotA_TreeNode*)treeNode)->getF();

	// new value is not accepted in this iteration
	if(newValue > costLimit)
	{
		if(newValue < minReject)
			minReject = newValue;
		return true;
	}

	TreeNode* oldOpenNode = openTable->getTreeNodeFromNode(node);
	TreeNode* oldCloseNode = closeTable->getTreeNodeFromNode(node);

	// If node already present in open hashtable
	if(oldOpenNode != NULL)
	{
		alreadysearched = true;
		int oldValue = ((BotA_TreeNode*)oldOpenNode)->getF();

		// If value of node is higher than previous stored node -> nothing
		if(newValue < oldValue)
		{
			// Move old treenode to attach it to parent of new treenode
			moveTreeNode(
					oldOpenNode, 			 // Treenode existing in tree
					treeNode->getParent());  // new parent for treenode

			// Decrease old treenode and every descendant
			((BotA_TreeNode*)oldOpenNode)->decreaseValue(
					oldValue - newValue);

			(*counter)++;
		}
	}
	// If node already present in close hashtable
	else if(oldCloseNode != NULL)
	{
		alreadysearched = true;
		int oldValue = ((BotA_TreeNode*)oldCloseNode)->getF();

		// If value of node is higher than previous stored node -> nothing
		if(newValue < oldValue)
		{
			// Move old treenode to attach it to parent of new treenode
			int heapStackCell =
			  ((BotA_HashTable*)closeTable)->getHeapStackCell(node);

			((BotA_TreeNode*)oldCloseNode)->setF(
					((BotA_TreeNode*)treeNode)->getF());
			((BotA_TreeNode*)oldCloseNode)->setG(
					((BotA_TreeNode*)treeNode)->getG());
			((BotA_HeapStack*)closeNodeList)->repositionCell(heapStackCell);

			moveTreeNode(
					oldCloseNode, 			 // Treenode existing in tree
					treeNode->getParent());  // new parent for treenode

			(*counter)++;
		}
	}

	return alreadysearched;
}

TreeNode* BotA::firstTreeNode(void)
{
	TreeNode* treeNode = NULL;

	// Get next treeNode from the list
	ListNode* listnode = closeNodeList->getFirstItem();

	if(listnode != NULL)
	{
		treeNode = listnode->getTreeNode();
		delete listnode;

		// Delete this treeNode from the list
		closeNodeList->deleteFirstItem();

		// Use this treeNode to get every children states
		return treeNode;
	}
	else
	{
		// No more nodes, impossible to resolve
		solved = true;

		if(!stopped)
			stats = new Stats(base, this, NULL, NULL, -1,
				"Impossible to solve : No more nodes in waiting list");
		else
			stats = new Stats(base, this, NULL, NULL, -1,
				"Impossible to solve : Ram Limit or Max Nodes Limit reached");

		return NULL;
	}
}

Child** BotA::findPonderedChildren(TreeNode* treeNode)
{
	if(treeNode)
	{
		closeTable->removeItem(treeNode->getNode());
		openTable->addItem(treeNode);
		Node** children = treeNode->getNode()->findChildren();

		return treeNode->getNode()->findMacroChildren(children);
	}
	else
		return NULL;
}

void BotA::addTreeNodeToCloseList(TreeNode* treeNode)
{
	((BotA_HeapStack*)closeNodeList)->addItem(treeNode);
}

void BotA::initHashTable(void)
{
	openTable = new HashTable(OPENTABLE_SIZE);
	closeTable = new BotA_HashTable(CLOSETABLE_SIZE);
}

void BotA::initCloseNodeList(void)
{
	closeNodeList = new BotA_HeapStack();
}

TreeNode* BotA::createTreeNode(Node* node, TreeNode* parentTreeNode,
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

	return newTreeNode;
}

void BotA::moveTreeNode(TreeNode* treeNode, TreeNode* newParent)
{
	// Start by deleting link from old parent to treenode
	TreeNode* oldParent = treeNode->getParent();
	if(oldParent != NULL)
	{
		TreeNode** pChildren = oldParent->getChildren();
		int pChildrenNumber = oldParent->getChildrenNumber();
		int i;
		for(i=0;i<pChildrenNumber;i++)
		{
			if(pChildren[i] == treeNode)
			{
				pChildren[i] = NULL;
				break;
			}
		}

		int j;
		for(j=i+1;j<pChildrenNumber;j++)
			pChildren[j-1] = pChildren[j];
		pChildren[j-1] = NULL;

		if(pChildrenNumber-1 == 0)
		{
			free(pChildren);
			pChildren = NULL;
		}
		else
			pChildren = (TreeNode**)realloc(pChildren,
										(pChildrenNumber-1)*sizeof(TreeNode*));
		oldParent->setChildrenNumber(pChildrenNumber-1);
	}

	if(newParent)
		newParent->addChild(treeNode);
	else
		treeNode->setParent(NULL);
}

bool BotA::testSolution(TreeNode* treeNode)
{
	bool solutionnode;
	if(treeNode)
		solutionnode = solutionNode(treeNode);
	else
		return true;

	if(solutionnode)
	{
		Path* path;
		if(onlyPushNumber)
			path = NULL;
		else
		{
			path = treeNode->getPath();
			if(!level->isSolution(path))
			{
				printf("BotA :: A solution is found but is wrong.\n");
				printf("BotA :: Tree construction must be good but not ");
				printf("treeNode::getPath()");
				exit(-1);
			}
		}

		stats = new Stats(base, this, treeNode, path,
				((BotA_TreeNode*)treeNode)->getG(),	"Solution found");

		return true;
	}
	else
		return false;
}

bool BotA::solutionNode(const TreeNode* treeNode) const
{
	// Test if node is solution or not
	if(((BotA_TreeNode*)treeNode)->getH() == 0)
		return true;
	else
		return false;
}

int* BotA::mostUsedPositions(int number, bool withGoals)
{
	int length;
	if(number >= zoneToLevelPosLength)
		length = number;
	else
		length = zoneToLevelPosLength;

	int* mostUsed = (int*)malloc(length*sizeof(int));
	int* stats = processingTreeRoot->computeTreeBoxStats();

	for(int i=0;i<length;i++)
		mostUsed[i] = findMaxCellAndReplaceValue(stats, zoneToLevelPosLength,
													withGoals);

	free(stats);

	return mostUsed;
}

int BotA::findMaxCellAndReplaceValue(int* tab, int tabLength, bool withGoals)
{
	int maxValue = INT_MIN;
	int cell = -1;

	for(int i=0;i<tabLength;i++)
	{
		// If new cell is bigger than maxValue
		if(tab[i] > maxValue)
		{
			// If we take goal or if we don't take goal and it's not one
			if(withGoals || (!withGoals && goalZone->readPos(i) != 1))
			{
				maxValue = tab[i];
				cell = i;
			}
		}
	}

	tab[cell] = -1;
	return cell;
}

//int BotA::computeTree(TreeNode* treeNode) const
//{
//	int sum = 0;
//
//	TreeNode** children = treeNode->getChildren();
//	int childrenNumber = treeNode->getChildrenNumber();
//
//	// recursive call for each children of treeNode
//	for(int i=0;i<childrenNumber;i++)
//		sum += computeTree(children[i]);
//
//	return sum+treeNode->getChildrenNumber();
//}

void BotA::printInfos(TreeNode* treeNode)
{
	if(treeNode && nodeNumber%500 == 0 && onlyPushNumber)
	{
		printf("NodeNumber : %d / %d\n", nodeNumber, maxNodeNumber);
		getSize();
	}

	if(treeNode && !onlyPushNumber &&
			(  quickSearch && nodeNumber%PRINT_EVERY_X_LEVELS_QUICK == 0
			|| !quickSearch && nodeNumber%PRINT_EVERY_X_LEVELS_SLOW == 0))
	{
		printf("\nInfos :\n");
		printf("-------\n");

		printf("Pack [ID] : %s [%d]\n", level->getPackName(), level->getId());
		printf("NodeNumber : %d\n", nodeNumber);
		printf("OpenTable : %d\n", openTable->getItemNumber());
		printf("CloseTable : %d\n", closeTable->getItemNumber());
//		printf("HeapStack : %d\n", closeNodeList->getLength());
//		printf("Tree : %d\n", computeTree(processingTreeRoot));

		printf("Actual f(x) : %d\n", ((BotA_TreeNode*)treeNode)->getF());
		printf("Actual g(x) : %d\n", ((BotA_TreeNode*)treeNode)->getG());
		printf("Actual h(x) : %d\n", ((BotA_TreeNode*)treeNode)->getH());

		printf("RamSize : %3.2f Mo\n\n", ((float)getSize())/(1024*1024));

		treeNode->getNode()->print();
		printf("\n--------------------------------------\n");
	}
}


