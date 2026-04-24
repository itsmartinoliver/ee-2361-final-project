#include <stdbool.h>
#include <stdio.h>

#define MAX_NODES 100
#define MAZE_WIDTH 20
#define MAZE_HEIGHT 19

char rawMazeMap[] = "********************"
                    "*S       *    *   **"
                    "***** ** * ** * *  *"
                    "*     ** * **   * **"
                    "* ******   ** ***  *"
                    "*   ********* *   **"
                    "*** **   *    * ****"
                    "* * ** * * ****    *"
                    "* * ** * * *  * ****"
                    "*   ** *   ** * ** *"
                    "* * ** ******      *"
                    "* *    *    * **** *"
                    "* ******* * *   ** *"
                    "*       * * *** *  *"
                    "*** *** * *   * * **"
                    "* * * * * * * * *  *"
                    "* * * * * * * * ****"
                    "*   *     *   *   F*"
                    "********************";
char mazeMap[MAZE_HEIGHT][MAZE_WIDTH];

typedef enum { LEFT, RIGHT, FORWARD } Direction;

typedef struct MazeVertex {
    // struct MazeVertex *left;
    // struct MazeVertex *right;
    // struct MazeVertex *forward;
    Direction currentDirection;
    bool isDfsHead;
} MazeNode;

typedef struct SolutionStack {
    Direction move;
    struct SolutionStack *next;
} SolutionStack;

MazeNode vertexStack[MAX_NODES];

// typedef struct {
//   Direction *items;
//   int top;
//   int capacity;
// } SolutionStack;

// int nodeStack = 0;

// void push(SolutionStack *stack, Direction dir) {
//     StackNode *newNode = malloc(sizeof(StackNode));
//     if (!newNode) return;
//     newNode->move = dir;
//     newNode->next = stack->head;
//     stack->head = newNode;
// }

// MazeNode stringToMazeTree(void) { return (MazeNode)0; }

// MazeNode exploreMaze(void) {
//     return 1;
// }

int main(void) {
    // printf("working build system\n");
    for (int i = 0; i < MAZE_HEIGHT; i++) {
        for (int j = 0; j < MAZE_WIDTH; j++) {
            mazeMap[i][j] = rawMazeMap[i*(MAZE_WIDTH)+j];
        }
    }
    for (int i = 0; i < MAZE_HEIGHT; i++) {
        for (int j = 0; j < MAZE_WIDTH; j++) {
            printf("%c", mazeMap[i][j]);
        }
        printf("\n");
    }



    __asm__ volatile("nop");
}
