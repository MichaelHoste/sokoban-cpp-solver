/*----------------------------------------------*/
/*================== ISokoban ==================*/
/*----------------------------------------------*/
/* Version : 1.0								*/
/* Author : Michaël Hoste						*/
/* Copyright : See LICENSE file for details   	*/
/*----------------------------------------------*/

#include "../include/Pack.h"

/* ------------*/
/* Constructor */
/* ------------*/
Pack::Pack(Base* base) :
	base(base),
	maxRowsNumber(0),
	maxColsNumber(0),
	levelList(NULL),
	n(0),
	nSuccess(0)
{
	fileName[0]='\0';
	player[0]='\0';
	title[0]='\0';
	description[0]='\0';
	copyright[0]='\0';
	email[0]='\0';
	url[0]='\0';
}

Pack::Pack(Base* base, const char* xmlPackFile, const char* player) :
	base(base),
	maxRowsNumber(0),
	maxColsNumber(0),
	levelList(NULL),
	n(0),
	nSuccess(0)
{
	fileName[0]='\0';
	this->player[0]='\0';
	title[0]='\0';
	description[0]='\0';
	copyright[0]='\0';
	email[0]='\0';
	url[0]='\0';

	xmlLoad(xmlPackFile, player);
}

/* -----------*/
/* Destructor */
/* -----------*/
Pack::~Pack()
{
	// Destruction of levels included in this pack
	if(levelList)
	{
		for(int i=0;i<n;i++)
		{
			if(levelList[i])
				delete levelList[i];
		}
	}
	free(levelList);
}

/* -------*/
/* Others */
/* -------*/
void Pack::print(const char* fileName) const
{
	if(fileName)
		printInFile(fileName);
	else printf("Pack :: invalid fileName");
}

void Pack::print(void) const
{
	printInFile(NULL);
}

void Pack::xmlSavePath(void) const
{
	FILE * fichier;
	char temp[257];
	char ext[257];
	int i;

	// Création des dossiers au cas où ils n'existeraient pas déjà.
#ifdef SLINUX
	sprintf(temp, "mkdir data/profiles/%s", player);
#endif
#ifdef SWIN32
	sprintf(temp, "mkdir data\\profiles\\%s", player);
#endif
	system(temp);

	// Transformation de l'extension
	strcpy(ext, this->fileName);
	ext[strlen(ext)-3]='s';
	ext[strlen(ext)-2]='o';
	ext[strlen(ext)-1]='l';

#ifdef SLINUX
	sprintf(temp, "data/profiles/%s/%s", player, ext);
#endif
#ifdef SWIN32
	sprintf(temp, "data\\profiles\\%s\\%s", player, ext);
#endif

	if((fichier=fopen(temp, "w")) == NULL)
	{
		printf("Pack :: can't create file %s.\nMaybe repertory doesn't exist.",
				temp);
		exit(-1);
	}

	fprintf(fichier, "<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\n");
	fprintf(fichier, "<!DOCTYPE SokobanPaths SYSTEM \"../SokobanPaths.dtd\">\n");
	fprintf(fichier, "<SokobanPaths NSuccess=\"%d\">\n", this->nSuccess);

	// Write each level paths in this file
	for(i=0;i<this->n;i++)
	{
		this->levelList[i]->xmlSavePath(fichier);
	}

	fprintf(fichier, "</SokobanPaths>\n");

	fclose(fichier);
}

