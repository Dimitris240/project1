#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>

#define MAX_MOVES 6

typedef struct {
    int parent;
    char operation[20];
    int cost;
    int visited;
    int f; // For A* (cost + heuristic)
} State;

State *states;
int LIMIT;

char *ops[MAX_MOVES] = {"increase", "decrease", "double", "half", "square", "sqrt"};

int getCost(int x, int op){
    if(op == 0) return 2; // increase
    if(op == 1) return 2; // decrease
    if(op == 2) return (x/2) + 1; // double
    if(op == 3) return (x/4) + 1; // half
    if(op == 4) return ((x*x - x)/4) + 1; // square
    if(op == 5) return abs(x - sqrt(x))/4 + 1; // sqrt
    return 1;
}

int apply(int value, int op){
    switch(op){
        case 0: return value + 1;
        case 1: return value > 0 ? value - 1 : -1;
        case 2: return value * 2;
        case 3: return value / 2;
        case 4: return value * value;
        case 5: return (int)sqrt(value);
        default: return -1;
    }
}

int heuristic(int current, int goal){
    return abs(goal - current);
}

int BFS(int start, int goal){
    int *queue = malloc((LIMIT + 1) * sizeof(int));
    int front = 0, rear = 0;

    queue[rear++] = start;

    while(front < rear){
        int current = queue[front++];

        if(current == goal){
            free(queue);
            return 1;
        }

        for(int i = 0; i < MAX_MOVES; i++){
            int next = apply(current, i);
            if(next < 0 || next > LIMIT) continue;

            if(!states[next].visited){
                states[next].visited = 1;
                states[next].parent = current;
                strcpy(states[next].operation, ops[i]);
                states[next].cost = states[current].cost + getCost(current, i);
                queue[rear++] = next;
            }
        }
    }
    free(queue);
    return 0;
}

int found = 0;

void DFS(int current, int goal){
    if(found) return;

    if(current == goal){
        found = 1;
        return;
    }

    for(int i = 0; i < MAX_MOVES; i++){
        int next = apply(current, i);
        if(next < 0 || next > LIMIT) continue;

        if(!states[next].visited){
            states[next].visited = 1;
            states[next].parent = current;
            strcpy(states[next].operation, ops[i]);
            states[next].cost = states[current].cost + getCost(current, i);
            DFS(next, goal);
        }
    }
}

// ---------------- BEST FIRST SEARCH -----------------

int best(int start, int goal){
    int safety = 50000;

    while(safety--){
        int bestNode = -1, bestH = 999999;

        for(int i = 0; i <= LIMIT; i++){
            if(states[i].visited == 1){
                int h = abs(goal - i) / 2;  // improved heuristic
                if(h < bestH){
                    bestH = h;
                    bestNode = i;
                }
            }
        }

        if(bestNode == -1) return 0;
        if(bestNode == goal) return 1;

        states[bestNode].visited = 2;

        for(int op = 0; op < MAX_MOVES; op++){
            int next = apply(bestNode, op);
            if(next < 0 || next > LIMIT) continue;

            if(states[next].visited == 0){
                states[next].visited = 1;
                states[next].parent = bestNode;
                strcpy(states[next].operation, ops[op]);
                states[next].cost = states[bestNode].cost + getCost(bestNode, op);
            }
        }
    }

    return 0;
}



// --------------------- A* SEARCH ---------------------

int astar(int start, int goal){
    int safety = 50000; // prevents infinite loop

    while(safety--){
        int bestNode = -1, bestF = 999999;

        for(int i = 0; i <= LIMIT; i++){
            if(states[i].visited == 1 && states[i].f < bestF){
                bestF = states[i].f;
                bestNode = i;
            }
        }

        if(bestNode == -1) return 0;
        if(bestNode == goal) return 1;

        states[bestNode].visited = 2;

        for(int op = 0; op < MAX_MOVES; op++){
            int next = apply(bestNode, op);
            if(next < 0 || next > LIMIT) continue;

            int newCost = states[bestNode].cost + getCost(bestNode, op);

            if(states[next].visited == 0 || newCost < states[next].cost){
                states[next].visited = 1;
                states[next].parent = bestNode;
                strcpy(states[next].operation, ops[op]);
                states[next].cost = newCost;
                states[next].f = newCost + abs(goal - next);
            }
        }
    }

    return 0;
}


void writeSolution(int start, int goal, char *filename){
    FILE *fp = fopen(filename, "w");
    if(!fp){
        printf("Error writing file.\n");
        return;
    }

    int *path = malloc((LIMIT + 1) * sizeof(int));
    int steps = 0;
    int current = goal;

    while(current != start){
        path[steps++] = current;
        current = states[current].parent;
    }
    path[steps++] = start;

    fprintf(fp, "%d %d\n", steps - 1, states[goal].cost);

    for(int i = steps - 1; i > 0; i--){
        int from = path[i];
        int to = path[i - 1];
        fprintf(fp, "%s %d %d\n", states[to].operation, to, states[to].cost);
    }

    fclose(fp);
    free(path);
    printf("Solution written to %s\n", filename);
}

int main(int argc, char *argv[]){
    if(argc != 5){
        printf("Usage: register.exe <breadth|depth|best|astar> <start> <goal> <output>\n");
        return 1;
    }

    char *method = argv[1];
    int start = atoi(argv[2]);
    int goal = atoi(argv[3]);

    LIMIT = (start > goal ? start : goal) * 4;
    states = calloc(LIMIT + 1, sizeof(State));
    states[start].visited = 1;
    states[start].parent = -1;
    states[start].cost = 0;
    states[start].f = heuristic(start, goal);

    clock_t t_start, t_end;
    double time_used;
    int solved = 0;

    // -------------------------- TIMING START --------------------------
    t_start = clock();

    if(strcmp(method, "breadth") == 0){
        solved = BFS(start, goal);
    }
    else if(strcmp(method, "depth") == 0){
        found = 0;
        DFS(start, goal);
        solved = found;
    }
    else if(strcmp(method, "best") == 0){
        solved = best(start, goal);
    }
    else if(strcmp(method, "astar") == 0){
        solved = astar(start, goal);
    }
    else {
        printf("Invalid method.\n");
        return 1;
    }

    t_end = clock();
    // -------------------------- TIMING END ----------------------------

    time_used = (double)(t_end - t_start) / CLOCKS_PER_SEC;
    printf("%s time = %f seconds\n", method, time_used);

    if(solved)
        writeSolution(start, goal, argv[4]);
    else
        printf("No solution found.\n");

    free(states);
    return 0;
}

