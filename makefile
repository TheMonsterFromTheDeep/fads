EXEC= fads
TARGETS= main.c expression.c graph.c terminal.c brscr.c
OBJECTS= $(TARGETS:%.c=%.o)
CCOPT= -lncursesw -lm

all : $(OBJECTS)
	$(CC) $? -o $(EXEC) $(CCOPT)

%.o : %.c %.h base.h
	$(CC) -c $< -o $@ $(CCOPT)
