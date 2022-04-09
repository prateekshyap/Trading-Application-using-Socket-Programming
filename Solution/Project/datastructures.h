//#include <string>
#include <bits/stdc++.h>
using namespace std;

int order_count = 0;
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
class Order;
class Trader
{
private:
	//int userId;
	string name, password, userName;
	bool isLoggedIn;
	int uniqueNumber;
	vector<Order*> all_trades;
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
	vector<Order *> getAllTrades() { return all_trades; }
	//queue of orders with status

	// add new trade
	void add_order(Order *o){
		all_trades.push_back(o);
	}
};

class Order
{
private:
	Trader *trader, *match;
	//Item *item;
	int itemIndex;
	int orderId, quantity, price, remaining_quantity;
	int type;
	char status;// status: F = FILLED, Q = In queue

public:
	Order(Trader * t, int itIn, int q, int p, int orderType)
	{
		trader = t;
		itemIndex = itIn;
		orderId = ++order_count;
		quantity = q;
		price = p;
		type = orderType;
		remaining_quantity = quantity;
		status = 'Q';
	}
	int getPrice();
	void printOrderDetails();


	void set_trader(Trader *t){trader = t;}
	void set_match(Trader *m){match = m;}
	//void set_item(Item *i){item = i;}
	void set_orderId(int id){orderId = id;}
	void set_quantity(int q){quantity = q;}
	void set_price(int p){price = p;}
	void set_type(char c){type = c;}
	void set_remaining_quantity(int q){remaining_quantity = q;}
	void set_status(char c){status = c;}

	char get_status(){return status;}
	Trader* get_trader(){return trader;}
	Trader* get_match(){return match;}
	int get_orderid(){return orderId;}
	//Item* get_item(){return item;}
	int get_item_index() {return itemIndex;}
	int get_quantity(){return quantity;}
	int get_price(){return price;}
	char get_type(){return type;}
	int get_remaining_quantity(){return remaining_quantity;}
};

/*class Heap
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
	Order ** getOrderBook();
	int getHeapSize();
};*/

class Item
{
private:
	int itemId, price;
	const char * itemName;
	//Heap * buyBook, * sellBook;
	vector<Order*> buy_book, sell_book;

public:
	Item(int i)
	{
		itemId = i;
		price = 0;
		itemName = item_names[i];
		//buyBook = new Heap(1);
		//sellBook = new Heap(0);
	}
	void insertItem(Order *,int);
	char * printItemQueues();
	vector<Order *> getBuyBook();
	vector<Order *> getSellBook();
	int getBuyBookSize();
	int getSellBookSize();
	const char * getItemName() { return itemName; }

	void add_buy_order(Order *);
	void add_sell_order(Order *);
	void trade_orders();
};


/////////////////////////////////////////////////////////////////////////////////////
// HEAP Functions
/////////////////////////////////////////////////////////////////////////////////////
/*int Heap :: left(int root) { return (2*root)+1; }
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

Order ** Heap :: getOrderBook()
{
	return orders;
}

int Heap :: getHeapSize()
{
	return heapSize;
}*/






/////////////////////////////////////////////////////////////////////////////////////
// ORDER Functions
/////////////////////////////////////////////////////////////////////////////////////
int Order :: getPrice()
{
	return price;
}

void Order :: printOrderDetails()
{
	Trader *trader, *match;
	//Item *item;
	int itemIndex;
	int orderId, quantity, price;
	int type;
	//char status;

	cout << endl << endl;
	cout << "Item: " << item_names[itemIndex] << endl;
	cout << "At Price: " << price << endl;
	cout << "Quantity: " << quantity << endl;
	cout << "Traded by: " << trader->getName() << endl;
}






/////////////////////////////////////////////////////////////////////////////////////
// ITEM Functions
/////////////////////////////////////////////////////////////////////////////////////
void Item :: insertItem(Order * newOrder, int type)
{
	if (type == 0) //buy
	{
		//cout << "insertItem - buy" << endl;
		//buyBook->insert(newOrder);
		add_buy_order(newOrder);
	}
	else if (type == 1) //sell
	{
		//cout << "insertItem - sell" << endl;
		//sellBook->insert(newOrder);
		add_sell_order(newOrder);
	}
}

void Item :: add_buy_order(Order *o){
		// find position 
		int i=0;
		while(i < buy_book.size() && o->get_price() < buy_book[i]->get_price()){
			i++;
		}
		// insert to the position
		buy_book.insert(buy_book.begin()+i, o);
		// perform trade
		trade_orders();
	}

void Item :: add_sell_order(Order *o){
		// find position
		int i=0;
		while(i < sell_book.size() && o->get_price() > sell_book[i]->get_price()){
			i++;
		}
		// insert to the position
		sell_book.insert(sell_book.begin()+i, o);
		// perform trade
		trade_orders();
	}

