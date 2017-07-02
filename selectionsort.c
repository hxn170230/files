#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

typedef struct linklist {
	int data;
	struct linklist * next;
}node;

node * selectionSort(node *head) {
	node *minptr = NULL;
	node *minParentPtr = NULL;
	node *replacePtr = head;
	node *curPtr = head;
	node *replaceParent = NULL;
	node *listHead = head;

	while (curPtr) {
		node *tmp = curPtr;
		minptr = curPtr;
		minParentPtr = curPtr;
		while (tmp) {
			if (tmp->next != NULL) {
				if ((tmp->next)->data < minptr->data) {
					minParentPtr = tmp;
					minptr = tmp->next;
				}
			}

			tmp = tmp->next;
		}

		if (minptr == minParentPtr) {
			// swap not required
		} else if (minptr == replacePtr) {
			// swap not required	
		} else {
			node *tmpPtr = minptr->next;
			if (minParentPtr == replacePtr) {
				// swap minptr and replacePtr	
				replaceParent->next = minptr;
				minptr->next = replacePtr;
				replacePtr->next = tmpPtr;
			} else {
				if (replaceParent != NULL)
					replaceParent->next = minptr;
				minptr->next = replacePtr->next;
				minParentPtr->next = replacePtr;
				replacePtr->next = tmpPtr;
				if (replaceParent == NULL) {
					listHead = minptr;
				}
			}
		}

		curPtr = minptr->next;
		replacePtr = minptr->next;
		replaceParent = minptr;
	}

	return listHead;
}

int main() {

	node *tmp = NULL;
	node *head = NULL;
	// populate
	int i = 0;
	for (i = 0; i < 10; i++) {
		int value = 10;
		tmp = (node *)malloc(sizeof(node));
		tmp->data = value;
		tmp->next = head;
		head = tmp;
	}
	tmp = head;
	printf("\n");
	while (tmp) {
		printf("%d->", tmp->data);
		tmp = tmp->next;
	}
	printf("NULL\n");
	head = selectionSort(head);
	// print
	tmp = head;
	printf("\n");
	while (tmp) {
		printf("%d->", tmp->data);
		tmp = tmp->next;
	}
	printf("NULL\n");

	return 0;
}
