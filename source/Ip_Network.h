#ifndef IP_NETWORK_H__
#define IP_NETWORK_H__

#include <stdint.h>
#include <stdbool.h>
#include "rbtree.h"

#define NUM_NODES 26

struct rb_root ip_network;
int matrix[NUM_NODES][NUM_NODES];


typedef struct node_infos{
  struct rb_node node;
  uint32_t id;
  uint64_t packets_treated;
  uint64_t packets_transmitted;
  uint64_t packets_received;
  uint64_t packets_lost;
  uint32_t queue_occupation;
  uint64_t node_queue_size;
}node_infos;

struct rb_root paquets;

typedef struct paquet{
  struct rb_node paquet_node;
  uint64_t pid;
  uint64_t source;
  uint64_t destination;
  int size;
  uint16_t nbr_routeur_intermediate;
  int routeurs_liste[NUM_NODES];
  float rout_inter_instant[NUM_NODES];
  float rout_queue_instant[NUM_NODES];
  float timestamp;//L'instant du premier départ de la file d'attente de la source
  float arrival_time;
  struct paquet *next;
} paquet;

typedef struct {
  paquet *head;
  paquet *tail;
  paquet *next;
}queue_infos;

struct rb_root all_flux;
typedef struct flux{
  struct rb_node flux_node;
  uint64_t fid;
  uint64_t packets_all;
  uint64_t packets_lost;
  uint64_t packets_received;
  bool first_paquet_born;
  float born_time;
  float dead_time;
  bool actif;
}flux;

node_infos * search(struct rb_root *network,uint32_t pos);

int insert(struct rb_root *network, node_infos *node);

paquet* search_paquet(struct rb_root *paquets,uint64_t pid);

int insert_paquet(struct rb_root *paquets,paquet *paquet_node);

flux* search_flux(struct rb_root *all_flux,uint64_t fid);

int insert_flux(struct rb_root *all_flux,flux *flux_node);


void init_node_infos(node_infos *node_init);

void init_paquet_infos(paquet *paquet_init);

void init_flux_infos(flux *flux_init);

int init_infos_network();

int init_infos_paquets();

int init_infos_flux();

void free_info_network();

void free_info_paquets();

void free_info_flux();

queue_infos *new_queue_infos();

void add_paquet_to_queue(queue_infos *queue,uint64_t pid, float timestamp);

//Drop tailing
void remove_tail_from_queue(queue_infos *queue);

#endif
