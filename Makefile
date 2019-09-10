
all: $(NAME)
	@make -C server/
	@make -C client/

	@mv server/server_app ./
	@mv client/client_app ./
del:
	@$(DEL) $(OBJ)

clean:
	@make -C server/ clean
	@make -C client/ clean

fclean: clean
	@rm client_app server_app

re: fclean all

.PHONY: all fclean clean re
