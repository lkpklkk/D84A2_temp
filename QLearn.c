/*
        CSC D84 - Unit 3 - Reinforcement Learning

        This file contains stubs for implementing the Q-Learning method
        for reinforcement learning as discussed in lecture. You have to
        complete two versions of Q-Learning.

        * Standard Q-Learning, based on a full-state representation and
          a large Q-Table
        * Feature based Q-Learning to handle problems too big to allow
          for a full-state representation

        Read the assignment handout carefully, then implement the
        required functions below. Sections where you have to add code
        are marked

        **************
        *** TO DO:
        **************

        If you add any helper functions, make sure you document them
        properly and indicate in the report.txt file what you added.

        Have fun!

        DO NOT FORGET TO 'valgrind' YOUR CODE - We will check for pointer
        management being done properly, and for memory leaks.

        Starter code: F.J.E. Jan. 16
*/
#include "QLearn.h"
// helper function that convert coordinate to the index for graph array
static void featureFuncHandlar(double gr[max_graph_size][4],
                               double features[25], int mouse_pos[1][2],
                               int cats[5][2], int cheeses[5][2], int size_X,
                               int graph_size, int feat_num);

typedef struct cor
{
  int x;
  int y;
} Cor;
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
Node *
newPriNode(int ind, int priority)
{
  Node *newNode = (Node *)malloc(sizeof(Node));
  newNode->ind = ind;
  newNode->next = NULL;
  newNode->priority = priority;
  return newNode;
}
Node *
newNode(int ind)
{
  Node *newNode = (Node *)malloc(sizeof(Node));
  newNode->ind = ind;
  newNode->next = NULL;
  newNode->priority = __INT_MAX__;
  return newNode;
}
Llist *
newQueue()
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
int toInd(int x, int y, int size_X)
{
  int index = x + (y * size_X);
  return index;
}
void indToCor(Cor *cor, int index, int size_X)
{
  cor->x = index % size_X;
  cor->y = index / size_X;
}
int getIndNei(int curIndex, int neiInd, int size_X)
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

int predBTCount(int graph_size, int *pred, int curIndex,
                int mouseInd)
{
  Cor *cor = (Cor *)malloc(sizeof(Cor));
  int path_counter = 1;
  while (path_counter < graph_size)
  {
    if (curIndex == mouseInd)
    {

      path_counter--;
      break;
    }
    curIndex = pred[curIndex];
    path_counter++;
  }
  return path_counter;
}

int H_cost(int graph_size, int x, int y, int cheese_loc[10][2],
           int mouse_loc[1][2], int cheeses, double gr[max_graph_size][4])
{
  /*
  This function computes and returns the heuristic cost for location x,y.
  As discussed in lecture, this means estimating the cost of getting from x,y
  to the goal. The goal is cheese. Which cheese is up to you. Whatever you code
  here, your heuristic must be admissible.

  Input arguments:

          x,y - Location for which this function will compute a heuristic
  search cost cat_loc - Cat locations cheese_loc - Cheese locations mouse_loc -
  Mouse location cats - # of cats cheeses - # of cheeses gr - The graph's
  adjacency list for the maze

          These arguments are as described in the search() function above
*/
  int closestCheese = __INT_MAX__;
  for (size_t i = 0; i < cheeses; i++)
  {
    int curCX = cheese_loc[i][0];
    int curCY = cheese_loc[i][1];
    int temp = abs(curCX - x) + abs(curCY - y);
    if (temp < closestCheese)
    {
      closestCheese = temp;
    }
  }

  return closestCheese; // <-- Evidently you will need to update this.
}

