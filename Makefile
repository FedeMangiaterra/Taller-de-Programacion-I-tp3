# Makefile de ejemplo para C/C++
# Creado: 15/04/2004 - Leandro Lucarella
# Modificado: 01/09/2016 - Pablo Roca
# Copyleft 2004 - Leandro Lucarella, Bajo licencia GPL [http://www.gnu.org/]

# CONFIGURACION
################

# ExtensiÃ³n de los archivos a compilar (c para C, cpp o cc o cxx para C++).
extension = cpp

# Archivos con el cÃ³digo fuente que componen el ejecutable. Si no se especifica,
# toma todos los archivos con la extensiÃ³n mencionada. Para especificar hay que
# descomentar la lÃ­nea (quitarle el '#' del principio).
# NOTA: No poner cabeceras (.h).
#fuentes = entrada.cpp

# Si usa funciones de math.h, descomentar (quitar el '#' a) la siguiente lÃ­nea.
math = si

# Si usa threads, descomentar (quitar el '#' a) la siguiente lÃ­nea.
threads = si

# Descomentar si se quiere ver como se invoca al compilador
verbose = si


# CONFIGURACION "AVANZADA"
###########################

# Opciones para el compilador C/C++ para tratamiento de errores y warnings.
CFLAGS += -Wall -Werror -pedantic -pedantic-errors

# Para optimizar el binario resultante lo mejor posible
CFLAGS += -O3

# Para valgrind o debug
CFLAGS += -ggdb -DDEBUG -fno-inline

# Opciones del enlazador.
#LDFLAGS =

# Estandar de C a usar
CSTD = c11

# Estandar de C++ a usar
CXXSTD = c++11

# Estandar POSIX que extiende C/C++. En teoria los grandes
# sistemas operativos incluyendo Windows son POSIX compliant
CFLAGS += -D _POSIX_C_SOURCE=200809L

# Si se quiere compilar estaticamente, descomentar la siguiente linea
#static = si

# Si se quiere simular pÃ©rdidas, definir la variable wrapsocks por linea
# de comandos: 'wrapsocks=si make'  o descomentar la siguiente linea
#wrapsocks = si


# VARIABLES CALCULADAS A PARTIR DE LA CONFIGURACION
####################################################


# Linkea con libm de ser necesario.
ifdef math
LDFLAGS += -lm
endif

# Linkea con threads de ser necesario. Permite el uso de pthread en C y C++. Permite el uso de built-in threads en C++.
ifdef threads
LDFLAGS += -pthread
endif

ifdef static
LDFLAGS += -static
endif

# Agrega simulaciÃ³n de pÃ©rdidas de bytes en las funciones de sockets
ifdef wrapsocks
CFLAGS += -Dwrapsocks=1
LDFLAGS += -Wl,--wrap=send -Wl,--wrap=recv
endif

# Se reutilizan los flags de C para C++ tambiÃ©n
CXXFLAGS += $(CFLAGS)

# Se usa enlazador de C++ si es cÃ³digo no C.
ifeq ($(extension), c)
CFLAGS += -std=$(CSTD)
LD = $(CC)
COMPILER = $(CC)
COMPILERFLAGS = $(CFLAGS)
else
CXXFLAGS += -std=$(CXXSTD)
LD = $(CXX)
COMPILER = $(CXX)
COMPILERFLAGS = $(CXXFLAGS)
endif

# Si no especifica archivos, tomo todos.
fuentes_client ?= $(wildcard ./client_src/*.$(extension)) $(wildcard ./client_*.$(extension))
fuentes_server ?= $(wildcard ./server_src/*.$(extension)) $(wildcard ./server_*.$(extension))
fuentes_common ?= $(wildcard ./common_src/*.$(extension)) $(wildcard ./common_*.$(extension))
directorios = $(shell find . -type d -regex '.*\w+')

occ := $(CC)
ocxx := $(CXX)
orm := $(RM)
old := $(LD)
ifdef verbose
RM := $(RM) -v
else
CC =  @echo "  CC  $@"; $(occ)
CXX = @echo "  CXX $@"; $(ocxx)
RM =  @echo "  CLEAN"; $(orm)
LD =  @echo "  LD  $@"; $(old)
endif

LDFLAGS-TSAN = $(LDFLAGS) -fsanitize=thread
COMPILERFLAGS-TSAN = $(COMPILERFLAGS) -fsanitize=thread


# REGLAS
#########

all: client server

o_common_files = $(patsubst %.$(extension),%.o,$(fuentes_common))
o_client_files = $(patsubst %.$(extension),%.o,$(fuentes_client))
o_server_files = $(patsubst %.$(extension),%.o,$(fuentes_server))
o-tsan_files = $(patsubst %.$(extension),%.o-tsan,$(fuentes_server) $(fuentes_common))

client: $(o_common_files) $(o_client_files)
	@if [ -z "$(o_client_files)" ]; \
	then \
		echo "No hay archivos de entrada en el directorio actual para el cliente. Recuerde que los archivos deben respetar la forma 'client*.$(extension)' y que no se aceptan directorios anidados."; \
		if [ -n "$(directorios)" ]; then echo "Directorios encontrados: $(directorios)"; fi; \
		false; \
	fi >&2
	$(LD) $(o_common_files) $(o_client_files) -o $@ $(LDFLAGS)

server: $(o_common_files) $(o_server_files)
	@if [ -z "$(o_server_files)" ]; \
	then \
		echo "No hay archivos de entrada en el directorio actual para el servidor. Recuerde que los archivos deben respetar la forma 'server*.$(extension)' y que no se aceptan directorios anidados."; \
		if [ -n "$(directorios)" ]; then echo "Directorios encontrados: $(directorios)"; fi; \
		false; \
	fi >&2
	$(LD) $(o_common_files) $(o_server_files) -o $@ $(LDFLAGS)

%.o-tsan: %.$(extension)
	$(COMPILER) $(COMPILERFLAGS-TSAN) -o $@ -c $<


server-tsan: $(o-tsan_files)
	@if [ -z "$(o-tsan_files)" ]; \
	then \
		echo "No hay archivos de entrada en el directorio actual. Recuerde que la extensiÃ³n debe ser '.$(extension)' y que no se aceptan directorios anidados."; \
		if [ -n "$(directorios)" ]; then echo "Directorios encontrados: $(directorios)"; fi; \
		false; \
	fi >&2
	$(LD) $(o-tsan_files) -o $@ $(LDFLAGS-TSAN)

clean: clean-obj
	$(RM) -f $(o_common_files) $(o_client_files) $(o_server_files) $(target-tsan) client server server-tsan

clean-obj:
	$(RM) -f $(o_files) $(o-tsan_files)

zip: clean
	zip -r entrega.zip ./common_src ./server_src ./client_src
