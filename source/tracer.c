#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "Ip_Network.h"


void matrix_parser(char* matrice) {
    FILE* fd = fopen(matrice,"r");
    char* line = NULL;
    char* tokens = NULL;
    size_t len = 0;
    ssize_t read;
    int i = 0;

    if(fd == NULL) {
        fprintf(stderr, "Error opening file");
        exit(1);
    }
    while ((read = getline(&line, &len, fd)) != -1) {
        tokens = strtok(line, "\t");
        while(tokens != NULL) {
            if(atoi(tokens)){
              matrix[i][(tokens-line)/2]=atoi(tokens);
              matrix[(tokens-line)/2][i]=atoi(tokens);
              //printf("Matrix[%d][%d]=%d\n",i+1,(tokens-line)/2+1,atoi(tokens));
            }
            tokens = strtok(NULL, "\t");
        }
        i++;
    }
    free(line);
    free(tokens);
    fclose(fd);
}


void trace_all_packets(char* trace){
  FILE* fd = fopen(trace,"r");
  char *line=NULL;
  char *tokens=NULL;
  float time;
  int code = -1, pid, fid, tos, bif, s, d, pos;
  paquet* paquet_curr=NULL;

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
          sscanf(line, "%f %d %d %d %d %d N%d N%d N%d",
                  &time, &code, &pid, &fid, &tos, &bif, &s, &d, &pos);

      } else { // in this case the line has a different format
          sscanf(line, "%f %d %d %d %d N%d N%d N%d",
                  &time, &code, &pid, &fid, &tos, &s, &d, &pos);
      }

      paquet_curr=search_paquet(&paquets,pid);
      if(!paquet_curr){
        fprintf(stderr, "Unable to acces infos of Paquet  %d\n",pid);
        EXIT_FAILURE;
      }
       else {
          switch(code) {
          case 0:
              paquet_curr->source=s;
              paquet_curr->destination=d;
              paquet_curr->routeurs_liste[0]=s;
              paquet_curr->rout_inter_instant[0]=time;
              break;
          case 1:
              paquet_curr->nbr_routeur_intermediate++;
              paquet_curr->routeurs_liste[paquet_curr->nbr_routeur_intermediate]=pos;
              paquet_curr->rout_inter_instant[paquet_curr->nbr_routeur_intermediate]=time;

              //on calcule le débit en se basant sur le premier instant d'arrivée a un Noued intérmediaire
              if(paquet_curr->nbr_routeur_intermediate==1){
                float time_link=(time -paquet_curr->timestamp);//
                int bandwith=matrix[s-1][pos-1];
                paquet_curr->size=time_link*bandwith*1000000;
              }

              break;
          case 2:
              paquet_curr->rout_queue_instant[paquet_curr->nbr_routeur_intermediate]=time;
              if(paquet_curr->nbr_routeur_intermediate==0)paquet_curr->timestamp=time;//L'instant du premier départ de la file d'attente de la source
              break;
          case 3:
              paquet_curr->routeurs_liste[paquet_curr->nbr_routeur_intermediate+1]=pos;
              paquet_curr->arrival_time=time;
              break;
          case 4:
              break;
          }
      }
        tokens = strtok(NULL, "\t");
    }
  }
}