int search_path_len(int size_X, int graph_size, double gr[max_graph_size][4],
                    int cheese_loc[10][2], int cheeses, int mouse_loc[1][2], int mode)
{
  int allChese[cheeses];

  for (size_t i = 0; i < cheeses; i++)
  {
    allChese[i] = toInd(cheese_loc[i][0], cheese_loc[i][1], size_X);
  }

  int mouseInd = toInd(mouse_loc[0][0], mouse_loc[0][1], size_X);

  Llist *queue = newQueue();
  if (mode == 1)
  {
    pushMinQueue(queue, mouseInd,
                 H_cost(graph_size, mouse_loc[0][0], mouse_loc[0][1],
                        cheese_loc, mouse_loc, cheeses, gr));
  }
  else
  {
    queueTail(queue, mouseInd);
  }
  // allocates variables
  int counter = 1;
  int *pred = (int *)malloc(sizeof(int) * graph_size);
  bool visited[graph_size];
  int curIndex;
  int dis[graph_size];
  Cor *cor = (Cor *)malloc(sizeof(Cor));
  // initialize variables
  for (size_t i = 0; i < graph_size; i++)
  {
    pred[i] = -1;
    visited[i] = 0;
    dis[i] = __INT_MAX__;
  }
  dis[mouseInd] = 0;

  /* -------------------------------
  initialization complete, now onto search, BFS and DFS are same except one
  push new nodes to tail of queue, one push to head interms of Heuristic
  search, a priority queue is used
*/
  while (queue->size != 0)
  {
    curIndex = popFirstInd(queue);
    visited[curIndex] = true;
    indToCor(cor, curIndex, size_X);

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
        int neiInd = getIndNei(curIndex, i, size_X);

        if (!visited[neiInd])
        {

          if (mode == 1)
          {
            indToCor(cor, neiInd, size_X);

            int f = H_cost(graph_size, cor->x, cor->y, cheese_loc,
                           mouse_loc, cheeses, gr);

            if (minQueueCheck(queue, neiInd))
            {
              if (minQueueUpdate(queue, neiInd,
                                 f + dis[neiInd] + 1))
              {
                dis[neiInd] = dis[curIndex] + 1;
              }
              else
              {
                continue;
              }
            }
            else
            {
              dis[neiInd] = dis[curIndex] + 1;
              pushMinQueue(queue, neiInd, f + dis[neiInd] + 1);
            }
          }
          else
          {
            queueTail(queue, neiInd);
          }

          // all paths are updated except for one of heuristic case :
          // current node is in minQueue but does not have better f(n)
          pred[neiInd] = curIndex;
        }
      }
    }
  }
  int bla = predBTCount(graph_size, pred, curIndex, mouseInd);
  queueFree(queue);
  free(queue);
  free(cor);
  free(pred);
  return bla;
}

int get_best_move(double *QTable, int s, int *available_move, int available_count)
{

  double max_qa_new = -DBL_MAX;
  int best_a;
  double temp;
  for (int i = 0; i < available_count; i++)
  {
    temp = *(QTable + 4 * s + *(available_move + i));
    if (temp > max_qa_new)
    {
      max_qa_new = temp;
      best_a = *(available_move + i);
    }
  }
  return best_a;
}

double
get_best_qsa(double *QTable, int s)
{
  double max_qsa = -DBL_MAX;
  double temp;
  for (size_t i = 0; i < 4; i++)
  {
    temp = *(QTable + 4 * s + i);
    if (temp > max_qsa)
    {
      max_qsa = temp;
    }
  }
  return max_qsa;
}

void get_newloc_on_nei_ind(int pos[1][2], int nei_ind)
{
  switch (nei_ind)
  {
  case 0:
    pos[0][1]--;
    break;
  case 1:
    pos[0][0]++;
    break;
  case 2:
    pos[0][1]++;
    break;
  case 3:
    pos[0][0]--;
    break;
  default:
    break;
  }
}

void QLearn_update(int s, int a, double r, int s_new, double *QTable)
{
  /*
    This function implementes the Q-Learning update as stated in Lecture. It
    receives as input a <s,a,r,s'> tuple, and updates the Q-table accordingly.

    Your work here is to calculate the required update for the Q-table entry
    for state s, and apply it to the Q-table

    The update involves two constants, alpha and lambda, which are defined in
    QLearn.h - you should not have to change their values. Use them as they
    are.

    Details on how states are used for indexing into the QTable are shown
    below, in the comments for QLearn_action. Be sure to read those as well!
  */

  /***********************************************************************************************
   * TO DO: Complete this function
   ***********************************************************************************************/

  double *cur_qa_ind = QTable + 4 * s + a;
  double max_qsa_new = get_best_qsa(QTable, s_new);
  double update = alpha * (r + lambda * max_qsa_new - *(cur_qa_ind));
  *(cur_qa_ind) = *(cur_qa_ind) + update;
}

