/******************************************************************************
*
* File name: connect4_functions.c
*
* Author:  Zihao Zhou
*          A13935712
*          ziz244@ucsd.edu
*
******************************************************************************/

/*-----------------------------------------------------------------------------
   Include files
-----------------------------------------------------------------------------*/
#include "connect4_functions.h"

/*=============================================================================
  Forward function declarations
=============================================================================*/

static int simulated_game(int board_after_move[][BOARD_SIZE_VERT],
  int computer_num);
static int best_single_move(int board[][BOARD_SIZE_VERT],
  int computer_num, int ran);
static int analyze(int board[][BOARD_SIZE_VERT], int height[BOARD_SIZE_HORIZ],
  int cri_col[20], int cri_row[20], int cri_pla[20], int my_num);
static int find_horiz3(int board[][BOARD_SIZE_VERT],
  int* player, int* row_num);
static int find_vert3(int board[][BOARD_SIZE_VERT],
  int* player, int* row_num);
static int find_down_diag3(int board[][BOARD_SIZE_VERT],
  int* player, int* row_num);
static int find_up_diag3(int board[][BOARD_SIZE_VERT],
  int* player, int* row_num);
static void rotate(int arr[4], int new_num);
static int check3(int arr[4], int* player);
static void copy(int copied[][BOARD_SIZE_VERT], int copy[][BOARD_SIZE_VERT]);

/*@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

                Function Definitions of Required Functions

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@*/


/******************************************************************************
*                                                                             *
*                       Function print_welcome()                              *
*                                                                             *
******************************************************************************/
int print_welcome(void) {
  srand(time(0));
  char c;
  puts("*** Welcome to the Connect-Four game!!! ***");
  printf("Would you like to make the first move [y/n]: ");
  if((c = getchar()) == 'n' || c == 'N') {
    while(getchar() != '\n');
    putchar('\n');
    return 2;
  }
  else if(c == '\n') return 1;
  else {
    while(getchar() != '\n');
    putchar('\n');
    return 1;
  }
}

/******************************************************************************
*                                                                             *
*                      Function display_board()                               *
*                                                                             *
******************************************************************************/
void display_board(int board[][BOARD_SIZE_VERT]) {
  int i, j;
  putchar('\n');
  for(j = 0; j < BOARD_SIZE_VERT; j++) {
    for(i = 0; i < BOARD_SIZE_HORIZ; i++) printf("+---");
    puts("+");
    for(i = 0; i < BOARD_SIZE_HORIZ; i++)
      printf("| %c ", (board[i][j] == 0)?' ':(board[i][j] == 1)?'X':'O');
    puts("|");
  }
  for(i = 0; i < BOARD_SIZE_HORIZ; i++) printf("+---");
  puts("+");
  for(i = 1; i <= BOARD_SIZE_HORIZ; i++) printf("  %d ", i);
  puts("\n");
}

/******************************************************************************
*                                                                             *
*                      Function random_move()                                 *
*                                                                             *
******************************************************************************/
int random_move(int board[][BOARD_SIZE_VERT], int computer_num) {
  int m;
  do m = rand() % BOARD_SIZE_HORIZ + 1; while(is_column_full(board, m));
  update_board(board, m, computer_num);
  return m;
}

/******************************************************************************
*                                                                             *
*                      Function player_move()                                 *
*                                                                             *
******************************************************************************/
int player_move(int board[][BOARD_SIZE_VERT], int player_num) {
  int m;
  while(1) {
    printf("Please enter your move: ");
    if(!scanf("%d", &m)) puts("Not a valid move. Enter a column number!");
    else if(m < 1 || m > BOARD_SIZE_HORIZ)
      puts("Not a valid move. Enter a column number!");
    else if(is_column_full(board, m)) puts("This column is full. Try again!.");
    else {
      while(getchar() != '\n');
      break;
    }
    while(getchar() != '\n');
  }
  update_board(board, m, player_num);
  return m;
}

/******************************************************************************
*                                                                             *
*                      Function check_win_or_tie()                            *
*                                                                             *
******************************************************************************/
bool check_win_or_tie(int board[][BOARD_SIZE_VERT], int last_move) {
  int winner = check_winner(board, last_move), i = 0, flag = 0;
  if(winner == 0) {
    while(i++ < BOARD_SIZE_HORIZ) if(!is_column_full(board, i)) flag = 1;
    if(flag) return false;
    else {
      puts("*****************************");
      puts("* Game is a tie!! No winner *");
      puts("*****************************");
      return true;
    }
  }
  puts("*****************************");
  if(winner == 1)
    puts("* Player X won!!! Game over *");
  else if(winner == 2)
    puts("* Player O won!!! Game over *");
  puts("*****************************");
  return true;
}

