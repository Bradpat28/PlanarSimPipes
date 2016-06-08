#include "planarSim.h"

int main(int argc, char **argv) {
   PlanarSim sim;   
   InitSim(&sim);
   SetupSim(&sim, argv, argc);
   CheckSim(&sim);
   ExeSim(&sim);
   return EXIT_SUCCESS; 
}

void InitSim(PlanarSim *sim) {
   sim->numRuns = -1;
   sim->gridNum = -1;

   sim->fixedU = 0;
   sim->fixedD = 0;
   sim->fixedL = 0;
   sim->fixedR = 0;

   sim->avgU = 0.0;
   sim->avgD = 0.0;
   sim->avgL = 0.0;
   sim->avgR = 0.0;
}

void PrintSim(PlanarSim *sim) {
   printf("S: %d G: %d\nFU: %d ", sim->numRuns, sim->gridNum, sim->fixedU);
   printf("FD: %d FL: %d FR: %d\n", sim->fixedD, sim->fixedL, sim->fixedR);
   printf("AU: %f AD: %f AL: %f AR: %f\n", sim->avgU, sim->avgD, sim->avgL, sim->avgR);
}

void SetupSim(PlanarSim *sim, char **argv, int argc) {
   int ndx;
   char testChar;
   int val1;
   double val2;

   for (ndx = 1; ndx < argc; ndx++) {
      testChar = *(argv[ndx]);
      switch (testChar) {
      case 'G':
         val1 = atoi(argv[ndx] + 1);
         if (val1 > 0 && val1 <= MAX_GRID) {
            if (sim->gridNum == -1) {
               sim->gridNum = val1;
            }
         }
         break;
      
      case 'S':
         val1 = atoi(argv[ndx] + 1);
         if (val1 >= 0 && sim->numRuns == -1) {
            sim->numRuns = val1;
         }
         break;
      
      case 'U':
         val2 = atof(argv[ndx] + 1);
         if (!sim->fixedU) {
            sim->fixedU = 1;
            sim->avgU = val2;
         }   
         break;
      
      case 'D':
         val2 = atof(argv[ndx] + 1);
         if (!sim->fixedD) {
            sim->fixedD = 1;
            sim->avgD = val2;
         }
      break;

      case 'L':
         val2 = atof(argv[ndx] + 1);
         if (!sim->fixedL) {
            sim->fixedL = 1;
            sim->avgL = val2;
         }
         break;

      case 'R':
         val2 = atof(argv[ndx] + 1);
         if (!sim->fixedR) {
            sim->fixedR = 1;
            sim->avgR = val2;
         }
         break;
      default:
         ;
      }
   }
}

void CheckSim(PlanarSim *sim) {
   if (sim->numRuns == -1 || sim->gridNum == -1) {
      fprintf(stderr, "Usage: ./PlanarSim G S U D L R (in any order)\n");
      exit(EXIT_FAILURE);
   }
   if (sim->numRuns > 150) {
      fprintf(stderr, "Usage: ./PlanarSim G S U D L R (in any order)\n");
      exit(EXIT_FAILURE);
   }
}

int **BuildFdArr(PlanarSim *sim) {
   int numFd = sim->gridNum * sim->gridNum + 1;
   int **fdArr = (int **) calloc(numFd, sizeof(int *));
   int ndx;
   //int count = 3;
   
   for (ndx = 0; ndx < numFd; ndx++) {
      fdArr[ndx] = (int *) calloc(2, sizeof(int));
      fdArr[ndx][0] = -1;
      fdArr[ndx][1] = -1;
      /*fdArr[ndx][0] = count++;
      fdArr[ndx][1] = count++; i
      printf("NDX: %d, fd0: %d, fd1: %d\n", ndx, fdArr[ndx][0], fdArr[ndx][1]);*/
   }
   return fdArr;
}

int ****BuildSimArr(PlanarSim *sim, int **fdArr) {
   int ****simArr;

   simArr = InitSimArr(sim->gridNum);
   FillSimArr(sim, simArr, fdArr);     
   //PrintSimArr(sim, simArr);
   return simArr;
}

int ****InitSimArr(int gridNum) {
   int ****retArr = NULL;
   int ndx1, ndx2, ndx3;
   
   retArr = (int ****) calloc(gridNum + 2, sizeof(int ***));
   for (ndx1 = 0; ndx1 < gridNum + 2; ndx1++) {
      retArr[ndx1] = (int ***) calloc(gridNum + 2, sizeof(int **));
      for (ndx2 = 0; ndx2 < gridNum + 2; ndx2++) {
         retArr[ndx1][ndx2] = calloc(2, sizeof(int *));
         retArr[ndx1][ndx2][INFD] = calloc(MAXINFD, sizeof(int));
         retArr[ndx1][ndx2][OUTFD] = calloc(MAXOUTFD, sizeof(int));
         for (ndx3 = 0; ndx3 < MAXINFD; ndx3++) {
            retArr[ndx1][ndx2][INFD][ndx3] = -1;
         } 
         for (ndx3 = 0; ndx3 < MAXOUTFD; ndx3++) {
            retArr[ndx1][ndx2][OUTFD][ndx3] = -1;
         }
      }
   }
   return retArr;
}

