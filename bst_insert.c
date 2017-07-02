#include <stdio.h>
#include <stdlib.h>
typedef struct tree {
    int data;
    struct tree *left;
    struct tree *right;
    struct tree *parent;
}node;

node *newnode(int data) {
    node *root = (node *)malloc(sizeof(node));
    root->data = data;
    root->left = NULL;
    root->right = NULL;
    root->parent = NULL;
    return root;
}

void insert(node *root, int data) {
    if (root == NULL) {
        root = newnode(data);
        return;
    } else {
        if (data < root->data) {
            if (!root->left) {
                node *temp = newnode(data);
                temp->parent = root;
                root->left = temp;
            } else {
                insert(root->left, data);
            }
        } else {
            if (!root->right) {
                node *temp = newnode(data);
                temp->parent = root;
                root->right = temp;
            } else {
                insert(root->right, data);
            }
        }
    }
}

void print(node *root) {
    if (root) {
            print(root->left);
            if (root->parent != NULL)
            printf("%d<--%d ", root->data, root->parent->data);
            else printf("%d<--NULL ", root->data);
            print(root->right);
    }
}

node *lca(int a, int b, node *root, int *afound, int *bfound) {
    if (root) {
	int lafound = 0;
	int lbfound = 0;
	int rafound = 0;
	int rbfound = 0;
        node *left = lca(a, b, root->left, &lafound, &lbfound);
	if (lafound==1 && lbfound==1) {
	    *afound=1;
	    *bfound=1;
	    return left;
	}
        node *right = lca(a, b, root->right, &rafound, &rbfound);
        if (rafound==1 && rbfound==1) {
		*afound = 1;
		*bfound = 1;
                return right;
        }
	if ((lafound==1 && rbfound==1) ||
	    (lbfound==1 && rafound==1)) {
		*afound = 1;
		*bfound = 1;
		return root;
        } else {
	    *afound = lafound || rafound;
	    *bfound = lbfound || rbfound;
	    if (root->data == a) {
		*afound=1;
	    } 
	    if (root->data == b) {
		*bfound = 1;
	    }
	    if (*afound==1 && *bfound==1) {
	        return root;
	    } else {
                return NULL;
	    }
	}
    }
    return NULL;
}

int findMax(node *root, int val) {
    if (root) {
	if (root->data >= val) {
	    return root->data;
	} else {
	    if (root->data < val) {
                return findMax(root->right, val);
	    }
	}
    }
    return -1;
}

int findsum(node *root, int *k) {
	if (root && ((*k) > 0)) {
		int val = 0;
		val = findsum(root->left, k);
		if (*k == 0) {
			return val;
		}
		(*k)-=1;
		val += root->data;
		return val + findsum(root->right, k);
	}
	return 0;
}

double findleafsum(node *root, double *pathvalue) {
	if (root) {
		(*pathvalue) = (*pathvalue)*10 + root->data;
		double lsum = 0;
		double rsum = 0;
		if (!root->left && !root->right) {
			return *pathvalue;
		}
		if (root->left)
			lsum = findleafsum(root->left, pathvalue);
		if (root->right)
			rsum = findleafsum(root->right, pathvalue);
		return lsum + rsum;
	}
	return 0;
}

double findleavessum(node *root) {
	double sum = 0;
	return findleafsum(root, &sum);
}

void printkdistanceutil(node *root, int nodeval, int k, int *nodefound, int *dist) {

}

void printkdistance(node *root, int nodeval, int k) {
	int nodefound = 0;
	int dist = -1;
	printkdistanceutil(root, nodeval, k, &nodefound, &dist);
}

int main() {
node *root = newnode(6);
insert(root, 8);
insert(root, 4);
insert(root, 3);
insert(root, 10);
insert(root, 1);
insert(root, 12);
insert(root, 2);
insert(root, 5);
insert(root, 7);
insert(root, 9);
insert(root, 11);
insert(root, 13);

print(root);
printf("\n");
int afound = 0;
int bfound = 0;
node *lc = lca(1, 10, root, &afound, &bfound);
int max = findMax(lc, 10);
printf("max: %d\n", max);

int k = 3;
for (k = 0; k < 8; k++) {
int j = k;
int sum = findsum(root, &j);
printf("sum of %d smallest elements: %d\n", k, sum);
}
double sumleaves = findleavessum(root);
printf("sum of roottoleaf paths: %lf\n", sumleaves);
printkdistance(root, 1, 2);
return 0;
}
