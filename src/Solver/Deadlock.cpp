/*----------------------------------------------*/
/*================== ISokoban ==================*/
/*----------------------------------------------*/
/* Version : 1.0								*/
/* Author : Michaël Hoste						*/
/* Copyright : See LICENSE file for details   	*/
/*----------------------------------------------*/

#include "../../include/Solver/Deadlock.h"
#include "../../include/Solver/BotGoodPushesS/BotGoodPushesS.h"

/* ------------*/
/* Constructor */
/* ------------*/
Deadlock::Deadlock(Solver* solver):
	base(solver->getBase()),
	solver(solver),
	deadlockZone(NULL),
	deadlockNodeList(NULL),
	deadlockNodeListLength(0),
	deadlockTestTab(NULL),
	deadlockTestTabLength(0),
	emptyLevel(NULL),
	level(solver->getLevel()),
	goalZone(solver->getGoalZone()),
	zoneToLevelPos(solver->getZoneToLevelPos()),
	zoneToLevelPosLength(solver->getZoneToLevelPosLength()),
	levelToZonePos(solver->getLevelToZonePos())
{
	// Init of deadlock zone (if a box is on a marked position of this zone,
	// state of level is deadlocked)
	deadlockZone = new Zone(level, levelToZonePos, zoneToLevelPos,
			zoneToLevelPosLength, Zone::PROCESS_DEADLOCK);

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

	// Init of deadlock zone list (list of zones with multiple boxes that
	// create deadlocks not catchable by traditionnal methods)
//	initDeadlockList(solver->getDeadlockedBoxesSearch());
}

/* -----------*/
/* Destructor */
/* -----------*/
Deadlock::~Deadlock()
{
	if(deadlockZone)
		delete deadlockZone;
	if(deadlockNodeList)
	{
		int i=0;
		while(deadlockNodeList[i] != NULL)
		{
			delete deadlockNodeList[i];
			i++;
		}
		free(deadlockNodeList);
	}
	if(deadlockTestTab)
		free(deadlockTestTab);
	if(emptyLevel)
		delete emptyLevel;
}

/* -------*/
/* Others */
/* -------*/
bool Deadlock::deadlockedNode(TreeNode* treenode) const
{
	if(simpleDeadlock(treenode))
		return true;
	else if(complexDeadlock(treenode))
		return true;
	else
		return false;
}

