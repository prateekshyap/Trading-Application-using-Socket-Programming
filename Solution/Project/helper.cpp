#include<bits/stdc++.h>
#include<string>
using namespace std;
int order_count = 0;
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

string tader_names[] = {
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

	// add new trade
	void add_trade(Order *o){
		all_trades.push_back(o);
	}
	
};

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

// Represents one order
class Order
{
	Trader *trader, *match;
	Item *item;
	int orderId, quantity, price;
	char type;
	// to store the remaining quantity of order after partial filling
	int remaining_quantity;
	// status: F = FILLED, Q = In queue
	char status;

	public:

	// Always use this constructor
	Order(Trader *tra, Item *i, int quan, int pri, char t){
		trader = tra;
		item = i;
		orderId = ++order_count;
		quantity = quan;
		price = pri;
		type = t;
		remaining_quantity = quan;
		status = 'Q';
	}

	void set_trader(Trader *t){trader = t;}
	void set_match(Trader *m){match = m;}
	void set_item(Item *i){item = i;}
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
	Item* get_item(){return item;}
	int get_quantity(){return quantity;}
	int get_price(){return price;}
	char get_type(){return type;}
	int get_remaining_quantity(){return remaining_quantity;}

};

// Represents trading entity
class Item
{
	int itemId, price;
	string itemName;
    
	public:

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

	// perform trade 
	void trade_orders(){
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

	// This function prints buy and sell orders of item
	void print_orders(){
		cout << "********************************************************************************" << endl;
		cout << "ORDER BOOK FOR " << itemName << endl;
		cout << "/////////////////////////////////////////////////////////////" << endl;
		cout << "\t\tBUY PRICE(Quantity)\t\tSELL PRICE(Quantity)" << endl;
		cout << "/////////////////////////////////////////////////////////////" << endl;
		for(int i=0; i<buy_book.size() || i<sell_book.size(); i++){
			if(i < buy_book.size()){
				cout << "\t\t\t" << buy_book[i]->get_price() << "(" << buy_book[i]->get_quantity() << ")\t\t";  
			}
			else{
				cout << "\t\t\t\t\t\t\t";
			}
			if(i < sell_book.size()){
				cout << "\t\t\t" << sell_book[i]->get_price() << "(" << sell_book[i]->get_quantity() << ")";  
			}
			cout << endl;
		}
		cout << "********************************************************************************" << endl;
	}
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

// HEAP NOT NEEDED
// AS BOTH ORDER BOOKS ARE SORTED
// AND AT A TIME WE ARE INSERTING ONLY ONE ORDER
// USING INSERTION SORT
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
