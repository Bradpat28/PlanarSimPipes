#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include "Report.h"

#define MAX_GRID 3 
#define MAXINFD 1
#define MAXOUTFD 5
#define INFD 0
#define OUTFD 1
#define NUMARGS 11
#define CMD_ARG_LEN 25

typedef struct PlanarSim {
   int numRuns;
   int gridNum;
   int fixedU;
   int fixedD;
   int fixedL;
   int fixedR;
   double avgU;
   double avgD;
   double avgL;
   double avgR;
} PlanarSim;

void InitSim(PlanarSim *sim);
void PrintSim(PlanarSim *sim);
void SetupSim(PlanarSim *sim, char **argv, int argc);
void CheckSim(PlanarSim *sim);
int **BuildFdArr(PlanarSim *sim);
int ****BuildSimArr(PlanarSim *sim, int **fdArr);
int ****InitSimArr(int gridNum);
void FillSimArr(PlanarSim *sim, int ****simArr, int **fdArr);
int CheckCorner(PlanarSim *sim, int x, int y);
void PrintSimArr(PlanarSim *sim, int ****fdArr);
void ExeSim(PlanarSim *sim);
void CloseFd(PlanarSim *sim, int ****simArr, int **fdArr, int x, int y);
char **MakeArgv(PlanarSim *sim, int ****simArr, int x, int y, int id);
int CheckEdge(PlanarSim *sim, int x, int y);
void ReadAllReports(PlanarSim *sim, int **fdArr);
