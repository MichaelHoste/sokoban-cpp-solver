/*----------------------------------------------*/
/*================== ISokoban ==================*/
/*----------------------------------------------*/
/* Version : 1.0								*/
/* Author : Michaël Hoste						*/
/* Copyright : See LICENSE file for details   	*/
/*----------------------------------------------*/

#include "../../include/Solver/TreeNode.h"

/* ------------*/
/* Constructor */
/* ------------*/
TreeNode::TreeNode(Node* node):
	node(node),
	parent(NULL),
	children(NULL),
	childrenNumber(0)
{

}

/* -----------*/
/* Destructor */
/* -----------*/
TreeNode::~TreeNode()
{
	// We start by destoying all children
	for(int i=0;i<childrenNumber;i++)
	{
		children[i]->setParent(NULL);
		delete children[i];
	}

	// Now we free tab containing children
	if(childrenNumber > 0)
	{
		free(children);
		children = NULL;
	}

	// Now we free father's link
	if(parent != NULL)
	{
		TreeNode** pChildren = parent->getChildren();
		int pChildrenNumber = parent->getChildrenNumber();

		int i;
		for(i=0;i<pChildrenNumber;i++)
		{
			if(pChildren[i] == this)
			{
				pChildren[i] = NULL;
				break;
			}
		}

		int j;
		for(j=i+1;j<pChildrenNumber;j++)
		{
			pChildren[j-1] = pChildren[j];
		}
		pChildren[j-1] = NULL;

		if(pChildrenNumber-1 == 0)
		{
			free(pChildren);
			pChildren = NULL;
		}
		else
			pChildren = (TreeNode**)realloc(pChildren,
										(pChildrenNumber-1)*sizeof(TreeNode*));

		parent->setChildrenNumber(pChildrenNumber-1);
	}
	delete node;
}

/* -------*/
/* Others */
/* -------*/
void TreeNode::addChild(TreeNode* newChild)
{
	newChild->setParent(this);
	childrenNumber++;
	if(childrenNumber == 1)
		children = (TreeNode**)malloc(sizeof(TreeNode*));
	else
		children = (TreeNode**)realloc(children, childrenNumber*sizeof(TreeNode*));
	children[childrenNumber-1] = newChild;
}

int TreeNode::getPusherPrePosition(void) const
{
	const Solver* solver = node->getSolver();
	int pos;
	int posBox;
	int prepos;

	// First node
	if(parent == NULL)
	{
		// Initialization with first position
		prepos = -1;
	}
	else
	{
		// * We find position of this node
		//   1. Test what box has moved (zone with 2 "1" : box from and box to)
		Zone* diffZone = new Zone(node->getBoxesZone(),
								  parent->getNode()->getBoxesZone(),
								  Zone::USE_XOR);

		//   2. Find 2 boxes positions
		int pos1 = -1;
		int pos2 = -1;
		for(int i=0;i<diffZone->getLength();i++)
		{
			if(diffZone->readPos(i) == 1)
			{
				if(pos1 == -1)
					pos1 = solver->getZoneToLevelPos()[i];
				else
				{
					pos2 = solver->getZoneToLevelPos()[i];
					break;
				}
			}
		}
		delete diffZone;

		//   3. If first position is empty on this node, then pusher is there,
		//		if not, then pusher is on the second position
		if(node->getBoxesZone()->readPos(solver->getLevelToZonePos()[pos1]) == 0)
		{
			pos = pos1;
			posBox = pos2;
		}
		else
		{
			pos = pos2;
			posBox = pos1;
		}

		//   4. We got pusher position and pushed box position.
		//      Now we need to know cell just before them in the same axe.
		//      Like this : |prepos|pos|posBox|.
		prepos = pos - (posBox - pos);
	}

	return prepos;
}

