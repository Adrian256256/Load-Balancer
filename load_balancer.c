/* Copyright 2023 <> */
#include <stdlib.h>
#include <string.h>
#include "utils.h"
#include "load_balancer.h"
#include "server.h"
#define MAX_SERVERS 99999
#define KEY_LENGTH 128
#define VALUE_LENGTH 65536
#define REQUEST_LENGTH 1024
#define HMAX 1000

unsigned int hash_function_servers(void *a) {
    unsigned int uint_a = *((unsigned int *)a);

    uint_a = ((uint_a >> 16u) ^ uint_a) * 0x45d9f3b;
    uint_a = ((uint_a >> 16u) ^ uint_a) * 0x45d9f3b;
    uint_a = (uint_a >> 16u) ^ uint_a;
    return uint_a;
}

unsigned int hash_function_key(void *a) {
    unsigned char *puchar_a = (unsigned char *)a;
    unsigned int hash = 5381;
    int c;

    while ((c = *puchar_a++))
        hash = ((hash << 5u) + hash) + c;

    return hash;
}

load_balancer *init_load_balancer() {
    load_balancer *load_balancer_curr = malloc(sizeof(load_balancer));
    DIE(NULL == load_balancer_curr, "load balancer fail\n");
    load_balancer_curr->size = 0;
    load_balancer_curr->hash_ring = NULL;
    load_balancer_curr->hash_ring_size = 0;
    load_balancer_curr->servers = NULL;
    return load_balancer_curr;
}

void loader_add_server(load_balancer *main, int server_id) {
    main->servers =
    realloc(main->servers, (main->size + 1) * sizeof(server_memory*));
    main->servers[main->size] = init_server_memory();
    server_memory *curr_server = main->servers[main->size];
    curr_server->server_id = server_id;
    main->size++;

    int tag = server_id;
    int ok = 0;
    // adaugam noile hash-uri la etichete in hash_ring
    unsigned int curr_hash = hash_function_servers(&tag);
    unsigned int hash_1 = curr_hash;
    if (main->hash_ring_size == 0) {
        main->hash_ring = realloc(main->hash_ring,
        (main->hash_ring_size + 1) * sizeof(unsigned int));
        main->hash_ring[0] = curr_hash;
        main->hash_ring_size++;
    } else {
        for (unsigned int i = 0; i < main->hash_ring_size; i++) {
            if (curr_hash <= main->hash_ring[i]) {
                main->hash_ring = realloc(main->hash_ring,
                (main->hash_ring_size + 1) * sizeof(unsigned int));
                for (unsigned int j = main->hash_ring_size; j > i; j--) {
                    main->hash_ring[j] = main->hash_ring[j - 1];
                }
                main->hash_ring[i] = curr_hash;
                ok = 1;
                main->hash_ring_size++;
                break;
            }
        }
        if (ok == 0) {
            main->hash_ring = realloc(main->hash_ring,
            (main->hash_ring_size + 1) * sizeof(unsigned int));
            main->hash_ring[main->hash_ring_size] = curr_hash;
            main->hash_ring_size++;
        }
    }

    ok = 0;
    tag = 1*100000 + server_id;
    curr_hash = hash_function_servers(&tag);
    unsigned int hash_2 = curr_hash;
    if (main->hash_ring_size == 0) {
        main->hash_ring = realloc(main->hash_ring,
        (main->hash_ring_size + 1) * sizeof(unsigned int));
        main->hash_ring[0] = curr_hash;
        main->hash_ring_size++;
    } else {
        for (unsigned int i = 0; i < main->hash_ring_size; i++) {
            if (curr_hash <= main->hash_ring[i]) {
                main->hash_ring = realloc(main->hash_ring,
                (main->hash_ring_size + 1) * sizeof(unsigned int));
                for (unsigned int j = main->hash_ring_size; j > i; j--) {
                    main->hash_ring[j] = main->hash_ring[j - 1];
                }
                main->hash_ring[i] = curr_hash;
                ok = 1;
                main->hash_ring_size++;
                break;
            }
        }
        if (ok == 0) {
            main->hash_ring = realloc(main->hash_ring,
            (main->hash_ring_size + 1) * sizeof(unsigned int));
            main->hash_ring[main->hash_ring_size] = curr_hash;
            main->hash_ring_size++;
        }
    }

    ok = 0;
    tag = 2*100000 + server_id;
    curr_hash = hash_function_servers(&tag);
    unsigned int hash_3 = curr_hash;
    if (main->hash_ring_size == 0) {
        main->hash_ring = realloc(main->hash_ring,
        (main->hash_ring_size + 1) * sizeof(unsigned int));
        main->hash_ring[0] = curr_hash;
        main->hash_ring_size++;
    } else {
        for (unsigned int i = 0; i < main->hash_ring_size; i++) {
            if (curr_hash <= main->hash_ring[i]) {
                main->hash_ring = realloc(main->hash_ring,
                (main->hash_ring_size + 1) * sizeof(unsigned int));
                for (unsigned int j = main->hash_ring_size; j > i; j--) {
                    main->hash_ring[j] = main->hash_ring[j - 1];
                }
                main->hash_ring[i] = curr_hash;
                ok = 1;
                main->hash_ring_size++;
                break;
            }
        }
        if (ok == 0) {
            main->hash_ring = realloc(main->hash_ring,
            (main->hash_ring_size + 1) * sizeof(unsigned int));
            main->hash_ring[main->hash_ring_size] = curr_hash;
            main->hash_ring_size++;
        }
    }

    for (unsigned int i = 0; i < main->hash_ring_size; i++) {
        curr_hash = main->hash_ring[i];
        if (curr_hash == hash_1 || curr_hash == hash_2 || curr_hash == hash_3) {
            // luam hash-ul mai mare, adica in sensul acelor de ceasornic
            unsigned int hash_to_see;
            if (i < main->hash_ring_size - 1)
                hash_to_see = main->hash_ring[i + 1];
            else
                hash_to_see = main->hash_ring[0];
            for (unsigned int j = 0; j < main->size; j++) {
                unsigned int curr_server_id = main->servers[j]->server_id;
                unsigned int curr_aux = curr_server_id;
                unsigned int curr_hash_to_see_1;
                curr_hash_to_see_1 = hash_function_servers(&curr_server_id);
                curr_server_id = 1 * 100000 + curr_aux;
                unsigned int curr_hash_to_see_2;
                curr_hash_to_see_2 = hash_function_servers(&curr_server_id);
                curr_server_id = 2 * 100000 + curr_aux;
                unsigned int curr_hash_to_see_3;
                curr_hash_to_see_3 = hash_function_servers(&curr_server_id);

                if (curr_hash_to_see_1 == hash_to_see ||
                    curr_hash_to_see_2 == hash_to_see ||
                    curr_hash_to_see_3 == hash_to_see) {
                    // am gasit serverul caruia ii apartine hash-ul
                    server_memory *server = main->servers[j];
                    for (unsigned int k = 0; k < HMAX; k++) {
                        ll_node_t *curr = server->buckets[k]->head;
                        while (curr != NULL) {
                            // scoatem key si value
                            // le adaugam iar
                            unsigned int key_l;
                            key_l = strlen(((info*)curr->data)->key) + 1;
                            char *key = malloc(key_l);
                            unsigned int value_l;
                            value_l = strlen(((info*)curr->data)->value) + 1;
                            char *value = malloc(value_l);
                            strcpy(key, ((info*)curr->data)->key);
                            strcpy(value, ((info*)curr->data)->value);
                            curr = curr->next;
                            ht_remove_entry(server, key);
                            int aux;
                            loader_store(main, key, value, &aux);
                            free(key);
                            free(value);
                        }
                    }
                }
            }
        }
    }
}