/******************************************************************************
*                                                                             *
*                     Function is_column_full()                               *
*                                                                             *
******************************************************************************/
bool is_column_full(int board[][BOARD_SIZE_VERT], int m) {
  if(board[m - 1][0] == 0) return false;
  else return true;
}

/******************************************************************************
*                                                                             *
*                      Function update_board()                                *
*                                                                             *
******************************************************************************/
void update_board(int board[][BOARD_SIZE_VERT], int m, int player_num) {
  int row_num = BOARD_SIZE_VERT - 1;
  while(board[m - 1][row_num] != 0) row_num--;
  board[m - 1][row_num] = player_num;
}

/******************************************************************************
*                                                                             *
*                      Function check_winner()                                *
*                                                                             *
******************************************************************************/
int check_winner(int board[][BOARD_SIZE_VERT], int last_move) {
  int i, j, row_num = 0, player_num, flag = 1, count = 0;
  int lefti, leftj, righti, rightj;
  // Find row number of the last move
  if(!is_column_full(board, last_move--))
    while(board[last_move][row_num] == 0) row_num++;
  //printf("POS %d %d\n", last_move, row_num);
  player_num = board[last_move][row_num];
  // Check vertical
  if(row_num + 3 < BOARD_SIZE_VERT) {
    for(j = row_num + 1; j < row_num + 4; j++) {
      if(board[last_move][j] != player_num) flag = 0;
      //printf("VERTI %d %d\n", last_move, j);
    }
    if(flag) return player_num;
  }
  // Check horizontal
  for(i = (last_move > 3)?last_move - 3:0;\
      i < ((last_move + 4 < BOARD_SIZE_HORIZ)?last_move + 4:BOARD_SIZE_HORIZ);\
      i++) {
    //printf("HORI %d %d\n", i, row_num);
    if(board[i][row_num] != 0 && board[i][row_num] == player_num) count++;
    else count = 0;
    if(count == 4) return player_num;
  }
  // Check diagonal
  count = 0;
  righti = lefti = last_move;
  rightj = leftj = row_num;
  for(i = 0; i < 3 && lefti != 0 && leftj != 0; i++, lefti--, leftj--);
  for(i = 0; i < 3 && righti != BOARD_SIZE_HORIZ - 1 &&\
      rightj != BOARD_SIZE_VERT - 1; i++, righti++, rightj++);
  for(i = lefti, j = leftj; i <= righti && j <= rightj; i++, j++) {
    //printf("DIA %d %d\n", i, j);
    if(board[i][j] != 0 && board[i][j] == player_num) count++;
    else count = 0;
    if(count == 4) return player_num;
  }
  count = 0;
  righti = lefti = last_move;
  rightj = leftj = row_num;
  for(i = 0; i < 3 && lefti != 0 && leftj != BOARD_SIZE_VERT - 1;\
      i++, lefti--, leftj++);
  for(i = 0; i < 3 && righti != BOARD_SIZE_HORIZ - 1 && rightj != 0;\
      i++, righti++, rightj--);
  for(i = lefti, j = leftj; i <= righti && j >= rightj; i++, j--) {
    //printf("DIA %d %d\n", i, j);
    if(board[i][j] != 0 && board[i][j] == player_num) count++;
    else count = 0;
    if(count == 4) return player_num;
  }
  return 0;
}


