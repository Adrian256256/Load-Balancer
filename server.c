/* Copyright 2023 <> */
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "utils.h"
#include "server.h"
#include "load_balancer.h"
#define HMAX 1000
#define KEY_LENGTH 128
#define VALUE_LENGTH 65536

linked_list_t *ll_create(unsigned int data_size)
{
    linked_list_t* ll;

    ll = malloc(sizeof(*ll));

    ll->head = NULL;
    ll->data_size = data_size;
    ll->size = 0;

    return ll;
}

/*
 * Pe baza datelor trimise prin pointerul new_data, se creeaza un nou nod care e
 * adaugat pe pozitia n a listei reprezentata de pointerul list. Pozitiile din
 * lista sunt indexate incepand cu 0 (i.e. primul nod din lista se afla pe
 * pozitia n=0). Daca n >= nr_noduri, noul nod se adauga la finalul listei. Daca
 * n < 0, eroare.
 */
void ll_add_nth_node(linked_list_t* list, unsigned int n, const void* new_data)
{
    ll_node_t *prev, *curr;
    ll_node_t* new_node;

    if (!list) {
        return;
    }

    /* n >= list->size inseamna adaugarea unui nou nod la finalul listei. */
    if (n > list->size) {
        n = list->size;
    }

    curr = list->head;
    prev = NULL;
    while (n > 0) {
        prev = curr;
        curr = curr->next;
        --n;
    }

    new_node = malloc(sizeof(*new_node));
    new_node->data = malloc(list->data_size);
    memcpy(new_node->data, new_data, list->data_size);

    new_node->next = curr;
    if (prev == NULL) {
        /* Adica n == 0. */
        list->head = new_node;
    } else {
        prev->next = new_node;
    }

    list->size++;
}

/*
 * Elimina nodul de pe pozitia n din lista al carei pointer este trimis ca
 * parametru. Pozitiile din lista se indexeaza de la 0 (i.e. primul nod din
 * lista se afla pe pozitia n=0). Daca n >= nr_noduri - 1, se elimina nodul de
 * la finalul listei. Daca n < 0, eroare. Functia intoarce un pointer spre acest
 * nod proaspat eliminat din lista. Este responsabilitatea apelantului sa
 * elibereze memoria acestui nod.
 */
ll_node_t *ll_remove_nth_node(linked_list_t* list, unsigned int n)
{
    ll_node_t *prev, *curr;

    if (!list || !list->head) {
        return NULL;
    }

    /* n >= list->size - 1 inseamna eliminarea nodului de la finalul listei. */
    if (n > list->size - 1) {
        n = list->size - 1;
    }

    curr = list->head;
    prev = NULL;
    while (n > 0) {
        prev = curr;
        curr = curr->next;
        --n;
    }

    if (prev == NULL) {
        /* Adica n == 0. */
        list->head = curr->next;
    } else {
        prev->next = curr->next;
    }

    list->size--;

    return curr;
}

/*
 * Functia intoarce numarul de noduri din lista al carei pointer este trimis ca
 * parametru.
 */
unsigned int ll_get_size(linked_list_t* list)
{
     if (!list) {
        return -1;
    }

    return list->size;
}

/*
 * Procedura elibereaza memoria folosita de toate nodurile din lista, iar la
 * sfarsit, elibereaza memoria folosita de structura lista si actualizeaza la
 * NULL valoarea pointerului la care pointeaza argumentul (argumentul este un
 * pointer la un pointer).
 */
void ll_free(linked_list_t** pp_list)
{
    ll_node_t* currNode;

    if (!pp_list || !*pp_list) {
        return;
    }

    while (ll_get_size(*pp_list) > 0) {
        currNode = ll_remove_nth_node(*pp_list, 0);
        free(currNode->data);
        currNode->data = NULL;
        free(currNode);
        currNode = NULL;
    }

    free(*pp_list);
    *pp_list = NULL;
}

