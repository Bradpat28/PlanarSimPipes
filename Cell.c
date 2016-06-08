#include "Cell.h"

int main(int argc, char **argv) {
   Cell cell;
   
   InitCell(&cell);
   SetupCell(&cell, argv, argc);
   CheckCell(&cell);
   //PrintCell(&cell);
   ExecuteCell(&cell);

   return 0;
}

void InitCell(Cell *cell) {
   int ndx;


   cell->numRuns = -1;
   cell->fixedVal = 0;
   cell->avg = 0;
   cell->id = 0;
   cell->lastWrite = 0;
   cell->readTo = -1;
   for (ndx = 0; ndx < WRITE_MAX; ndx++) {
      cell->writeTo[ndx] = -1;
   }
}

void PrintCell(Cell *cell) {
   int ndx;
   
   printf("Cell: (id) %d (numRuns) %d (fixedVal) %d (avg) %f\n",
    cell->id, cell->numRuns, cell->fixedVal, cell->avg);
   for (ndx = 0; ndx < WRITE_MAX; ndx++) {
      printf("Write FD %d: %d\n", ndx, cell->writeTo[ndx]);
   }
   printf("Read FD: %d\n", cell->readTo);
}

void SetupCell(Cell *cell, char **argv, int argc) {
   int ndx;
   char testC;
   int val;
   double val2;

   for (ndx = 1; ndx < argc; ndx++) {
      testC = *(argv[ndx]);
      switch (testC) {
      case 'S':
         val = atoi(argv[ndx] + 1);
         if (val >= 0) {
            if (cell->numRuns == -1) {
               cell->numRuns = val;
            }
         }
         break;
      case 'V':
         val2 = atof(argv[ndx] + 1);
         if (cell->fixedVal == 0) {
            cell->fixedVal = 1;
            cell->avg = val2;
         }
         break;
      case 'D':
         val = atoi(argv[ndx] + 1);
         
         if (val > 0) {
            cell->id = val;
         }
         break;

      case 'O':
         val = atoi(argv[ndx] + 1);
         if (val > 0 && (cell->lastWrite < WRITE_MAX) && val <= MAX_VAL) { 
            cell->writeTo[cell->lastWrite++] = val;
         }
         break;

      case 'I':
         val = atoi(argv[ndx] + 1);
         if (val > 0 && val <= MAX_VAL && cell->readTo == -1) {
            cell->readTo = val;
         }
         break;
      default:
         ;
      }
   }
}

void ExecuteCell(Cell *cell) {
   int numTimes;
   Report reportBuffer;
   Report buffer[cell->numRuns + 1][4];
   int bufferCount[cell->numRuns + 1];
   int numWrite;
   int flag;
   double newAvg = 0;
   Report sendReport;
   int val = 0;
  
   InitBufferCount(cell, bufferCount);
   for (numTimes = 0; numTimes <= cell->numRuns; numTimes++) {
      flag = 1;
      numWrite = cell->lastWrite;
      sendReport = BuildReport(cell, numTimes);
      if (numTimes == cell->numRuns) {
         if (write(cell->writeTo[0], &sendReport, sizeof(Report)) < 0) {
            perror("ERROR: ");
         }
      }
      else {
         while (numWrite--) {
            if (write(cell->writeTo[numWrite], &sendReport, sizeof(Report)) < 0) {
               perror("ERROR: ");
            }
            //printf("Cell %d wrote to %d on run %d\n", cell->id, cell->writeTo[numWrite], numTimes);
         } 
      }
      if (numTimes != cell->numRuns) {
         while (flag && cell->readTo != -1) {
            if ((val = read(cell->readTo, &reportBuffer, sizeof(Report))) < 0) {
               perror("ERROR: ");
            }
            if (val == 0) {
               flag = 0;
            }
            if (val != 0) {
               bufferCount[reportBuffer.step] += 1;
               buffer[reportBuffer.step][bufferCount[reportBuffer.step]] = reportBuffer;
               if (bufferCount[numTimes] == 3) {
                  flag = 0;
               }
               //printf("Cell %d Report %d, step: %d\n", cell->id, reportBuffer.id, reportBuffer.step);  
            }
            // printf("Cell %d, READ RETURN VAL %d\n", cell->id, val);
         } 
      }
      //printf("Cell %d, Run %d, avg %f\n", cell->id, numTimes, cell->avg);
      if (cell->fixedVal) {
         newAvg = cell->avg;
      }
      else {
         if (numTimes != cell->numRuns) {
            newAvg = (buffer[numTimes][0].value + buffer[numTimes][1].value + 
             buffer[numTimes][2].value + buffer[numTimes][3].value) / 4;
         }  
      }
      cell->avg = newAvg;
   }
   //printf("---CELL %d LAST WRITE %d \n", cell->id, cell->lastWrite);
   for (numWrite = 0; numWrite < cell->lastWrite; numWrite++) {
     // printf("----CEll %d closed %d\n", cell->id, cell->writeTo[numWrite]);
      close(cell->writeTo[numWrite]);
   }
   if (cell->readTo != -1) {
      while ((val = read(cell->readTo, &reportBuffer, sizeof(Report)))) {
        // printf("--Cell %d, val %d\n", cell->id, val);
      }
      close(cell->readTo);
      //printf("Cell %d CLOSED %d\n", cell->id, cell->readTo);
   }
}

Report BuildReport(Cell *cell, int step) {
   Report sendReport;

   sendReport.id = cell->id;
   sendReport.step = step;
   sendReport.value = cell->avg;

   return sendReport;
}

void InitBufferCount(Cell *cell, int *bufCount) {
   int ndx;

   for (ndx = 0; ndx < cell->numRuns; ndx++) {
      bufCount[ndx] = -1;
   }
}

void CheckCell(Cell *cell) {
   if (cell->numRuns == -1 || cell->writeTo[0] == -1) {
      fprintf(stderr, "Usage: ./Cell S O I D V (in any order)\n");
      exit(EXIT_FAILURE);
   }
}

int bufferCheck(int *bufferCount, Report buffer[][4], int numTimes) {
   int ndx;

   for (ndx = 0; ndx < 4; ndx++) {
      printf("NumTime %d, Buffer Count %d, Report %d\n", numTimes,
       bufferCount[numTimes], buffer[numTimes][ndx].id);
   }
   
   for (ndx = 0; ndx < 4; ndx++) {
      //if (stepC == -1
   }

   return 1;
}
