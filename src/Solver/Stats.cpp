/*----------------------------------------------*/
/*================== ISokoban ==================*/
/*----------------------------------------------*/
/* Version : 1.0								*/
/* Author : Michaël Hoste						*/
/* Copyright : See LICENSE file for details   	*/
/*----------------------------------------------*/

#include "../../include/Solver/Stats.h"
#include "../../include/Solver/BotBestPushesS/BotBestPushesS.h"

/* ------------*/
/* Constructor */
/* ------------*/
Stats::Stats(Base* base, Solver* solver, TreeNode* treeNode, Path* solutionPath,
		int numOfPushes, char* message):
	base(base),
	solver(solver),
	treeNode(treeNode),
	solved(solver->getSolved()),
	solutionPath(solutionPath),
	numOfPushes(numOfPushes),
	message(NULL)
{
	solved = solver->getSolved();
	this->message = (char*)malloc((strlen(message)+1)*sizeof(char));
	strcpy(this->message, message);
}

Stats::Stats(const Stats* otherStats):
	base(otherStats->getBase()),
	solver(otherStats->getSolver()),
	treeNode(otherStats->getTreeNode()),
	solved(otherStats->getSolved()),
	numOfPushes(otherStats->getNumOfPushes())
{
	char* otherMessage = otherStats->getMessage();
	this->message = (char*)malloc((strlen(otherMessage)+1)*sizeof(char));
	strcpy(this->message, otherMessage);

	if(otherStats->getSolutionPath() != NULL)
		solutionPath = new Path(base, otherStats->getSolutionPath());
	else
		solutionPath = NULL;
}

/* -----------*/
/* Destructor */
/* -----------*/
Stats::~Stats()
{
	if(solutionPath)
		delete solutionPath;
	if(message)
		free(message);
}

/* -------*/
/* Others */
/* -------*/
void Stats::print(void) const
{
	printInFile(NULL);
}

void Stats::print(const char* fileName) const
{
	printInFile(fileName);
}

int* Stats::hashingRepartitionStats() const
{
	const HashTable* ht = solver->getOpenTable();

	// Number of cells we need
	int max = 0;
	for(int i=0;i<solver->OPENTABLE_SIZE;i++)
	{
		int thismax = ht->getTable()[i]->getLength();
		if(thismax > max)
			max = thismax;
	}

	int* tab = (int*)malloc((max+2)*sizeof(int));
	tab[max+1] = -1;
	int tabLength = max+1;

	// Initialization
	for(int i=0;i<tabLength;i++)
		tab[i]=0;

	// Calculation
	for(int i=0;i<solver->OPENTABLE_SIZE;i++)
		tab[ht->getTable()[i]->getLength()]++;

	return tab;
}

void Stats::printInFile(const char* fileName) const
{
	FILE * file;

	/* If fileName is NULL, standard output is used to print */
	if(fileName!=NULL)
		file=fopen(fileName, "a");
	else file=stdout;

	/* print infos */
	fprintf(file, "\n--------------------------------------------------\n\n");

	int rowsNumber = this->solver->getLevel()->getRowsNumber();
	int colsNumber = this->solver->getLevel()->getColsNumber();
	const char* grid = this->solver->getLevel()->getGrid();

	/* Double loop used to print every symbols of this level */
	for(int j=0;j<rowsNumber;j++)
	{
		for(int k=colsNumber*j;k<colsNumber*(j+1);k++)
		{
			/* We change 's' in ' ' for better lisibility */
			if(grid[k] == 's')
				fprintf(file, " ");
			else
				fprintf(file, "%c", grid[k]);
		}
		fprintf(file, "\n");
	}
	fprintf(file, "\n--------------------------\n\n");

	fprintf(file, "Solver : %s\n", this->solver->SOLVER_NAME());
	fprintf(file, "Message : %s\n", this->message);
	fprintf(file, "Pack name : %s\n", this->solver->getLevel()->getPackName());
	fprintf(file, "Level ID : %d\n", this->solver->getLevel()->getId());

	if(   strcmp(solver->SOLVER_NAME(), "BotIDA") == 0
	   || strcmp(solver->SOLVER_NAME(), "BotBestPushesS") == 0)
		fprintf(file, "Actual Cost : %d\n",
				((BotA*)this->solver)->getCostLimit());

	fprintf(file, "Node Number : %d\n", this->solver->getNodeNumber());
	fprintf(file, "OpenTable : %d\n",
			this->solver->getOpenTable()->getItemNumber());
	fprintf(file, "CloseTable : %d\n",
			this->solver->getCloseTable()->getItemNumber());

	int* tmp = hashingRepartitionStats();
	free(tmp);

	if(solutionPath != NULL)
	{
		fprintf(file, "Path moves : %d\n", this->solutionPath->getNMoves());
		fprintf(file, "Path pushes : %d\n", this->solutionPath->getNPushes());
		fprintf(file, "Path : %s\n", this->solutionPath->getMoves());
	}

	fprintf(file, "\n--------------------------------------------------\n");

	if(strcmp(solver->SOLVER_NAME(), "BotBestPushesS") == 0)
	{
		if(((BotBestPushesS*)this->solver)->getQuickSearch() == true)
			fprintf(file, "Found with quick search mode\n");
		else
			fprintf(file, "Found without quick search mode\n");
	}

	if(fileName != NULL)
		fclose(file);
}