server_memory *init_server_memory()
{
	server_memory* hashtable = malloc (sizeof(server_memory));
    DIE(NULL == hashtable, "server memory fail\n");

    hashtable->buckets = malloc(HMAX * sizeof (linked_list_t*));
	DIE(NULL == hashtable->buckets, "server memory fail\n");
    for (int i = 0; i < HMAX; i++) {
        hashtable->buckets[i] = ll_create(sizeof(info));
	}
    hashtable->size = 0;
	return hashtable;
}

void ht_put(server_memory *ht, void *key, unsigned int key_size,
	void *value, unsigned int value_size)
{
	info data;
	unsigned int index = hash_function_key(key) % HMAX;
	linked_list_t *bucket = ht->buckets[index];
    ll_node_t *node = bucket->head;
	for (unsigned int i = 0; i < bucket->size; i++) {
		if (strcmp((char *)key, ((char *)((info *)node->data)->key)) == 0) {
			break;
		}
		node = node->next;
	}
	if (!node) {
		data.key = malloc(key_size);
		DIE(NULL == data.key, "key memory fail");
		memcpy(data.key, key, key_size);
		data.value = malloc(value_size);
		DIE(NULL == data.value, "value memory fail");
		memcpy(data.value, value, value_size);
		ll_add_nth_node(bucket, 0, &data);
	} else {
		memcpy(((info *)node->data)->value, value, value_size);
	}
	ht->size++;
}

void server_store(server_memory *server, char *key, char *value) {
	ht_put(server, key, strlen(key) + 1, value, strlen(value) + 1);
}

void *ht_get(server_memory *ht, void *key)
{
	unsigned int index = hash_function_key(key) % HMAX;
	linked_list_t *bucket = ht->buckets[index];
    ll_node_t *current = bucket->head;

	for (unsigned int i = 0; i < bucket->size; i++) {
		if (strcmp((char *)key, ((char *)((info *)current->data)->key)) == 0) {
            return ((info *)current->data)->value;
		}
		current = current->next;
	}
	return NULL;
}

char *server_retrieve(server_memory *server, char *key) {
    char *value = ht_get(server, key);
    return value;
}

void ht_remove_entry(server_memory *ht, void *key)
{
	unsigned int index = hash_function_key(key) % HMAX;
	linked_list_t *bucket = ht->buckets[index];
    ll_node_t *node = bucket->head;
    int cnt = 0;
	for (unsigned int i = 0; i < bucket->size; ++i) {
		if (strcmp((char *)key, ((char *)((info *)node->data)->key)) == 0) {
            cnt = i;
            break;
		}
		node = node->next;
	}

	if (node != NULL) {
		ll_node_t *removed = ll_remove_nth_node(bucket, cnt);
		free(((info *)removed->data)->value);
		free(((info *)removed->data)->key);
		free(removed->data);
		free(removed);
		ht->size--;
	}
}

void server_remove(server_memory *server, char *key) {
	ht_remove_entry(server, key);
}

void ht_free(server_memory *ht)
{
	ll_node_t *current;
	for (unsigned int i = 0; i < HMAX; i++) {
		if (ht->buckets[i]->head != NULL) {
			current = ht->buckets[i]->head;
			while (current != NULL) {
				free(((info *)current->data)->value);
				free(((info *)current->data)->key);
				current = current->next;
			}
		}
		ll_free(&ht->buckets[i]);
	}

	free(ht->buckets);
	free(ht);
}

void free_server_memory(server_memory *server) {
	ht_free(server);
}

info **return_server_key_value(server_memory *server, unsigned int data_size) {
    info **data = malloc(data_size * sizeof(info*));
    for (unsigned int i = 0; i < data_size; i++) {
        data[i] = NULL;
    }
    unsigned int cnt = 0;
        for (unsigned int i = 0; i < HMAX; i++) {
            ll_node_t *curr = server->buckets[i]->head;
            while (curr != NULL) {
                data[cnt] = curr->data;
                cnt++;
                curr = curr->next;
            }
        }
    return data;
}