bool Deadlock::deadlockedLastMove(const TreeNode* treenode, int posBox) const
{
	if(posBox == -2)
		posBox = treenode->getPushedBoxPostPosition();

	// If it's not first node and not in a goal position
	if(posBox != -1)
	{
		// ========================
		// Last move made a square
		// ========================

		int n = level->getColsNumber();

		// neighbours positions
		int posLeft =  levelToZonePos[posBox - 1];
		int posRight = levelToZonePos[posBox + 1];
		int posDown =  levelToZonePos[posBox + n];
		int posUp =    levelToZonePos[posBox - n];

		int posUpLeft =    levelToZonePos[posBox - n - 1];
		int posDownLeft =  levelToZonePos[posBox + n - 1];
		int posUpRight =   levelToZonePos[posBox - n + 1];
		int posDownRight = levelToZonePos[posBox + n + 1];

		// neighbours goals status
		int goalBox = goalZone->readPos(levelToZonePos[posBox]);
		int goalLeft = -1;
		int goalRight = -1;
		int goalDown = -1;
		int goalUp = -1;

		if(posLeft != -1)
			goalLeft = goalZone->readPos(posLeft);
		if(posRight != -1)
			goalRight = goalZone->readPos(posRight);
		if(posDown != -1)
			goalDown = goalZone->readPos(posDown);
		if(posUp != -1)
			goalUp = goalZone->readPos(posUp);

		Zone * boxesZone = treenode->getNode()->getBoxesZone();

		// up-right square
		if(		(posUp == -1 || boxesZone->readPos(posUp) == 1)
			&&  (posUpRight == -1 || boxesZone->readPos(posUpRight) == 1)
			&&  (posRight == -1 || boxesZone->readPos(posRight) == 1))
		{
			// If box or one of concerned neighbours is not on goal, we block
			if(goalBox == 0 || goalUp == 0 || goalRight == 0)
				return true;
		}

		// up-left square
		if(		(posUp == -1 || boxesZone->readPos(posUp) == 1)
			&&  (posUpLeft == -1 || boxesZone->readPos(posUpLeft) == 1)
			&&  (posLeft == -1 || boxesZone->readPos(posLeft) == 1))
		{
			// If box or one of concerned neighbours is not on goal, we block
			if(goalBox == 0 || goalUp == 0 || goalLeft == 0)
				return true;
		}

		// down-right square
		if(		(posDown == -1 || boxesZone->readPos(posDown) == 1)
			&&  (posDownRight == -1 || boxesZone->readPos(posDownRight) == 1)
			&&  (posRight == -1 || boxesZone->readPos(posRight) == 1))
		{
			// If box or one of concerned neighbours is not on goal, we block
			if(goalBox == 0 || goalDown == 0 || goalRight == 0)
				return true;
		}

		// down-left square
		if(		(posDown == -1 || boxesZone->readPos(posDown) == 1)
			&&  (posDownLeft == -1 || boxesZone->readPos(posDownLeft) == 1)
			&&  (posLeft == -1 || boxesZone->readPos(posLeft) == 1))
		{
			// If box or one of concerned neighbours is not on goal, we block
			if(goalBox == 0 || goalDown == 0 || goalLeft == 0)
				return true;
		}

		// ===================
		// Last move made a Z
		// ===================
		// * Z deadlock :
		// * #####   #####   ######   ######
		// * #   #   #   #   # #  #   #  # #
		// * ##O #   # O##   # OO #   # OO #
		// * # O##   ##O #   #  # #   # #  #
		// * #   #   #   #   ######   ######
		// * #####   #####

		// fig 1, upper box
		if(posLeft == -1 && posDownRight == -1
				&& boxesZone->readPos(posDown) == 1)
			if(goalBox == 0 || goalDown == 0)
				return true;

		// fig 1, lower box
		if(posRight == -1 && posUpLeft == -1
				&& boxesZone->readPos(posUp) == 1)
			if(goalBox == 0 || goalUp == 0)
				return true;

		// fig 2, upper box
		if(posRight == -1 && posDownLeft == -1
				&& boxesZone->readPos(posDown) == 1)
			if(goalBox == 0 || goalDown == 0)
				return true;

		// fig 2, lower box
		if(posLeft == -1 && posUpRight == -1
				&& boxesZone->readPos(posUp) == 1)
			if(goalBox == 0 || goalUp == 0)
				return true;

		// fig 3, left box
		if(posUp == -1 && posDownRight == -1
				&& boxesZone->readPos(posRight) == 1)
			if(goalBox == 0 || goalRight == 0)
				return true;

		// fig 3, right box
		if(posDown == -1 && posUpLeft == -1
				&& boxesZone->readPos(posLeft) == 1)
			if(goalBox == 0 || goalLeft == 0)
				return true;

		// fig 4, left box
		if(posDown == -1 && posUpRight == -1
				&& boxesZone->readPos(posRight) == 1)
			if(goalBox == 0 || goalRight == 0)
				return true;

		// fig 4, right box
		if(posUp == -1 && posDownLeft == -1
				&& boxesZone->readPos(posLeft) == 1)
			if(goalBox == 0 || goalLeft == 0)
				return true;
	}

	return false;
}

bool Deadlock::deadlockedCorner(const TreeNode* treenode) const
{
	bool ret;
	// A box is on a deadlock cell
	Zone* test = new Zone(treenode->getNode()->getBoxesZone(),
			deadlockZone, Zone::USE_AND);
	if(!test->isFullOf0())
		ret = true;
	else
		ret = false;

	delete test;

	return ret;
}