void trace_all_flux(char* trace){
  FILE* fd = fopen(trace,"r");
  FILE* cooldow_warmup;
  cooldow_warmup=fopen("flux_actifs.txt","w");
  if(cooldow_warmup == NULL) {
        fprintf(stderr, "Error opening flux_actifs variation file\n");
        exit(EXIT_FAILURE);
    }
  int temps_passe=0;
  char *line=NULL;
  char *tokens=NULL;
  float time;
  int code = -1, pid, fid, tos, bif, s, d, pos;
  flux* flux_curr=NULL;

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
          sscanf(line, "%f %d %d %d %d %d N%d N%d N%d",
                  &time, &code, &pid, &fid, &tos, &bif, &s, &d, &pos);

      } else { // in this case the line has a different format
          sscanf(line, "%f %d %d %d %d N%d N%d N%d",
                  &time, &code, &pid, &fid, &tos, &s, &d, &pos);
      }

      flux_curr=search_flux(&all_flux,fid);
      if(!flux_curr){
        fprintf(stderr, "Unable to acces infos of Flux  %d\n",pid);
        EXIT_FAILURE;
      }
       else {
          switch(code) {
          case 0:
              if(flux_curr->first_paquet_born)
                {
                  flux_curr->born_time=time;
                  flux_curr->first_paquet_born=false;
                  flux_curr->actif=true;
                }
                flux_curr->packets_all++;
              break;
          case 1:
              flux_curr->actif=true;
              break;
          case 2:
            flux_curr->actif=true;
              break;
          case 3:
              flux_curr->dead_time=time;
              flux_curr->actif=false;
              flux_curr->packets_received++;
              break;
          case 4:
              flux_curr->dead_time=time;
              flux_curr->actif=false;
              flux_curr->packets_lost++;
              break;
          }
      }
        tokens = strtok(NULL, "\t");
    }
    flux *node;
    int nombre_flux_actifs=0;
    if(time > (temps_passe*0.01)) {
      for (node = rb_first(&all_flux); node; node = rb_next(node)){
        flux* flu_curr=rb_entry(node,paquet, paquet_node);
        if(flu_curr->actif) nombre_flux_actifs++;
      }
      fprintf(cooldow_warmup, "%f %lu\n",time,nombre_flux_actifs );
      temps_passe++;
    }

  }
}
int main(int argc,char** argv){
    if(argc != 4) {
        printf("Usage: ./tracer <trace_file> <adj_matrix> <-p|-f>\n");
        exit(EXIT_FAILURE);
    }
    if(strcmp(argv[3],"-p")==0){
        clock_t start,stop;
        double elapsed;
        start=clock();
        printf("Parsing de la matrice %s...........\n",argv[2] );
        matrix_parser(argv[2]);
        printf("Construction de l'arbre bicolore contenant les infos de tous les paquets dans le fichier %s\n",argv[1]);
        init_infos_paquets();
        trace_all_packets(argv[1]);
        stop=clock();
        elapsed=((double)stop-start)/1000000;
        printf("Temps pour la construction de l'arbre des paquets: %f s\n",elapsed);
        int pid=3018;
        while(1){
          printf("Entrer le pid du paquet a tracer ou 0 pour quitter:  ");
          scanf("%d",&pid);
          if(pid==0){
            return 0;
          }
          paquet *paq=search(&paquets,pid);

          printf("Source: N%d\n",paq->source);
          printf("destination: N%d\n",paq->destination);
          printf("La taille est: %d\n",paq->size);

          for(int i=1;i<=paq->nbr_routeur_intermediate;i++){
            printf("Le paquet %d est dans la file d'attente du routeur N%d a l'instant: %f et ",pid,paq->routeurs_liste[i],paq->rout_queue_instant[i-1]);
            printf("traverse le routeur N%d a l'instant:%f\n",paq->routeurs_liste[i],paq->rout_inter_instant[i]);
          }
          printf("Le temps d'attente du paquet %d est: ",pid);
          float temps_total_attente_paquet=0;
          for(int i=0;i<paq->nbr_routeur_intermediate;i++){
            temps_total_attente_paquet = temps_total_attente_paquet + (paq->rout_queue_instant[i]-paq->rout_inter_instant[i]);
            printf(" +%fs(N%d)",paq->rout_queue_instant[i]-paq->rout_inter_instant[i],paq->routeurs_liste[i+1]);
          }
          printf(" = %fs\n",temps_total_attente_paquet);
          printf("Le temps de transmission du paquet %d est: ",pid);
          float temps_transmission=0;
          for(int i=0;i<paq->nbr_routeur_intermediate;i++){
            temps_transmission = temps_transmission + (paq->rout_inter_instant[i+1]-paq->rout_queue_instant[i]);
            printf(" +%fs(lien N%d-N%d)",(paq->rout_inter_instant[i+1]-paq->rout_queue_instant[i]),paq->routeurs_liste[i],paq->routeurs_liste[i+1]);
          }
          printf(" = %fs\n",temps_transmission);
          printf("Le paquet arrive a %f \n",paq->arrival_time);
        }
        free_info_paquets();
      }
      else if((strcmp(argv[3],"-f"))==0){
        clock_t start,stop;
        double elapsed;
        start=clock();
        printf("Construction de l'arbre bicolore contenant les infos de tous les flux dans le fichier %s\n",argv[1]);
        init_infos_flux();
        trace_all_flux(argv[1]);
        stop=clock();
        elapsed=((double)stop-start)/1000000;
        printf("Temps pour la construction de l'arbre des flux : %f s\n",elapsed);

        int fid=0;
        while(1){
          printf("Entrer le fid du flux a afficher infos ou -1 pour quitter:  ");
          scanf("%d",&fid);
          if(fid==-1){
            return 0;
          }
          flux *f=search(&all_flux,fid);
          printf("Total des paquets : %d\n",f->packets_all);
          printf("Nombre de paquets recu : %d\n",f->packets_received);
          printf("Nombre de paquets détruits : %d\n",f->packets_lost);
          printf("Taux de perte %f pourcent\n",100*(double)f->packets_lost/(double)f->packets_received);
          printf("La durée de vie du flux est %fs(dead) - %fs(born) = %fs\n",f->dead_time,f->born_time,f->dead_time-f->born_time);
        }

        free_info_flux();
      }
      free_info_flux();
      free_info_paquets();


  return 0;
}
