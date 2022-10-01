NAME = scop
SRCDIR = srcs/
OBJDIR = .obj/
INCDIR = includes/
SRCFILES =	main.c \
			matrix.c \
			loop.c \
			init.c \
			parse.c \
			utils.c \
			event.c \
			draw.c \

INCFILES = scop.h stb_image.h
SRC = $(addprefix $(SRCDIR),$(SRCFILES))
INC = $(addprefix $(INCDIR),$(INCFILES))
OBJ = $(addprefix $(OBJDIR),$(SRCFILES:.c=.o))
FLAG = -I includes # -I /Users/mtaquet/.brew/Cellar/glew/2.2.0_1/include/ -I /Users/mtaquet/.brew/Cellar/glfw/3.3.8/include
LINK = -lglfw -lGLEW -lGL -lm# -L/Users/mtaquet/.brew/Cellar/glew/2.2.0_1/lib -L/Users/mtaquet/.brew/Cellar/glfw/3.3.8/lib

all: $(NAME)

$(OBJDIR)%.o: $(SRCDIR)%.c $(INC)
	@mkdir $(OBJDIR) 2> /dev/null || true
	gcc -c -o $@ $< $(FLAG)

$(NAME):$(OBJ) $(INC)
	gcc -o $@ $(OBJ) $(LINK)
clean:
	rm -f $(OBJ)

fclean: clean
	rm -f $(NAME)

re: fclean all