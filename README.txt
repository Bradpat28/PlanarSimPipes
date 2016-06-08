Planar Sim 

Created by Bradley Patterson

California Polytechnic State University, San Luis Obispo
CPE 357 - Spring 2016 - Final Project


ABOUT:
This program is an extension of a previosly done project called "LinearSim",
which simulated S number of simulations of a CFD. LinearSim was done with 
simply linear functionality, however, as the name describes, Planar Sim 
is a 2 dimensional heat flow simulation.

COMPILING/RUNNING:
Files you will need: planarSim.c planarSim.h Cell.c Cell.h Report.h

To compile (without make file):
   1. run the following commands
      gcc -Wall -Werror -o PlanarSim planarSim.c 
      gcc -Wall -Werror -o Cell cell.c

To run:
-Arguments 
   1. -S# : This is the number of simulations that will be carried out 
            for the specified heatflow problem. This argument must be 
            specified or Planar Sim will not run.
   2. -G# : This is the grid number for the 2D simulation. G1 is equivalent
            to a 3x3 grid of cells, G2 is equivalent to a 4x4 grid of cells,
            and G3 is equivalent to a 5x5 grid of cells. This argument must also
            be specifed in order for Planar Sim to run. (Currently only
            functions for Grids 1-3. Never versions will work for more grids)
   3. -U# : This is the specified fixed value for the "U"pper row on the grid, 
            with float precision. Will take the first specified flag, or, if 
            one is not specified, then will default to a value of zero. 
   4. -D# : This is the specified fixed value for the "D"own (bottom) row of 
            the grid, with float precision. Same functionality as "-U#"
   5. -L# : This is the specified fixed value for the "L"eft row of the grid,
            with float precision. Same functionality as "-U#"
   6. -R# : This is the specified fixed value for the "R"ight row of the grid, 
            with float precision. Sane functionality as "-U#"
-Example Run
   ./PlanarSim S1 G2 R12.03 L-123 U.321
   This example creates a 4x4 grid with the right side fixed to 12.03, the left
   side fixed to -123, the top row to .321, and the bottom row defaulted to 0. 
   This will simulate 1 iteration. 

VERSION: 
   1.0.0 - Finished grid functionality for grid sizes of 1-3. Uses pipes, forks,
            and execs to create the cells. Report structures still only specify
            a sigle "value". 

FUTURE UPDATES:
   - Extend functionality to grids of sizes larger than G3. 
   - Convert the pipes used for communication to threads.
   - Add more features/values to be simulated. 
   - 3D w/ graphics?


