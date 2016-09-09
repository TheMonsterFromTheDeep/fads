EXEC= termcalc
TARGETS= main.c expression.c graph.c
OBJECTS= $(TARGETS:%.c=%.o)
CCOPT= -lcurses -lm

all : $(OBJECTS)
	$(CC) $? -o $(EXEC) $(CCOPT)

%.o : %.c
	$(CC) -c $< -o $@ $(CCOPT)
