/* Program to print and play checker games.

  Skeleton program written by Artem Polyvyanyy, artem.polyvyanyy@unimelb.edu.au,
  September 2021, with the intention that it be modified by students
  to add functionality, as required by the assignment specification.

  Student Authorship Declaration:

  (1) I certify that except for the code provided in the initial skeleton file,
  the program contained in this submission is completely my own individual
  work, except where explicitly noted by further comments that provide details
  otherwise. I understand that work that has been developed by another student,
  or by me in collaboration with other students, or by non-students as a result
  of request, solicitation, or payment, may not be submitted for assessment in
  this subject. I understand that submitting for assessment work developed by
  or in collaboration with other students or non-students constitutes Academic
  Misconduct, and may be penalized by mark deductions, or by other penalties
  determined via the University of Melbourne Academic Honesty Policy, as
  described at https://academicintegrity.unimelb.edu.au.

  (2) I also certify that I have not provided a copy of this work in either
  softcopy or hardcopy or any other form to any other student, and nor will I
  do so until after the marks are released. I understand that providing my work
  to other students, regardless of my intention or any undertakings made to me
  by that other student, is also Academic Misconduct.

  (3) I further understand that providing a copy of the assignment specification
  to any form of code authoring or assignment tutoring service, or drawing the
  attention of others to such services and code that may have been made
  available via such a service, may be regarded as Student General Misconduct
  (interfering with the teaching activities of the University and/or inciting
  others to commit Academic Misconduct). I understand that an allegation of
  Student General Misconduct may arise regardless of whether or not I personally
  make use of such solutions or sought benefit from such actions.

  Signed by: Haohong Liang
  Dated:     06/10/2021

*/

#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <assert.h>
#include <math.h>
#include <string.h>

/* some #define's from my sample solution ------------------------------------*/
#define EXIT_SUCCESS        0       // finished execution
#define EXIT_ERROR          1       // exit with error
#define BOARD_SIZE          8       // board size
#define ROWS_WITH_PIECES    3       // number of initial rows with pieces
#define CELL_EMPTY          '.'     // empty cell character
#define CELL_BPIECE         'b'     // black piece character
#define CELL_WPIECE         'w'     // white piece character
#define CELL_BTOWER         'B'     // black tower character
#define CELL_WTOWER         'W'     // white tower character
#define COST_PIECE          1       // one piece cost
#define COST_TOWER          3       // one tower cost
#define TREE_DEPTH          3       // minimax tree depth
#define COMP_ACTIONS        10      // number of computed actions
#define START_PIECE         12      // number of piece on initiailised board
#define ROW_BREAKER         "   +---+---+---+---+---+---+---+---+"
#define COLUMN_BREAKER      "|"
#define START_LINE          "\n=====================================\n"
#define MAX_DECISION        96      // amount of max action in a single turn
#define MAX_ACTION          8       // max possible move decision
#define PRE_INPUT           0       // outside input order
#define COMP_INPUT          1       // order computed itself

typedef unsigned char board_t[BOARD_SIZE][BOARD_SIZE];  // board type
typedef struct node node_t;
struct node {
    board_t board;
    char order[5];
    int cost;
    node_t* parent;
    node_t* children[MAX_DECISION];
    int children_counter;
    char decision[5];
};

void board_initialiser(board_t board);
void board_printer(board_t board);
void board_type(void);
int move_input(board_t board, int turn_counter, char order[]);
void turn_starter(board_t board, int turn, char move[], int indicator);
int next_action(board_t board, int turn);
void ten_action(board_t board, int turn);
int error_check(board_t board, int turn, int source[], int target[]);
void error_printer(int code);
int is_white_turn(int turn);
int is_capture(int turn, int source[], int target[], board_t board);
void chess_move(int source[], int target[], board_t board);
void chess_capture(int source[], int target[], board_t board);
void upgrade_tower(int target[], board_t board);
int cost_calculator(board_t board);
int move_checker(board_t board, int col, int row, int turn, char order[]);
void tree_builder(node_t* root, int turn, int depth);
void tree_freer(node_t* root);
node_t* node_maker(node_t* root, char order[], int turn, int i);

