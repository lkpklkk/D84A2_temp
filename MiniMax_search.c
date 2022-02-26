/*
	CSC D84 - Unit 2 - MiniMax search and adversarial games

	This file contains stubs for implementing a MiniMax search
        procedure with alpha-beta pruning. Please read the assignment
	handout carefully - it describes the game, the data you will
	have to handle, and the search functions you must provide.

	Once you have read the handout carefully, implement your search
	code in the sections below marked with

	**************
	*** TO DO:
	**************

	Make sure to add it to your report.txt file - it will be marked!

	Have fun!

	DO NOT FORGET TO 'valgrind' YOUR CODE - We will check for pointer
	management being done properly, and for memory leaks.

	Starter code: F.J.E. Sep. 15
*/

#include "MiniMax_search.h"

int getIndNei(int curIndex, int neiInd)
{
	if (neiInd == 0)
	{
		return curIndex - size_X;
	}
	if (neiInd == 1)
	{
		return curIndex + 1;
	}
	if (neiInd == 2)
	{
		return curIndex + size_X;
	}
	if (neiInd == 3)
	{
		return curIndex - 1;
	}
	return -1;
}

bool isChese(int *allChese, int cheses, int curInd)
{
	for (size_t i = 0; i < cheses; i++)
	{
		if (allChese[i] == curInd)
		{
			return 1;
		}
	}
	return 0;
}

bool isCat(int *allCats, int cats, int curInd)
{
	for (size_t i = 0; i < cats; i++)
	{
		if (allCats[i] == curInd)
		{
			return 1;
		}
	}
	return 0;
}

typedef struct Node
{
	int ind;
	int priority;
	struct Node *next;
} Node;

typedef struct LinkedList
{
	struct Node *head;
	struct Node *tail;
	int size;
} Llist;
Node *newPriNode(int ind, int priority)
{
	Node *newNode = (Node *)malloc(sizeof(Node));
	newNode->ind = ind;
	newNode->next = NULL;
	newNode->priority = priority;
	return newNode;
}
Node *newNode(int ind)
{
	Node *newNode = (Node *)malloc(sizeof(Node));
	newNode->ind = ind;
	newNode->next = NULL;
	newNode->priority = __INT_MAX__;
	return newNode;
}
Llist *newQueue()
{
	Llist *newList = (Llist *)malloc(sizeof(Llist));
	newList->head = NULL;
	newList->tail = NULL;
	newList->size = 0;
	return newList;
}
int popFirstInd(Llist *linkedList)
{
	if (linkedList->size == 0)
	{
		return -1;
	}
	int index = linkedList->head->ind;
	Node *nextHead = linkedList->head->next;
	free(linkedList->head);
	linkedList->head = nextHead;
	linkedList->size--;
	if (linkedList->size == 0)
	{
		linkedList->tail = NULL;
	}

	return index;
}
void queueHead(Llist *lList, int ind)
{
	Node *curNode = newNode(ind);
	Node *temp = lList->head;
	if (temp == NULL)
	{
		lList->head = curNode;
		lList->tail = curNode;
		lList->size++;
		return;
	}

	curNode->next = temp;
	lList->head = curNode;
	lList->size++;
	return;
}
void queueTail(Llist *LinkedList, int ind)
{
	Node *curNode = newNode(ind);
	if (LinkedList->tail == NULL)
	{

		LinkedList->head = curNode;
		LinkedList->tail = curNode;
		LinkedList->size++;
	}
	else
	{
		Node *temp = LinkedList->tail;
		temp->next = curNode;
		LinkedList->tail = curNode;
		LinkedList->size++;
	}
	return;
}
void queueFree(Llist *lList)
{
	Node *node = lList->head;

	while (node != NULL)
	{
		Node *next = node->next;
		free(node);
		node = next;
	}
}
void pushMinQueue(Llist *lList, int ind, int priority)
{
	Node *start = lList->head;
	Node *temp = newPriNode(ind, priority);

	if (start == NULL)
	{
		lList->head = temp;
		lList->tail = temp;
		lList->size++;
		return;
	}
	if (lList->size == 1)
	{
		if (start->priority > priority)
		{
			temp->next = start;
			lList->head = temp;
			lList->tail = start;
		}
		else
		{
			start->next = temp;
			lList->tail = temp;
		}
	}
	else
	{
		while (start->next != NULL && start->next->priority < priority)
		{
			start = start->next;
		}

		temp->next = start->next;
		start->next = temp;
	}
	lList->size++;
}
bool minQueueCheck(LinkedList *lList, int ind)
{
	Node *start = lList->head;
	while (start != NULL)
	{
		if (start->ind == ind)
		{
			return true;
		}
		start = start->next;
	}
	return false;
}
bool minQueueUpdate(LinkedList *lList, int ind, int priority)
{
	Node *start = lList->head;
	if (start == NULL)
	{
		return false;
	}
	else if (start->ind == ind)
	{
		if (start->priority > priority)
		{
			start->priority = priority;
			return true;
		}
		else
		{
			return false;
		}
	}

	while (start->next != NULL)
	{
		if (start->next->ind == ind)
		{
			if (start->next->priority > priority)
			{
				Node *temp = start->next;
				start->next = temp->next;
				if (temp->next == NULL)
				{
					lList->tail = start;
				}
				free(temp);
				pushMinQueue(lList, ind, priority);
				return true;
			}
			else
			{
				return false;
			}
		}
		start = start->next;
	}
	return false;
}
int toInd(int x, int y)
{
	int index = x + (y * size_X);
	return index;
}

