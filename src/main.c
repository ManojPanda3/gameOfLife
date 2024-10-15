#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <time.h>
#include <unistd.h>

#define and &&
#define or ||
#define FPS 60.0f
typedef enum : char { false, true } bool;

struct termios orig_termios;
void getTerminalSize(int *rows, int *cols);
void copyArrTo(char **dest, char **src, int rows, int cols);
int COL, ROW;

int main(void) {
  // Get terminal size
  getTerminalSize(&ROW, &COL);

  // Allocate 2D arrays dynamically
  char **board = (char **)malloc(ROW * sizeof(char *));
  char **future = (char **)malloc(ROW * sizeof(char *));
  for (int i = 0; i < ROW; i++) {
    board[i] = (char *)calloc(COL, sizeof(char));
    future[i] = (char *)calloc(COL, sizeof(char));
  }

  board[1][2] = 1;
  board[2][3] = 1;
  board[3][1] = 1;
  board[3][2] = 1;
  board[3][3] = 1;

  // seting up 1/FPS so the animation dont excede certain fps
  struct timespec ts = {0, (long)(1e9f / FPS)};

  // the string len to draw
  size_t bufferSize = (ROW * (COL * 3 + 1)) + 1;
  // the string
  char *frameBuffer = (char *)malloc(bufferSize);
  if (!frameBuffer) {
    perror("Failed to allocate frame buffer");
    return 1;
  }

  // the game/animation loop
  while (true) {
    // loop through the row and col to check wether its gonna alive or not
    for (int r = 0; r < ROW; r++) {
      for (int c = 0; c < COL; c++) {
        char aliveCell = 0;

        // check neighbour cells are alive or not
        for (int i = (r > 0 ? r - 1 : r); i <= (r < ROW - 1 ? r + 1 : r); i++) {
          for (int j = (c > 0 ? c - 1 : c); j <= (c < COL - 1 ? c + 1 : c);
               j++) {
            aliveCell += board[i][j];
          }
        }
        aliveCell -= board[r][c];

        // if the cell is alive and dont have [2,3] amount of neighbour alive
        // cells
        if (board[r][c] and (aliveCell < 2 or aliveCell > 3)) {
          future[r][c] = 0;
        }
        // if the cell is dead and have 3 alive neighbour cell
        else if (!board[r][c] and aliveCell == 3) {
          future[r][c] = 1;
        } else {
          future[r][c] = board[r][c];
        }
      }
    }

    // copy the future to board so the future can become current
    copyArrTo(board, future, ROW, COL);

    size_t index = 0;
    // update the string to draw the animation
    for (int r = 0; r < ROW; r++) {
      for (int c = 0; c < COL; c++) {
        if (board[r][c]) {
          memcpy(&frameBuffer[index], "█", 3); // Copy '█' in UTF-8
          index += 3;
        } else {
          frameBuffer[index++] = ' ';
        }
      }
      frameBuffer[index++] = '\n';
    }
    frameBuffer[index] = '\0';

    // clear the terminal
    printf("\033[2J \033[H");
    // print the frame
    printf("%s", frameBuffer);
    fflush(stdout); // Ensure everything is printed immediately

    nanosleep(&ts, NULL);
  }

  // Free allocated memory
  for (int i = 0; i < ROW; i++) {
    free(board[i]);
    free(future[i]);
  }
  free(board);
  free(future);
  free(frameBuffer);
  return 0;
}

void getTerminalSize(int *rows, int *cols) {
  struct winsize w;
  if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) == -1) {
    perror("ioctl");
    exit(EXIT_FAILURE);
  }
  *rows = w.ws_row;
  *cols = w.ws_col;
}

void copyArrTo(char **dest, char **src, int rows, int cols) {
  for (int i = 0; i < rows; i++) {
    memcpy(dest[i], src[i], cols * sizeof(char));
  }
}
