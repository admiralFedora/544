typedef struct Fifo Fifo;

struct Fifo{
  float value;
  Fifo* next;
};

void insertAndPop(Fifo* node, Fifo **head){
  Fifo *temp = *head;
  Fifo *temp2;
  while(temp->next){
    temp2 = temp; //item before the last
    temp = temp->next; // last item
  }

  temp2->next = NULL; // item before last is now the last item
  free(temp); // remove last item

  node->next = *head; // set new node to point to original head
  *head = node; // move head to the new node
}

float returnAverage(Fifo *head){
  Fifo *temp = head;
  int count = 0;
  float avg = 0.0;
  while(temp){
    avg += temp->value;
    temp = temp->next;
    count++;
  }

  return avg/((float)count);
}

int getCount(Fifo *head){
  Fifo *temp = head;
  int count = 0;
  while(temp){
    temp = temp->next;
    count++;
  }

  return count;
}