bool Deadlock::deadlockedZone(TreeNode* treenode, int maxN) const
{
	Zone* boxesZone = treenode->getNode()->getBoxesZone();
	Zone* pusherZone = treenode->getNode()->getPusherZone();

	// Level representation of box position
	int levelPosBox = treenode->getPushedBoxPostPosition();

	int zonePosBox;
	if(levelPosBox == -1)
	{
		zonePosBox = -1;
		maxN = 1;
	}
	else
		zonePosBox = levelToZonePos[levelPosBox];

	// Creation of tab of all empty zones except the ones with pusher
	Zone** zone1Tab = createZone1Tab(boxesZone, pusherZone);

	// If maxN = -1, we assign maximum number of possible junction to maxN
	if(maxN == -1)
		for(maxN=0;zone1Tab[maxN] != NULL;maxN++);

	// Creation of a tab (maxN cells) of chained lists with zones
	ZoneList** zoneListTab = (ZoneList**)malloc((maxN+1)*sizeof(ZoneList*));
	for(int i=0;i<maxN;i++)
		zoneListTab[i] = NULL;
	zoneListTab[maxN] = NULL;

	// If no parent node, we only test every zone1Tab
//	if(levelPosBox == -1)
//	{
//		int i=0;
//		while(zone1Tab[i] != NULL)
//		{
//			zoneListTab[0] = new ZoneList(new Zone(zone1Tab[i]),
//					zoneListTab[0]);
//			i++;
//		}
//	}

	// Get all empty neighbours positions of last moved box (8 positions)
	int* neiTab = createNeighboursTab(boxesZone, levelPosBox);

	zoneListTab = createInitialZoneListTab(neiTab, boxesZone, pusherZone,
			zoneListTab);

	int maxZoneToTest = DEADLOCKEDZONE_MAXZONESTOCREATE;
	for(int i=1;i<maxN && maxZoneToTest>0;i++)
		zoneListTab = createIZoneListTab(zoneListTab, zone1Tab, i+1,
											&maxZoneToTest);

	// Test each created zone
	bool blocked = false;
	for(int i=0;i<maxN && blocked == false;i++)
	{
		ZoneList* currentZoneList = zoneListTab[i];
		while(currentZoneList != NULL)
		{
			if(isBlockedZone(currentZoneList->zone, treenode, BLOCKEDZONE_DEEPNESS))
			{
				blocked = true;
				break;
			}
			else
				currentZoneList = currentZoneList->next;
		}
	}

	// Free memory
	int i=0;
	while(zone1Tab[i] != NULL)
	{
		delete zone1Tab[i];
		i++;
	}
	free(zone1Tab);

	for(int i=0;i<maxN;i++)
	{
		delete zoneListTab[i];
	}
	free(zoneListTab);
	free(neiTab);

	return blocked;
}

bool Deadlock::simpleDeadlock(TreeNode* treenode) const
{
	if(deadlockedCorner(treenode))
		return true;
	else if(deadlockedLastMove(treenode))
		return true;
	else
		return false;
}

bool Deadlock::complexDeadlock(TreeNode* treenode) const
{
	if(deadlockedZone(treenode, DEADLOCKEDZONE_MAXLEVELOFJUNCTIONS))
		return true;
	else
		return false;
}

Zone** Deadlock::createZone1Tab(Zone* boxesZone, Zone* pusherZone) const
{
	int nCols = level->getColsNumber();

	Zone** zone1Tab = (Zone**)malloc(sizeof(Zone*));
	int zone1TabLength = 0;
	zone1Tab[0] = NULL;

	// Create zone full of 0 except boxes positions and pusher zone position
	Zone* markedZone = new Zone(boxesZone, pusherZone, Zone::USE_OR);

	// For each pos of the zone
	for(int i=0;i<zoneToLevelPosLength;i++)
	{
		// If pos is not marked
		if(markedZone->readPos(i) == 0)
		{
			// Create (pusher) zone from this position
			Zone* currentZone = new Zone(boxesZone, zoneToLevelPos[i],
					levelToZonePos, nCols);

			// Mark this zone
			markedZone->applyOrWith(currentZone);

			// We add this zone on the zone tab
			zone1TabLength++;
			zone1Tab = (Zone**)realloc(zone1Tab,
								(zone1TabLength+1)*sizeof(Zone*));
			zone1Tab[zone1TabLength-1] = currentZone;
			zone1Tab[zone1TabLength] = NULL;
		}
	}

	delete markedZone;

	return zone1Tab;
}

int* Deadlock::createNeighboursTab(Zone* boxesZone, int posBox) const
{
	int* tab;
	tab = (int*)malloc(8*sizeof(int));

	// Initial state
	if(posBox == -1)
	{
		for(int i=0;i<8;i++)
			tab[i] = -1;
		return tab;
	}

	int nCols = level->getColsNumber();

	// Get every neighbours cells (zone representation)
	tab[0] = levelToZonePos[posBox+1];
	tab[1] = levelToZonePos[posBox-1];
	tab[2] = levelToZonePos[posBox+nCols];
	tab[3] = levelToZonePos[posBox-nCols];
	tab[4] = levelToZonePos[posBox+1+nCols];
	tab[5] = levelToZonePos[posBox-1+nCols];
	tab[6] = levelToZonePos[posBox+1-nCols];
	tab[7] = levelToZonePos[posBox-1-nCols];

	// If neighbours is a box, don't keep it
	for(int i=0;i<8;i++)
	{
		if(tab[i] != -1 && boxesZone->readPos(tab[i]))
			tab[i] = -1;
	}

	return tab;
}