int QLearn_action(double gr[max_graph_size][4], int mouse_pos[1][2],
                  int cats[5][2], int cheeses[5][2], double pct, double *QTable,
                  int size_X, int graph_size)
{
  /*
     This function decides the action the mouse will take. It receives as
   inputs
     - The graph - so you can check for walls! The mouse must never move
   through walls
     - The mouse position
     - The cat position
     - The chees position
     - A 'pct' value in [0,1] indicating the amount of time the mouse uses the
   QTable to decide its action, for example, if pct=.25, then 25% of the time
   the mouse uses the QTable to choose its action, the remaining 75% of the
   time it chooses randomly among the available actions.

     Remember that the training process involves random exploration initially,
   but as training proceeds we use the QTable more and more, in order to
   improve our QTable values around promising actions.

     The value of pct is controlled by QLearn_core_GL, and increases with each
   round of training.

     This function *must return* an action index in [0,3] where
        0 - move up
        1 - move right
        2 - move down
        3 - move left

     QLearn_core_GL will print a warning if your action makes the mouse cross a
   wall, or if it makes the mouse leave the map - this should not happen. If
   you see a warning, fix the code in this function!

   The Q-table has been pre-allocated and initialized to 0. The Q-table has
   a size of

        graph_size^3 x 4

   This is because the table requires one entry for each possible state, and
   the state is comprised of the position of the mouse, cat, and cheese.
   Since each of these agents can be in one of graph_size positions, all
   possible combinations yield graph_size^3 states.

   Now, for each state, the mouse has up to 4 possible moves (up, right,
   down, and left). We ignore here the fact that some moves are not possible
   from some states (due to walls) - it is up to the QLearn_action() function
   to make sure the mouse never crosses a wall.

   So all in all, you have a big table.

   For example, on an 8x8 maze, the Q-table will have a size of

       64^3 x 4  entries

       with

       size_X = 8		<--- size of one side of the maze
       graph_size = 64		<--- Total number of nodes in the graph

   Indexing within the Q-table works as follows:

     say the mouse is at   i,j
         the cat is at     k,l
         the cheese is at  m,n

     state = (i+(j*size_X)) + ((k+(l*size_X))*graph_size) +
   ((m+(n*size_X))*graph_size*graph_size)
     ** Make sure you undestand the state encoding above!

     Entries in the Q-table for this state are

     *(QTable+(4*state)+a)      <-- here a is the action in [0,3]

     (yes, it's a linear array, no shorcuts with brackets!)

     NOTE: There is only one cat and once cheese, so you only need to use
   cats[0][:] and cheeses[0][:]
   */

  /***********************************************************************************************
   * TO DO: Complete this function
   ***********************************************************************************************/

  int mouse_ind = toInd(mouse_pos[0][0], mouse_pos[0][1], size_X);
  int *available_move = (int *)malloc(4 * sizeof(int));

  int available_count = 0;
  for (int i = 0; i < 4; i++)
  {
    if (gr[mouse_ind][i])
    {
      *(available_move + available_count) = i;
      available_count++;
    }
  }

  double c_int = (double)(rand() % 101);
  double c = c_int / 100;
  int a;
  // random, since pct increases, so we do >=, less likely to generate random
  // as pct gets larger
  if (c > pct)
  {
    int action_index = rand() % available_count;
    a = *(available_move + action_index);
  }
  else
  {
    int i = mouse_pos[0][0];
    int j = mouse_pos[0][1];
    int k = cats[0][0];
    int l = cats[0][1];
    int m = cheeses[0][0];
    int n = cheeses[0][1];
    int s = (i + (j * size_X)) + ((k + (l * size_X)) * graph_size) + ((m + (n * size_X)) * graph_size * graph_size);
    a = get_best_move(QTable, s, available_move, available_count);
  }
  free(available_move);
  return (a); // <--- of course, you will change this!
}