int TreeNode::getPusherPosition(void) const
{
	const Solver* solver = node->getSolver();
	const Level* level = solver->getLevel();
	const int* lToZPos = solver->getLevelToZonePos();
	int nCols = level->getColsNumber();
	int pos = -1;

	// First node
	if(parent == NULL)
	{
		// Initialization with first position
		int posM = level->getPusherPosM();
		int posN = level->getPusherPosN();
		pos = posM*level->getColsNumber()+posN;
	}
	else
	{
		// * We find position of this node
		//   1. Test what box has moved (zone with 2 "1" : box from and box to)
		Zone* diffZone = new Zone(node->getBoxesZone(),
								  parent->getNode()->getBoxesZone(),
								  Zone::USE_XOR);

		//   2. Find 2 boxes positions in this zone
		int pos1 = -1;
		int pos2 = -1;
		for(int i=0;i<diffZone->getLength();i++)
		{
			if(diffZone->readPos(i) == 1)
			{
				if(pos1 == -1)
					pos1 = solver->getZoneToLevelPos()[i];
				else
				{
					pos2 = solver->getZoneToLevelPos()[i];
					break;
				}
			}
		}
		delete diffZone;

		//   3. We get position of node box (so after it moves)
		int nodeBoxPos;
		int parentBoxPos;
		if(node->getBoxesZone()->readPos(lToZPos[pos1]) == 1)
		{
			nodeBoxPos = pos1;
			parentBoxPos = pos2;
		}
		else
		{
			nodeBoxPos = pos2;
			parentBoxPos = pos1;
		}

		//   4. We get last push of parent
		int boxPosA = parentBoxPos;
		int boxPosB = nodeBoxPos;
		DijkstraBox* dBox = new DijkstraBox(node->getSolver(),
				parent->getNode(), boxPosA);
		int* tmp = dBox->resolvePositions();

		int min = INT_MAX;
		int minDir = -1;
		for(int i=0;i<4;i++)
		{
			int newValue = dBox->getPos(boxPosB, i);
			if(newValue < min)
			{
				min = newValue;
				minDir = i;
			}
		}
		free(tmp);
		delete dBox;

		if(minDir == DijkstraBox::RIGHT)
			pos = nodeBoxPos + 1;
		else if(minDir == DijkstraBox::LEFT)
			pos = nodeBoxPos - 1;
		else if(minDir == DijkstraBox::UP)
			pos = nodeBoxPos - nCols;
		else if(minDir == DijkstraBox::DOWN)
			pos = nodeBoxPos + nCols;
	}

	return pos;
}

int TreeNode::getPushedBoxPrePosition(void) const
{
	const Solver* solver = node->getSolver();
	int pos;

	// First node
	if(parent == NULL)
	{
		pos = -1;
	}
	else
	{
		// * We find position of this node
		//   1. Test what box has moved (zone with 2 "1" : box from and box to)
		Zone* diffZone = new Zone(node->getBoxesZone(),
								  parent->getNode()->getBoxesZone(),
								  Zone::USE_XOR);

		//   2. Find 2 boxes positions in this zone
		int pos1 = -1;
		int pos2 = -1;
		int length = diffZone->getLength();
		for(int i=0;i<length;i++)
		{
			if(diffZone->readPos(i) == 1)
			{
				if(pos1 == -1)
					pos1 = solver->getZoneToLevelPos()[i];
				else
				{
					pos2 = solver->getZoneToLevelPos()[i];
					break;
				}
			}
		}
		delete diffZone;

		//   3. If first position is a box on parent node, then pushed box is
		//      there, if not, then pushed box is on the second position
		Zone* pBoxesZone = parent->getNode()->getBoxesZone();
		if(pBoxesZone->readPos(solver->getLevelToZonePos()[pos1]) == 1)
			pos = pos1;
		else
			pos = pos2;
	}

	return pos;
}

int TreeNode::getPushedBoxPostPosition(void) const
{
	const Solver* solver = node->getSolver();
	int pos;

	// First node
	if(parent == NULL)
	{
		pos = -1;
	}
	else
	{
		// * We find position of this node
		//   1. Test what box has moved (zone with 2 "1" : box from and box to)
		Zone* diffZone = new Zone(node->getBoxesZone(),
								  parent->getNode()->getBoxesZone(),
								  Zone::USE_XOR);

		//   2. Find 2 boxes positions in this zone
		int pos1 = -1;
		int pos2 = -1;
		int length = diffZone->getLength();
		for(int i=0;i<length;i++)
		{
			if(diffZone->readPos(i) == 1)
			{
				if(pos1 == -1)
					pos1 = solver->getZoneToLevelPos()[i];
				else
				{
					pos2 = solver->getZoneToLevelPos()[i];
					break;
				}
			}
		}
		delete diffZone;

		//   3. If first position is a box on this node, then pushed box is there,
		//		if not, then pushed box is on the second position
		if(node->getBoxesZone()->readPos(solver->getLevelToZonePos()[pos1]) == 1)
			pos = pos1;
		else
			pos = pos2;
	}

	return pos;
}