ZoneList** Deadlock::createInitialZoneListTab(int* neiTab, Zone* boxesZone,
		Zone* pusherZone, ZoneList** zoneListTab) const
{
	int nCols = level->getColsNumber();

	// For each neighbours
	for(int i=0;i<8;i++)
	{
		// if neighbours is a free cell
		if(neiTab[i] != -1)
		{
			// if position is not included in previous keeped zone
			ZoneList* currentZoneList = zoneListTab[0];
			bool found = false;
			while(currentZoneList != NULL)
			{
				if(currentZoneList->zone->readPos(neiTab[i]))
				{
					found = true;
					break;
				}
				else
					currentZoneList = currentZoneList->next;
			}

			// If new zone
			if(!found)
			{
				// Add new zone to the first chained list of zones
				Zone* newZone = new Zone(boxesZone, zoneToLevelPos[neiTab[i]],
						levelToZonePos, nCols);
				if(!pusherZone->isEgal(newZone))
					zoneListTab[0] = new ZoneList(newZone, zoneListTab[0]);
				else
					delete newZone;
			}
		}
	}

	return zoneListTab;
}

ZoneList** Deadlock::createIZoneListTab(ZoneList** zoneListTab,
		Zone** zoneTab, int size, int* maxZoneToTest) const
{
	// for each zone of last iteration
	ZoneList* currentZoneList = zoneListTab[size-2];
	while(currentZoneList != NULL)
	{
		// For each zone of tab
		int i=0;
		while(zoneTab[i] != NULL && (*maxZoneToTest) > 0)
		{
			// If they are at least one box in common and are not the same
			Zone* testZone = new Zone(currentZoneList->zone,
					zoneTab[i], Zone::USE_AND);
			if(!testZone->isFullOf0())
			{
				Zone* newZone = new Zone(currentZoneList->zone,
					zoneTab[i], Zone::USE_OR);

				// If junction is not the same than previous size zone
				if(!newZone->isEgal(currentZoneList->zone))
				{
					// If this junction is new
					ZoneList* currentZoneList2 = zoneListTab[size-1];
					bool found = false;
					while(currentZoneList2 != NULL)
					{
						if(newZone->isEgal(currentZoneList2->zone))
						{
							found = true;
							(*maxZoneToTest)--;
							break;
						}
						else
							currentZoneList2 = currentZoneList2->next;
					}

					if(!found)
						zoneListTab[size-1] = new ZoneList(newZone,
								zoneListTab[size-1]);
					else
						delete newZone;
				}
				else
					delete newZone;
			}
			delete testZone;
			i++;
		}
		currentZoneList = currentZoneList->next;
	}

	return zoneListTab;
}

bool Deadlock::isBlockedZone(const Zone* zone, TreeNode* treenode, int n) const
{
	// Limit of recursivity so complexity doesn't explode with some levels
	if(n == 0)
		return 0;

	bool ret = true;

	Zone* boxesZone = treenode->getNode()->getBoxesZone();

	// Is any pusher or goal present in this zone ?
	Zone* pusherZone = treenode->getNode()->getPusherZone();
	Zone* reachablePusherZone = new Zone(zone, pusherZone, Zone::USE_AND);
	reachablePusherZone->applyMinusWith(boxesZone);

	Zone* reachableGoal = new Zone(zone, goalZone, Zone::USE_AND);

	// If pusher and goal not in zone
	if(reachablePusherZone->isFullOf0() && reachableGoal->isFullOf0())
	{
		Zone* reachableBoxes = new Zone(zone, boxesZone, Zone::USE_AND);

		// For each pos of the zone
		for(int j=0;j<zoneToLevelPosLength;j++)
		{
			// If this is a box of the zone
			if(reachableBoxes->readPos(j) == 1)
			{
				ret = isZoneBlockedBox(zoneToLevelPos[j],
						treenode, zone, n);

				if(ret == false)
					break;
			}
		}

		delete reachableBoxes;
	}
	else
		ret = false;

	delete reachablePusherZone;
	delete reachableGoal;

	return ret;
}

