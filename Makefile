PlanarSim : planarSim.c planarSim.h Cell
	gcc -Wall -Werror -g -o PlanarSim planarSim.c
Cell : Cell.c Cell.h
	gcc -Wall -Werror -g -o Cell Cell.c
