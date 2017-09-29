all: tree

tree: tree.c
	$(CC) $< -o $@