/******************************************************************************
*                                                                             *
*                       Function best_move()                                  *
*                                                                             *
*******************************************************************************
*                                                                             *
*  This is the EXTRA-CREDIT function to be used for the student competition.  *
*                                                                             *
******************************************************************************/
int best_move(int board[][BOARD_SIZE_VERT], int computer_num) {
  static int first_run = 1;
  if(first_run) {
    puts("NOTICE: The ee15wcn best_move function does not work on the server");
    puts("        Please do the tournament on PC where it works well\n");
    first_run = 0;
  }

  int evaluation[BOARD_SIZE_HORIZ] = {0};
  int board_after_move[BOARD_SIZE_HORIZ][BOARD_SIZE_VERT] = {{0}};
  int i, j, max_evaluation = -400, max_index = 0;
  for(i = 0; i < BOARD_SIZE_HORIZ; i++) {
    evaluation[i] = 0;
    if(is_column_full(board, i + 1)) {
      evaluation[i] = -300;
      continue;
    }
    for(j = 0; j < 100; j++) {
      copy(board, board_after_move);
      update_board(board_after_move, i + 1, computer_num);
      //printf("GAME RESULT %d\n",
      //k = simulated_game(board_after_move, computer_num));
      evaluation[i] += simulated_game(board_after_move, computer_num);
    }
  }

  // Analyze the current position
  int height[BOARD_SIZE_HORIZ] = {0};
  int cri_col[20] = {0};
  int cri_row[20] = {0};
  int cri_pla[20] = {0};
  analyze(board, height, cri_col, cri_row, cri_pla, computer_num);

  for(i = 0; i < 20; i++) {
    if(cri_col[i] == -1) continue;
    // If player is me, then connect 4
    // If player is the enemy, then break 4
    if(cri_row[i] == height[cri_col[i]]) {
      update_board(board, cri_col[i] + 1, computer_num);
      return cri_col[i] + 1;
    }
    // If player is the enemy, then should not go there
    if(cri_row[i] == height[cri_col[i]] - 1 && cri_pla[i] != computer_num) {
      evaluation[cri_col[i]] = -200;
    }
  }

  for(i = 0; i < BOARD_SIZE_HORIZ; i++) {
    if(evaluation[i] > max_evaluation) {
      max_evaluation = evaluation[i];
      max_index = i;
    }
  }
  for(i = 0; i < BOARD_SIZE_HORIZ; i++)
    if(board[i][0] != 0) evaluation[i] = -300;
  //for(i = 0; i < BOARD_SIZE_HORIZ; i++) printf("  %d ", evaluation[i]);
  //putchar('\n');
  //printf("FINAL BEST MOVE %d \n", max_index + 1);
  update_board(board, max_index + 1, computer_num);
  return max_index + 1;
}

/******************************************************************************
*                                                                             *
*                      Function simulated_game()                              *
*                                                                             *
******************************************************************************/
static int simulated_game(int board_after_move[][BOARD_SIZE_VERT],
  int computer_num) {
  int human_num = (computer_num == 1)?2:1, i, flag, last_move, check;
  while(1) {
    last_move = best_single_move(board_after_move, human_num, rand());
    //display_board(board_after_move);
    //printf("last move %d\n", last_move);
    check = check_winner(board_after_move, last_move);
    //printf("check winner %d\n", check);
    if(check == computer_num) return 1;
    else if(check == human_num) return -1;
    else {
      flag = i = 0;
      while(i++ < BOARD_SIZE_HORIZ)
        if(!is_column_full(board_after_move, i)) flag = 1;
      if(!flag) return 0;
    }
    last_move = best_single_move(board_after_move, computer_num, rand());
    //display_board(board_after_move);
    //printf("last move %d\n", last_move);
    check = check_winner(board_after_move, last_move);
    //printf("check winner %d\n", check);
    if(check == computer_num) return 1;
    else if(check == human_num) return -1;
    else {
      flag = i = 0;
      while(i++ < BOARD_SIZE_HORIZ)
        if(!is_column_full(board_after_move, i)) flag = 1;
      if(!flag) return 0;
    }
  }
}


