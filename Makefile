#--------------------------#
# VARIABLES INITIALISATION #
#--------------------------#
CC=g++
RM=rm
OPT=-O3
#-pg
ifeq ($(OS),Windows_NT)
	SOS=SWIN32
	CFLAGS=
#	LIBS=-l glu32 -l opengl32 -l mingw32 -l SDLmain -l SDL -l SDL_Mixer -l libxml2
	LIBS=-l libxml2
else
	SOS=SLINUX
#	CFLAGS=`xml2-config --cflags` `sdl-config --cflags`
#	LIBS=`sdl-config --libs` -lGL -lGLU -lX11 -lXmu -lXi -lm -lrt -lxml2 -lstdc++ -lSDL_mixer
	CFLAGS=`xml2-config --cflags`
	LIBS=-lxml2
endif

#-----#
# ALL #
#-----#
ifeq ($(SOS),SWIN32)
all: debug/Main.o debug/Base.o debug/Display.o debug/Path.o debug/Level.o debug/Pack.o debug/Util.o debug/StringList.o debug/Data.o debug/Zone.o debug/Solver.o debug/Node.o debug/ListNode.o debug/TreeNode.o debug/ChainedList.o debug/HashTable.o debug/Stats.o debug/Deadlock.o debug/DijkstraBox.o debug/BotBFS.o debug/BotDFS.o debug/BotA.o debug/BotA_HeapStack.o debug/BotA_ListNode.o debug/BotA_ListNode2.o debug/BotA_TreeNode.o debug/BotA_HashTable.o debug/BotA_ChainedList.o debug/BotA_Child.o debug/BotBestMovesS.o debug/BotBestPushesS.o debug/BotBestPushesS_Matrix.o debug/BotBestPushesS_Munkres.o debug/BotBestPushesS_Penalties.o debug/BotGoodPushesS.o debug/BotIDA.o debug/icone.o
	$(CC) debug/Main.o debug/Base.o debug/Display.o debug/Path.o debug/Level.o debug/Pack.o debug/Util.o debug/StringList.o debug/Data.o debug/Zone.o debug/Solver.o debug/Node.o debug/ListNode.o debug/TreeNode.o debug/ChainedList.o debug/HashTable.o debug/Stats.o debug/Deadlock.o debug/DijkstraBox.o debug/BotBFS.o debug/BotDFS.o debug/BotA.o debug/BotA_HeapStack.o debug/BotA_ListNode.o debug/BotA_ListNode2.o debug/BotA_TreeNode.o debug/BotA_HashTable.o debug/BotA_ChainedList.o debug/BotA_Child.o debug/BotBestMovesS.o debug/BotBestPushesS.o debug/BotBestPushesS_Matrix.o debug/BotBestPushesS_Munkres.o debug/BotBestPushesS_Penalties.o debug/BotGoodPushesS.o debug/BotIDA.o debug/icone.o -o sokoban.exe -Wall $(OPT) $(LIBS) 
else
all: debug/Main.o debug/Base.o debug/Display.o debug/Path.o debug/Level.o debug/Pack.o debug/Util.o debug/StringList.o debug/Data.o debug/Zone.o debug/Solver.o debug/Node.o debug/ListNode.o debug/TreeNode.o debug/ChainedList.o debug/HashTable.o debug/Stats.o debug/Deadlock.o debug/DijkstraBox.o debug/BotBFS.o debug/BotDFS.o debug/BotA.o debug/BotA_HeapStack.o debug/BotA_ListNode.o debug/BotA_ListNode2.o debug/BotA_TreeNode.o debug/BotA_HashTable.o debug/BotA_ChainedList.o debug/BotA_Child.o debug/BotBestMovesS.o debug/BotBestPushesS.o debug/BotBestPushesS_Matrix.o debug/BotBestPushesS_Munkres.o debug/BotBestPushesS_Penalties.o debug/BotGoodPushesS.o debug/BotIDA.o
	$(CC) debug/Main.o debug/Base.o debug/Display.o debug/Path.o debug/Level.o debug/Pack.o debug/Util.o debug/StringList.o debug/Data.o debug/Zone.o debug/Solver.o debug/Node.o debug/ListNode.o debug/TreeNode.o debug/ChainedList.o debug/HashTable.o debug/Stats.o debug/Deadlock.o debug/DijkstraBox.o debug/BotBFS.o debug/BotDFS.o debug/BotA.o debug/BotA_HeapStack.o debug/BotA_ListNode.o debug/BotA_ListNode2.o debug/BotA_TreeNode.o debug/BotA_HashTable.o debug/BotA_ChainedList.o debug/BotA_Child.o debug/BotBestMovesS.o debug/BotBestPushesS.o debug/BotBestPushesS_Matrix.o debug/BotBestPushesS_Munkres.o debug/BotBestPushesS_Penalties.o debug/BotGoodPushesS.o debug/BotIDA.o -o sokoban -Wall $(OPT) $(LIBS)