typedef struct cor
{
	int x;
	int y;
} Cor;

void indToCor(Cor *cor, int index)
{
	cor->x = index % size_X;
	cor->y = index / size_Y;
}

void directionToCor(Cor* cor, int direction, int curIndex){
	Cor* tempCor = (Cor *)malloc(sizeof(Cor));
	indToCor(tempCor,curIndex);
	switch (direction)
	{
	case 0:
		cor->x = tempCor->x;
		cor->y = tempCor->y -1;
		break;
	case 1:
		cor ->x = tempCor ->x +1;
		cor -> y = tempCor -> y;
		break;
	case 2:
		cor->x = tempCor ->x;
		cor->y = tempCor->y+1;
		break;
	case 3:
		cor -> x = tempCor -> x -1;
		cor->y = tempCor -> y;
		break;
	default:
		break;
	}
	free(tempCor);
}

void fillCats(int cats_loc_tofill[10][2], int cat_loc[10][2], int cats){
	for (size_t i = 0; i < cats; i++)
	{
		cats_loc_tofill[i][0] = cat_loc[i][0];
		cats_loc_tofill[i][1] = cat_loc[i][1];
	}
}

double MiniMax(double gr[graph_size][4], int path[1][2], double minmax_cost[size_X][size_Y], int cat_loc[10][2], int cats, int cheese_loc[10][2], int cheeses, int mouse_loc[1][2], int mode, double (*utility)(int cat_loc[10][2], int cheese_loc[10][2], int mouse_loc[1][2], int cats, int cheeses, int depth, double gr[graph_size][4]), int agentId, int depth, int maxDepth, double alpha, double beta)
{
 //base case, time to return
 if (checkForTerminal(mouse_loc,cat_loc,cheese_loc,cats,cheeses) || depth == maxDepth){
	return utility(cat_loc,cheese_loc,mouse_loc,cats,cheeses,depth,gr);
 }
double tempAlpha = alpha;
double tempBeta = beta;

 //mouse turn
 if (agentId == 0)
 {	
	 double bestScore = -__DBL_MAX__;
	 double tempScore = -__DBL_MAX__;
	 Cor* bestMove =  (Cor *)malloc(sizeof(Cor));
	 Cor* mouseCor = (Cor *)malloc(sizeof(Cor));
	 mouseCor->x = mouse_loc[0][0];
	 mouseCor->y = mouse_loc[0][1];
	 int mouseInd = toInd(mouseCor->x,mouseCor->y);
	 Cor* attemptCor = (Cor *)malloc(sizeof(Cor));
	 int attemptMouse_loc[1][2];
	 for (size_t i = 0; i < 4; i++)
	 {
		if (gr[mouseInd][i]==1)	//valid move
		{	
			directionToCor(attemptCor, i,mouseInd);
			attemptMouse_loc[0][0] = attemptCor->x;
			attemptMouse_loc[0][1] = attemptCor->y;
			tempScore = MiniMax(gr,path,minmax_cost,cat_loc,cats,cheese_loc,cheeses,attemptMouse_loc,mode,utility,1,depth+1,maxDepth,tempAlpha,tempBeta);
			minmax_cost[attemptCor->x][attemptCor->y] = tempScore;
			if (tempScore>bestScore)
			{
				bestScore = tempScore;
				bestMove->x =  attemptCor->x;
				bestMove->y =  attemptCor->y;
			}
			if (mode==1)
			{
				if (tempScore>tempAlpha)
				{
					tempAlpha = tempScore;
				}
				if (tempAlpha>=tempBeta)
				{
					break;
				}
			}
		}
	 }
	 minmax_cost[mouseCor->x][mouseCor->y] = bestScore;
	 path[0][0] = bestMove->x;
	 path[0][1] = bestMove->y;
	 free(mouseCor);
	 free(attemptCor);
	 free(bestMove);
	 return bestScore;
 }else{
	double bestScore = __DBL_MAX__;
	 double tempScore = __DBL_MAX__;
	 Cor* bestMove = (Cor *)malloc(sizeof(Cor));
	 Cor* catCor = (Cor *)malloc(sizeof(Cor));
	 int catId = agentId-1;
	 catCor->x = cat_loc[catId][0];
	 catCor->y =  cat_loc[catId][1];
	 int catInd = toInd(catCor->x,catCor->y );
	 Cor* attemptCor = (Cor *)malloc(sizeof(Cor));
	 int attemptCat_loc[10][2];
	 fillCats(attemptCat_loc,cat_loc,cats);
	 for (size_t i = 0; i < 4; i++)
	 {
		if (gr[catInd][i]==1)	//valid move
		{	
			directionToCor(attemptCor, i,catInd);
			attemptCat_loc[catId][0] = attemptCor->x;
			attemptCat_loc[catId][1] = attemptCor->y;
			// last cat to move
			if (catId==cats-1)
			{
				tempScore = MiniMax(gr,path,minmax_cost,attemptCat_loc,cats,cheese_loc,cheeses,mouse_loc,mode,utility,0,depth+1,maxDepth,tempAlpha,tempBeta);
			}else{
				tempScore = MiniMax(gr,path,minmax_cost,attemptCat_loc,cats,cheese_loc,cheeses,mouse_loc,mode,utility,agentId+1,depth+1,maxDepth,tempAlpha,tempBeta);
			}

			if (tempScore < bestScore)
			{
				bestScore = tempScore;
				bestMove->x =  attemptCor->x;
				bestMove->y =  attemptCor->y;
			}
			if (mode==1)
			{
				if (tempScore<tempBeta)
				{
					tempBeta = tempScore;
				}
				if (tempBeta<=tempAlpha)
				{
					break;
				}
				
				
			}
			
		}

	 }
	 free(catCor);
	 free(attemptCor);
	 free(bestMove);
	 return bestScore;
 }

 

 
 /*
   This function is the interface between your solution for the assignment and the driver code. The driver code
   in MiniMax_search_core_GL will call this function once per frame, and provide the following data
   
   Board and game layout:

	Exactly the same as for Assignment 1 - have a look at your code if you need a reminder of how the adjacency
	list and agent positions are stored.	

	Note that in this case, the path will contain a single move - at the top level, this function will provide
	an agent with the 'optimal' mini-max move given the game state.


   IMPORTANT NOTE: Mini-max is a recursive procedure. This function will need to fill-in the mini-max values for 
	 	   all game states down to the maximum search depth specified by the user. In order to do that,
		   the function needs to be called with the correct state at each specific node in the mini-max
		   search tree.

		   The game state is composed of:

			* Mouse, cat, and cheese positions (and number of cats and cheeses)
			
		   At the top level (when this function is called by the mini-max driver code), the game state
		   correspond to the current situation of the game. But once you start recursively calling
		   this function for lower levels of the search tree the positions of agents will have changed.
		   
		   Therefore, you will need to define local variables to keep the game state at each node of the
		   mini-max search tree, and you will need to update this state when calling recursively so that
		   the search does the right thing.

		   This function *must check* whether:
			* A candidate move results in a terminal configuration (cat eats mouse, mouse eats cheese)
			  at which point it calls the utility function to get a value
	 		* Maximum search depth has been reached (depth==maxDepth), at which point it will also call
			  the utility function to get a value
			* Otherwise, call recursively using the candidate configuration to find out what happens
			  deeper into the mini-max tree.

   Arguments:
		gr[graph_size][4]   		- This is an adjacency list for the maze
		path[1][2] 			- Your function will return the optimal mini-max move in this array.
		minmax_cost[size_X][size_Y]	- An array in which your code will store the
						  minimax value for maze locations expanded by
						  the search *when called for the mouse, not
						  for the cats!*

						  This array will be used to provide a visual 
						  display of minimax values during the game.

		cat_loc[10][2], cats   - Location of cats and number of cats (we can have at most 10,
					 but there can be fewer). Only valid cat locations are 0 to (cats-1)
		cheese_loc[10][2], cheeses - Location and number of cheese chunks (again at most 10,
					     but possibly fewer). Valid locations are 0 to (cheeses-1)
		mouse_loc[1][2] - Mouse location - there can be only one!
		mode - Search mode selection:
					mode = 0 	- No alpha-beta pruning
					mode = 1	- Alpha-beta pruning

		(*utility)(int cat_loc[10][2], int cheese_loc[10][2], int mouse_loc[1][2], int cats, int cheeses, int depth, double gr[graph_size][4]);
				- This is a pointer to the utility function which returns a value for a specific game configuration

				   NOTE: Unlike the search assignment, this utility function also gets access to the graph so you can do any processing 					 that requires knowledge of the maze for computing the utility values.

				  * How to call the utility function from within this function : *
					- Like any other function:
						u = utility(cat_loc, cheese_loc, mouse_loc, cats, cheeses, depth, gr);
						
		agentId: Identifies which agent we are doing MiniMax for. agentId=0 for the mouse, agentId in [1, cats] for cats. Notice that recursive calls
                         to this function should increase the agentId to reflect the fact that the next level down corresponds to the next agent! For a game
                         with two cats and a mouse, the agentIds for the recursion should look like 0, 1, 2, 0, 1, 2, ...
	
		depth: Current search depth - whether this is a MIN or a MAX node depends both on depth and agentId.
		
		maxDepth: maximum desired search depth - once reached, your code should somehow return
			  a minimax utility value for this location.

		alpha. beta: alpha and beta values passed from the parent node to constrain search at this
			     level.

   Return values:
		Your search code will directly update data passed-in as arguments:
		
		- Mini-Max value	: Notice this function returns a double precision number. This is
					  the minimax value at this level of the tree. It will be used 
					  as the recursion backtracks filling-in the mini-max values back
					  from the leaves to the root of the search tree. 

		- path[1][2]		: Your MiniMax function will return the location for the agent's 
					  next location (i.e. the optimal move for the agent). 
		- minmax_cost[size_X][size_Y] 	:  Your search code will update this array to contain the
						   minimax value for locations that were expanded during
						   the search. This must be done *only* for the mouse.

						   Values in this array will be in the range returned by
						   your utility function.

		* Your code MUST NOT modify the locations or numbers of cats and/or cheeses, the graph,
	     	  or the location of the mouse - if you try, the driver code will know it *
			
		That's that, now, implement your solution!
 */

 /********************************************************************************************************
 * 
 * TO DO:	Implement code to perform a MiniMax search. This will involve a limited-depth BFS-like
 *              expansion. Once nodes below return values, your function will propagate minimax utilities
 *		as per the minimax algorithm.
 *	
 *		Note that if alpha-beta pruning is specified, you must keep track of alphas and betas
 *		along the path.
 *
 *		You can use helper functions if it seems reasonable. Add them to the MiniMax_search.h
 *		file and explain in your code why they are needed and how they are used.
 *
 *		Recursion should appear somewhere.
 *
 *		MiniMax cost: If the agentId=0 (Mouse), then once you have a MiniMax value for a location
 *		in the maze, you must update minmax_cost[][] for that location.
 *
 *		How you design your solution is up to you. But:
 *
 *		- Document your implementation by adding concise and clear comments in this file
 *		- Document your design (how you implemented the solution, and why) in the report
 *
 ********************************************************************************************************/

 // Stub so that the code compiles/runs - This will be removed and replaced by your code!


 return(0.0);
}

