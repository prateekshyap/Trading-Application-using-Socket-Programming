#include <string>
#include <bits/stdc++.h>
using namespace std;

string item_names[] = {
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

string trader_names[] = {
	"n1",
	"n2",
	"n3",
	"n4",
	"n5",
};

class Trader
{
private:
	//int userId;
	string name, password, userName;
	bool isLoggedIn;
	int uniqueNumber;

public:
	// stores all orders of a trader
	vector<Order*> all_trades;
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

	/*// add new trade
	void add_trade(Order *o){
		all_trades.push_back(o);
	}*/
};

class Order
{
	Trader *trader, *match;
	Item *item;
	int orderId, quantity, price;
	char type;
	//char status;

	/*public:
	void set_trader(Trader *t){trader = t;}
	void set_match(Trader *m){match = m;}
	void set_item(Item *i){item = i;}
	void set_orderId(int id){orderId = id;}
	void set_quantity(int q){quantity = q;}
	void set_price(int p){price = p;}
	void set_type(char c){type = c;}

	Trader* get_trader(){return trader;}
	Trader* get_match(){return match;}
	int get_orderid(){return orderId;}
	Item* get_item(){return item;}
	int get_quantity(){return quantity;}
	int get_price(){return price;}
	char get_type(){return type;}*/
};

/*
// list of all traders
vector<Trader*> traders;

// function to add trader to the list
void add_new_trader(Trader *t){
	// TO DO: check for unique numbers with list
	traders.push_back(t);
}

// function to get list of all loggedin traders
vector<Trader*> get_loggedin_traders(){
	vector<Trader*> loggedin_traders;
	for(int i=0; i<traders.size(); i++){
		if(traders[i]->getLoginStatus()){
			loggedin_traders.push_back(traders[i]);
		}
	}
	return loggedin_traders;
}
*/

class Item
{
	int itemId, price;
	string itemName;
	//buybook sellbook queues - array of order

	/*public:

	int get_itemId(){return itemId;}
	int get_price(){return price;}
	string get_itemName(){return itemName;}

	void set_itemId(int id){itemId = id;}
	void set_price(int p){price = p;}
	void set_name(string n){itemName = n;}

	//buybook sellbook queues - array of order
	vector<Order*> buy_book, sell_book;

	Item(){ 
	}

	Item(int id, int p, string name){
		itemId = id;
		price = p;
		itemName = name;
	}

	// add new order to buy book
	void add_buy_order(Order *o){
		// find position 
		int pos;
		int i=0;
		while(i < buy_book.size() && o->get_price() < buy_book[i]->get_price()){
			i++;
		}
		// insert to the position
		buy_book.insert(buy_book.begin()+pos, o);
		// perform trade
		trade_orders();
	}

	// add new order to sell book
	void add_sell_order(Order *o){
		// find position
		int pos;
		int i=0;
		while(i < sell_book.size() && o->get_price() > sell_book[i]->get_price()){
			i++;
		}
		// insert to the position
		sell_book.insert(sell_book.begin()+pos, o);
		// perform trade
		trade_orders();
	}

	// TO DO: 
	// perform trade 
	void trade_orders(){
		// check whether trade can be performed or not
		if(buy_book.size() > 0 && sell_book.size() > 0){
			// check if top matches -- top is at index 0
			// CASE 1: TOP OF BUY AND SELL BOOK MATCH
			////////////////////////////////////////////////////
			//			BUY				SELL
			//-----------------------------------------------
			//			10				10
			//			9				11
			if(buy_book[0]->get_price() == sell_book[0]->get_price()){
				// proper match
			}
			// CASE 2: BUY MATCHES WITH MANY SELLS
			else if(){
				
			}
			// case 3: SELL MATCHES WITH MANY BUYS
			else if(){
				
			}
		}
	}*/
};

// List of all items
vector<Item*> items;

// Function to initialize items
void init_items(){
	for(int i=0; i<10; i++){
		Item *t;
		Item temp(i+1, (i+1)*100, item_names[i]);
		t = &temp;
		items.push_back(t);
	}
}

class Heap
{
private:
	Order * orders;
	int heapSize;

	int left(int);
	int right(int);
	void heapify(int);

public:
	void insert(Order *);
	Order extractRoot();
};