endif

#---------#
# OBJECTS #
#---------#
# General files #
debug/Main.o: Main.cpp
	$(CC) -c -o debug/Main.o Main.cpp -Wall $(CFLAGS) $(OPT) -D $(SOS)
	
debug/Base.o: src/Base.cpp include/Base.h
	$(CC) -c -o debug/Base.o src/Base.cpp -Wall $(CFLAGS) $(OPT) -D $(SOS)
	
debug/Display.o: src/Display.cpp include/Display.h
	$(CC) -c -o debug/Display.o src/Display.cpp -Wall $(CFLAGS) $(OPT) -D $(SOS)
	
debug/Path.o: src/Path.cpp include/Path.h
	$(CC) -c -o debug/Path.o src/Path.cpp -Wall $(CFLAGS) $(OPT) -D $(SOS)
	
debug/Level.o: src/Level.cpp include/Level.h
	$(CC) -c -o debug/Level.o src/Level.cpp -Wall $(CFLAGS) $(OPT) -D $(SOS)
	
debug/Pack.o: src/Pack.cpp include/Pack.h
	$(CC) -c -o debug/Pack.o src/Pack.cpp -Wall $(CFLAGS) $(OPT) -D $(SOS)
	
debug/Util.o: src/Util.cpp include/Util.h
	$(CC) -c -o debug/Util.o src/Util.cpp -Wall $(CFLAGS) $(OPT) -D $(SOS)
	
debug/StringList.o: src/StringList.cpp include/StringList.h
	$(CC) -c -o debug/StringList.o src/StringList.cpp -Wall $(CFLAGS) $(OPT) -D $(SOS)
	
debug/Data.o: src/Data.cpp include/Data.h
	$(CC) -c -o debug/Data.o src/Data.cpp -Wall $(CFLAGS) $(OPT) -D $(SOS)

# General solver #
debug/Zone.o: src/Solver/Zone.cpp include/Solver/Zone.h
	$(CC) -c -o debug/Zone.o src/Solver/Zone.cpp -Wall $(CFLAGS) $(OPT) -D $(SOS)
	
debug/Solver.o: src/Solver/Solver.cpp include/Solver/Solver.h
	$(CC) -c -o debug/Solver.o src/Solver/Solver.cpp -Wall $(CFLAGS) $(OPT) -D $(SOS)
	
debug/Node.o: src/Solver/Node.cpp include/Solver/Node.h
	$(CC) -c -o debug/Node.o src/Solver/Node.cpp -Wall $(CFLAGS) $(OPT) -D $(SOS)
	
debug/ListNode.o: src/Solver/ListNode.cpp include/Solver/ListNode.h
	$(CC) -c -o debug/ListNode.o src/Solver/ListNode.cpp -Wall $(CFLAGS) $(OPT) -D $(SOS)
	
debug/TreeNode.o: src/Solver/TreeNode.cpp include/Solver/TreeNode.h
	$(CC) -c -o debug/TreeNode.o src/Solver/TreeNode.cpp -Wall $(CFLAGS) $(OPT) -D $(SOS)
	