double getAvgCatDis(int cat_loc[10][2],int cats, int mouse_loc[1][2]){
	double accu = 0.0;
	for (size_t i = 0; i < cats; i++)
	{
		accu += (double)(abs(cat_loc[i][0]-mouse_loc[0][0])+abs(cat_loc[i][1]-mouse_loc[0][1]));
	}
	return accu/cats;
	
}

double getDisToCheese(int mouse_loc[1][2],int cheese_loc[10][2], int cheeses){
	int closestCheese = size_X+size_Y;
	for (size_t i = 0; i < cheeses; i++)
	{
		int tempDis = abs(cheese_loc[i][0]-mouse_loc[0][0])+abs(cheese_loc[i][1]-mouse_loc[0][1]);
		if (tempDis<closestCheese)
		{
			closestCheese = tempDis;
		}
		
	}
	return (double)closestCheese;
	
}

double openess(int mouse_loc[1][2],double gr[graph_size][4]){
	int index = toInd(mouse_loc[0][0],mouse_loc[0][1]);
	int count = 0;
	for (size_t i = 0; i < 4; i++)
	{
		if (gr[index][i]==0)
		{
			count++;
		}
		
	}
	return count;
	
}
int getPathLen(int pred[graph_size], int curIndex, int mouseInd)
{
	Cor *cor = (Cor *)malloc(sizeof(Cor));
	int path_counter = 1;
	int path_reversed[graph_size];
	path_reversed[0] = curIndex;
	while (path_counter < graph_size)
	{
		if (curIndex == mouseInd)
		{

			path_counter--;
			break;
		}
		path_reversed[path_counter] = pred[curIndex];
		curIndex = pred[curIndex];
		path_counter++;
	}
	free(cor);
	return path_counter;
}
int search(double gr[graph_size][4], int cat_loc[10][2], int cats, int cheese_loc[10][2], int cheeses, int mouse_loc[1][2])
{
	/* -----------------------------
	initialization begin, location translation, queue, visited, path arrays set up
	*/
	int allChese[cheeses];
	int allCats[cats];
	for (size_t i = 0; i < cheeses; i++)
	{
		allChese[i] = toInd(cheese_loc[i][0], cheese_loc[i][1]);
	}
	for (size_t i = 0; i < cats; i++)
	{
		allCats[i] = toInd(cat_loc[i][0], cat_loc[i][1]);
	}

	int mouseInd = toInd(mouse_loc[0][0], mouse_loc[0][1]);

	Llist *queue = newQueue();


	queueTail(queue, mouseInd);
	
	// allocates variables
	int counter = 1;
	int pred[graph_size];
	bool visited[graph_size];
	int curIndex;
	int dis[graph_size];
	Cor *cor = (Cor *)malloc(sizeof(Cor));
	//initialize variables
	for (size_t i = 0; i < graph_size; i++)
	{
		pred[i] = -1;
		visited[i] = 0;
		dis[i] = __INT_MAX__;
	}
	dis[mouseInd] = 0;

	/* -------------------------------
	initialization complete, now onto search, BFS and DFS are same except one push new nodes to tail of queue, one push to head
	interms of Heuristic search, a priority queue is used 
*/

	while (queue->size != 0)
	{
		curIndex = popFirstInd(queue);
		visited[curIndex] = true;
		indToCor(cor, curIndex);
		counter++;
		if (isChese(allChese, cheeses, curIndex))
		{
			break;
		}

		for (size_t i = 0; i < 4; i++)
		{
			int neibour = gr[curIndex][i];
			if (neibour != 0)
			{
				int neiInd = getIndNei(curIndex, i);
				if (isCat(allCats, cats, neiInd))
				{
					continue;
				}
				if (!visited[neiInd])
				{

					 //BFS
					
					queueTail(queue, neiInd);
					
				
					// all paths are updated except for one of heuristic case : current node is in minQueue but does not have better f(n)
					pred[neiInd] = curIndex;
				}
			}
		}
	}
	queueFree(queue);
	free(queue);
	free(cor);
	return getPathLen(pred, curIndex, mouseInd);
	

}