bool Deadlock::isZoneLimitedBox(int boxPos, TreeNode* treenode,
		const Zone* testZone) const
{
	// Get the boxesZone from treenode
	Zone* boxesZone = treenode->getNode()->getBoxesZone();

	int nCols = level->getColsNumber();

	// Store every positions around the box...
	int listPos[4];
	listPos[0] = levelToZonePos[boxPos+1];
	listPos[1] = levelToZonePos[boxPos - nCols];
	listPos[2] = levelToZonePos[boxPos-1];
	listPos[3] = levelToZonePos[boxPos + nCols];

	// We test every move and store -1 in impossible move (wall or zone's box)
	bool proceed;
	bool nearZoneBox = false;
	for(int i=0;i<4;i++)
	{
		proceed = false;
		int currentPos = listPos[i];

		if(currentPos == -1)
			proceed = true;
		else if((    boxesZone->readPos(currentPos) == 1
		    && testZone->readPos(currentPos) == 1))
		{
			proceed = true;
			nearZoneBox = true;
		}

		if(proceed)
			listPos[i] = listPos[(i+2)%4] = -1;
	}

	 // We need to have minimum one neighbours position on a zone's box
	int count = 0;
	if(nearZoneBox)
	{
		for(int i=0;i<4 && listPos[i] == -1;i++)
			count++;
	}

	if(count == 4)
		return true;
	else
		return false;
}

