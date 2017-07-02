#include <stdio.h>
#include <stdlib.h>

typedef struct tree {
	int value;
	struct tree * left;
	struct tree * right;
}node;

long max(long a, long b) {
	return (a<b?b:a);
}

long findDiameter(node *root, long *diag) {
	if (root == NULL) {
		*diag = 0;
		return 0;
	} else {
		long rightSize = 0;
		if (root->right) {
			rightSize = findDiameter(root->right, diag);	
		}

		long leftSize = 0;
		if (root->left) {
			leftSize = findDiameter(root->left, diag);
		}

		if (*diag < (leftSize + rightSize)+1) {
			*diag = (leftSize + rightSize)+1;
		}

		return 1 + max(leftSize, rightSize);
	}
}

int main() {
	node *root = (node *) malloc (sizeof(node));
	node *temp = root;
	int i = 0;
	for (i = 0; i < 10; i++) {
		temp->right = (node *) malloc (sizeof(node));
		temp->right->left = NULL;
		temp->right->right = NULL;
		temp->left = (node *)malloc(sizeof(node));
		temp->left->left = NULL;
		temp->left->right = NULL;
		temp = temp->right;
		temp->left = NULL;
		temp->right = NULL;
	}

	long diameter=  0;
	findDiameter(root, &diameter);
	printf("Diameter: %ld\n", diameter);
	return 0;
}