double
QLearn_reward(double gr[max_graph_size][4], int mouse_pos[1][2],
              int cats[5][2], int cheeses[5][2], int size_X, int graph_size)
{
  /*
    This function computes and returns a reward for the state represented by
    the input mouse, cat, and cheese position.

    You can make this function as simple or as complex as you like. But it
    should return positive values for states that are favorable to the mouse,
    and negative values for states that are bad for the mouse.

    I am providing you with the graph, in case you want to do some processing
    on the maze in order to decide the reward.

    This function should return a maximim/minimum reward when the mouse
    eats/gets eaten respectively.
   */

  /***********************************************************************************************
   * TO DO: Complete this function
   ***********************************************************************************************/
  double r = 0;

  if (cats[0][0] == mouse_pos[0][0] && cats[0][1] == mouse_pos[0][1])
  {
    r -= (double)100;
  }
  if (cheeses[0][0] == mouse_pos[0][0] && cheeses[0][1] == mouse_pos[0][1])
  {
    r += (double)100;
  }
  int count = 0;
  int mouse_ind = toInd(mouse_pos[0][0], mouse_pos[0][1], size_X);
  for (size_t i = 0; i < 4; i++)
  {
    if (!gr[mouse_ind][i])
    {
      count++;
    }
  }
  // if (count == 3)
  // {
  //   r -= 10;
  // }

  return r;
}

void feat_QLearn_update(double gr[max_graph_size][4], double weights[25],
                        double reward, int mouse_pos[1][2], int cats[5][2],
                        int cheeses[5][2], int size_X, int graph_size)
{
  /*
    This function performs the Q-learning adjustment to all the weights
    associated with your features. Unlike standard Q-learning, you don't
    receive a <s,a,r,s'> tuple, instead, you receive the current state (mouse,
    cats, and cheese potisions), and the reward associated with this action
    (this is called immediately after the mouse makes a move, so implicit in
    this is the mouse having selected some action)

    Your code must then evaluate the update and apply it to the weights in the
    weight array.
   */

  /***********************************************************************************************
   * TO DO: Complete this function
   ***********************************************************************************************/
  // order of implementation
  // i can get qas, maxqsa, then evaluate, then we can have this working
  double *features = (double *)malloc(25 * sizeof(double));
  double *maxU = (double *)malloc(sizeof(double));
  int *maxA = (int *)malloc(sizeof(int));
  double q_now;
  double q_new;

  // fill in maxU, maxA
  maxQsa(gr, weights, mouse_pos, cats, cheeses, size_X, graph_size, maxU,
         maxA);

  // fill in features
  evaluateFeatures(gr, features, mouse_pos, cats, cheeses, size_X,
                   graph_size);
  q_now = Qsa(weights, features);
  q_new = *maxU;
  for (size_t i = 0; i < numFeatures; i++)
  {
    weights[i] += (alpha * (reward + lambda * (q_new)-q_now)) * features[i];
  }
  free(maxU);
  free(maxA);
  free(features);
}

int feat_QLearn_action(double gr[max_graph_size][4], double weights[25],
                       int mouse_pos[1][2], int cats[5][2], int cheeses[5][2],
                       double pct, int size_X, int graph_size)
{
  /*
    Similar to its counterpart for standard Q-learning, this function returns
    the index of the next action to be taken by the mouse.

    Once more, the 'pct' value controls the percent of time that the function
    chooses an optimal action given the current policy.

    E.g. if 'pct' is .15, then 15% of the time the function uses the current
    weights and chooses the optimal action. The remaining 85% of the time, a
    random action is chosen.

    As before, the mouse must never select an action that causes it to walk
    through walls or leave the maze.
   */

  /***********************************************************************************************
   * TO DO: Complete this function
   ***********************************************************************************************/

  int mouse_ind = toInd(mouse_pos[0][0], mouse_pos[0][1], size_X);
  int *available_move = (int *)malloc(4 * sizeof(int));

  int available_count = 0;
  for (int i = 0; i < 4; i++)
  {
    if (gr[mouse_ind][i])
    {
      *(available_move + available_count) = i;
      available_count++;
    }
  }

  double c_int = (double)(rand() % 101);
  double c = c_int / 100;
  int a;
  // random, since pct increases, so we do >=, less likely to generate random
  // as pct gets larger
  if (c > pct)
  {
    int action_index = rand() % available_count;
    a = *(available_move + action_index);
  }
  else
  {
    double *maxU = (double *)malloc(sizeof(double));
    int *maxA = (int *)malloc(sizeof(int));
    maxQsa(gr, weights, mouse_pos, cats, cheeses, size_X, graph_size, maxU,
           maxA);
    a = *maxA;
    free(maxU);
    free(maxA);
  }
  free(available_move);

  return (a);
}