int main(int argc, char* argv[]) {
    board_t board;
    int turn_counter = 1;
    char order[5];

    //initial setup
    board_type();
    board_initialiser(board);
    board_printer(board);

    //input and execute
    while (scanf("%s\n", order) && order[0]) {
        if (order[0] == 'A' && strlen(order) == 1) {
            turn_counter = next_action(board, turn_counter);
        }
        else if (order[0] == 'P' && strlen(order) == 1) {
            ten_action(board, turn_counter);
            printf("\n");
        }
        else {
            move_input(board, turn_counter, order);
            turn_starter(board, turn_counter, order, PRE_INPUT);
            printf("BOARD COST: %d\n", cost_calculator(board));
            board_printer(board);
            turn_counter += 1;
        }
        order[0] = '\0';
    }
    printf("\n");
    return EXIT_SUCCESS;
    // exit program with the success code
}

//function used to initialise chess board
void board_initialiser(board_t board) {
    int i, j;
    //chess cell initialise
    for (i = 1; i < BOARD_SIZE; i += 2) {
        board[0][i] = CELL_WPIECE;
        board[2][i] = CELL_WPIECE;
        board[6][i] = CELL_BPIECE;
    }

    for (i = 0; i < BOARD_SIZE; i += 2) {
        board[1][i] = CELL_WPIECE;
        board[5][i] = CELL_BPIECE;
        board[7][i] = CELL_BPIECE;
    }
    //empty cell initialise
    for (i = 0; i < BOARD_SIZE; i++) {
        for (j = 0;j < BOARD_SIZE; j++) {
            if (board[i][j] != CELL_BPIECE && board[i][j] != CELL_WPIECE) {
                board[i][j] = CELL_EMPTY;
            }
        }
    }
}

//function used to print chess board
void board_printer(board_t board) {
    int i, j;
    printf("     A   B   C   D   E   F   G   H\n");
    printf(ROW_BREAKER);
    printf("\n");

    for (i = 0; i < BOARD_SIZE; i++) {
        printf(" %d ", i + 1);
        printf(COLUMN_BREAKER);

        for (j = 0; j < BOARD_SIZE; j++) {
            printf(" %c ", board[i][j]);
            printf(COLUMN_BREAKER);
        }
        printf("\n");

        if (i < BOARD_SIZE - 1) {
            printf(ROW_BREAKER);
            printf("\n");
        }
        else {
            printf(ROW_BREAKER);
        }
    }
}

// function used to report board configuration
void board_type(void) {
    printf("BOARD SIZE: %dx%d\n", BOARD_SIZE, BOARD_SIZE);
    printf("#BLACK PIECES: %d\n", START_PIECE);
    printf("#WHITE PIECES: %d\n", START_PIECE);
}

// function used to receive input data and move chess
int move_input(board_t board, int turn_counter, char order[]) {
    int source[2], target[2];

    // translate character input to array location
    source[0] = order[1] - 49;
    source[1] = order[0] - 65;
    target[0] = order[4] - 49;
    target[1] = order[3] - 65;

    error_printer(error_check(board, turn_counter, source, target));

    if (is_capture(turn_counter, source, target, board)) {
        chess_capture(source, target, board);
    }
    else {
        chess_move(source, target, board);
    }

    if (target[0] == 0 || target[0] == 7) {
        upgrade_tower(target, board);
    }

    return turn_counter + 1;
}

// function used to display info of turn
void turn_starter(board_t board, int turn, char move[], int indicator) {
    //print start sentence
    printf(START_LINE);
    if (indicator) {
        printf("*** ");
    }
    if (is_white_turn(turn)) {
        printf("WHITE ACTION #%d: %c%c%c%c%c\n", turn, move[0], move[1]
            , move[2], move[3], move[4]);
    }
    else {
        printf("BLACK ACTION #%d: %c%c%c%c%c\n", turn, move[0], move[1]
            , move[2], move[3], move[4]);
    }
}

