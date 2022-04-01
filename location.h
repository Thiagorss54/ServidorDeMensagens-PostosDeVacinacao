#pragma once
#define MSGSZ 500
 struct Location{
   
  int x;
  int y;

  
};

struct Log{
    char msg[MSGSZ]; 
    char buffer[MSGSZ];
}client_log; 

void addLogMsg(char* m);
char * getLogMsg();

void clearClientLog();

struct Location* findLocation(int x, int y);

int findPosition(int x, int y);

void addLocation(int x, int y);

void removeLocation(int x, int y);

void listLocations();

double EuclideanDistance(struct Location l1,struct Location l2);

void query(int x, int y);