double utility(int cat_loc[10][2], int cheese_loc[10][2], int mouse_loc[1][2], int cats, int cheeses, int depth, double gr[graph_size][4])
{
 /*
	This function computes and returns the utility value for a given game configuration.
	As discussed in lecture, this should return a positive value for configurations that are 'good'
	for the mouse, and a negative value for locations that are 'bad' for the mouse.

	How to define 'good' and 'bad' is up to you. Note that you can write a utility function
	that favours your mouse or favours the cats, but that would be a bad idea... (why?)

	Input arguments:

		cat_loc - Cat locations
		cheese_loc - Cheese locations
		mouse_loc - Mouse location
		cats - # of cats
		cheeses - # of cheeses
		depth - current search depth
		gr - The graph's adjacency list for the maze

		These arguments are as described in A1. Do have a look at your solution!
 */
int blockedNode = openess(mouse_loc,gr);
 double distanceToCheese = getDisToCheese(mouse_loc,cheese_loc,cheeses);
 double distanceToCats = getAvgCatDis(cat_loc,cats,mouse_loc);
 return -search(gr,cat_loc,cats,cheese_loc,cheeses,mouse_loc)-blockedNode*4+distanceToCheese + 2*distanceToCats + random() % 3;   // <--- Obviously, this will be replaced by your computer utilities
}

int checkForTerminal(int mouse_loc[1][2],int cat_loc[10][2],int cheese_loc[10][2],int cats,int cheeses)
{
 /* 
   This function determines whether a given configuration constitutes a terminal node.
   Terminal nodes are those for which:
     - A cat eats the mouse
     or
     - The mouse eats a cheese
   
   If the node is a terminal, the function returns 1, else it returns 0
 */

 // Check for cats having lunch
 for (int i=0; i<cats; i++)
  if (mouse_loc[0][0]==cat_loc[i][0]&&mouse_loc[0][1]==cat_loc[i][1]) return(1);

 // Check for mouse having lunch
 for (int i=0; i<cheeses; i++)
  if (mouse_loc[0][0]==cheese_loc[i][0]&&mouse_loc[0][1]==cheese_loc[i][1]) return(1);

 return(0);

}