bool Deadlock::isZoneBlockedBox(int boxPos, TreeNode* treenode,
		const Zone* testZone, int n) const
{
	// Get the boxesZone from treenode
	Zone* boxesZone = treenode->getNode()->getBoxesZone();

	int nCols = level->getColsNumber();

	// Store every positions around the box...
	int listPos[4][2];
	listPos[0][0] = levelToZonePos[boxPos+1];
	listPos[1][0] = levelToZonePos[boxPos-1];
	listPos[2][0] = levelToZonePos[boxPos - nCols];
	listPos[3][0] = levelToZonePos[boxPos + nCols];
	// .. and there opposite relative position
	listPos[0][1] = listPos[1][0];
	listPos[1][1] = listPos[0][0];
	listPos[2][1] = listPos[3][0];
	listPos[3][1] = listPos[2][0];

	// Store every position in the zone where we push this box
	int zonePos[4];
	int zonePosLength=0;

	// We store each possible moves in the zone of this box. If a move is
	// possible out of the zone, then this box is not blocked and return false
	for(int i=0;i<4;i++)
	{
		int prevPos = listPos[i][0];
		int nextPos = listPos[i][1];
		bool prevPosIsInTheZone = false;
		bool prevPosIsLimitedBox = false;
		bool nextPosIsInTheZone = false;
		bool nextPosIsLimitedBox = false;

		if(prevPos != -1)
		{
			prevPosIsInTheZone = testZone->readPos(prevPos);
			if(boxesZone->readPos(prevPos) == 1)
				prevPosIsLimitedBox = isZoneLimitedBox(zoneToLevelPos[prevPos],
					treenode, testZone);
		}

		if(nextPos != -1)
		{
			nextPosIsInTheZone = testZone->readPos(nextPos);
			if(boxesZone->readPos(nextPos) == 1)
				nextPosIsLimitedBox = isZoneLimitedBox(zoneToLevelPos[nextPos],
					treenode, testZone);
		}

		if(  prevPos != -1	    // this position is not a wall...
		  && !prevPosIsInTheZone// ... and not in the zone (box included)...
		  && !prevPosIsLimitedBox) // ... and not a zone limited box
		{
			// if a opposite position is free and not in testzone...
			if(  nextPos != -1
			  && !nextPosIsInTheZone
			  && !nextPosIsLimitedBox)
			{
				//... And new node is not blocked, zone's not blocked

				// Creation of temporary new treeNode to test
				Zone* tempBoxesZone = new Zone(boxesZone);
				tempBoxesZone->write0ToPos(levelToZonePos[boxPos]);
				tempBoxesZone->write1ToPos(nextPos);
				Zone* tempPusherZone = new Zone(tempBoxesZone, boxPos,
						levelToZonePos,	nCols);
				Node* tempNode = new Node(solver);
				tempNode->setBoxesZone(tempBoxesZone);
				tempNode->setPusherZone(tempPusherZone);

				// Create temp treenode
				TreeNode* tempTreeNode = new TreeNode(tempNode);
				if(treenode)
					treenode->addChild(tempTreeNode);

				if(!simpleDeadlock(tempTreeNode))
				{
					delete tempTreeNode;
					return 0;
				}
				delete tempTreeNode;
			}
			// else if a opposite position is free and in testzone, we keep
			// this pos in memory to test it just after
			else if(  nextPos != -1
				   && !boxesZone->readPos(nextPos)
				   && testZone->readPos(nextPos))
			{
				zonePos[zonePosLength] = nextPos;
				zonePosLength++;
			}
		}
	}

	// We create a temporary treeNode with simulation of every pushes of this box
	// in the zone and we try every deadlock test on this temporary treeNode
	bool blocked = true;
	int blockedNumber = 0;
	for(int i=0;i<zonePosLength && blocked == true;i++)
	{
		// Creation of temporary new treeNode to test
		Zone* tempBoxesZone = new Zone(boxesZone);
		tempBoxesZone->write0ToPos(levelToZonePos[boxPos]);
		tempBoxesZone->write1ToPos(zonePos[i]);
		Zone* tempPusherZone = new Zone(tempBoxesZone, boxPos, levelToZonePos,
										nCols);
		Node* tempNode = new Node(solver);
		tempNode->setBoxesZone(tempBoxesZone);
		tempNode->setPusherZone(tempPusherZone);

		// Create temp treenode
		TreeNode* tempTreeNode = new TreeNode(tempNode);
		if(treenode)
			treenode->addChild(tempTreeNode);

		// Test of every deadlock on this temporary treeNode, if each stored
		// move of the box is deadlocked, box is blocked and we return true

		// Test simple deadlocks (no recursivity)
		if(simpleDeadlock(tempTreeNode))
			blockedNumber++;
		// if pusher is not in the new zone and zone blocked box
		else
		{
			// We create a new sub-testZone from initial testZone and test
			// if these new sub-testZone are deadlocked. If one of them is
			// deadlocked, this box is blocked.
			Zone* markedZone = new Zone(tempBoxesZone);
			int newPosToMakeTestZone = -1;
			Zone** newTestZone = (Zone**)malloc(sizeof(Zone*));
			int newTestZoneLength = 0;
			for(int j=0;j<zoneToLevelPosLength;j++)
			{
				// Is not marked position
				if(markedZone->readPos(j) == 0)
				{
					int tzPos = testZone->readPos(j);
					if(tzPos == 1 && levelToZonePos[boxPos] != j)
					{
						newPosToMakeTestZone = zoneToLevelPos[j];

						// Store newTestZone
						newTestZone = (Zone**)realloc(newTestZone,
								(newTestZoneLength+1)*sizeof(Zone*));
						newTestZone[newTestZoneLength]
						      = new Zone(tempBoxesZone, newPosToMakeTestZone,
						                		  levelToZonePos, nCols);

						// Mark this pusherZone
						markedZone->applyOrWith(newTestZone[newTestZoneLength]);

						newTestZoneLength++;
					}
				}
			}

			blocked = false;

			// If one of subzone is blocked, then box is blocked
			for(int j=0;j<newTestZoneLength && blocked == false;j++)
				blocked = isBlockedZone(newTestZone[j], tempTreeNode, n-1);

			// If total zone is blocked, then box is blocked
			if(blocked == false && newTestZoneLength >= 2)
			{
				Zone* totalTestZone = new Zone(newTestZone[0]);
				for(int j=1;j<newTestZoneLength;j++)
					totalTestZone->applyOrWith(newTestZone[j]);

				blocked = isBlockedZone(totalTestZone, tempTreeNode, n-1);

				delete totalTestZone;
			}

			if(blocked == true)
				blockedNumber++;

			for(int j=0;j<newTestZoneLength;j++)
				delete newTestZone[j];
			free(newTestZone);

			delete markedZone;
		}

		delete tempTreeNode;
	}

	if(zonePosLength == 0 || blockedNumber == zonePosLength)
		return true; // every moves of this box in the zone is deadlocked
	else
		return false; // possible move with no deadlock
}