/******************************************************************************
*                                                                             *
*                      Function best_single_move()                            *
*                                                                             *
******************************************************************************/
static int best_single_move(int board[][BOARD_SIZE_VERT],
  int computer_num, int ran) {
  int human_num = (computer_num == 1)?2:1, i, j;

  // Analyze the current position
  int height[BOARD_SIZE_HORIZ] = {0};
  int cri_col[20] = {0};
  int cri_row[20] = {0};
  int cri_pla[20] = {0};
  analyze(board, height, cri_col, cri_row, cri_pla, computer_num);
  //printf("Height: ");
  //for(i = 0; i < BOARD_SIZE_HORIZ; i++) printf("  %d ", height[i]);
  //putchar('\n');

  // Setting up evaluation
  int evaluation[BOARD_SIZE_HORIZ] = {0};
  for(i = 0; i < BOARD_SIZE_HORIZ; i++) {
    if(height[i] == -1) evaluation[i] = -10;
    else evaluation[i] = 0;
  }

  for(i = 0; i < 20; i++) {
    if(cri_col[i] == -1) continue;
    // If player is me, then connect 4
    // If player is the enemy, then break 4
    if(cri_row[i] == height[cri_col[i]]) {
      update_board(board, cri_col[i] + 1, computer_num);
      return cri_col[i] + 1;
    }
    // If player is the enemy, then should not go there
    if(cri_row[i] == height[cri_col[i]] - 1 && cri_pla[i] == human_num) {
      evaluation[cri_col[i]] = -10;
    }
  }

  int board_after_move[BOARD_SIZE_HORIZ][BOARD_SIZE_VERT] = {{0}};
  for(i = 0; i < BOARD_SIZE_HORIZ; i++) {
    if(evaluation[i] == -10) continue;
    copy(board, board_after_move);
    j = BOARD_SIZE_VERT - 1;
    while(board_after_move[i][j] != 0) j--;
    board_after_move[i][j] = computer_num;
    evaluation[i] = analyze(board_after_move, height, cri_col, cri_row,
                            cri_pla, computer_num);
  }
  int max_evaluation = evaluation[0];
  int max_index[BOARD_SIZE_HORIZ] = {0};
  int size;
  for(i = 0; i < BOARD_SIZE_HORIZ; i++) max_index[i] = -1;
  //printf("Evaluation: ");
  //for(i = 0; i < BOARD_SIZE_HORIZ; i++) printf("  %d ", evaluation[i]);
  //putchar('\n');
  for(i = 0; i < BOARD_SIZE_HORIZ; i++)
    if(evaluation[i] > max_evaluation) max_evaluation = evaluation[i];
  for(i = 0, j = 0; i < BOARD_SIZE_HORIZ; i++)
    if(evaluation[i] == max_evaluation) max_index[j++] = i;
  for(i = 0; max_index[i] != -1 && i < BOARD_SIZE_HORIZ; i++);
  size = i;
  int m = ran % size;
  //printf("random number is %d\n", ran);
  //for(i = 0; i < size; i++) printf("  %d ", max_index[i]);
  //putchar('\n');
  //printf("final result %d \n", max_index[m]);
  update_board(board, max_index[m] + 1, computer_num);
  return max_index[m] + 1;
}


/******************************************************************************
*                                                                             *
*                      Function analyze()                                     *
*                                                                             *
******************************************************************************/
static int analyze(int board[][BOARD_SIZE_VERT], int height[BOARD_SIZE_HORIZ],
  int cri_col[20], int cri_row[20], int cri_pla[20], int my_num) {
  int i, row_num, player = 0, result = 0;
  int my3_count = 0, enemy3_count = 0;
  for(i = 0; i < BOARD_SIZE_HORIZ; i++) {
    if(board[i][0] != 0) {
      height[i] = -1;
      continue;
    }
    row_num = BOARD_SIZE_VERT - 1;
    while(board[i][row_num] != 0) row_num--;
    height[i] = row_num;
  }
  for(i = 0; i < 20; i++) cri_pla[i] = cri_row[i] = cri_col[i] = -1;
  i = row_num = 0;
  while(result != -1) {
    result = cri_col[i] = find_horiz3(board, &player, &row_num);
    cri_row[i] = row_num;
    cri_pla[i] = player;
    i++;
  }
  result = 0;
  while(result != -1) {
    result = cri_col[i] = find_vert3(board, &player, &row_num);
    cri_row[i] = row_num;
    cri_pla[i] = player;
    i++;
  }
  result = 0;
  while(result != -1) {
    result = cri_col[i] = find_up_diag3(board, &player, &row_num);
    cri_row[i] = row_num;
    cri_pla[i] = player;
    i++;
  }
  result = 0;
  while(result != -1) {
    result = cri_col[i] = find_down_diag3(board, &player, &row_num);
    cri_row[i] = row_num;
    cri_pla[i] = player;
    i++;
  }
  for(i = 0; i < 20; i++) {
    if(cri_col[i] == -1) continue;
    if(cri_pla[i] == my_num) my3_count++;
    else enemy3_count++;
  }
  for(i = 0; i < BOARD_SIZE_HORIZ; i++)
    if(board[i][0] != 0) height[i] = -1;
  return my3_count - enemy3_count;
}