debug/ChainedList.o: src/Solver/ChainedList.cpp include/Solver/ChainedList.h
	$(CC) -c -o debug/ChainedList.o src/Solver/ChainedList.cpp -Wall $(CFLAGS) $(OPT) -D $(SOS)
	
debug/HashTable.o: src/Solver/HashTable.cpp include/Solver/HashTable.h
	$(CC) -c -o debug/HashTable.o src/Solver/HashTable.cpp -Wall $(CFLAGS) $(OPT) -D $(SOS)
	
debug/Stats.o: src/Solver/Stats.cpp include/Solver/Stats.h
	$(CC) -c -o debug/Stats.o src/Solver/Stats.cpp -Wall $(CFLAGS) $(OPT) -D $(SOS)
	
debug/Deadlock.o: src/Solver/Deadlock.cpp include/Solver/Deadlock.h
	$(CC) -c -o debug/Deadlock.o src/Solver/Deadlock.cpp -Wall $(CFLAGS) $(OPT) -D $(SOS)
	
debug/DijkstraBox.o: src/Solver/DijkstraBox.cpp include/Solver/DijkstraBox.h
	$(CC) -c -o debug/DijkstraBox.o src/Solver/DijkstraBox.cpp -Wall $(CFLAGS) $(OPT) -D $(SOS)

# BFS solver #
debug/BotBFS.o: src/Solver/BotBFS/BotBFS.cpp include/Solver/BotBFS/BotBFS.h
	$(CC) -c -o debug/BotBFS.o src/Solver/BotBFS/BotBFS.cpp -Wall $(CFLAGS) $(OPT) -D $(SOS)
	
# DFS solver #
debug/BotDFS.o: src/Solver/BotDFS/BotDFS.cpp include/Solver/BotDFS/BotDFS.h
	$(CC) -c -o debug/BotDFS.o src/Solver/BotDFS/BotDFS.cpp -Wall $(CFLAGS) $(OPT) -D $(SOS)

# A solver#
debug/BotA.o: src/Solver/BotA/BotA.cpp include/Solver/BotA/BotA.h
	$(CC) -c -o debug/BotA.o src/Solver/BotA/BotA.cpp -Wall $(CFLAGS) $(OPT) -D $(SOS)
	
debug/BotA_HeapStack.o: src/Solver/BotA/BotA_HeapStack.cpp include/Solver/BotA/BotA_HeapStack.h
	$(CC) -c -o debug/BotA_HeapStack.o src/Solver/BotA/BotA_HeapStack.cpp -Wall $(CFLAGS) $(OPT) -D $(SOS)
	
debug/BotA_ListNode.o: src/Solver/BotA/BotA_ListNode.cpp include/Solver/BotA/BotA_ListNode.h
	$(CC) -c -o debug/BotA_ListNode.o src/Solver/BotA/BotA_ListNode.cpp -Wall $(CFLAGS) $(OPT) -D $(SOS)
	
debug/BotA_ListNode2.o: src/Solver/BotA/BotA_ListNode2.cpp include/Solver/BotA/BotA_ListNode2.h
	$(CC) -c -o debug/BotA_ListNode2.o src/Solver/BotA/BotA_ListNode2.cpp -Wall $(CFLAGS) $(OPT) -D $(SOS)
	
debug/BotA_TreeNode.o: src/Solver/BotA/BotA_TreeNode.cpp include/Solver/BotA/BotA_TreeNode.h
	$(CC) -c -o debug/BotA_TreeNode.o src/Solver/BotA/BotA_TreeNode.cpp -Wall $(CFLAGS) $(OPT) -D $(SOS)

debug/BotA_HashTable.o: src/Solver/BotA/BotA_HashTable.cpp include/Solver/BotA/BotA_HashTable.h
	$(CC) -c -o debug/BotA_HashTable.o src/Solver/BotA/BotA_HashTable.cpp -Wall $(CFLAGS) $(OPT) -D $(SOS)

