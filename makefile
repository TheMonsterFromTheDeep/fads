EXEC= termcalc
TARGETS= main.c
OBJECTS= $(TARGETS:%.c=%.o)
CCOPT= -lcurses -lm

all : $(OBJECTS)
	$(CC) $? -o $(EXEC) $(CCOPT)

%.o : %.c
	$(CC) -c $< -o $@ $(CCOPT)