void evaluateFeatures(double gr[max_graph_size][4], double features[25],
                      int mouse_pos[1][2], int cats[5][2], int cheeses[5][2],
                      int size_X, int graph_size)
{
  /*
   This function evaluates all the features you defined for the game
   configuration given by the input mouse, cats, and cheese positions. You are
   free to define up to 25 features. This function will evaluate each, and
   return all the feature values in the features[] array.

   Take some time to think about what features would be useful to have, the
   better your features, the smarter your mouse!

   Note that instead of passing down the number of cats and the number of
   cheese chunks (too many parms!) the arrays themselves will tell you what are
   valid cat/cheese locations.

   You can have up to 5 cats and up to 5 cheese chunks, and array entries for
   the remaining cats/cheese will have a value of -1 - check this when
   evaluating your features!
  */

  /***********************************************************************************************
   * TO DO: Complete this function
   ***********************************************************************************************/
  for (size_t i = 0; i < numFeatures; i++)
  {
    featureFuncHandlar(gr, features, mouse_pos, cats, cheeses, size_X,
                       graph_size, i);
  }
}

double
Qsa(double weights[25], double features[25])
{
  /*
    Compute and return the Qsa value given the input features and current
    weights
   */

  /***********************************************************************************************
   * TO DO: Complete this function
   ***********************************************************************************************/
  double qsa = 0;
  for (size_t i = 0; i < numFeatures; i++)
  {
    qsa += weights[i] * features[i];
  }

  return (qsa); // <--- stub! compute and return the Qsa value
}

void maxQsa(double gr[max_graph_size][4], double weights[25], int mouse_pos[1][2],
            int cats[5][2], int cheeses[5][2], int size_X, int graph_size,
            double *maxU, int *maxA)
{
  /*
    Given the state represented by the input positions for mouse, cats, and
    cheese, this function evaluates the Q-value at all possible neighbour
    states and returns the max. The maximum value is returned in maxU and the
    index of the action corresponding to this value is returned in maxA.

    You should make sure the function does not evaluate moves that would make
    the mouse walk through a wall.
   */

  /***********************************************************************************************
   * TO DO: Complete this function
   ***********************************************************************************************/
  *maxU = -DBL_MAX; // <--- stubs! your code will compute actual values for
                    // these two variables!
  double *features = (double *)malloc(sizeof(double) * 25);
  int mouse_ind = toInd(mouse_pos[0][0], mouse_pos[0][1], size_X);
  int cur_mouse_x = mouse_pos[0][0];
  int cur_mouse_y = mouse_pos[0][1];
  double temp = 0;
  for (int i = 0; i < 4; i++)
  {
    if (gr[mouse_ind][i])
    {
      get_newloc_on_nei_ind(mouse_pos, i);
      evaluateFeatures(gr, features, mouse_pos, cats, cheeses, size_X,
                       graph_size);
      temp = Qsa(weights, features);
      if (temp >= *maxU)
      {
        *maxU = temp;

        *maxA = i;
      }
      mouse_pos[0][0] = cur_mouse_x;
      mouse_pos[0][1] = cur_mouse_y;
    }
  }
  free(features);
  return;
}

/***************************************************************************************************
 *  Add any functions needed to compute your features below
 *                 ---->  THIS BOX <-----
 * *************************************************************************************************/

double range_map(double input, double i_s, double i_e, double o_s, double o_e)
{
  double slope = 1.0 * (o_e - o_s) / (i_e - i_s);
  double output = o_s + slope * (input - i_s);
  return output;
}
int get_man_dis_closest(int target[5][2], int mouse_loc[1][2], int size_X, int graph_size)
{
  int closest = __INT_MAX__;
  int count = 0;
  for (size_t i = 0; i < 5; i++)
  {
    if (target[i][0] == -1)
    {
      break;
    }
    count++;
  }
  int x = mouse_loc[0][0];
  int y = mouse_loc[0][1];
  for (size_t i = 0; i < count; i++)
  {

    int curCX = target[i][0];
    int curCY = target[i][1];
    int temp = abs(curCX - x) + abs(curCY - y);
    if (temp < closest)
    {
      closest = temp;
    }
  }

  return closest;
}