void loader_remove_server(load_balancer *main, int server_id) {
    info **data = NULL;
    unsigned int data_size = 0;
    server_memory *server_to_delete;
    // scoatem serverul din main->servers
    for (unsigned int i = 0; i < main->size; i++) {
        if (main->servers[i]->server_id == server_id) {
            data_size = main->servers[i]->size;
            // scoatem datele din serverul pe care vrem sa il stergem
            data = return_server_key_value(main->servers[i], data_size);
            server_to_delete = main->servers[i];
            for (unsigned int j = i; j < main->size - 1; j++) {
                main->servers[j] = main->servers[j + 1];
            }
            main->size--;
            break;
        }
    }
    main->servers =
    realloc(main->servers, (main->size) * sizeof(server_memory*));
    // scoatem hash-urile serverului scos din hash_ring
    unsigned int curr_hash = hash_function_servers(&server_id);
    for (unsigned int i = 0; i < main->hash_ring_size; i++) {
        if (curr_hash == main->hash_ring[i]) {
            for (unsigned int j = i; j < main->hash_ring_size - 1; j++) {
                main->hash_ring[j] = main->hash_ring[j + 1];
            }
            main->hash_ring = realloc(main->hash_ring,
            (main->hash_ring_size - 1) * sizeof(unsigned int));
            main->hash_ring_size--;
            break;
        }
    }

    unsigned int original_id = server_id;
    server_id = 1 * 100000 + original_id;
    curr_hash = hash_function_servers(&server_id);
    for (unsigned int i = 0; i < main->hash_ring_size; i++) {
        if (curr_hash == main->hash_ring[i]) {
            for (unsigned int j = i; j < main->hash_ring_size - 1; j++) {
                main->hash_ring[j] = main->hash_ring[j + 1];
            }
            main->hash_ring = realloc(main->hash_ring,
            (main->hash_ring_size - 1) * sizeof(unsigned int));
            main->hash_ring_size--;
            break;
        }
    }

    server_id = 2 * 100000 + original_id;
    curr_hash = hash_function_servers(&server_id);
    for (unsigned int i = 0; i < main->hash_ring_size; i++) {
        if (curr_hash == main->hash_ring[i]) {
            for (unsigned int j = i; j < main->hash_ring_size - 1; j++) {
                main->hash_ring[j] = main->hash_ring[j + 1];
            }
            main->hash_ring = realloc(main->hash_ring,
            (main->hash_ring_size - 1) * sizeof(unsigned int));
            main->hash_ring_size--;
            break;
        }
    }
    for (unsigned int i = 0; i < data_size; i++) {
        int aux;
        if (data[i] != NULL)
            loader_store(main, data[i]->key, data[i]->value, &aux);
    }
    free_server_memory(server_to_delete);
    free(data);
}

