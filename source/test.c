#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "rbtree.h"
#include <inttypes.h>
#include "Ip_Network.h"
#include "parser_timeline.h"
#include "tracer.h"

int main(){

/*  queue_infos *new=new_queue_infos();
  add_paquet_to_queue(new,2,0.01);
    printf("%d\n",new->head->pid);
  add_paquet_to_queue(new,5,0.01);
  printf("%d\n",new->head->pid);
  remove_head_from_queue(new);
  printf("%d\n",new->head->pid);
  add_paquet_to_queue(new,4,0.01);
  printf("%d\n",new->head->pid);*/
  /*init_infos_network();
  parse_timeline();*/
  /*paquet *new=malloc(sizeof(paquet));
  if(new == NULL) {
      fprintf(stderr, "Error allocating memory for paquet_infos \n");
      exit(EXIT_FAILURE);
  }
  init_paquet_infos(new);
  new->pid = 1;
  insert_paquet(&paquets,new);
  printf("%d\n",rb_entry(rb_first(&paquets),paquet, paquet_node)->source);*/
  //init_infos_network();
  clock_t start,stop;
  double elapsed;
  start=clock();

  matrix_parser();


  init_infos_paquets();

    stop=clock();
    elapsed=((double)stop-start)/1000000;
    printf("Temps pour l'affichage des données globales: %f s\n",elapsed);
  //parse_timeline();
  /*struct rb_node *node;
	printf("search all paquets: \n");
	for (node = rb_first(&paquets); node; node = rb_next(node)){
    paquet* paquet_curr=rb_entry(node,paquet, paquet_node);
    printf("paquet numéro:%d source:N%d destination:N%d \n ",paquet_curr->pid,paquet_curr->source,paquet_curr->destination);
  }*/
  /*int i=0;
  struct rb_node *node;
	printf("search all paquets: \n");
	for (node = rb_first(&paquets); node; node = rb_next(node)){
    paquet* paquet_curr=rb_entry(node,paquet, paquet_node);
    i++;
    if(i>26){
      break;
    }
    printf("paquet numéro:%d source:N%d destination:N%d \n ",paquet_curr->pid,paquet_curr->source,paquet_curr->destination);
  }*/
  trace_all_packets();
  paquet *firstpaq=search(&paquets,207505);
  for(int i=0;i<firstpaq->nbr_routeur_intermediate;i++){
    printf("%f\n",firstpaq->rout_queue_instant[i]);
    printf("%f\n",firstpaq->rout_inter_instant[i]);
  }
  printf("Le paquet arrive a %f \n",firstpaq->arrival_time);
  printf("La taille est: %d",firstpaq->size);
  return 0;
}
