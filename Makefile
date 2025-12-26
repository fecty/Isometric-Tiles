# For Reference
# 	g++ -std=c++17 main.cpp -o main.out -I../../include -L../../lib -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
# -isystem ../../include instead of -I../../include to disable third-party warnings.
.PHONY: clear clean

CC := g++
CC_FLAGS := -std=c++17 -isystem include/ -Llib -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
CC_OTHER_FLAGS := -Wall -Wextra -Wpedantic -Wshadow -Wconversion -Wsign-conversion -O0 -g
BIN_DIR := bin
SRC_DIR := src
SOURCE := main
OTHER_SOURCES :=  

all: clear build-test


${BIN_DIR}/${SOURCE}.out: ${SRC_DIR}/${SOURCE}.cpp ${BIN_DIR} 
	@echo "Building ${BIN_DIR}/${SOURCE}.out"
	@${CC} ${SRC_DIR}/${SOURCE}.cpp ${OTHER_SOURCES} -o ${BIN_DIR}/${SOURCE}.out ${CC_FLAGS} ${CC_OTHER_FLAGS}


test: ${BIN_DIR}/${SOURCE}.out ${BIN_DIR} 
	./${BIN_DIR}/${SOURCE}.out

build-test: ${BIN_DIR}/${SOURCE}.out ${BIN_DIR}
	@${CC} ${SRC_DIR}/${SOURCE}.cpp ${OTHER_SOURCES} -o ${BIN_DIR}/${SOURCE}.out ${CC_FLAGS} ${CC_OTHER_FLAGS}
	./${BIN_DIR}/${SOURCE}.out




WEB_DIR := ${BIN_DIR}/web

setup_env: 
	bash --login

build-web: ${WEB_DIR}
	emcc -o ${WEB_DIR}/triangle.html ${SRC_DIR}/${SOURCE}.cpp ${OTHER_SOURCES} -Wall -std=c++17 -D_DEFAULT_SOURCE -Wno-missing-braces -Wunused-result -Os -I. -I raylib/src -I raylib/src/external -L. -L raylib/src -s USE_GLFW=3 -s ASYNCIFY -s TOTAL_MEMORY=67108864 -s FORCE_FILESYSTEM=1 --shell-file raylib/src/shell.html lib/web/libraylib.web.a -DPLATFORM_WEB -s 'EXPORTED_FUNCTIONS=["_free","_malloc","_main"]' -s EXPORTED_RUNTIME_METHODS=ccall

run-web:
	python3 -m http.server

clean-web:
	rm -rf ${WEB_DIR}/*


test-web: clean-web build-web run-web


${BIN_DIR}:
	mkdir ${BIN_DIR}

${WEB_DIR}:
	mkdir ${WEB_DIR}

${OTHER_SOURCES}:
	echo "build"


clear:
	clear

clean: clean-web
	rm -rf ${BIN_DIR}/*.out

