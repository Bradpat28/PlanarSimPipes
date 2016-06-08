#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "Report.h"

#define WRITE_MAX 5
#define MAX_VAL 30

typedef struct Cell {
   int numRuns;
   int fixedVal;
   double avg;
   int id;
   int writeTo[WRITE_MAX];
   int readTo;
   int lastWrite;
} Cell;

void SetupCell(Cell *cell, char **argv, int argc);
void InitCell(Cell *cell);
void PrintCell(Cell *cell);
void ExecuteCell(Cell *cell);
Report BuildReport(Cell *cell, int step);
void InitBufferCount(Cell *cell, int *bufCount);
void CheckCell(Cell *cell);
int bufferCheck(int *bufferCount, Report buffer[][4], int numTimes);
