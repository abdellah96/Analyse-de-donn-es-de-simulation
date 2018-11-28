#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include <sys/time.h>
#include "Ip_Network.h"


void global_data(char* file){
  FILE* fd = fopen(file,"r");
  char *line=NULL;
  char *tokens=NULL;
  static uint64_t packets_all, packets_lost_all, packets_send_all, packets_received_all;
  static uint64_t packets_current = 0;
  float time;
  int code = -1, pid, fid, tos, bif, s, d, pos;
  node_infos* nodeinf = NULL;

  size_t len = 0;
  ssize_t read;

  if(fd == NULL) {
    fprintf(stderr, "Error opening file \n");
    exit(1);
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
          sscanf(line, "%f %d %d %"SCNu64" %d %d N%"SCNu32" N%"SCNu32" N%"SCNu32,
                  &time, &code, &pid, &fid, &tos, &bif, &s, &d, &pos);
          nodeinf = search(&ip_network,pos);

      } else {
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
              break;
          case 1:
              nodeinf->queue_occupation++;
              nodeinf->node_queue_size=nodeinf->queue_occupation;
              break;
          case 2:
              packets_current++;
              nodeinf->packets_treated++;
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
              nodeinf->queue_occupation--;
              break;
          }
        }
        tokens = strtok(NULL, "\t");
      }
    }

  printf("Le nombre de paquets totale émis: %"PRIu64"\n",packets_all);
  printf("Le nombre de paquets totale recus %"PRIu64"\n",packets_received_all);
  printf("Le nombre de paquets totale perdus: %"PRIu64"\n",packets_lost_all);
  printf("\n");
  printf("Noued\tNbr-paquets_perdues\tProp_paquets_perdus\tTaux de perte\t Taille file d'attente\tpaquets traités\n");
  node_infos *node;
	for (node = rb_first(&ip_network); node; node = rb_next(node)){
    node_infos* node_curr=rb_entry(node,paquet, paquet_node);
    printf("N%"PRIu16"\t\t%"PRIu64"\t\t%f\t\t%f\t\t%"PRIu64"\t\t%"PRIu64"\n",
    node_curr->id,
    node_curr->packets_lost,
    100*(double)(node_curr->packets_lost/(double)packets_lost_all),
    100*(double)(node_curr->packets_lost/(double)(node_curr->packets_treated)),
    node_curr->node_queue_size,
    node_curr->packets_treated);
  }
  printf("\n");
}



int main(int argc, char **argv){
  clock_t start,stop;
  double elapsed;
  start=clock();

  init_infos_network();
  global_data(argv[1]);


  stop=clock();
  elapsed=((double)stop-start)/1000000;
  printf("Temps pour l'affichage des données globales: %f s\n",elapsed);
  free_info_network();
  return 0;

}
