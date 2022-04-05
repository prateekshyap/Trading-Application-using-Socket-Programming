//#include <string>
#include <bits/stdc++.h>
using namespace std;

const char * item_names[] = {
	"GOLD",
	"SILVER",
	"DOLLAR",
	"EURO",
	"YEN",
	"LIRA",
	"RUBLE",
	"PESO",
	"WON",
	"BAHT",
};

char itemList[] = "1. GOLD\n2. SILVER\n3. DOLLAR\n4. EURO\n5. YEN\n6. LIRA\n7. RUBLE\n8. PESO\n9. WON\n10. BAHT\n";

/*string trader_names[] = {
	"n1",
	"n2",
	"n3",
	"n4",
	"n5",
};*/



class Trader
{
private:
	//int userId;
	string name, password, userName;
	bool isLoggedIn;
	int uniqueNumber;

public:
	Trader(){}
	Trader(string un, string uid, string pw, int num)
	{
		name = un;
		userName = uid;
		password = pw;
		uniqueNumber = num;
		isLoggedIn = false;
	}

	void setLoginStatus(bool status) { isLoggedIn = status; }
	void setNumber(int num) { uniqueNumber = num; }

	bool getLoginStatus() { return isLoggedIn; }
	int getNumber() { return uniqueNumber; }
	string getName() { return name; }
	string getUserId() { return userName; }
	string getPassword() { return password; }
	//queue of orders with status
};

class Order
{
private:
	Trader *trader, *match;
	//Item *item;
	int orderId, quantity, price;
	char type;
	//char status;

public:
	int getPrice();
	//void insertItem(Order *,int);
};

class Heap
{
private:
	Order ** orders;
	int heapSize;
	int capacity = 100;
	int sequence; // 0 -> min heap, 1 -> max heap

	int left(int);
	int right(int);
	int parent(int);
	void heapify(int);

public:
	Heap(int seq)
	{
		orders = new Order*[100];
		heapSize = 0;
		sequence = seq;
	}
	void insert(Order *);
	Order * extractRoot();
	void decreaseKey(int, int);
};

class Item
{
private:
	int itemId, price;
	string itemName;
	Heap * buyBook, * sellBook;

public:
	Item()
	{
		buyBook = new Heap(1);
		sellBook = new Heap(0);
	}
	void insertItem(Order *,int);
};




/////////////////////////////////////////////////////////////////////////////////////
// Functions
/////////////////////////////////////////////////////////////////////////////////////
int Heap :: left(int root) { return (2*root)+1; }
int Heap :: right(int root) { return (2*root)+2; }
int Heap :: parent(int root) { return (root-1)/2; }

void Heap :: heapify(int root)
{
	if (sequence == 0)
	{
		int min = root;
	    if (left(root) < heapSize && orders[left(root)]->getPrice() < orders[min]->getPrice())
	        min = left(root);
	    if (right(root) < heapSize && orders[right(root)]->getPrice() < orders[min]->getPrice())
	        min = right(root);

	    //if min is not root, then we need to swap and recur on the child node
	    if (min != root)
	    {
	        Order * temp = orders[min];
	        orders[min] = orders[root];
	        orders[root] = temp;

	        //recur
	        heapify(min);
	    }
	}
	else if (sequence == 1)
	{
		int max = root;
	    if (left(root) < heapSize && orders[left(root)]->getPrice() > orders[max]->getPrice())
	        max = left(root);
	    if (right(root) < heapSize && orders[right(root)]->getPrice() > orders[max]->getPrice())
	        max = right(root);

	    //if max is not root, then we need to swap and recur on the child node
	    if (max != root)
	    {
	        Order * temp = orders[max];
	        orders[max] = orders[root];
	        orders[root] = temp;

	        //recur
	        heapify(max);
	    }
	}
}

void Heap :: insert(Order * newOrder)
{
	orders[heapSize] = newOrder;
	int par = parent(heapSize);
	int index = heapSize++;
	while (index != 0)
	{
		if ((sequence == 0 && orders[index]->getPrice() < orders[par]->getPrice()) || (sequence == 1 && orders[index]->getPrice() > orders[par]->getPrice()))
		{
			Order * temp = orders[index];
			orders[index] = orders[par];
			orders[par] = temp;
			index = par;
			par = parent(par);
		}
		else break;
	}
}

Order * Heap :: extractRoot()
{
	Order * temp = orders[0];
	orders[0] = orders[heapSize-1];
	--heapSize;
	heapify(0);
	return temp;
}





int Order :: getPrice()
{
	return price;
}




void Item :: insertItem(Order * newOrder, int type)
{
	if (type == 0) //buy
		buyBook->insert(newOrder);
	else if (type == 1) //sell
		sellBook->insert(newOrder);
}