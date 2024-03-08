project('P2', 'C') # definition of project name followed by programming languaje used

exe='vad' # name of the program to generate
src=['src/main_vad.c', 'src/vad.c', 'src/pav_analysis.c'] # source code of the program
lib=['-lm', '-lsndfile'] # the libraries with which we want to link the main program

executable(exe, sources: src, link_args: lib) # comando generación del objetivo. En nuestro caso el objectivo es un ejecutable.
# a exeecutable() le pasamos: (1) nombre del objetivo (2) códigos fuente que hay que compilar para obtenerlo (3) libreries que usas en el programa