void FillSimArr(PlanarSim *sim, int ****simArr, int **fdArr) {
   int ndx1;
   int ndx2;
   int botFixed = (sim->gridNum - 1) * sim->gridNum;
   int lFixed = 1 - sim->gridNum;
   int inFdCount = 1;
   int fdAbove = -1 - sim->gridNum;
   int fdBelow = -1 + sim->gridNum;
   
   for (ndx1 = 0; ndx1 < sim->gridNum + 2; ndx1++) {
      for (ndx2 = 0; ndx2 < sim->gridNum + 2; ndx2++) {
         if (!CheckCorner(sim, ndx1, ndx2)){
            //Top of the grid
            if (ndx1 == 0) {
               simArr[ndx1][ndx2][OUTFD][0] = fdArr[0][1];
               simArr[ndx1][ndx2][OUTFD][1] = fdArr[ndx2][1];  
            }
            //Bottom of the grid
            else if (ndx1 == sim->gridNum + 1) {
               simArr[ndx1][ndx2][OUTFD][0] = fdArr[0][1];
               simArr[ndx1][ndx2][OUTFD][1] = fdArr[ndx2 + botFixed][1];
            }
            //Left side of the grid
            else if (ndx2 == 0) {
               simArr[ndx1][ndx2][OUTFD][0] = fdArr[0][1];
               simArr[ndx1][ndx2][OUTFD][1] = fdArr[ndx1 * sim->gridNum + lFixed][1];
            }
            //Right side of the grid
            else if (ndx2 == sim->gridNum + 1) {
               simArr[ndx1][ndx2][OUTFD][0] = fdArr[0][1];
               simArr[ndx1][ndx2][OUTFD][1] = fdArr[ndx1 * sim->gridNum][1];
            }
            //Non-Edge and Non-Corner
            else {
               //Setup In FD's
               simArr[ndx1][ndx2][INFD][0] = fdArr[inFdCount++][0];

               //Setup Out FD's
               simArr[ndx1][ndx2][OUTFD][0] = fdArr[0][1];
               //Next to L edge
               if (ndx2 == 1) {
                  //Grid 1 case
                  if (ndx2 == sim->gridNum) {
                     //No more outFd's
                  }
                  //Next to Top edge
                  else if (ndx1 == 1) {
                     simArr[ndx1][ndx2][OUTFD][1] = fdArr[inFdCount][1];
                     simArr[ndx1][ndx2][OUTFD][2] = fdArr[inFdCount + fdBelow][1];
                  }
                  //Next to Bottom
                  else if (ndx1 == sim->gridNum) {
                     simArr[ndx1][ndx2][OUTFD][1] = fdArr[inFdCount][1];
                     simArr[ndx1][ndx2][OUTFD][2] = fdArr[inFdCount + fdAbove][1];
                  }
                  //Not Next to top or bottom
                  else {
                     simArr[ndx1][ndx2][OUTFD][1] = fdArr[inFdCount][1];
                     simArr[ndx1][ndx2][OUTFD][2] = fdArr[inFdCount + fdAbove][1];
                     simArr[ndx1][ndx2][OUTFD][3] = fdArr[inFdCount + fdBelow][1];
                  }
               }
               //Next to R edge
               else if (ndx2 == sim->gridNum) {
                  //Next to top edge
                  if (ndx1 == 1) {
                     simArr[ndx1][ndx2][OUTFD][1] = fdArr[inFdCount + fdBelow][1];
                     simArr[ndx1][ndx2][OUTFD][2] = fdArr[inFdCount - 2][1];
                  }
                  //Next to Bottom Edge
                  else if (ndx1 == sim->gridNum) {
                     simArr[ndx1][ndx2][OUTFD][1] = fdArr[inFdCount + fdAbove][1];
                     simArr[ndx1][ndx2][OUTFD][2] = fdArr[inFdCount - 2][1];
                  }
                  //Not Next to top or bottom
                  else {
                     simArr[ndx1][ndx2][OUTFD][1] = fdArr[inFdCount + fdAbove][1];
                     simArr[ndx1][ndx2][OUTFD][2] = fdArr[inFdCount + fdBelow][1];
                     simArr[ndx1][ndx2][OUTFD][3] = fdArr[inFdCount - 2][1];
                  }
               }
               //Not Next to edge
               else {
                  //Next to top
                  if (ndx1 == 1) {
                     simArr[ndx1][ndx2][OUTFD][1] = fdArr[inFdCount + fdBelow][1];
                     simArr[ndx1][ndx2][OUTFD][2] = fdArr[inFdCount][1];
                     simArr[ndx1][ndx2][OUTFD][3] = fdArr[inFdCount - 2][1];
                  }
                  //Next to bottom
                  else if (ndx1 == sim->gridNum) {
                     simArr[ndx1][ndx2][OUTFD][1] = fdArr[inFdCount + fdAbove][1];
                     simArr[ndx1][ndx2][OUTFD][2] = fdArr[inFdCount][1];
                     simArr[ndx1][ndx2][OUTFD][3] = fdArr[inFdCount - 2][1];
                  }
                  //Not next to bottom or top
                  else {
                     simArr[ndx1][ndx2][OUTFD][1] = fdArr[inFdCount + fdAbove][1];
                     simArr[ndx1][ndx2][OUTFD][2] = fdArr[inFdCount + fdBelow][1];
                     simArr[ndx1][ndx2][OUTFD][3] = fdArr[inFdCount][1];
                     simArr[ndx1][ndx2][OUTFD][4] = fdArr[inFdCount - 2][1];
                  } 
               } 
            }
         }
      } 
   }
}