void loader_store(load_balancer *main, char *key, char *value, int *server_id) {
    // cautam cel mai apropiat hash clock-wise
    unsigned int curr_hash = hash_function_key(key);
    unsigned int wanted_hash;
    int ok = 0;
    for (unsigned int i = 0; i < main->hash_ring_size - 1; i++) {
        if (curr_hash > main->hash_ring[i] &&
            curr_hash <= main->hash_ring[i + 1]) {
            wanted_hash = main->hash_ring[i + 1];
            ok = 1;
            break;
        }
    }
    if (ok == 0) {
        wanted_hash = main->hash_ring[0];
    }
    // cautam serverul caruia ii apartine acest hash
    // verificam toate cele 3 tag-uri ale serverelor
    // server_id devine id ul serverului cu care lucram

    int nr_servers = main->size;
    for (int i = 0; i < nr_servers; i++) {
        int curr_tag = main->servers[i]->server_id;
        int orig_tag = curr_tag;
        if (hash_function_servers(&curr_tag) == wanted_hash) {
            *server_id = (main->servers[i]->server_id);
            break;
        }

        curr_tag = 1*100000 + orig_tag;
        if (hash_function_servers(&curr_tag) == wanted_hash) {
            *server_id = (main->servers[i]->server_id);
            break;
        }

        curr_tag = 2*100000 + orig_tag;
        if (hash_function_servers(&curr_tag) == wanted_hash) {
            *server_id = (main->servers[i]->server_id);
            break;
        }
    }

    // stocam informatia in serverul pe care l am gasit
    for (unsigned int i = 0; i < main->size; i++) {
        if (main->servers[i]->server_id == (*server_id)) {
            server_memory *server = main->servers[i];
            server_store(server, key, value);
            break;
        }
    }
}

char *loader_retrieve(load_balancer *main, char *key, int *server_id) {
    // cautam cel mai apropiat hash clock-wise
    unsigned int curr_hash = hash_function_key(key);
    unsigned int wanted_hash;

    int ok = 0;
    for (unsigned int i = 0; i < main->hash_ring_size - 1; i++) {
        if (curr_hash > main->hash_ring[i] &&
            curr_hash <= main->hash_ring[i + 1]) {
            wanted_hash = main->hash_ring[i + 1];
            ok = 1;
            break;
        }
    }
    if (ok == 0) {
        wanted_hash = main->hash_ring[0];
    }
    // cautam serverul caruia ii apartine acest hash
    // verificam toate cele 3 tag-uri ale serverelor
    // server_id devine id ul serverului cu care lucram
    int nr_servers = main->size;
    for (int i = 0; i < nr_servers; i++) {
        int curr_tag = main->servers[i]->server_id;
        int orig_tag = curr_tag;
        if (hash_function_servers(&curr_tag) == wanted_hash) {
            *server_id = (main->servers[i]->server_id);
            break;
        }

        curr_tag = 1*100000 + orig_tag;
        if (hash_function_servers(&curr_tag) == wanted_hash) {
            *server_id = (main->servers[i]->server_id);
            break;
        }

        curr_tag = 2*100000 + orig_tag;
        if (hash_function_servers(&curr_tag) == wanted_hash) {
            *server_id = (main->servers[i]->server_id);
            break;
        }
    }
    // preluam informatia din serverul pe care l am gasit
    for (unsigned int i = 0; i < main->size; i++) {
        if (main->servers[i]->server_id == (*server_id)) {
            server_memory *server = main->servers[i];
            char *value = server_retrieve(server, key);
            return value;
        }
    }

    return NULL;
}

void free_load_balancer(load_balancer *main) {
    free(main->hash_ring);
    for (unsigned int i = 0; i < main->size; i++) {
        ht_free(main->servers[i]);
    }
    free(main->servers);
    free(main);
}
