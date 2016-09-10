EXEC= termcalc
TARGETS= main.c expression.c graph.c terminal.c
OBJECTS= $(TARGETS:%.c=%.o)
CCOPT= -lcurses -lm

all : $(OBJECTS)
	$(CC) $? -o $(EXEC) $(CCOPT)

%.o : %.c %.h base.h
	$(CC) -c $< -o $@ $(CCOPT)