int CheckCorner(PlanarSim *sim, int x, int y) {
   int ret = 0;
   int max = sim->gridNum + 1;

   if (x == 0 && y == 0)
      ret = 1;
   if (x == 0 && y == max)
      ret = 1;
   if (x == max && y == 0)
      ret = 1;
   if (x == max && y == max)
      ret = 1;
   return ret;
}

void PrintSimArr(PlanarSim *sim, int ****fdArr) {
   int ndx1, ndx2, ndx3;

   for (ndx1 = 0; ndx1 < sim->gridNum + 2; ndx1++) {
      for (ndx2 = 0; ndx2 < sim->gridNum + 2; ndx2++) {
         printf("Row %d, Col %d :\n", ndx1, ndx2);
         for (ndx3 = 0; ndx3 < MAXINFD; ndx3++) {
            if (fdArr[ndx1][ndx2][INFD][ndx3] != -1)
               printf("    IN FD %d : %d\n", ndx3, fdArr[ndx1][ndx2][INFD][ndx3]);
         } 
         for (ndx3 = 0; ndx3 < MAXOUTFD; ndx3++) {
            if (fdArr[ndx1][ndx2][OUTFD][ndx3] != -1)
               printf("    OUT FD %d : %d\n", ndx3, fdArr[ndx1][ndx2][OUTFD][ndx3]);
         }
         printf("\n");
      }
   }
}

void ExeSim(PlanarSim *sim) {
   int ndx1, ndx2;
   int pipeCount = 0;
   int ****simArr = NULL;
   int **fdArr = NULL;
   int childPIDs[(sim->gridNum + 2) * (sim->gridNum + 2) - 4];
   int childCount = 0;
   int id = -1;
   int wait;
   int error;

   fdArr = BuildFdArr(sim);
   //Setup pipes, and edge cels
   if (pipe(fdArr[pipeCount++])) {
      perror("PIPE Error: ");
   }
   for (ndx1 = 0; ndx1 < sim->gridNum + 2; ndx1++) {
      for (ndx2 = 0; ndx2 < sim->gridNum + 2; ndx2++) {
         if (!CheckCorner(sim, ndx1, ndx2)) {
            id++;
            //If not the last row
            if (ndx1 < sim->gridNum) {
               //if in middle
               if (ndx2 > 0 && ndx2 < sim->gridNum + 1) {
                  if (pipe(fdArr[pipeCount++])) {
                     perror("PIPE Error2: ");
                  }
               }
            }
            simArr = BuildSimArr(sim, fdArr);
            if ((childPIDs[childCount++] = fork()) < 0) {
               fprintf(stderr, "Bad Fork Call!");
               exit(EXIT_FAILURE);
            }
            //Parent
            else if (childPIDs[childCount - 1] > 0) {
               free(simArr);
            }
            //Child
            else {
               CloseFd(sim, simArr, fdArr, ndx1, ndx2);
               execv("./Cell", MakeArgv(sim, simArr, ndx1, ndx2, id));
            }
         } 
      }
   }
   for (ndx1 = 1; ndx1 < sim->gridNum * sim->gridNum + 1; ndx1++) {
      for (ndx2 = 0; ndx2 < 2; ndx2++) {
         close(fdArr[ndx1][ndx2]);
      }
   }
   close(fdArr[0][1]);
   wait = 0;
   while (wait < childCount) {
      waitpid(childPIDs[wait++], &error, 0);
      if (!WIFEXITED(error)) {
         printf("CHILD %d EXIT: %d\n", wait - 1, WEXITSTATUS(error));
      }
   }
   ReadAllReports(sim, fdArr);
}

