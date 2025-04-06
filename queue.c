#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "queue.h"

/* Create an empty queue */
struct list_head *q_new()
{
    struct list_head *q = malloc(sizeof(struct list_head));
    // if (!q)
    //     return NULL;
    while (q == NULL)
        q = malloc(sizeof(struct list_head));
    INIT_LIST_HEAD(q);
    return q;
}

/* Free all storage used by queue */
void q_free(struct list_head *head)
{
    element_t *entry = NULL, *safe;
    list_for_each_entry_safe(entry, safe, head, list) {
        q_release_element(entry);
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
        q_release_element(element);
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
        q_release_element(element);
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

    list_for_each(li, head)
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
    if (!head || list_empty(head))
        return false;

    bool dup = false;
    element_t *node, *safe;
    list_for_each_entry_safe(node, safe, head, list) {
        if (&safe->list != head && !strcmp(node->value, safe->value)) {
            list_del(&node->list);
            q_release_element(node);
            dup = true;
        } else if (dup) {
            list_del(&node->list);
            q_release_element(node);
            dup = false;
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
    list_for_each_safe(node, safe, head) {
        if (safe == head)
            return;

        struct list_head *next = safe->next;
        struct list_head *prev = node->prev;

        safe->prev = prev;
        safe->next = node;
        node->prev = safe;
        node->next = next;
        prev->next = safe;
        next->prev = node;

        safe = node->next;
    }
}

/* Reverse elements in queue */
void q_reverse(struct list_head *head)
{
    if (!head || list_empty(head) || list_is_singular(head))
        return;

    struct list_head *node = NULL, *safe = NULL;
    list_for_each_safe(node, safe, head) {
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

    int times = q_size(head) / k;
    struct list_head *tail = NULL;

    LIST_HEAD(tmp);
    LIST_HEAD(new_head);

    for (int i = 0; i < times; i++) {
        int j = 0;
        list_for_each(tail, head) {
            if (j >= k)
                break;
            j++;
        }
        list_cut_position(&tmp, head, tail->prev);
        q_reverse(&tmp);
        list_splice_tail(&tmp, &new_head);
    }
    list_splice(&new_head, head);
}

bool cmp(const struct list_head *a, const struct list_head *b, bool descend)
{
    element_t const *e1 = list_entry(a, element_t, list);
    element_t const *e2 = list_entry(b, element_t, list);

    if (descend)
        return strcmp(e1->value, e2->value) >= 0;

    return strcmp(e1->value, e2->value) <= 0;
}

struct list_head *merge(bool descend, struct list_head *a, struct list_head *b)
{
    struct list_head *head = NULL, **tail = &head;

    for (;;) {
        if (cmp(a, b, descend)) {
            *tail = a;
            tail = &a->next;
            a = a->next;
            if (!a) {
                *tail = b;
                break;
            }
        } else {
            *tail = b;
            tail = &b->next;
            b = b->next;
            if (!b) {
                *tail = a;
                break;
            }
        }
    }
    return head;
}

void merge_final(bool descend,
                 struct list_head *head,
                 struct list_head *a,
                 struct list_head *b)
{
    struct list_head *tail = head;

    for (;;) {
        if (cmp(a, b, descend)) {
            tail->next = a;
            a->prev = tail;
            tail = a;
            a = a->next;
            if (!a)
                break;
        } else {
            tail->next = b;
            b->prev = tail;
            tail = b;
            b = b->next;
            if (!b) {
                b = a;
                break;
            }
        }
    }

    tail->next = b;
    do {
        b->prev = tail;
        tail = b;
        b = b->next;
    } while (b);

    tail->next = head;
    head->prev = tail;
}

/* Sort elements of queue in ascending/descending order */
void q_sort(struct list_head *head, bool descend)
{
    if (!head || list_empty(head) || list_is_singular(head))
        return;

    struct list_head *list = head->next, *pending = NULL;
    size_t count = 0;

    head->prev->next = NULL;

    do {
        size_t bits;
        struct list_head **tail = &pending;

        for (bits = count; bits & 1; bits >>= 1)
            tail = &(*tail)->prev;

        if (bits) {
            struct list_head *a = *tail, *b = a->prev;

            a = merge(descend, b, a);
            a->prev = b->prev;
            *tail = a;
        }

        list->prev = pending;
        pending = list;
        list = list->next;
        pending->next = NULL;
        count++;
    } while (list);

    list = pending;
    pending = pending->prev;
    for (;;) {
        struct list_head *next = pending->prev;

        if (!next)
            break;
        list = merge(descend, pending, list);
        pending = next;
    }
    merge_final(descend, head, pending, list);
}

/* Remove every node which has a node with a strictly less value anywhere to
 * the right side of it */
int q_ascend(struct list_head *head)
{
    if (!head || list_empty(head))
        return 0;
    if (list_is_singular(head))
        return 1;

    struct list_head *left = head->prev->prev, *right = head->prev;
    while (left != head) {
        if (cmp(left, right, false)) {
            left = left->prev;
            right = right->prev;
        } else {
            struct list_head *prev = left->prev;
            list_del(left);
            q_release_element(list_entry(left, element_t, list));
            left = prev;
        }
    }
    return q_size(head);
}

/* Remove every node which has a node with a strictly greater value anywhere to
 * the right side of it */
int q_descend(struct list_head *head)
{
    if (!head || list_empty(head))
        return 0;
    if (list_is_singular(head))
        return 1;

    struct list_head *left = head->prev->prev, *right = head->prev;
    while (left != head) {
        if (cmp(left, right, true)) {
            left = left->prev;
            right = right->prev;
        } else {
            struct list_head *prev = left->prev;
            list_del(left);
            q_release_element(list_entry(left, element_t, list));
            left = prev;
        }
    }
    return q_size(head);
}

int _q_merge(struct list_head *head1, struct list_head *head2, bool descend)
{
    if (!head2 || list_empty(head2))
        return 0;
    if (!head1 || list_empty(head1)) {
        if (!list_empty(head2))
            list_splice_tail(head2->next, head1);
        return q_size(head1);
    }
    LIST_HEAD(tmp_head);

    struct list_head *head = NULL, **ptr = &head, **node;
    struct list_head *l1 = head1->next, *l2 = head2->next;
    for (node = NULL; l1 != head1 && l2 != head2; *node = (*node)->next) {
        node = cmp(l1, l2, descend) ? &l1 : &l2;
        *ptr = *node;
        ptr = &(*ptr)->next;
    }

    *ptr = l1 ? l1 : l2;

    struct list_head *cur = head, *prev = cur;
    while (cur != head1 && cur != head2) {
        struct list_head *next = cur->next;
        cur->next->prev = cur;
        prev = cur;
        cur = next;
    }
    head1->next = head;
    head->prev = head1;
    head1->prev = prev;
    prev->next = head1;
    return q_size(head1);
}

/* Merge all the queues into one sorted queue, which is in ascending/descending
 * order */
int q_merge(struct list_head *head, bool descend)
{
    if (!head || list_empty(head))
        return 0;

    queue_contex_t *first = list_first_entry(head, queue_contex_t, chain);

    if (list_is_singular(head))
        return q_size(first->q);

    queue_contex_t *second =
        list_first_entry(first->chain.next, queue_contex_t, chain);

    size_t count = q_size(head);
    size_t size = 0;
    for (int i = 0; i < count - 2; i++) {
        size = _q_merge(first->q, second->q, descend);
        second = list_first_entry(second->chain.next, queue_contex_t, chain);
    }

    return size;
}