debug/BotA_ChainedList.o: src/Solver/BotA/BotA_ChainedList.cpp include/Solver/BotA/BotA_ChainedList.h
	$(CC) -c -o debug/BotA_ChainedList.o src/Solver/BotA/BotA_ChainedList.cpp -Wall $(CFLAGS) $(OPT) -D $(SOS)
	
debug/BotA_Child.o: src/Solver/BotA/BotA_Child.cpp include/Solver/BotA/BotA_Child.h
	$(CC) -c -o debug/BotA_Child.o src/Solver/BotA/BotA_Child.cpp -Wall $(CFLAGS) $(OPT) -D $(SOS)
	
# BestMovesS solver #
debug/BotBestMovesS.o: src/Solver/BotBestMovesS/BotBestMovesS.cpp include/Solver/BotBestMovesS/BotBestMovesS.h
	$(CC) -c -o debug/BotBestMovesS.o src/Solver/BotBestMovesS/BotBestMovesS.cpp -Wall $(CFLAGS) $(OPT) -D $(SOS)
	
# BestPushesS solver #
debug/BotBestPushesS.o: src/Solver/BotBestPushesS/BotBestPushesS.cpp include/Solver/BotBestPushesS/BotBestPushesS.h
	$(CC) -c -o debug/BotBestPushesS.o src/Solver/BotBestPushesS/BotBestPushesS.cpp -Wall $(CFLAGS) $(OPT) -D $(SOS)
	
debug/BotBestPushesS_Matrix.o: src/Solver/BotBestPushesS/BotBestPushesS_Matrix.cpp include/Solver/BotBestPushesS/BotBestPushesS_Matrix.h
	$(CC) -c -o debug/BotBestPushesS_Matrix.o src/Solver/BotBestPushesS/BotBestPushesS_Matrix.cpp -Wall $(CFLAGS) $(OPT) -D $(SOS)
	
debug/BotBestPushesS_Munkres.o: src/Solver/BotBestPushesS/BotBestPushesS_Munkres.cpp include/Solver/BotBestPushesS/BotBestPushesS_Munkres.h
	$(CC) -c -o debug/BotBestPushesS_Munkres.o src/Solver/BotBestPushesS/BotBestPushesS_Munkres.cpp -Wall $(CFLAGS) $(OPT) -D $(SOS)
	
debug/BotBestPushesS_Penalties.o: src/Solver/BotBestPushesS/BotBestPushesS_Penalties.cpp include/Solver/BotBestPushesS/BotBestPushesS_Penalties.h
	$(CC) -c -o debug/BotBestPushesS_Penalties.o src/Solver/BotBestPushesS/BotBestPushesS_Penalties.cpp -Wall $(CFLAGS) $(OPT) -D $(SOS)
	
# GoodPushesS solver #
debug/BotGoodPushesS.o: src/Solver/BotGoodPushesS/BotGoodPushesS.cpp include/Solver/BotGoodPushesS/BotGoodPushesS.h
	$(CC) -c -o debug/BotGoodPushesS.o src/Solver/BotGoodPushesS/BotGoodPushesS.cpp -Wall $(CFLAGS) $(OPT) -D $(SOS)
	
# IDA solver #
debug/BotIDA.o: src/Solver/BotIDA/BotIDA.cpp include/Solver/BotIDA/BotIDA.h
	$(CC) -c -o debug/BotIDA.o src/Solver/BotIDA/BotIDA.cpp -Wall $(CFLAGS) $(OPT) -D $(SOS)
	
debug/icone.o: sokoban.ico icone.rc
	windres icone.rc debug/icone.o
	
#-------#
# DEBUG #
#-------#
debug: all
	valgrind --leak-check=full --show-reachable=yes ./sokoban "michael"
	
massif: all
	valgrind --tool=massif ./sokoban "michael"

#-------#
# CLEAN #
#-------#
.PHONY: clean

clean:
	$(RM) -rf debug
	mkdir debug
	$(RM) sokoban.exe
	$(RM) ./sokoban

#-----#
# RUN #
#-----#
run:
#	./sokoban "michael" -w "640x480"
#	./sokoban "michael" -f
	./sokoban "michael"