void CloseFd(PlanarSim *sim, int ****simArr, int **fdArr, int x, int y) {
   int count1, count2, count3;
   int numFD = sim->gridNum * sim->gridNum + 1;
   int open = 0;

   for (count1 = 0; count1 < numFD; count1++) {
      for (count2 = 0; count2 < 2; count2++) {

         if (fdArr[count1][count2] != -1) {
            for (count3 = 0; count3 < MAXOUTFD; count3++) {
               if (simArr[x][y][OUTFD][count3] == fdArr[count1][count2])
                  open = 1;
            }
            if (simArr[x][y][INFD][0] == fdArr[count1][count2])
               open = 1;
            if (!open) {
               if (close(fdArr[count1][count2])) {
                  perror("ERROR Close: ");      
               }
            }
            open = 0;
         }
      }
   }
}

void ReadAllReports(PlanarSim *sim, int **fdArr) {
   int fd = fdArr[0][0];
   Report **repArr;
   int grid = (sim->gridNum + 2) * (sim->gridNum + 2);
   int ndx1, ndx2, ndx3, count, val; 
   Report buffer;

   repArr = (Report **) calloc(sim->numRuns + 1, sizeof(Report *));
   for (ndx1 =  0; ndx1 < sim->numRuns + 1; ndx1++) {
      repArr[ndx1] = (Report *) calloc(grid, sizeof(Report));
   }

   while ((val = read(fd, &buffer, sizeof(Report)))) {
      if (val < 0) {
         perror("Error: ");
      } 
      repArr[buffer.step][buffer.id] = buffer;
   }
   
   printf("-------------Planar Sim-------------\n");
   printf("Grid: %dx%d       Simulations: %d\n", sim->gridNum + 2,
     sim->gridNum + 2, sim->numRuns);
   printf("------------------------------------\n");
   printf("Top Row: %.3f Bottom Row: %.3f\nLeft Row: %.3f Right Row: %.3f\n", 
     sim->avgU, sim->avgD, sim->avgL, sim->avgR);
   for (ndx1 = 0; ndx1 < sim->numRuns + 1; ndx1++) {
      count = 0;
      printf("------------------------------------\n");
      printf("Simulation: %d/%d Total Cells: %d\n", ndx1, sim->numRuns, grid);
      printf("------------------------------------\n");
      for (ndx2 = 0; ndx2 < sim->gridNum + 2; ndx2++) {
         for (ndx3 = 0; ndx3 < sim->gridNum + 2; ndx3++) {
            if (!CheckCorner(sim, ndx2, ndx3)) {
               if (ndx3 == sim->gridNum)
                  printf("%*.3f", 15, repArr[ndx1][count++].value);
               else
                  printf("%*.3f", 15, repArr[ndx1][count++].value);
            }
            else {
               if (ndx3 == 0)
                  printf("%*s", 15, "X");
               else
                  printf("%*s", 15, "X");
            }
         }
         printf("\n");
         if (ndx2 != sim->gridNum + 1)
            printf("\n");
      }
   }
}

char **MakeArgv(PlanarSim *sim, int ****simArr, int x, int y, int id) {
   char **argv = (char **) calloc(NUMARGS, sizeof(char *));
   int ndx, count;

   for (ndx = 0; ndx < NUMARGS; ndx++) {
      argv[ndx] = (char *) calloc(CMD_ARG_LEN, sizeof(char));
   }

   ndx = 0;

   sprintf(argv[ndx++], "./Cell");
   sprintf(argv[ndx++], "S%d ", sim->numRuns);
   sprintf(argv[ndx++], "D%d ", id);
   
   //Top Edge 
   if (x == 0) {
      sprintf(argv[ndx++], "V%f ", sim->avgU);
   }
   //Bottom Edge
   else if (x == sim->gridNum + 1) {
      sprintf(argv[ndx++], "V%f ", sim->avgD);
   } 
   //Left Edge
   else if (y == 0) {
      sprintf(argv[ndx++], "V%f ", sim->avgL);
   }
   //Right Edge
   else if (y == sim->gridNum + 1) {
      sprintf(argv[ndx++], "V%f ", sim->avgR);
   } 
   sprintf(argv[ndx++], "I%d ", simArr[x][y][INFD][0]);
   for (count = 0; count < MAXOUTFD; count++) {
      sprintf(argv[ndx++], "O%d ", simArr[x][y][OUTFD][count]);
   }
   argv[ndx] = NULL;
   return argv;
}

int CheckEdge (PlanarSim *sim, int x, int y) {
   int ret = 0;
 
   if (x == 0)
      ret = 1;
   else if (x == sim->gridNum + 1)
      ret = 1;
   else if (y == 0)
      ret = 1;
   else if (y == sim->gridNum + 1)
      ret = 1;
   return ret;
}