int* TreeNode::listOfPusherPositions(void)
{
	int* positions = NULL;

	if(parent == NULL)
	{
		positions = (int*)malloc(sizeof(int));
		positions[0] = -1;
	}
	else
	{
		positions = parent->listOfPusherPositions();
	}

	int pos = getPusherPosition();

	int size=0;
	while(positions[size] != -1)
		size++;
	positions = (int*)realloc(positions, (size+2)*sizeof(int));
	positions[size] = pos;
	positions[size+1] = -1;

	return positions;
}

Path* TreeNode::getPath(void)
{
	const Solver* solver = node->getSolver();
	Base* base = solver->getBase();
	const Level* level = solver->getLevel();
	int nCols = level->getColsNumber();
	Path* path = NULL;
	char* pathChar = (char*)malloc(sizeof(char));
	int pathCharLength = 0;
	pathChar[0]='\0';

	// Until we reach first node
	TreeNode* parentNode = parent;
	TreeNode* actualNode = this;

	while(parentNode != NULL)
	{
		// Find pusher moves to push box
		int boxPosA = actualNode->getPushedBoxPrePosition();
		int boxPosB = actualNode->getPushedBoxPostPosition();
		DijkstraBox* dBox = new DijkstraBox(node->getSolver(),
				parentNode->getNode(), boxPosA);

		int* tmp = dBox->resolvePositions();
		free(tmp);
		char* pusherMoves = dBox->listMovesToPosition(boxPosB);
		delete dBox;

		// Find pos of pusher before he pushes any box
		int prePusherPos = -1;
		if(strlen(pusherMoves) != 0)
		{
			char firstMove = pusherMoves[0];
			if(firstMove == 'R')
				prePusherPos = boxPosA - 1;
			else if(firstMove == 'L')
				prePusherPos = boxPosA + 1;
			else if(firstMove == 'U')
				prePusherPos = boxPosA + nCols;
			else if(firstMove == 'D')
				prePusherPos = boxPosA - nCols;
		}

		char* prePusherMoves;
		if(prePusherPos != -1)
		{
			int parentPusherPos = parentNode->getPusherPosition();
			Node* pNode = parentNode->getNode();
			prePusherMoves = pNode->listMovesFromAToB(parentPusherPos,
					prePusherPos);
		}
		else
		{
			prePusherMoves = (char*)malloc(sizeof(char));
			prePusherMoves[0] = '\0';
		}

		// Copy (reverse) pusherMoves to pathChar
		int pusherMovesL = strlen(pusherMoves);
		pathCharLength += pusherMovesL;
		pathChar = (char*)realloc(pathChar, (pathCharLength+1)*sizeof(char));
		for(int i=0;i<pusherMovesL;i++)
			pathChar[pathCharLength-pusherMovesL+i]
			         = pusherMoves[pusherMovesL-1-i];
		pathChar[pathCharLength] = '\0';

		// Copy(reverse) pre-pusher moves to pathChar
		int prePusherMovesL = strlen(prePusherMoves);
		pathCharLength += prePusherMovesL;
		pathChar = (char*)realloc(pathChar, (pathCharLength+1)*sizeof(char));
		for(int i=0;i<prePusherMovesL;i++)
			pathChar[pathCharLength-prePusherMovesL+i]
			         = prePusherMoves[prePusherMovesL-1-i];
		pathChar[pathCharLength] = '\0';

		free(pusherMoves);
		free(prePusherMoves);

		actualNode = parentNode;
		parentNode = parentNode->getParent();
	}

	path = new Path(base);
	pathCharLength = strlen(pathChar);
	for(int i=pathCharLength-1;i>=0;i--)
	{
		path->addDisplacement(pathChar[i]);
	}

	free(pathChar);

	return path;
}

int* TreeNode::computeTreeBoxStats(int* statsTab) const
{
	const Solver* solver = node->getSolver();
	int length = solver->getZoneToLevelPosLength();

	if(statsTab == NULL)
	{
		statsTab = (int*)malloc(length*sizeof(int));
		for(int i=0;i<length;i++)
			statsTab[i] = 0;
	}

	Zone* boxes = node->getBoxesZone();
	for(int i=0;i<length;i++)
	{
		if(boxes->readPos(i) == 1)
			statsTab[i]++;
	}

	for(int i=0;i<childrenNumber;i++)
		statsTab = children[i]->computeTreeBoxStats(statsTab);

	return statsTab;
}
