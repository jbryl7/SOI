#include <stdio.h>
#include <iostream>
#include <cstdlib>
#include "monitor.h"


#define MAXSIZE 10
#define CONSUMERS 5
#define PRODUCERS 2
#define NUMBEROFBUFFS CONSUMERS
class Buffer;
class BufferMonitor;
class BuffersMonitor;

//buffer
class Buffer
{
    int count, stack[MAXSIZE];
 public:   
	Buffer() {count = 0;}
    void pushItem(int item);
    int removeItem();
    void printBuffer(int consNumber);
    int sizeOfStack();
};
//monitor
class BufferMonitor: public Monitor
{
    Condition full, empty;
    Buffer buffer;
    public:
    void pushItem(int item, int consNumber);
    int readItem(BuffersMonitor &buffersMonitor, int consNumber);
    int getSize();
};
//Manage monitors
class BuffersMonitor: Monitor
{
    int emptyCount;
    Condition allEmpty;
public:
    BuffersMonitor() {emptyCount = MAXSIZE * CONSUMERS;}
    void findFreeSlotAndAdd(int producerID, BufferMonitor *buff); //tab of monitorsl
    void up();
};

//////////////////////////////////////////////////////

void Buffer::pushItem(int item)
{
    stack[count++] = item;
}
int Buffer::removeItem()
{
    return stack[--count];
}
void Buffer::printBuffer(int consNumber)
{
    std::cout<<consNumber<<" Buffer elements: ";
    for (int i = 0; i < count; i++)
        std::cout<<stack[i]<<' ';
    std::cout<<'\n';
}
int Buffer::sizeOfStack()
{
    return count;
}
////////////////////////////////////////////////////////////

void BufferMonitor::pushItem(int item, int consNumber)
{
    enter();
    while (buffer.sizeOfStack() == MAXSIZE)
        wait(empty);
    buffer.pushItem(item);
	buffer.printBuffer(consNumber);
    if (buffer.sizeOfStack() == 1)
        signal(full);
    leave();
}
int BufferMonitor::readItem(BuffersMonitor &buffersMonitor, int consNumber)
{
    enter();
    while (buffer.sizeOfStack() == 0)
        wait(full);
    int valueToReturn = buffer.removeItem();
	buffer.printBuffer(consNumber);
    if (buffer.sizeOfStack() == MAXSIZE - 1)
        signal(empty);
    leave();
    buffersMonitor.up();// signal for allBuffersMonitor to
    return valueToReturn;// increment its count of freeSlots
}
int BufferMonitor::getSize()
{
    enter();
    int size = buffer.sizeOfStack();
    leave();
    return size;
}

///////////////////////////////////////////////////////////////

void BuffersMonitor::findFreeSlotAndAdd(int producerID, BufferMonitor *buff) //tab of monitors
{
    enter();
    while (emptyCount == 0)
        wait(allEmpty);
    int i = rand() % NUMBEROFBUFFS;
    while (buff[i].getSize() == MAXSIZE)//searching for free slot
        i = (i + 1) % NUMBEROFBUFFS;
    std::cout<<producerID<<" put item in "<<i<<" buffer.\n";
    buff[i].pushItem(rand() % 50, i);
    emptyCount--;
    leave();
}

void BuffersMonitor::up()
{
    enter();
    emptyCount++;
    if(emptyCount == 1)
        signal(allEmpty);
    leave();
}
//////////////////////////////////////////////////////////////////































/*#include "Buffer.cpp"
#define NUMBEROFBUFFS 5
#define MAXSIZE 10
#define MAXITEMSINBUFFFS NUMBEROFBUFFS * MAXSIZE
class Buffers: Buffer{
	Buffer* buffers[NUMBEROFBUFFS];
	condition allFull, canRead[NUMBEROFBUFFS];
	int counterOfElements;



	void producer(int producerID);{
		enter();
		while(counterOfElements < MAX){
			std::cout<<"Producer "<<producerID<<"is waiting for free space"<<'\n';
			wait(allFull);
		}
		int buffN = rand() % NUMBEROFBUFFS;
		while((buffers[buffN]->count == MAXSIZE)){
			buffN++;
			buffN = buffN % NUMBEROFBUFFS;
		}
		std::cout<<"Producer "<<producerID<<"is entering buffer "<<buffN<<'\n';
		buffers[buffN]->pushItem(producerID);
		if(buffers[buffN]->count == 1)
			signal(canRead[buffN]);
		leave();
	}

	void consumer(int consumerNumber){
		enter();
		while(((buffers[consumerNumber])->counter) == 0)
			wait(canRead[consumerNumber]);

		(buffers[consumerNumber])->readItem();
		counterOfElements--;
		if(counterOfElements == (MAXITEMSINBUFFFS - 1))
			signal(allFull);
		leave();
	}
};*/
