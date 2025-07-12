# Load Balancer Implementation

---

## General Description

This project implements a load balancer using hash tables, leveraging concepts learned in the laboratory. The load balancer distributes key-value pairs across multiple server memories, each represented as a hash table.

---

## File Overview

- **main.c**  
  The program entry point, handling user interaction via commands read from an input file. It contains parsing functions `get_key_value` and `get_key` to extract keys and values from input lines, which are then managed by the load balancer for distribution across servers.

- **server.c**  
  Implements basic functions for linked lists, hash tables, and the `server_memory` structure:  
  - `init_server_memory`: allocates and initializes a new server.  
  - `server_store`: inserts a key-value pair into the server hash table.  
  - `server_retrieve`: retrieves a value by key.  
  - `server_remove_entry`: removes a key-value pair.  
  - `free_server_memory`: frees server resources.  
  - `return_server_key_value`: returns all key-value pairs stored in a server.

- **load_balancer.c**  
  Contains functions managing the load balancer:  
  - `hash_function_servers`: computes hash for server mask or ID.  
  - `hash_function_key`: computes hash for a key.  
  - `init_load_balancer`: initializes the load balancer structure.  
  - `loader_add_server`: adds a server, reallocating memory and maintaining a sorted hash ring; redistributes data among servers.  
  - `loader_remove_server`: removes a server, redistributes its data, and frees memory.  
  - `loader_store`: adds a key-value pair to the appropriate server via consistent hashing.  
  - `loader_retrieve`: retrieves a value by key from the correct server.  
  - `free_load_balancer`: frees load balancer resources.

- **load_balancer.h**  
  Defines the `load_balancer` structure containing:  
  - `hashring`: array of server hashes (sorted).  
  - `size`: number of hashes in the hashring.  
  - `servers`: array of pointers to servers.  
  - `size`: number of servers.

- **server.h**  
  Defines the `server_memory` structure representing a hash table with:  
  - `buckets`: array of linked lists.  
  - `server_id`: unique identifier for the server.  
  - `size`: number of buckets.

---

## Summary

This project demonstrates a consistent hashing load balancer using hash tables to distribute and manage key-value pairs across multiple servers dynamically. It supports adding and removing servers with data redistribution to maintain balanced load and high availability.

---
