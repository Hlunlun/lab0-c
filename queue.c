#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "queue.h"

/* Notice: sometimes, Cppcheck would find the potential NULL pointer bugs,
 * but some of them cannot occur. You can suppress them by adding the
 * following line.
 *   cppcheck-suppress nullPointer
 */

/* Create an empty queue */
struct list_head *q_new()
{
    struct list_head *q = malloc(sizeof(struct list_head));
    if (q == NULL)
        return NULL;
    INIT_LIST_HEAD(q);
    return q;
}

/* Free all storage used by queue */
void q_free(struct list_head *head)
{
    element_t *entry = NULL, *safe;
    list_for_each_entry_safe (entry, safe, head, list) {
        free(entry->value);
        free(entry);
    }
    free(head);
    return;
}

/* Insert an element at head of queue */
bool q_insert_head(struct list_head *head, char *s)
{
    if (!head)
        return false;
    element_t *element = malloc(sizeof(element_t));
    if (!element)
        return false;
    struct list_head *node = &element->list;
    INIT_LIST_HEAD(node);
    element->value = strdup(s);
    if (!element->value) {
        free(element);
        return false;
    }
    list_add(node, head);
    return true;
}

/* Insert an element at tail of queue */
bool q_insert_tail(struct list_head *head, char *s)
{
    if (!head)
        return false;
    element_t *element = malloc(sizeof(element_t));
    if (!element)
        return false;
    struct list_head *node = &element->list;
    INIT_LIST_HEAD(node);
    element->value = strdup(s);
    if (!element->value) {
        free(element);
        return false;
    }
    list_add_tail(node, head);
    return true;
}

/* Remove an element from head of queue */
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;
    element_t *entry = list_first_entry(head, element_t, list);
    list_del(&entry->list);
    if (sp) {
        size_t dlen = strnlen(entry->value, bufsize - 1);
        mempcpy(sp, entry->value, dlen);
        *(sp + dlen) = 0;
    }
    return entry;
}

/* Remove an element from tail of queue */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;
    element_t *entry = list_last_entry(head, element_t, list);
    list_del(&entry->list);
    if (sp) {
        size_t dlen = strnlen(entry->value, bufsize - 1);
        mempcpy(sp, entry->value, dlen);
        *(sp + dlen) = 0;
    }
    return entry;
}

/* Return number of elements in queue */
int q_size(struct list_head *head)
{
    if (!head)
        return 0;

    int len = 0;
    struct list_head *li;

    list_for_each (li, head)
        len++;
    return len;
}

/* Delete the middle node in queue */
bool q_delete_mid(struct list_head *head)
{
    if (!head || list_empty(head))
        return false;

    struct list_head **indir = &(head->next);
    for (const struct list_head *fast = head->next;
         fast != head && fast->next != head; fast = fast->next->next)
        indir = &(*indir)->next;

    element_t *entry = list_entry(*indir, element_t, list);
    list_del(*indir);
    free(entry->value);
    free(entry);
    return true;
}

/* Delete all nodes that have duplicate string */
bool q_delete_dup(struct list_head *head)
{
    if (!head || list_empty(head) || list_is_singular(head))
        return false;

    struct list_head **indir = &(head->next);
    while (*indir != head) {
        struct list_head *cur = *indir;
        element_t *entry = list_entry(cur, element_t, list);
        if (cur->next != head &&
            !strcmp(entry->value,
                    list_entry(cur->next, element_t, list)->value)) {
            const char *str = entry->value;

            while (cur != head && !strcmp(entry->value, str)) {
                struct list_head *next = cur->next;
                free(entry->value);
                free(entry);
                cur = next;
                entry = list_entry(next, element_t, list);
            }
            *indir = cur;
        } else {
            indir = &(*indir)->next;
        }
    }
    return true;
}

/* Swap every two adjacent nodes */
void q_swap(struct list_head *head)
{
    if (!head || list_empty(head) || list_is_singular(head))
        return;

    struct list_head *node = NULL, *safe = NULL;
    list_for_each_safe (node, safe, head) {
        struct list_head *pos = safe->prev;
        list_del(safe);

        safe->next = node->next;
        safe->next->prev = safe;
        safe->prev = node->prev;
        node->prev = safe;
        if (pos == node)
            pos = safe;
        list_add(node, pos);

        safe = node->next;
    }
}

/* Reverse elements in queue */
void q_reverse(struct list_head *head)
{
    if (!head || list_empty(head) || list_is_singular(head))
        return;

    struct list_head *node = NULL, *safe = NULL;
    list_for_each_safe (node, safe, head) {
        node->next = node->prev;
        node->prev = safe;
    }
    node->next = node->prev;
    node->prev = safe;
    return;
}

/* Reverse the nodes of the list k at a time */
void q_reverseK(struct list_head *head, int k)
{
    if (!head || list_empty(head) || list_is_singular(head) || k <= 1 ||
        k > q_size(head))
        return;

    int count = k;
    struct list_head *node = head->next, *safe = node->next;
    while (count--) {
        node->next = safe->next;
        node->prev = safe;
        node = safe;
        safe = node->next;
    }

    q_reverseK(node, k);
    return;
}

/* Sort elements of queue in ascending/descending order */
void q_sort(struct list_head *head, bool descend) {}

/* Remove every node which has a node with a strictly less value anywhere to
 * the right side of it */
int q_ascend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    return 0;
}

/* Remove every node which has a node with a strictly greater value anywhere to
 * the right side of it */
int q_descend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    return 0;
}

/* Merge all the queues into one sorted queue, which is in ascending/descending
 * order */
int q_merge(struct list_head *head, bool descend)
{
    // https://leetcode.com/problems/merge-k-sorted-lists/

    return 0;
}
