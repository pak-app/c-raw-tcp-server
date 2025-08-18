#ifndef TOOL_H
#define TOOL_H

void readData(char *fileName);
void readLines(char *fileName);

void tcpServer(char *address, int port, int bufSize);

#endif