// function used to compute next action
int next_action(board_t board, int turn) {
    int i, j;

    //declare and initialise the root
    node_t* root = malloc(sizeof(node_t));
    root->parent = NULL;
    root->children_counter = 0;

    //store current board into root
    for (i = 0;i < BOARD_SIZE;i++) {
        for (j = 0; j < BOARD_SIZE;j++) {
            root->board[i][j] = board[i][j];
        }
    }
    tree_builder(root, turn, 0);
    //check decision and print
    if (root->cost == INT_MAX || root->cost == INT_MIN) {
        if (is_white_turn(turn)) {
            printf("\nBLACK WIN!\n");
            exit(EXIT_SUCCESS);
        }
        else {
            printf("\nWHITE WIN!\n");
            exit(EXIT_SUCCESS);
        }
    }
    else {
        move_input(board, turn, root->decision);
    }
    turn_starter(board, turn, root->decision, COMP_INPUT);
    printf("BOARD COST: %d\n", cost_calculator(board));
    board_printer(board);
    tree_freer(root);

    return turn + 1;
}

// function used to compute next ten actions
void ten_action(board_t board, int turn) {
    int i;
    int turn_counter = turn;
    for (i = 0;i < COMP_ACTIONS;i++) {
        turn_counter = next_action(board, turn_counter);
    }
    printf("\n");
    exit(EXIT_SUCCESS);
}

// function used to print error message
void error_printer(int code) {
    if (code == 1) {
        printf("\nERROR: Source cell is outside of the board.\n");
        exit(EXIT_ERROR);
    }
    if (code == 2) {
        printf("\nERROR: Target cell is outside of the board.\n");
        exit(EXIT_ERROR);
    }
    if (code == 3) {
        printf("\nERROR: Source cell is empty.\n");
        exit(EXIT_ERROR);
    }
    if (code == 4) {
        printf("\nERROR: Target cell is not empty.\n");
        exit(EXIT_ERROR);
    }
    if (code == 5) {
        printf("\nERROR: Source cell holds opponent's piece/tower.\n");
        exit(EXIT_ERROR);
    }
    if (code == 6) {
        printf("\nERROR: Illegal action.\n");
        exit(EXIT_ERROR);
    }
}

//function used to check error
int error_check(board_t board, int turn, int source[], int target[]) {
    // Error 1
    if (source[0] < 0 || source[0] >7 || source[1] < 0 || source[1] >7) {
        return 1;
    }
    // Error 2
    if (target[0] < 0 || target[0] >7 || target[1] < 0 || target[1] >7) {
        return 2;
    }
    // Error 3
    if (board[source[0]][source[1]] == CELL_EMPTY) {
        return 3;
    }
    // Error 4
    if (board[target[0]][target[1]] != CELL_EMPTY) {
        return 4;
    }
    // Error 5
    if (is_white_turn(turn)) {
        // white turn but source is black
        if (board[source[0]][source[1]] == CELL_BPIECE
            || board[source[0]][source[1]] == CELL_BTOWER) {
            return 5;
        }
    }
    else {
        // black turn but source is white
        if (board[source[0]][source[1]] == CELL_WPIECE
            || board[source[0]][source[1]] == CELL_WTOWER) {
            return 5;
        }
    }
    //Error 6
    //illegal capture
    if ((abs(target[1] - source[1]) + abs(target[0] - source[0])) == 4
        && !is_capture(turn, source, target, board)) {
        return 6;
    }
    else if (!is_capture(turn, source, target, board)) {
        //illegal tower move
        if (board[source[0]][source[1]] == CELL_BTOWER
            || board[source[0]][source[1]] == CELL_WTOWER) {
            if ((abs(target[1] - source[1]) +
                abs(target[0] - source[0])) != 2) {
                return 6;
            }
        }
        //illegal white piece move
        else if (board[source[0]][source[1]] == CELL_WPIECE) {
            if (source[0] > target[0]
                || (abs(target[1] - source[1]) +
                    abs(target[0] - source[0])) != 2) {
                return 6;
            }
        }
        //illegal black piece move
        else if (board[source[0]][source[1]] == CELL_BPIECE) {
            if (source[0] < target[0]
                || (abs(target[1] - source[1]) +
                    abs(target[0] - source[0])) != 2) {
                return 6;
            }
        }
        //illegal capture
        else if ((abs(target[1] - source[1]) +
            abs(target[0] - source[0])) == 4
            && !is_capture(turn, source, target, board)) {
            return 6;
        }
    }
    return 0;
}

// used to check is turn of white
int is_white_turn(int turn) {
    if ((turn % 2) == 0) {
        return 1;
    }
    else {
        return 0;
    }
}

