#include "location.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_SZ_SITES 50


struct Location  vaccination_sites[MAX_SZ_SITES];

int numLocations=0; //numero de localizacoes cadastradas


struct Location* findLocation(int x, int y){

  struct Location* aux = vaccination_sites;

  for(int i=0; i< numLocations;i++){
    if(vaccination_sites[i].x == x && vaccination_sites[i].y == y){
      return aux+i;
    }
  }
  return NULL;
}

int findPosition(int x, int y){
  for(int i=0; i< numLocations;i++){
    if(vaccination_sites[i].x == x && vaccination_sites[i].y == y){
      return i;
    }
  }
  return -1;
}



void addLogMsg(char* m){
  strcat(client_log.msg,m);
}

char * getLogMsg(){
  return client_log.msg;
}

void clearClientLog(){
  memset(client_log.msg,0,MSGSZ);
  memset(client_log.buffer,0,MSGSZ);
}

void addLocation(int x, int y){
//testar se ja existe
  if(findLocation(x,y) !=NULL){
    sprintf(client_log.buffer,"%i %i already exists\n",x,y);
  }

//testar se está cheio
 else if(numLocations==MAX_SZ_SITES){
    
    sprintf(client_log.buffer,"limit exceeded\n");
  }

//adicionar
  else{
    vaccination_sites[numLocations].x = x;
    vaccination_sites[numLocations].y = y;
    sprintf(client_log.buffer,"%i %i added\n",x,y);
    numLocations++;
    }

  addLogMsg(client_log.buffer);
  
}

void removeLocation(int x, int y){
  struct Location* l = findLocation(x,y);
  if(l==NULL){
    sprintf(client_log.buffer,"%i %i does not exist\n",x,y);

  }
  else{
    
    int pos = findPosition(x,y);
    for(int a =pos;a<numLocations-1;a++){
      vaccination_sites[a].x = vaccination_sites[a+1].x ;
      vaccination_sites[a].y = vaccination_sites[a+1].y ;
    }
    numLocations--;
    sprintf(client_log.buffer,"%i %i removed\n",x,y);
  }
  addLogMsg(client_log.buffer);
}

void listLocations(){
  
  if(numLocations==0){
    sprintf(client_log.buffer,"none\n");
    addLogMsg(client_log.buffer);
  }
  else{
    char aux[MSGSZ]="";
    for(int i = 0; i< numLocations;i++){
      sprintf(client_log.buffer,"%i %i ",vaccination_sites[i].x,vaccination_sites[i].y);
      strcat(aux,client_log.buffer);
    }
    aux[strlen(aux)-1]='\n';
    addLogMsg(aux);
  }

}
double EuclideanDistance(struct Location l1,struct Location l2){
  double deltaX=l1.x-l2.x;
  double deltaY=l1.y-l2.y;
  return sqrt(pow(deltaX,2) + pow(deltaY,2));
}
void query(int x, int y){
  if(numLocations==0){
    sprintf(client_log.buffer,"none\n");
  }
  else{
    struct Location clientLocation;
    clientLocation.x = x;
    clientLocation.y = y;
    struct Location closestLocation;
    closestLocation = vaccination_sites[0];
    for(int i=1; i< numLocations;i++){
      if(EuclideanDistance(clientLocation,vaccination_sites[i]) <EuclideanDistance(clientLocation,closestLocation) ){
        closestLocation = vaccination_sites[i];
      }
    }
    sprintf(client_log.buffer,"%i %i\n",closestLocation.x,closestLocation.y);
  }

  addLogMsg(client_log.buffer);
}
