#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "Ip_Network.h"



node_infos * search(struct rb_root *network,uint32_t id){
  struct rb_node *node = network->rb_node;

  while (node) {
    node_infos *data = container_of(node, node_infos, node);
    int result;

    result = id - data->id;

    if (result < 0)
        node = node->rb_left;
    else if (result > 0)
        node = node->rb_right;
    else
        return data;
      }
  return NULL;

}

paquet* search_paquet(struct rb_root *paquets,uint64_t pid){
  struct rb_node *paquet_node = paquets->rb_node;

  while (paquet_node) {
    paquet *data = container_of(paquet_node, paquet, paquet_node);
    int result;

    result = pid - data->pid;

    if (result < 0)
        paquet_node = paquet_node->rb_left;
    else if (result > 0)
        paquet_node = paquet_node->rb_right;
    else
        return data;
      }
  return NULL;
}

flux* search_flux(struct rb_root *all_flux,uint64_t fid){
  struct rb_node *flux_node = all_flux->rb_node;

  while (flux_node) {
    flux *data = container_of(flux_node, flux, flux_node);
    int result;

    result = fid - data->fid;

    if (result < 0)
        flux_node = flux_node->rb_left;
    else if (result > 0)
        flux_node = flux_node->rb_right;
    else
        return data;
      }
  return NULL;
}

int insert(struct rb_root *network, node_infos *node){
  struct rb_node **new = &(network->rb_node), *parent = NULL;

  /* Figure out where to put new node */
  while (*new) {
    node_infos *this = container_of(*new, node_infos, node);
    int result = node->id - this->id;

    parent = *new;
    if (result < 0)
      new = &((*new)->rb_left);
    else if (result > 0)
      new = &((*new)->rb_right);
    else
      return 0;
  }

  /* Add new node and rebalance tree. */
  rb_link_node(&node->node, parent, new);
  rb_insert_color(&node->node, network);

  return 1;

}

int insert_paquet(struct rb_root *paquets, paquet* paquet_node){
  struct rb_node **new = &(paquets->rb_node), *parent = NULL;
  /* Figure out where to put new node */
  while (*new) {
    paquet *this = container_of(*new, paquet, paquet_node);
    int result = paquet_node->pid - this->pid;

    parent = *new;
    if (result < 0)
      new = &((*new)->rb_left);
    else if (result > 0)
      new = &((*new)->rb_right);
    else
      return 0;
  }

  /* Add new node and rebalance tree. */
  rb_link_node(&paquet_node->paquet_node, parent, new);
  rb_insert_color(&paquet_node->paquet_node, paquets);

  return 1;

}

int insert_flux(struct rb_root *all_flux, flux* flux_node){
  struct rb_node **new = &(all_flux->rb_node), *parent = NULL;
  /* Figure out where to put new node */
  while (*new) {
    flux *this = container_of(*new, flux, flux_node);
    int result = flux_node->fid - this->fid;

    parent = *new;
    if (result < 0)
      new = &((*new)->rb_left);
    else if (result > 0)
      new = &((*new)->rb_right);
    else
      return 0;
  }

  /* Add new node and rebalance tree. */
  rb_link_node(&flux_node->flux_node, parent, new);
  rb_insert_color(&flux_node->flux_node, all_flux);

  return 1;

}


void init_node_infos(node_infos *node_init) {
    node_init->id = -1;
    node_init->packets_treated = 0;
    node_init->packets_transmitted = 0;
    node_init->packets_received = 0;
    node_init->packets_lost = 0;
    node_init->node_queue_size=0;
}

void init_paquet_infos(paquet* paquet){
  paquet->pid= -1 ;
  paquet->source=0;
  paquet->destination=0;
  paquet->size=0;
  paquet->timestamp=0;
  paquet->arrival_time=0;
  paquet->nbr_routeur_intermediate=0;
}

void init_flux_infos(flux *flux_init){
  flux_init->fid=-1;
  flux_init->packets_all=0;
  flux_init->packets_lost=0;
  flux_init->packets_received=0;
  flux_init->first_paquet_born=true;
  flux_init->born_time=0;
  flux_init->dead_time=0;
  flux_init->actif=false;
}


int init_infos_network() {

    /* *insert */
  	int i = 0;
  	//printf("insert node from 1 to NUM_NODES(26): \n");
  	for (i = 1; i <= NUM_NODES; i++) {
      node_infos *mn=malloc(sizeof(node_infos));
      if(mn == NULL) {
          fprintf(stderr, "Error allocating memory for node_stat #%d\n", i);
          exit(EXIT_FAILURE);
      }
      init_node_infos(mn);
      mn->id = i;
  		insert(&ip_network, mn);
      //printf("Inséré");
  	}

    return 0;
}

int init_infos_paquets(){
  /* *insert */
  int i = 0;
  //printf("insert node from 1 to NUM_NODES(26): \n");
  for (i = 0; i <= 800000; i++) {
    paquet *mn=malloc(sizeof(paquet));
    if(mn == NULL) {
        fprintf(stderr, "Error allocating memory for paquet_stat #%d\n", i);
        exit(EXIT_FAILURE);
    }
    init_paquet_infos(mn);
    mn->pid = i;
    insert_paquet(&paquets, mn);
  }

  return 0;
}

int init_infos_flux(){
  /* *insert */
  int i = 0;
  //printf("insert node from 1 to NUM_NODES(26): \n");
  for (i = 0; i <= 3500; i++) {
    flux *mn=malloc(sizeof(flux));
    if(mn == NULL) {
        fprintf(stderr, "Error allocating memory for flux_stat #%d\n", i);
        exit(EXIT_FAILURE);
    }
    init_flux_infos(mn);
    mn->fid = i;
    insert_flux(&all_flux, mn);
  }

  return 0;
}




void free_info_network() {
  struct rb_node *node;
  for (node = rb_first(&ip_network); node; node = rb_next(node)){
    free(rb_entry(node,node_infos, node));
  }
}

void free_info_paquets(){
  struct rb_node *node;
  for (node = rb_first(&paquets); node; node = rb_next(node)){
    free(rb_entry(node,paquet, paquet_node));
  }
}
void free_info_flux(){
  struct rb_node *node;
  for (node = rb_first(&all_flux); node; node = rb_next(node)){
    free(rb_entry(node,flux,flux_node));
  }
}



queue_infos *new_queue_infos() {
    queue_infos *new = malloc(sizeof(queue_infos));

    new->head = NULL;
    new->tail  = NULL;
    new->next  = NULL;

    return new;
}

void add_paquet_to_queue(queue_infos *queue,uint64_t pid, float timestamp){
    paquet *new_paquet = malloc(sizeof(paquet));
    new_paquet->pid = pid;
    new_paquet->timestamp = timestamp;
    new_paquet->next = NULL;
    //printf("%d\b", new_paquet->pid);

    if(queue->head == NULL) {
        queue->head = new_paquet;
        queue->tail = new_paquet;
        queue->next = NULL;
    } else {
        queue->tail->next = new_paquet;
        queue->tail = new_paquet;
    }
}

void remove_tail_from_queue(queue_infos *queue){
    if(queue->tail != NULL) {
        paquet *tmp = queue->tail;
        free(queue->tail);
        if(tmp->next == NULL)
            queue->tail = NULL;
        else queue->tail = tmp->next;
    }
}
