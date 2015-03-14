/*----------------------------------------------*/
/*================== ISokoban ==================*/
/*----------------------------------------------*/
/* Version : 1.0								*/
/* Author : Michaël Hoste						*/
/* Copyright : See LICENSE file for details   	*/
/*----------------------------------------------*/

#include "../../../include/Solver/BotA/BotA_TreeNode.h"

#include "../../../include/Solver/BotA/BotA_HashTable.h"
#include "../../../include/Solver/BotA/BotA_HeapStack.h"

/* ------------*/
/* Constructor */
/* ------------*/
BotA_TreeNode::BotA_TreeNode(Node* node, const int f, const int g)
	: TreeNode(node)
{
	this->f = f;
	this->g = g;
}

BotA_TreeNode::BotA_TreeNode(Node* node)
	: TreeNode(node)
{
	this->f = -1;
	this->g = -1;
}

/* -----------*/
/* Destructor */
/* -----------*/

/* -------*/
/* Others */
/* -------*/
void BotA_TreeNode::decreaseValue(int value)
{
	// We decrease this value
	this->g = this->g - value;
	this->f = this->f - value;

	// We decrease value of children
	for(int i=0;i<childrenNumber;i++)
	{
		((BotA_TreeNode*)children[i])->decreaseValue(value);

		// If treenode to decrease is in closeTable, we move it in the heapStack
		HashTable* closeTable = node->getSolver()->getCloseTable();
		if(closeTable->isPresent(children[i]->getNode()))
		{
			ChainedList* heapStack = node->getSolver()->getCloseNodeList();
			int heapStackCell = ((BotA_HashTable*)closeTable)->getHeapStackCell(children[i]->getNode());
			((BotA_HeapStack*)heapStack)->repositionCell(heapStackCell);

//			printf("BotA_TreeNode :: ERREUR : un noeud de l'arbre se trouvant ");
//			printf("dans la liste des noeuds fermés (heap stack) a sa valeur ");
//			printf("changée alors qu'il n'est pas reclassé ");
//			exit(1);
		}
	}
}

