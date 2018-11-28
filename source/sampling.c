#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "parser_timeline.h"
#include "Ip_Network.h"

#define temps_echantillonage 0.1

void sampling(int queue_pid){
  FILE* fd = fopen("trace2650.txt","r");
  char *line=NULL;
  char *tokens=NULL;
  static uint64_t packets_all, packets_lost_all, packets_send_all, packets_received_all;
  static uint64_t packets_current = 0;
  //static uint64_t n4_losses_current = 0;
  static int times_went_through = 0;
  float time;
  int code = -1, pid, fid, tos, bif, s, d, pos;
  node_infos* nodeinf = NULL;

  size_t len = 0;
  ssize_t read;

  if(fd == NULL) {
    fprintf(stderr, "Error opening file \n");
    exit(1);
  }

  FILE* transit = fopen("transit.txt", "w");
  if(transit == NULL) {
      fprintf(stderr, "Error opening transit variation file\n");
      exit(EXIT_FAILURE);
  }

  FILE* destructions = fopen("destructions.txt", "w");
  if(destructions == NULL) {
      fprintf(stderr, "Error opening destructions variation file\n");
      exit(EXIT_FAILURE);
  }

  FILE *routeur_file=fopen("emitted_routeur26.txt","w");
  if(routeur_file == NULL) {
      fprintf(stderr, "Error opening transit variation queue %d\n",queue_pid);
      exit(EXIT_FAILURE);
  }

  queue_infos *queue=new_queue_infos();
  char queue_variation_file[10];
  sprintf(queue_variation_file,"%d",queue_pid);
  strcat(queue_variation_file,".txt");
  FILE *queue_file=fopen(queue_variation_file,"w");
  if(queue_file == NULL) {
      fprintf(stderr, "Error opening transit variation queue %d\n",queue_pid);
      exit(EXIT_FAILURE);
  }


  while((read = getline(&line,&len,fd)) != -1 ){
    tokens = strtok(line, "\t");
    while(tokens != NULL) {

      for (size_t i = 0; code == -1; i++) {
          if(line[i] == ' ') {
              code = atoi(&line[i+1]);
              break;
          }
      }

      if(code != 4) {
          sscanf(line, "%f %d %d %d %d %d N%d N%d N%d",
                  &time, &code, &pid, &fid, &tos, &bif, &s, &d, &pos);
          nodeinf = search(&ip_network,pos);

      } else { // in this case the line has a different format
          sscanf(line, "%f %d %d %d %d N%d N%d N%d",
                  &time, &code, &pid, &fid, &tos, &s, &d, &pos);
          nodeinf = search(&ip_network,pos);
      }

      if (!nodeinf) {
          fprintf(stdout,
                  "Unable to access stats map object with node id %d.\n",
                  pos);
          EXIT_FAILURE;
      } else {
          switch(code) {
          case 0:

              packets_all++;
              packets_send_all++;
              packets_current++;
              nodeinf->packets_treated++;
              nodeinf->packets_transmitted++;

              if(pos == queue_pid) {
                  add_paquet_to_queue(queue, pid, time);
                  fprintf(queue_file, "%.9f %d\n", time, nodeinf->queue_occupation);
              }

              break;
          case 1:
              nodeinf->queue_occupation++;
              nodeinf->node_queue_size=nodeinf->queue_occupation;
              if(pos == queue_pid) {
                  add_paquet_to_queue(queue, pid, time);
                  fprintf(queue_file, "%.9f %d\n", time, nodeinf->queue_occupation);
              }
              break;

          case 2:
              packets_current++;
              nodeinf->packets_treated++;

              //printf("%d\n", queue_9->first->value);
              if( !queue && queue->head->pid == pid) {
                  node_infos *node_infos = search(&ip_network,queue_pid);
                  node_infos->queue_occupation--;
                  remove_tail_from_queue(queue);
                  fprintf(queue_file, "%.9f %d\n", time, node_infos->queue_occupation);
              }
              break;

          case 3:
              packets_received_all++;
              packets_current--;
              nodeinf->packets_received++;
              break;

          case 4:
              packets_lost_all++;
              packets_current--;
              nodeinf->packets_lost++;
              //if(pos == 4) n4_losses_current++;
              nodeinf->queue_occupation--;
              if(nodeinf->node_queue_size <= nodeinf->queue_occupation)
                nodeinf->node_queue_size=nodeinf->queue_occupation;

              if(pos == queue_pid) {
                  fprintf(queue_file, "%.9f %d\n", time, nodeinf->queue_occupation);
              }
              break;

        }



        if(time > (times_went_through*temps_echantillonage)) {
            fprintf(transit, "%f %lu\n", time, packets_current);
            fprintf(destructions, "%f %lu\n", time, packets_lost_all);
            /*fprintf(losses_n4, "%f %lu\n", time, n4_losses_current);*/
            packets_current = 0;
            packets_lost_all=0;

            node_infos* routeur=search(&ip_network,queue_pid);
            fprintf(routeur_file, "%d\n",routeur->packets_transmitted);
            routeur->packets_transmitted=0;

            //n4_losses_current = 0;
            times_went_through++;
        }
        tokens = strtok(NULL, "\t");
      }
    }
  }
  /*printf("Le nombre de paquets totale émis: %d\n",packets_all);
  printf("Le nombre de paquets totale recus %d\n",packets_received_all);
  printf("Le nombre de paquets totale perdus: %d\n",packets_lost_all);
  printf("\n");
  printf("Noued\tProportion de paquets perdus\tTaux de perte\t Taille file d'attente\n");
  node_infos *node;
	for (node = rb_first(&ip_network); node; node = rb_next(node)){
    node_infos* node_curr=rb_entry(node,paquet, paquet_node);
    printf("N%d\t\t%f\t\t%f\t\t%d\n",node_curr->id,100*(double)(node_curr->packets_lost/(double)packets_lost_all),
    100*(double)(node_curr->packets_lost/(double)(node_curr->packets_treated)),node_curr->node_queue_size);
  }
  printf("\n");*/

}


int main(int argc, char **argv){
    init_infos_network();
    sampling(atoi(argv[1]));
    /*queue_infos *queue_4=new_queue_infos();
    add_paquet_to_queue(queue_4,1,0.01);
    printf("%d",queue_4->head->pid);
    remove_head_from_queue(queue_4);
    if(!queue_4->head)printf("%d",queue_4->head->pid);*/



}
