#include <XBee.h>

typedef struct Node Node;

struct Node{
  XBeeAddress64 addr64;
  int id;
  bool leader;
  Node* next;
  Node* prev;
};

static void insertNewNode(Node* newNode, Node** head){
  if(*head == NULL){
    *head = newNode;
    newNode->next = newNode;
    newNode->prev = newNode;
    return;
  }

  (*head)->prev->next = newNode;
  newNode->prev = (*head)->prev;
  (*head)->prev = newNode;
  newNode->next = (*head);
}

static Node* getHighestNode(Node* head){
  int id = head->id;
  Node* temp = head->next;
  Node* highestNode;
  while(temp != head){
    if(temp->id > id){
      id = temp->id;
      highestNode = temp;
    }
    temp = temp->next;
  }

  return highestNode;
}

static Node* getNextHighestNode(Node* head, int target){
  int id = head->id;
  if(id >= target){
    id = 0;
  }
  Node* temp = head->next;
  Node* highestNode;
  while(temp != head){
    if((temp->id > id) && (temp->id < target)){
      id = temp->id;
      highestNode = temp;
    }
    temp = temp->next;
  }

  return highestNode;
}

static Node* findLeader(Node* head){
  Node* temp = head->next;

  if(head->leader){
    return head;
  }

  while(temp != head){
    if(temp->leader){
      return temp;
    }

    temp = temp->next;
  }

  return NULL; // this is bad
}

static Node* findNodeById(int id, Node* head){
  Node* temp = head->next;

  if(head->id == id){
    return head;
  }

  while(temp != head){
    if(temp->id == id){
      return temp;
    }

    temp = temp->next;
  }

  return NULL;
}

static Node* removeNode(Node* toRemove, Node** head){
  toRemove->prev->next = toRemove->next;
  toRemove->next->prev = toRemove->prev;

  if(toRemove == (*head)){
    *head = toRemove->next;
  }

  free(toRemove);
}

