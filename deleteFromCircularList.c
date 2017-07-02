#include <stdio.h>

typedef struct linkedList {
	int element;
	struct linkedList *pptr;
	struct linkedList *nptr;
}dlist;

void delete(dlist *ptr, int del)
{
	dlist *head = ptr;
	dlist *temp;
	dlist *tail = ptr->pptr;

	if (head == NULL)
		return;

	while (head != tail)
	{
		if (head->element == del) {
			temp = head->pptr;
			temp->nptr = head->nptr;
			head->nptr->pptr = temp;

			temp = head;
			free(temp);
			head = head->nptr;
		}
	}

	if (tail->element == del)
	{
		temp = tail->pptr;
		temp->nptr = tail->nptr;
		tail->nptr->pptr = temp;
		free(tail);
	}
}

int main ()
{
	dlist *ptr = getList();
	delete(ptr);
	return 0;
}
