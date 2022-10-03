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

OBJ = $(SRCFILES:.c=.o)
OBJS = $(addprefix $(OBJDIR),$(OBJ))

FLAG = -I includes -I /Users/mtaquet/.brew/Cellar/glew/2.2.0_1/include/ -I /Users/mtaquet/.brew/Cellar/glfw/3.3.8/include 
LINK = -lglfw -lGLEW -framework OpenGL -lm -L/Users/mtaquet/.brew/Cellar/glew/2.2.0_1/lib -L/Users/mtaquet/.brew/Cellar/glfw/3.3.8/lib

all: $(NAME)

$(OBJDIR)%.o: $(SRCDIR)%.c $(INC)
	@mkdir $(OBJDIR) 2> /dev/null || true
	gcc  -c $< -o $@ $(FLAG)

$(NAME):$(OBJS) $(INC)
	gcc -o $@ $(OBJS) $(LINK)
clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all