double
relative_mous_cat_to_cheese(double gr[max_graph_size][4], int mouse_pos[1][2],
                            int cats[5][2], int cheeses[5][2], int size_X,
                            int graph_size)
{

  // int man_dis_cheese = get_man_dis_closest(cheeses, mouse_pos, size_X, graph_size);
  // int man_dis_cat = get_man_dis_closest(cats, mouse_pos, size_X, graph_size);
  // int diff = man_dis_cat - man_dis_cheese;
  // double shrinked = diff / (double(size_X * 2));
  // return shrinked;
  return 1 - range_map(get_man_dis_closest(cats, mouse_pos, size_X, graph_size), 0, size_X * 2, 0, 1);
}

double path_len_to_cheese(double gr[max_graph_size][4], int mouse_pos[1][2],
                          int cats[5][2], int cheeses[5][2], int size_X,
                          int graph_size)
{

  int num_cheese = 0;

  for (int i = 0; i < 5; i++)
  {

    if (cheeses[i][0] != -1)
    {
      num_cheese++;
      // printf("mouse pos : %d,%d, cheese pos %d,%d\n", mouse_pos[0][0], mouse_pos[0][1], cheeses[i][0], cheeses[i][1]);
    }
  }
  int path_len = search_path_len(size_X, graph_size, gr,
                                 cheeses, num_cheese, mouse_pos, 1);
  // printf("sizeX %d, path_len %d\n", size_X, path_len);
  if (path_len > size_X * 2)
  {
    path_len = size_X * 2;
  }

  double slope = 1.0 * (1.) / ((double)(size_X * 2));
  double output = slope * ((double)path_len);
  return 1. - output;
}

double cats_in_between(double gr[max_graph_size][4], int mouse_pos[1][2],
                       int cats[5][2], int cheeses[5][2], int size_X,
                       int graph_size)
{
  int cats_in_betwen = 0;
  int mouse_x = mouse_pos[0][0];
  int mouse_y = mouse_pos[0][1];
  int num_cheese = 0;
  int num_cats = 0;
  for (int i = 0; i < 5; i++)
  {

    if (cheeses[i][0] != -1)
    {
      num_cheese++;
    }
    if (cheeses[i][0] != -1)
    {
      num_cats++;
    }
  }
  for (size_t i = 0; i < num_cheese; i++)
  {
    int cheese_x = cheeses[i][0];
    int cheese_y = cheeses[i][1];
    double a = (cheeses[i][1] - mouse_pos[0][1]) / (double)(cheeses[i][0] - mouse_pos[0][0]);
    double b = -1.;
    double c = ((double)mouse_pos[0][1] - a * ((double)(mouse_pos[0][0])));
    double eu = sqrt(pow(mouse_pos[0][0] - cheeses[i][0], 2) + pow(mouse_pos[0][1] - cheeses[i][1], 2));
    for (size_t i = 0; i < num_cats; i++)
    {
      double d = abs(a * ((double)cats[i][0]) + b * ((double)cats[i][1]) + c) / sqrt(pow(a, 2) + pow(b, 2));
      int cat_x = cats[i][0];
      int cat_y = cats[i][1];

      if (d < (eu / 2.))
      {
        if ((cat_x > mouse_x && cat_x < cheese_x) || (cat_x < mouse_x && cat_x > cheese_x))
        {
          cats_in_betwen++;
        }
      }
    }
  }
  return range_map((double)cats_in_betwen, 0., (double)num_cats, 0., 1.);
}

static void
featureFuncHandlar(double gr[max_graph_size][4], double features[25],
                   int mouse_pos[1][2], int cats[5][2], int cheeses[5][2],
                   int size_X, int graph_size, int feat_num)
{
  switch (feat_num)
  {
  case 0:
    features[0] = relative_mous_cat_to_cheese(gr, mouse_pos, cats, cheeses,
                                              size_X, graph_size);
    break;
  case 1:
    features[1] = path_len_to_cheese(gr, mouse_pos, cats, cheeses,
                                     size_X, graph_size);
    break;
  case 2:
    features[2] = cats_in_between(gr, mouse_pos, cats, cheeses,
                                  size_X, graph_size);
    break;
  default:
    break;
  }
}
