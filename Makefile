# files
SRC   = ${wildcard src/*.cc}
DEPS += ${wildcard src/*.hh}
OBJ   = ${addsuffix .o,${subst src/,bin/,${basename ${SRC}}}}

APP = ./bin/atmo

# compiler related
CXX = g++
CXXVER = c++17
CXXFLAGS = \
	-O3 \
	-std=${CXXVER} \
	-Wall \
	-Wextra \
	-Werror \
	-pedantic \
	-g

# rules
compile: ./bin ${OBJ} ${SRC}
	${CXX} -o ${APP} ${OBJ}

./bin:
	mkdir -p bin

bin/%.o: src/%.cc
	${CXX} -c $< ${CXXFLAGS} ${CXXLIBS} -o $@

clean:
	rm bin/*.o $(APP)

install:
	cp $(APP) /usr/bin/

all:
	@echo compile
	@echo clean
	@echo install
