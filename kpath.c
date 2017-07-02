#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

typedef struct tree {
	int data;
	struct tree *left;
	struct tree *right;
}node;

typedef struct PATH {
	node *pointer;
	struct PATH *next;
}path;

typedef struct list {
	int value;
	path *tree_node;
	struct list *next;
}lnode;

void print(path *kpath) {
	if (kpath == NULL)
		return;

	path *tmp = kpath;
	while (tmp) {
		if (tmp->pointer != NULL)
			printf("%d->", (tmp->pointer)->data);
		tmp = tmp->next;
	}
	printf("NULL\n");
}

void printKPath(int k, node *root, lnode *list) {
	lnode *newnode = (lnode *)malloc(sizeof(lnode));
	newnode->value = 0;
	newnode->tree_node = NULL;
	newnode->next = list;
	list = newnode;
	while (list) {
		list->value += root->data;
		path *tmp1 = list->tree_node;
		if (tmp1 == NULL) {
			tmp1 = (path *)malloc(sizeof(path));
			tmp1->next = NULL;
			tmp1->pointer = root;
			list->tree_node = tmp1;
		} else {
			while (tmp1->next) {
				tmp1 = tmp1->next;
			}
			tmp1->next = (path *)malloc(sizeof(path));
		 	tmp1->next->next = NULL;
			tmp1->next->pointer = root;
		}
		if (list->value == k) {
			print(list->tree_node);
		}
		list = list->next;
	}
	list = newnode;

	if (root->left) {
		printKPath(k, root->left, list);
	}

	if (root->right) {
		printKPath(k, root->right, list);
	}
}

int main() {
	node root;
	root.data = 1;
	root.left = NULL;
	root.right = NULL;
	//populate tree
	int k = 1;
	printKPath(k, &root, NULL);
	return 0;
}
