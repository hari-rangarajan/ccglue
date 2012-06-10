NAME 	= TestSuite
SRC	= TestSuite.cpp TestToolBoxTrie.cpp ../src/Exception.cpp
INCL  	= TestToolBoxTrie.h

IPATH   = -I..
OBJOPT  = -O2 -Wall -ansi -pedantic
EXEOPT  = -O2 -Wall -ansi -pedantic

CXX 	= g++
OBJS 	= $(SRC:.cpp=.o)
RM 	= /bin/rm -f 	

CFLAGS  = $(OBJOPT) $(IPATH)
LDFLAGS = $(EXEOPT) $(LPATH) -lcppunit

all:	$(NAME)
$(NAME): $(OBJS) $(SRC) $(INCL)  
	$(CXX) $(OBJS) $(LDFLAGS) -o $(NAME) 

clean:
	-$(RM) $(NAME) $(OBJS) *~
.cpp.o:
	$(CXX) $(CFLAGS) -c $< -o $@
