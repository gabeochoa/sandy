MAKEFLAGS := --jobs=16
MAKEFLAGS += --output-sync=target

# FLAGS = -std=c++2a -stdlib=libc++ -Wall -Wextra -g -fsanitize=address
# FLAGS = -std=c++2a -Wall -Wextra -g -I/usr/local/include -stdlib=libc++ -fsanitize=address -fsanitize=undefined
# consider adding -Wfloat-equal
FLAGS = -std=c++2a -I/usr/local/include
DEBUG_FLAGS = -Wall -Wextra -Wpedantic -Wuninitialized -Wshadow -Wmost -g 
LIBS = -lsfml-window -lsfml-graphics -lsfml-system -lsfml-audio
CCC = clang++
MFLAGS = -MMD -MP 


all: 
	$(CCC) $(DEBUG_FLAGS) $(FLAGS) sf.cpp $(LIBS)
	./a.out

speed:
	$(CCC) -O3 $(FLAGS) sf.cpp $(LIBS)
	./a.out

size:
	$(CCC) -Os $(FLAGS) sf.cpp $(LIBS)
	./a.out

windows:
	x86_64-w64-mingw32-g++ -std=gnu++20 -c sf.cpp -I./windows_build/SFML-2.5.1/include -Wno-narrowing
	x86_64-w64-mingw32-g++ sf.o -o ./main_win.exe -L./windows_build/SFML-2.5.1/lib -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio -lwinpthread

.PHONY: all 