/******************************************************************************
*                                                                             *
*                      Function find_horiz3()                                 *
*                                                                             *
******************************************************************************/
static int find_horiz3(int board[][BOARD_SIZE_VERT],
  int* player, int* row_num) {
  int i, j, count, zeropos;
  static int arr[4] = {0};
  static int endi = 0, endj = 0;
  if(endi == -1 && endj == -1) {
    endi = 0;
    endj = 0;
    for(count = 0; count < 4; count++) arr[count] = 0;
  }
  for(j = endj; j < BOARD_SIZE_VERT; j++) {
    for(i = (j == endj)?endi:0; i < BOARD_SIZE_HORIZ; i++) {
      rotate(arr, board[i][j]);
      //for(count = 0; count < 4; count++) printf("%d ", arr[count]);
      //putchar('\n');
      zeropos = check3(arr, &*player);
      if(zeropos != -1) {
        if(zeropos + i - 3 < 0) continue;
        endi = i + 1;
        endj = j;
        *row_num = j;
        //printf("endi&endj %d %d\n", endi, endj);
        return zeropos + i - 3;
      }
    }
    for(count = 0; count < 4; count++) arr[count] = 0;
  }
  endi = -1;
  endj = -1;
  return -1; // Return -1 if no 3 found
}


/******************************************************************************
*                                                                             *
*                      Function find_vert3()                                  *
*                                                                             *
******************************************************************************/
static int find_vert3(int board[][BOARD_SIZE_VERT],
  int* player, int* row_num) {
  int i, j, count;
  static int arr[4] = {0};
  static int endi = 0;
  if(endi == -1) {
    endi = 0;
    for(count = 0; count < 4; count++) arr[count] = 0;
  }
  for(i = endi; i < BOARD_SIZE_HORIZ; i++) {
    for(j = 0; j < BOARD_SIZE_VERT; j++) {
      rotate(arr, board[i][j]);
      //for(count = 0; count < 4; count++) printf("%d ", arr[count]);
      //putchar('\n');
      if(check3(arr, &*player) != -1) {
        endi = i + 1;
        if((*row_num = j - 3) < 0) continue;
        for(count = 0; count < 4; count++) arr[count] = 0;
        return i;
      }
    }
    for(count = 0; count < 4; count++) arr[count] = 0;
  }
  endi = -1;
  return -1;
}


/******************************************************************************
*                                                                             *
*                      Function find_down_diag3()                             *
*                                                                             *
******************************************************************************/
static int find_down_diag3(int board[][BOARD_SIZE_VERT],
  int* player, int* row_num) {
  int i, j, count, zeropos;
  static int arr[4] = {0};
  static int endi = 0, endj = BOARD_SIZE_VERT - 4;
  if(endi == -1 && endj == -1) {
    endi = 0;
    endj = BOARD_SIZE_VERT - 4;
    for(count = 0; count < 4; count++) arr[count] = 0;
  }

  if(endj - endi >= 0) {
    for(i = endi, j = endj; j >= 0; j = j - i - 1, i = 0) {
      for(; i < BOARD_SIZE_HORIZ && j < BOARD_SIZE_VERT; i++, j++) {
        rotate(arr, board[i][j]);
        //for(count = 0; count < 4; count++) printf("%d ", arr[count]);
        //putchar('\n');
        zeropos = check3(arr, &*player);
        if(zeropos != -1) {
          if(zeropos + i - 3 < 0) continue;
          if(i == BOARD_SIZE_VERT - 1 || j == BOARD_SIZE_HORIZ - 1) {
            endj = j - i - 1;
            endi = 0;
            for(count = 0; count < 4; count++) arr[count] = 0;
          }
          else {
            endi = i + 1;
            endj = j + 1;
          }
          *row_num = zeropos + j - 3;
          return zeropos + i - 3;
        }
      }
      for(count = 0; count < 4; count++) arr[count] = 0;
    }
    endi = 1;
    endj = 0;
  }
  if(endj - endi < 0) {
    for(i = endi, j = endj; (i - j + 1) <= BOARD_SIZE_HORIZ - 3;
        i = i - j + 1, j = 0) {
      for(; i < BOARD_SIZE_HORIZ && j < BOARD_SIZE_VERT; i++, j++) {
        rotate(arr, board[i][j]);
        //for(count = 0; count < 4; count++) printf("%d ", arr[count]);
        //putchar('\n');
        zeropos = check3(arr, &*player);
        if(zeropos != -1) {
          if(zeropos + j - 3 < 0) continue;
          if(i == BOARD_SIZE_VERT - 1 || j == BOARD_SIZE_HORIZ - 1) {
            endi = i - j + 1;
            endj = 0;
            for(count = 0; count < 4; count++) arr[count] = 0;
          }
          else {
            endi = i + 1;
            endj = j + 1;
          }
          *row_num = zeropos + j - 3;
          return zeropos + i - 3;
        }
      }
      for(count = 0; count < 4; count++) arr[count] = 0;
    }
  }
  endi = -1;
  endj = -1;
  return -1; // Return -1 if no 3 found
}


