/*----------------------------------------------*/
/*================== ISokoban ==================*/
/*----------------------------------------------*/
/* Version : 1.0								*/
/* Author : Michaël Hoste						*/
/* Copyright : See LICENSE file for details   	*/
/*----------------------------------------------*/
/* TODO Utiliser gprof pour les statistiques sur les fonctions à optimiser
 *
 * TODO à chaque noeud, récupérer une fois pour toutes les positions des
 * caisses afin de limiter le nombre de boucles et d'opérations inutiles
 */

#include <stdio.h>
#include <unistd.h>
#include "include/Base.h"
#include "include/Pack.h"
#include "include/Path.h"
#include "include/StringList.h"
#include "include/Data.h"
#include "include/Solver/Solver.h"
#include "include/Solver/Stats.h"
#include "include/Solver/BotBFS/BotBFS.h"
#include "include/Solver/BotDFS/BotDFS.h"
#include "include/Solver/BotBestMovesS/BotBestMovesS.h"
#include "include/Solver/BotBestPushesS/BotBestPushesS.h"
#include "include/Solver/BotGoodPushesS/BotGoodPushesS.h"
#include "include/Solver/BotIDA/BotIDA.h"

#ifdef __cplusplus
extern "C"
{
#endif

int main(int argc, char **argv)
{
	/* BASE (init) */
	Base* base = new Base(argc, argv);

	/* PACK (type here the pack you want to test)*/
	Pack* pack = new Pack(base, "Original.slc", "michael");

	/* RESOLUTION */
	for(int i=0;i<90;i++)
	{
		/* Solveur IDA* en "recherche rapide" limité à 5 millions de noeuds et 700Mo
		 * Un pré-traitement de 3 caisses est lancé avant la création de l'arbre
		 * de recherche */
		BotIDA* solver = new BotIDA(base, pack->getLevelList()[i], 5000000,
				700, 870967, 51827, 3, false, true);

		/* Solveur IDA* en "recherche lente" limité à 5 millions de noeuds et 700Mo
		 * Un pré-traitement de 3 caisses est lancé avant la création de l'arbre
		 * de recherche */
//		BotIDA* solver = new BotIDA(base, pack->getLevelList()[i], 5000000,
//				700, 870967, 51827, 3, false, false);

		solver->resolve();
		delete solver;

	}

	delete pack;

/* Code permettant de tester tous les packs présents un par un*/

//	StringList* packList = base->getData()->getPackNameList();
//	for(int i=0;i<packList->getLength();i++)
//	{
//		Pack* pack2 = new Pack(base, packList->getString(i), "michael");
//		for(int j=0;j<pack2->getN();j++)
//		{
//			BotIDA* solver = new BotIDA(base, pack2->getLevelList()[j], 5000000,
//					700, 870967, 51827, 3, false, true);
//			solver->resolve();
//			delete solver;
//		}
//		delete pack;
//	}


	delete base;

	return 0;
}


#ifdef __cplusplus
}
#endif
