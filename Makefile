NAME = scop
SRCDIR = srcs/
INCDIR = includes/
SRCFILES = main.c

INCFILES = scop.h
SRC = $(addprefix $(SRCDIR),$(SRCFILES))
INC = $(addprefix $(INCDIR),$(INCFILES))
OBJ = $(SRC:.c=.o)
FLAG = -I includes  -I /Users/mtaquet/.brew/Cellar/glew/2.2.0_1/include/ -I /Users/mtaquet/.brew/Cellar/glfw/3.3.8/include 
LINK = -lglfw -lGLEW -framework OpenGL -L/Users/mtaquet/.brew/Cellar/glew/2.2.0_1/lib -L/Users/mtaquet/.brew/Cellar/glfw/3.3.8/lib

all: $(NAME)

%.o: %.c $(INC)
	gcc -c -o $@ $< $(FLAG)

$(NAME):$(OBJ) $(INC)
	gcc -o $@ $(OBJ) $(LINK)

clean:
	rm -f $(OBJ)

fclean: clean
	rm -f $(NAME)

re: fclean all