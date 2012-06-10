NAME 	= example
SRC	= example.cpp src/Exception.cpp

IPATH   = -I..
OBJOPT  = -O2 -Wall -ansi -pedantic
EXEOPT  = -O2 -Wall -ansi -pedantic

CXX 	= g++
OBJS 	= $(SRC:.cpp=.o)
RM 	= /bin/rm -f 	

CFLAGS  = $(OBJOPT) $(IPATH)
LDFLAGS = $(EXEOPT) $(LPATH)

all:	$(NAME)
$(NAME): $(OBJS) $(SRC) $(INCL)  
	$(CXX) $(OBJS) $(LDFLAGS) -o $(NAME) 

clean:
	-$(RM) $(NAME) $(OBJS) *~
.cpp.o:
	$(CXX) $(CFLAGS) -c $< -o $@