//function used to check can capture
int is_capture(int turn, int source[], int target[], board_t board) {
    // white capture black
    if (is_white_turn(turn)) {
        // there is a chess between
        if (board[source[0] + ((target[0] - source[0]) / 2)]
            [source[1] + ((target[1] - source[1]) / 2)] == CELL_BPIECE
            || board[source[0] + ((target[0] - source[0]) / 2)]
            [source[1] + ((target[1] - source[1]) / 2)] == CELL_BTOWER) {
            return 1;
        }
    }
    // black capture white
    else {
        if (board[source[0] + ((target[0] - source[0]) / 2)]
            [(source[1] + ((target[1] - source[1]) / 2))] == CELL_WPIECE
            || board[source[0] + ((target[0] - source[0]) / 2)]
            [source[1] + ((target[1] - source[1]) / 2)] == CELL_WTOWER) {
            return 1;
        }
    }
    return 0;
}

// function used to move chess
void chess_move(int source[], int target[], board_t board) {
    board[target[0]][target[1]] = board[source[0]][source[1]];
    board[source[0]][source[1]] = CELL_EMPTY;
}

// function used to capture
void chess_capture(int source[], int target[], board_t board) {
    board[target[0]][target[1]] = board[source[0]][source[1]];
    board[source[0]][source[1]] = CELL_EMPTY;
    board[source[0] + ((target[0] - source[0]) / 2)]
        [source[1] + ((target[1] - source[1]) / 2)] = CELL_EMPTY;
}

// function used to upgrade piece to tower
void upgrade_tower(int target[], board_t board) {
    if(board[target[0]][target[1]] == CELL_BPIECE) {
        board[target[0]][target[1]] = CELL_BTOWER;
    }
    else if(board[target[0]][target[1]] == CELL_WPIECE) {
        board[target[0]][target[1]] = CELL_WTOWER;
    }
}

//used to calculate cost and print cost
int cost_calculator(board_t board) {
    int i, j;
    int board_cost, wpiece = 0, wtower = 0, bpiece = 0, btower = 0;
    //count chess type and number
    for (i = 0; i < BOARD_SIZE; i++) {
        for (j = 0; j < BOARD_SIZE; j++) {
            if (board[i][j] == CELL_BPIECE) {
                bpiece += 1;
            }
            else if (board[i][j] == CELL_WPIECE) {
                wpiece += 1;
            }
            else if (board[i][j] == CELL_BTOWER) {
                btower += 1;
            }
            else if (board[i][j] == CELL_WTOWER) {
                wtower += 1;
            }
        }
    }

    //  calculate board cost: formula = b + 3B - w -3W
    board_cost = (bpiece * COST_PIECE) + (btower * COST_TOWER)
        - (wpiece * COST_PIECE) - (wtower * COST_TOWER);

    return board_cost;
}

//  used to check where the chess could move or capture
int move_checker(board_t board, int col, int row, int turn, char order[]) {
    int i;
    int source[2] = { row, col };
    int row_len[8] = { -2,-1, 2, 1, 2, 1, -2, -1 };
    int col_len[8] = { 2, 1, 2, 1,-2,-1, -2, -1 };
    int target[2] = { 0 };
    int counter = 0;

    //check move
    for (i = 0; i < MAX_ACTION;i++) {
        // checking clockwise from far top right
        target[0] = source[0] + row_len[i];
        target[1] = source[1] + col_len[i];
        // store the valid order
        if (!error_check(board, turn, source, target)) {
            order[0 + (5 * counter)] = source[1] + 65;
            order[1 + (5 * counter)] = source[0] + 49;
            order[2 + (5 * counter)] = '-';
            order[3 + (5 * counter)] = target[1] + 65;
            order[4 + (5 * counter)] = target[0] + 49;
            counter += 1;
        }
    }
    return counter;
}