void Pack::xmlLoad(const char* xmlPackFile, const char* player)
{
	xmlDocPtr packTree = NULL; /* Pack File Data Tree */
	xmlDocPtr pathTree = NULL; /* Path File Data Tree */
	xmlNodePtr* levelNode; /* Nodes of levels in xml file*/
	xmlNodePtr* pathNode; /* Nodes of path in xml file*/
	char* temp;

	temp = Util::cutText(xmlPackFile, 32);
	strcpy(fileName, temp);
	free(temp);

	temp = Util::cutText(player, 32);
	strcpy(this->player, temp);
	free(temp);

	// Library Init and Version test
	LIBXML_TEST_VERSION

	// Open level file and path file and get tab of nodes
	levelNode=xmlLoadLevels(&packTree, xmlPackFile);
	pathNode=xmlLoadPath(&pathTree, player, xmlPackFile);

	levelList=(Level**)malloc(this->n*sizeof(Level*));
	for(int i=0;levelNode[i]!=NULL;i++)
	{
		levelList[i] = new Level(base, levelNode[i], pathNode[i], i+1, fileName);
	}

	// delete nodelists
	free(levelNode);
	free(pathNode);
	// Delete data trees
	xmlFreeDoc(packTree);
	xmlFreeDoc(pathTree);
	/* Cleanup function for the XML library. */
	xmlCleanupParser();
	/* this is to debug memory for regression tests */
	xmlMemoryDump();
}

xmlNodePtr* Pack::xmlLoadLevels(xmlDocPtr* tree, const char* xmlPackFile)
{
	xmlNodePtr* nodeList;
	xmlNodePtr cur = NULL;
	char *temp, *temp2;
	char * filename = (char*)malloc(256*sizeof(char));
	sprintf(filename, "%s/%s", "data/levels", xmlPackFile);

	*tree = xmlParseFile(filename);
	if (*tree == NULL)
	{
		printf("Pack :: Failed to parse %s\n", filename);
		exit(1);
	}

	/*Get the root element node */
	cur = xmlDocGetRootElement(*tree);
	if(strcmp((char*)cur->name, "SokobanLevels") != 0)
	{
		printf("Pack :: Failed to parse %s\n", filename);
		exit(1);
	}

	// Save content of <Title>
	cur = cur->children->next;
	if(strcmp((char*)cur->name, "Title") == 0)
	{
		temp = (char*)xmlNodeGetContent(cur);
		temp2 = Util::cutText(temp, 64);
		strcpy(title, temp2);
		free(temp);
		free(temp2);
	}
	else
	{
		printf("Pack :: Failed to parse %s. <Title> doesn't exist\n",
				filename);
		exit(1);
	}

	// Save content of <Description>
	cur = cur->next->next;
	if(strcmp((char*)cur->name, "Description") == 0)
	{
		temp = (char*)xmlNodeGetContent(cur);
		temp2 = Util::cutText(temp, 512);
		strcpy(description, temp2);
		free(temp);
		free(temp2);
	}
	else
	{
		printf("Pack :: Failed to parse %s. <Description> doesn't exist\n",
				filename);
		exit(1);
	}

	// Save content of <Email> (if present)
	if(strcmp((char*)cur->next->next->name, "Email") == 0)
	{
		cur = cur->next->next;
		temp = (char*)xmlNodeGetContent(cur);
		temp2 = Util::cutText(temp, 64);
		strcpy(email, temp2);
		free(temp);
		free(temp2);
	}
	else email[0]='\0';

	// Save content of <Url> (if present)
	if(strcmp((char*)cur->next->next->name, "Url") == 0)
	{
		cur = cur->next->next;
		temp = (char*)xmlNodeGetContent(cur);
		temp2 = Util::cutText(temp, 128);
		strcpy(url, temp2);
		free(temp);
		free(temp2);
	}
	else url[0]='\0';

	// Go through <LevelCollection>
	cur = cur->next->next;
	if(strcmp((char*)cur->name, "LevelCollection") == 0)
	{
		temp = (char*)xmlGetProp(cur, (unsigned char*)"Copyright");
		temp2 = Util::cutText(temp, 64);
		strcpy(copyright, temp2);
		free(temp);
		free(temp2);

		temp = (char*)xmlGetProp(cur, (unsigned char*)"MaxWidth");
		temp2 = (char*)xmlGetProp(cur, (unsigned char*)"MaxHeight");
		maxRowsNumber = atoi(temp);
		maxColsNumber = atoi(temp2);
		free(temp);
		free(temp2);

		nodeList=(xmlNodePtr*)malloc(sizeof(xmlNodePtr));

		// Go through each level
		cur = cur->children->next;

		while(cur)
		{
			nodeList=(xmlNodePtr*) realloc(nodeList, (n+1)*sizeof(xmlNodePtr));
			nodeList[this->n] = cur;
			this->n++;
			cur = cur->next->next;
		}
	}
	else
	{
		printf("Pack :: Failed to parse %s. <LevelCollection> doesn't exist\n",
				filename);
		exit(1);
	}

	nodeList=(xmlNodePtr*)realloc(nodeList, (n+1)*sizeof(xmlNodePtr));
	nodeList[n] = NULL;

	free(filename);

	return nodeList;
}