// perform trade 
void Item :: trade_orders(){
	// check whether trade can be performed or not
	if(buy_book.size() > 0 && sell_book.size() > 0){
		// check if top matches -- top is at index 0
		// CASE 1: TOP OF BUY AND SELL BOOK MATCH
		///////////////////////////////////////////////////////////
		//			BUY				SELL
		//---------------------------------------------------------
		//			10				10
		//			9				11
		// CASE 2: TOP OF BUY IS MORE THAN TOP OF SELL
		///////////////////////////////////////////////////////////
		//			BUY				SELL
		//---------------------------------------------------------
		//			14				11
		//			9				12
		//			7				13
		//			6				16
		if(buy_book[0]->get_price() >= sell_book[0]->get_price()){
			// proper price match
			// set current price
			price = buy_book[0]->get_price();
			// check quantity
			if(buy_book[0]->get_remaining_quantity() == sell_book[0]->get_remaining_quantity()){
				// BOTH HAVE SAME QUANTITY
				// set the match of order
				buy_book[0]->set_match(sell_book[0]->get_trader());
				sell_book[0]->set_match(buy_book[0]->get_trader());
				buy_book[0]->set_status('F');
				sell_book[0]->set_status('F');
				buy_book[0]->set_remaining_quantity(0);
				sell_book[0]->set_remaining_quantity(0);
				// remove both orders
				buy_book.erase(buy_book.begin());
				sell_book.erase(sell_book.begin());
			}
			else if(buy_book[0]->get_remaining_quantity() > sell_book[0]->get_remaining_quantity()){
				// TOP OF THE BUY BOOK HAS MORE QUANTITY
				// set match of sell order
				sell_book[0]->set_match(buy_book[0]->get_trader());
				sell_book[0]->set_status('F');
				// update quantity of buy order
				cout << buy_book[0]->get_remaining_quantity() << "," << sell_book[0]->get_remaining_quantity() << "----------------" << endl;
				buy_book[0]->set_remaining_quantity(buy_book[0]->get_remaining_quantity() - sell_book[0]->get_remaining_quantity());
				sell_book[0]->set_remaining_quantity(0);
				// remove sell order
				sell_book.erase(sell_book.begin());
			}
			else{
				// TOP OF SELL BOOK HAS MORE QUANTITY
				// set match of buy order
				buy_book[0]->set_match(sell_book[0]->get_trader());
				buy_book[0]->set_status('F');
				// update quantity of sell order
				sell_book[0]->set_remaining_quantity(sell_book[0]->get_remaining_quantity() - buy_book[0]->get_remaining_quantity());
				buy_book[0]->set_remaining_quantity(0);
				// remove buy order
				buy_book.erase(buy_book.begin());
			}
			// recursive call to see if other orders can match or not
			trade_orders();
		}
		// CASE 3: SELL AND BUY DONT MATCH
		///////////////////////////////////////////////////////////
		//			BUY				SELL
		//---------------------------------------------------------
		//			10				12
		//			9				14
		//			7				16
		else {
			// TRADE IS NOT POSSIBLE
			// EXIT
		}
	}
}

char returnStr[10240] = {'\0'};
/*char * Item :: printItemQueues()
{
	sprintf(returnStr,"%s","////////////////////////////////////////////////////////////////////////////////\n");
	strcat(returnStr,"ORDER BOOK FOR ");
	strcat(returnStr,itemName.c_str());
	strcat(returnStr,"\n********************************************************************************\n");
	strcat(returnStr,"\t\tBUY PRICE(Quantity)\t\tSELL PRICE(Quantity)\n");
	strcat(returnStr,"********************************************************************************\n");
	for(int i=0; i<buy_book.size() || i<sell_book.size(); i++){
		char orderDetails[1024] = {'\0'};
		if(i < buy_book.size()){
			sprintf(orderDetails,"\t\t\t%d(%d)\t\t",buy_book[i]->get_price(),buy_book[i]->get_remaining_quantity());
			//cout << "\t\t\t" << buy_book[i]->get_price() << "(" << buy_book[i]->get_remaining_quantity() << ")\t\t";  
		}
		else{
			strcat(orderDetails,"\t\t\t\t\t");
			//cout << "\t\t\t\t\t\t";
		}
		if(i < sell_book.size()){
			sprintf(orderDetails,"\t\t\t%d(%d)\t\t",sell_book[i]->get_price(),sell_book[i]->get_remaining_quantity());
			//cout << "\t\t\t" << sell_book[i]->get_price() << "(" << sell_book[i]->get_remaining_quantity() << ")";  
		}
		strcat(orderDetails,"\n");
		strcat(returnStr,orderDetails);
		//cout << endl;
	}
	strcat(returnStr,"////////////////////////////////////////////////////////////////////////////////\n");
	return returnStr;
}*/

vector<Order *> Item :: getBuyBook()
{
	return buy_book;
}

vector<Order *> Item :: getSellBook()
{
	return sell_book;
}

int Item :: getBuyBookSize()
{
	return buy_book.size();
}

int Item :: getSellBookSize()
{
	return sell_book.size();
}