/******************************************************************************
*                                                                             *
*                      Function find_up_diag3()                               *
*                                                                             *
******************************************************************************/
static int find_up_diag3(int board[][BOARD_SIZE_VERT],
  int* player, int* row_num) {
  int i, j, count, zeropos;
  static int arr[4] = {0};
  static int endi = 0, endj = 3;
  if(endi == -1 && endj == -1) {
    endi = 0;
    endj = 3;
    for(count = 0; count < 4; count++) arr[count] = 0;
  }

  if(endj + endi < BOARD_SIZE_VERT) {
    for(i = endi, j = endj; (i + j) < BOARD_SIZE_VERT; j = i + j + 1, i = 0) {
      for(; i < BOARD_SIZE_HORIZ && j >= 0; i++, j--) {
        rotate(arr, board[i][j]);
        //for(count = 0; count < 4; count++) printf("%d ", arr[count]);
        //putchar('\n');
        zeropos = check3(arr, &*player);
        if(zeropos != -1) {
          if(zeropos + i - 3 < 0) continue;
          if(i == BOARD_SIZE_VERT - 1 || j == 0) {
            endj = i + j + 1;
            endi = 0;
            for(count = 0; count < 4; count++) arr[count] = 0;
          }
          else {
            endi = i + 1;
            endj = j - 1;
          }
          *row_num = j - zeropos + 3;
          return zeropos + i - 3;
        }
      }
      for(count = 0; count < 4; count++) arr[count] = 0;
    }
    endi = 1;
    endj = 5;
  }
  if(endj + endi >= BOARD_SIZE_VERT) {
    for(i = endi, j = endj; (i + j) < BOARD_SIZE_HORIZ + BOARD_SIZE_VERT - 4;
        i = i + j - BOARD_SIZE_VERT + 2, j = BOARD_SIZE_VERT - 1) {
      for(; i < BOARD_SIZE_HORIZ && j >= 0; i++, j--) {
        rotate(arr, board[i][j]);
        //for(count = 0; count < 4; count++) printf("%d ", arr[count]);
        //putchar('\n');
        zeropos = check3(arr, &*player);
        if(zeropos != -1) {
          if(zeropos + j - 3 <= 0) continue;
          if(i == BOARD_SIZE_VERT - 1 || j == 0) {
            endi = i + j - BOARD_SIZE_VERT + 2;
            endj = BOARD_SIZE_VERT - 1;
            //printf("endi %d endj %d\n", endi, endj);
            for(count = 0; count < 4; count++) arr[count] = 0;
          }
          else {
            endi = i + 1;
            endj = j - 1;
          }
          *row_num = j - zeropos + 3;
          //printf("row_num %d zeropos %d\n", *row_num, zeropos);
          return zeropos + i - 3;
        }
      }
      for(count = 0; count < 4; count++) arr[count] = 0;
    }
  }
  endi = -1;
  endj = -1;
  return -1; // Return -1 if no 3 found
}


/******************************************************************************
*                                                                             *
*                      Function rotate()                                      *
*                                                                             *
******************************************************************************/
static void rotate(int arr[4], int new_num) {
  arr[0] = arr[1];
  arr[1] = arr[2];
  arr[2] = arr[3];
  arr[3] = new_num;
}


/******************************************************************************
*                                                                             *
*                      Function check3()                                      *
*                                                                             *
******************************************************************************/
static int check3(int arr[4], int* player) {
  int i, Xcount = 0, Ocount = 0, zero_count = 0, zero_pos;
  for(i = 0; i < 4; i++)
    if(arr[i] == 0) {
      zero_count++;
      zero_pos = i;
    }
    else if(arr[i] == 1) Xcount++;
    else if(arr[i] == 2) Ocount++;
  if(zero_count == 1) {
    if(Xcount == 3) {
      *player = 1;
      return zero_pos;
    }
    else if(Ocount == 3) {
      *player = 2;
      return zero_pos;
    }
  }
  *player = 0;
  return -1;
}


/******************************************************************************
*                                                                             *
*                      Function copy()                                        *
*                                                                             *
******************************************************************************/
static void copy(int copied[][BOARD_SIZE_VERT], int copy[][BOARD_SIZE_VERT]) {
   int i, j;
   for(i = 0; i < BOARD_SIZE_HORIZ; i++)
     for(j = 0; j < BOARD_SIZE_VERT; j++)
       copy[i][j] = copied[i][j];
}


/*=============================================================================
  Function definitions of other functions
=============================================================================*/

/******************************************************************************
                                End of file
******************************************************************************/