//  function used to find all reachable board and make decision
void tree_builder(node_t* root, int turn, int depth) {
    int i, j, k;
    char order[5 * MAX_ACTION] = { 0 };
    int order_num;

    //  recursion checker
    if (depth == TREE_DEPTH) {
        return;
    }
    //  next depth is white
    if (is_white_turn(turn)) {
        for (i = 0; i < BOARD_SIZE;i++) {
            for (j = 0;j < BOARD_SIZE;j++) {
                if (root->board[i][j] == CELL_WPIECE
                    || root->board[i][j] == CELL_WTOWER) {
                    order_num = move_checker(root->board, j, i, turn, order);
                    for (k = 0; k < order_num; k++) {
                        node_t* new_node = node_maker(root, order, turn, k);
                        root->children[root->children_counter] = new_node;
                        root->children_counter += 1;
                    }
                }
            }
        }
    }
    //  next depth is black
    else {
        for (i = 0; i < BOARD_SIZE;i++) {
            for (j = 0;j < BOARD_SIZE;j++) {
                if (root->board[i][j] == CELL_BPIECE
                    || root->board[i][j] == CELL_BTOWER) {
                    order_num = move_checker(root->board, j, i, turn, order);
                    for (k = 0; k < order_num; k++) {
                        node_t* new_node = node_maker(root, order, turn, k);
                        root->children[root->children_counter] = new_node;
                        root->children_counter += 1;
                    }
                }
            }
        }
    }
    //  find the next depth
    for (i = 0;i < root->children_counter; i++) {
        tree_builder(root->children[i], turn + 1, depth + 1);
    }
    //  decision making
    //  next turn is white
    if (is_white_turn(turn)) {
        // max strategy
        if (depth == 1) {
            int cur_cost = INT_MIN;
            //save the max cost decision
            for (i = 0;i < root->children_counter;i++) {
                if (root->children[i]->cost > cur_cost) {
                    cur_cost = root->children[i]->cost;
                    for (j = 0;j < 5;j++) {
                        root->decision[j] = root->children[i]->order[j];
                    }
                }
            }
            root->cost = cur_cost;
        }
        // min strategy
        if (depth == 0 || depth == 2) {
            int cur_cost = INT_MAX;
            //save the max cost decision
            for (i = 0;i < root->children_counter;i++) {
                if (root->children[i]->cost < cur_cost) {
                    cur_cost = root->children[i]->cost;
                    for (j = 0;j < 5;j++) {
                        root->decision[j] = root->children[i]->order[j];
                    }
                }
            }
            root->cost = cur_cost;
        }
    }
    //  next turn is black
    else {
        // max strategy
        if (depth == 0 || depth == 2) {
            int cur_cost = INT_MIN;
            //save the max cost decision
            for (i = 0;i < root->children_counter;i++) {
                if (root->children[i]->cost > cur_cost) {
                    cur_cost = root->children[i]->cost;
                    for (j = 0;j < 5;j++) {
                        root->decision[j] = root->children[i]->order[j];
                    }
                }
            }
            root->cost = cur_cost;
        }
        // min strategy
        if (depth == 1) {
            int cur_cost = INT_MAX;
            //save the max cost decision
            for (i = 0;i < root->children_counter;i++) {
                if (root->children[i]->cost < cur_cost) {
                    cur_cost = root->children[i]->cost;
                    for (j = 0;j < 5;j++) {
                        root->decision[j] = root->children[i]->order[j];
                    }
                }
            }
            root->cost = cur_cost;
        }
    }
}

// function used to make a new node
node_t* node_maker(node_t* root, char order[], int turn, int i) {
    int j;
    node_t* new_node = malloc(sizeof(node_t));
    new_node->parent = root;
    for (j = 0;j < 5;j++) {
        new_node->order[j] = order[j + (i * 5)];
    }

    //store current board into new node
    for (i = 0;i < BOARD_SIZE;i++) {
        for (j = 0; j < BOARD_SIZE;j++) {
            new_node->board[i][j] = root->board[i][j];
        }
    }
    //move chess with order and calculate cost
    move_input(new_node->board, turn, new_node->order);
    new_node->cost = cost_calculator(new_node->board);
    new_node->children_counter = 0;

    return new_node;
}

// function used to free the tree
void tree_freer(node_t* root) {
    int i;

    if (!root->children_counter) {
        return;
    }
    // free the children tree
    for (i = 0; i < root->children_counter; i++) {
        tree_freer(root->children[i]);
    }
    free(root);
}
/*algorithms are fun!*/
/* THE END -------------------------------------------------------------------*/