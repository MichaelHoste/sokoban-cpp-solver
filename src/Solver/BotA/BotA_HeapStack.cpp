/*----------------------------------------------*/
/*================== ISokoban ==================*/
/*----------------------------------------------*/
/* Version : 1.0								*/
/* Author : Michaël Hoste						*/
/* Copyright : See LICENSE file for details   	*/
/*----------------------------------------------*/

#include "../../../include/Solver/BotA/BotA_HeapStack.h"

/* ------------*/
/* Constructor */
/* ------------*/
BotA_HeapStack::BotA_HeapStack():
	ChainedList(),
	treeNodeTab(NULL),
	tabLength(0)
{
	tabLength = TAB_MEMORY_STEP;
	treeNodeTab = (BotA_TreeNode**)malloc(tabLength*sizeof(BotA_TreeNode*));
}

/* -----------*/
/* Destructor */
/* -----------*/
BotA_HeapStack::~BotA_HeapStack()
{
	free(treeNodeTab);
}

/* -------*/
/* Others */
/* -------*/
void BotA_HeapStack::addItem(TreeNode* item)
{
	if(item)
	{
		length++;
		if(length > tabLength)
		{
			tabLength += TAB_MEMORY_STEP;
			treeNodeTab = (BotA_TreeNode**)realloc(treeNodeTab,
					tabLength*sizeof(BotA_TreeNode*));
		}
		treeNodeTab[length-1] = (BotA_TreeNode*)item;

		// store index of tab in the close hashtable
		const Solver* solver = item->getNode()->getSolver();
		((BotA_HashTable*)solver->getCloseTable())->changeHeapStackCell(
									item->getNode(), length-1);

		repositionCell(length-1);
	}
	else
		printf("BotA_HeapStack :: TreeNode to add is NULL.\n");
}

void BotA_HeapStack::deleteFirstItem(void)
{
	if(length > 0)
	{
		treeNodeTab[0] = treeNodeTab[length-1];
		length--;
		if(length < tabLength - 2*TAB_MEMORY_STEP)
		{
			tabLength -= 2*TAB_MEMORY_STEP;
			treeNodeTab = (BotA_TreeNode**)realloc(treeNodeTab,
					tabLength*sizeof(BotA_TreeNode*));
		}

		HashTable* closeTable = treeNodeTab[0]->getNode()
										->getSolver()->getCloseTable();
		// modify index of tab in the close hashtable
		((BotA_HashTable*)closeTable)->changeHeapStackCell(
				treeNodeTab[0]->getNode(), 0);

		minHeapify(0);
	}
	else
		printf("BotA_HeapStack :: No more item to delete (heapstack is empty).\n");
}

ListNode* BotA_HeapStack::getFirstItem(void) const
{
	if(length > 0)
		return new BotA_ListNode(treeNodeTab[0]);
	else
		return NULL;
}

bool BotA_HeapStack::isSmaller(int cell1, int cell2) const
{
	const Solver* solver = treeNodeTab[cell1]->getNode()->getSolver();
	int costLimit = ((BotA*)solver)->getCostLimit();

	// If costLimit = +infty, then use of A* with no limit, else if
	// costLimit = value, then use of A* with IDA* and quick result
	if(costLimit == INT_MAX)
		return
		(treeNodeTab[cell1]->getF() < treeNodeTab[cell2]->getF()
		  ||(  treeNodeTab[cell1]->getF() == treeNodeTab[cell2]->getF()
		    && treeNodeTab[cell1]->getH() < treeNodeTab[cell2]->getH()));
	else
		return (treeNodeTab[cell1]->getH() < treeNodeTab[cell2]->getH());
}

void BotA_HeapStack::repositionCell(int cell)
{
	HashTable* closeTable = treeNodeTab[cell]->getNode()
									->getSolver()->getCloseTable();

	// While new cell F value is lesser than parent cell, we switch
	// If new cell F value is egal but H value is lesser, we switch too
	while(cell>0 && isSmaller(cell, parent(cell)))
	{
		BotA_TreeNode* temp = treeNodeTab[cell];
		treeNodeTab[cell] = treeNodeTab[parent(cell)];
		treeNodeTab[parent(cell)] = temp;

		// modify index of tab in the close hashtable
		((BotA_HashTable*)closeTable)->changeHeapStackCell(
				treeNodeTab[cell]->getNode(), cell);
		((BotA_HashTable*)closeTable)->changeHeapStackCell(
				treeNodeTab[parent(cell)]->getNode(), parent(cell));

		cell = parent(cell);
	}
}

void BotA_HeapStack::addFirstItem(TreeNode* item)
{
	printf("BotA_HeapStack :: you don't need to use this ");
	printf("function in this context\n");
	exit(-1);
}

void BotA_HeapStack::addLastItem(TreeNode* item)
{
	printf("BotA_HeapStack :: you don't need to use this ");
	printf("function in this context\n");
	exit(-1);
}

void BotA_HeapStack::deleteLastItem(void)
{
	printf("BotA_HeapStack :: you don't need to use this ");
	printf("function in this context\n");
	exit(-1);
}

TreeNode* BotA_HeapStack::getLastItem(void)
{
	printf("BotA_HeapStack :: you don't need to use this ");
	printf("function in this context\n");
	exit(-1);
}

void BotA_HeapStack::minHeapify(int cell)
{
	HashTable* closeTable = treeNodeTab[cell]->getNode()
										->getSolver()->getCloseTable();

	int l = leftChild(cell);
	int r = rightChild(cell);
	int smallest = -1;

	if(l<length && isSmaller(l, cell))
		smallest = l;
	else
		smallest = cell;

	if(r<length && isSmaller(r, smallest))
		smallest = r;

	if(smallest != cell)
	{
		BotA_TreeNode* temp = treeNodeTab[cell];
		treeNodeTab[cell] = treeNodeTab[smallest];
		treeNodeTab[smallest] = temp;

		// modify index of tab in the close hashtable
		((BotA_HashTable*)closeTable)->changeHeapStackCell(
				treeNodeTab[cell]->getNode(), cell);
		((BotA_HashTable*)closeTable)->changeHeapStackCell(
				treeNodeTab[smallest]->getNode(), smallest);

		minHeapify(smallest);
	}
}
