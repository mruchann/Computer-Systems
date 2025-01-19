 struct Node {
	long data;
	struct Node* next;
};

/* NOTE: (void *) 0 is how NULL is defined in C.
   Basically, NULL should correspond to zero in your
   y86-64 code, nothing confusing! */


/* Sort the given linked list in ascending order using iterative Selection Sort.
 * Return NULL if the linked list is empty. */
 
struct Node* selection_sort_it(struct Node* head)
{
    // check if there are elements to sort
    if (head == NULL || head->next == NULL)
        return head;
 
    
    struct Node* sorted = NULL;
    
    while (head != NULL) {
        struct Node* max = head;
        struct Node* prevMax = NULL;
        struct Node* curr = head;
        struct Node* prev = NULL;

        // Find the node with the maximum value
        while (curr != NULL) {
            if (curr->data > max->data) {
                max = curr;
                prevMax = prev;
            }
            prev = curr;
            curr = curr->next;
        }

        // Remove max from the unsorted part
        if (max == head) {
            head = head->next;
        } 
        else {
            prevMax->next = max->next;
        }

        // Insert max at the 
        // beginning of the sorted list
        max->next = sorted;
        sorted = max;
    }
    return sorted; /* Return the sorted list */
}


 /* Sort the given linked list in ascending order using recursive Selection Sort.
 * Return NULL if the linked list is empty. */
 
struct Node* selection_sort_rec(struct Node* head)
{
    // when last node is reached return it
    if (head == NULL || head->next == NULL)
        return head;
 
    // initialize min pointer with current node which will store
    // address of minimum valued node
     struct Node* min = head;
 
    // initialize prevMin pointer with current node which will
    // store address of previous node pointed by min 
     struct Node* prevMin = NULL;
     struct Node* curr;
 
    // search for the minimum value
    for (curr = head; curr->next != NULL; curr = curr->next) {
 
        // if true, then update min and prevMin
        if (curr->next->data < min->data) {
            min = curr->next;
            prevMin = curr;
        }
    }
 
    // if min is not same as the current node then
    // swap the head node with the min node
    if (min != head) {
    	struct Node* temp = head->next;
    	head->next = min->next;
    	if (temp == min) {
    	    min->next = head;
    	}
    	else {
    	    min->next = temp;
    	    prevMin->next = head;
    	}
    	head = min;
    }

    // call the recursive function for rest of the list
    head->next = selection_sort_rec(head->next);
 
    return head; /* Return the sorted list */
}


