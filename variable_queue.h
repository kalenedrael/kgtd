#ifndef VARIABLE_QUEUE_H
#define VARIABLE_QUEUE_H

#include <stddef.h>
#define Q_HEAD(Q_ELEM_TYPE) struct {                     \
                              struct Q_ELEM_TYPE *head;  \
                              struct Q_ELEM_TYPE *tail;  \
                            }
#define Q_STATIC_INITIALIZER { NULL, NULL }
#define Q_NEW_LINK(Q_ELEM_TYPE) struct { struct Q_ELEM_TYPE *next, *prev; }
#define Q_INIT_HEAD(Q_HEAD) do { (Q_HEAD)->head = 0; (Q_HEAD)->tail = 0; } while(0)
#define Q_INIT_ELEM(Q_ELEM, LINK_NAME) do {                            \
                                         (Q_ELEM)->LINK_NAME.next = 0; \
                                         (Q_ELEM)->LINK_NAME.prev = 0; \
                                       } while(0)

#define Q_INSERT_HEAD(Q_HEAD, Q_ELEM, LINK_NAME)          \
do {                                                       \
	if((Q_HEAD)->head != 0) {                          \
		(Q_HEAD)->head->LINK_NAME.prev = Q_ELEM;   \
		(Q_ELEM)->LINK_NAME.next = (Q_HEAD)->head; \
	} else {                                           \
		(Q_HEAD)->tail = Q_ELEM;                   \
		(Q_ELEM)->LINK_NAME.next = 0;              \
	}                                                  \
	(Q_HEAD)->head = Q_ELEM;                           \
	(Q_ELEM)->LINK_NAME.prev = 0;                      \
} while(0)

#define Q_INSERT_TAIL(Q_HEAD, Q_ELEM, LINK_NAME)           \
do {                                                       \
	if((Q_HEAD)->tail != 0) {                          \
		(Q_HEAD)->tail->LINK_NAME.next = Q_ELEM;   \
		(Q_ELEM)->LINK_NAME.prev = (Q_HEAD)->tail; \
	} else {                                           \
		(Q_HEAD)->head = Q_ELEM;                   \
		(Q_ELEM)->LINK_NAME.prev = 0;              \
	}                                                  \
	(Q_HEAD)->tail = Q_ELEM;                           \
	(Q_ELEM)->LINK_NAME.next = 0;                      \
} while(0)

#define Q_GET_HEAD(Q_HEAD) ((Q_HEAD)->head)
#define Q_GET_TAIL(Q_HEAD) ((Q_HEAD)->tail)
#define Q_GET_NEXT(Q_ELEM, LINK_NAME) ((Q_ELEM)->LINK_NAME.next)
#define Q_GET_PREV(Q_ELEM, LINK_NAME) ((Q_ELEM)->LINK_NAME.prev)

#define Q_INSERT_AFTER(Q_HEAD,Q_INQ,Q_TOINSERT,LINK_NAME)               \
do {                                                                    \
	if((Q_HEAD)->tail == Q_INQ) {                                   \
		(Q_TOINSERT)->LINK_NAME.next = 0;                       \
		(Q_HEAD)->tail = Q_TOINSERT;                            \
	} else {                                                        \
		(Q_TOINSERT)->LINK_NAME.next = (Q_INQ)->LINK_NAME.next; \
		(Q_INQ)->LINK_NAME.next->LINK_NAME.prev = Q_TOINSERT;   \
	}                                                               \
	(Q_TOINSERT)->LINK_NAME.prev = Q_INQ;                           \
	(Q_INQ)->LINK_NAME.next = Q_TOINSERT;                           \
} while(0)

#define Q_INSERT_BEFORE(Q_HEAD,Q_INQ,Q_TOINSERT,LINK_NAME)              \
do {                                                                    \
	if((Q_HEAD)->head == Q_INQ) {                                   \
		(Q_TOINSERT)->LINK_NAME.prev = 0;                       \
		(Q_HEAD)->head = Q_TOINSERT;                            \
	} else {                                                        \
		(Q_TOINSERT)->LINK_NAME.prev = (Q_INQ)->LINK_NAME.prev; \
		(Q_INQ)->LINK_NAME.prev->LINK_NAME.next = Q_TOINSERT;   \
	}                                                               \
	(Q_TOINSERT)->LINK_NAME.next = Q_INQ;                           \
	(Q_INQ)->LINK_NAME.prev = Q_TOINSERT;                           \
} while(0)

#define Q_REMOVE(Q_HEAD,Q_ELEM,LINK_NAME)                                            \
do {                                                                                 \
	if((Q_ELEM)->LINK_NAME.prev)                                                 \
		(Q_ELEM)->LINK_NAME.prev->LINK_NAME.next = (Q_ELEM)->LINK_NAME.next; \
	else                                                                         \
		(Q_HEAD)->head = (Q_ELEM)->LINK_NAME.next;                           \
	if((Q_ELEM)->LINK_NAME.next)                                                 \
		(Q_ELEM)->LINK_NAME.next->LINK_NAME.prev = (Q_ELEM)->LINK_NAME.prev; \
	else                                                                         \
		(Q_HEAD)->tail = (Q_ELEM)->LINK_NAME.prev;                           \
} while(0)

#define Q_FOREACH(CURRENT_ELEM,Q_HEAD,LINK_NAME) \
for(CURRENT_ELEM = (Q_HEAD)->head; CURRENT_ELEM; CURRENT_ELEM = (CURRENT_ELEM)->LINK_NAME.next)

#endif