xmlNodePtr* Pack::xmlLoadPath(xmlDocPtr* tree, const char* player,
							  const char* xmlPackFile)
{
	xmlNodePtr* nodeList;
	xmlNodePtr cur = NULL; /* Current Node */
	char * filename = (char*)malloc(256*sizeof(char));
	int i;
	int n=0;

	sprintf(filename, "data/profiles/%s/%s", player, xmlPackFile);
	int tmplength = strlen(filename);
	filename[tmplength-3]='s';
	filename[tmplength-2]='o';
	filename[tmplength-1]='l';

	FILE* fichier = fopen(filename, "r");
	if(fichier != NULL)
	{
		fclose(fichier);

		*tree = xmlParseFile(filename);

		if (*tree == NULL)
		{
			printf("Pack :: Failed to parse %s\n", filename);
			exit(1);
		}

		/*Get the root element node */
		cur = xmlDocGetRootElement(*tree);
		if(strcmp((char*)cur->name, "SokobanPaths") != 0)
		{
			printf("Pack :: Failed to parse %s\n", filename);
			exit(1);
		}
		else
		{
			char* temp;
			temp = (char*)xmlGetProp(cur, (unsigned char*)"NSuccess");
			nSuccess = atoi(temp);
			free(temp);
		}

		nodeList=(xmlNodePtr*)malloc(sizeof(xmlNodePtr));

		// Go through each path
		cur = cur->children->next;

		while(cur)
		{
			nodeList=(xmlNodePtr*)realloc(nodeList, (n+1)*sizeof(xmlNodePtr));
			nodeList[n] = cur;
			n++;
			cur = cur->next->next;
		}
	}
	else
	{
		nSuccess = 0;

		// Scores (paths) file initialisation
		nodeList=(xmlNodePtr*)malloc((this->n+1)*sizeof(xmlNodePtr));
		for(i=0;i<this->n;i++)
		{
			nodeList[i]=NULL;
		}
	}

	free(filename);

	return nodeList;
}

void Pack::printInFile(const char* fileName) const
{
	int i;
	FILE * file;

	/* If fileName is NULL, then print on standard output */
	if(fileName!=NULL)
		file=fopen(fileName, "w");
	else file=stdout;

	/* Print general infos */
	fprintf(file, "Title : %s\n", title);
	fprintf(file, "Description : %s\n", description);
	fprintf(file, "Copyright : %s\n", copyright);
	fprintf(file, "Number of levels : %d\n", n);
	fprintf(file, "Number of success levels : %d\n", nSuccess);
	fprintf(file, "Max Width (n) : %d\n", maxColsNumber);
	fprintf(file, "Max Height (m) : %d\n", maxRowsNumber);
	fprintf(file, "\n--------------------------\n\n");

	if(fileName!=NULL)
		fclose(file);

	/* For each level on the list... */
	for(i=0;i<n;i++)
	{
		/*... Print this level */
		if(fileName)
			levelList[i]->print(fileName);
		else
			levelList[i]->print();
	}
}
