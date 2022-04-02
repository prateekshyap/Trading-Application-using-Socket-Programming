class Item
{
	int itemId, price;
	string itemName;
	//buybook sellbook queues - array of order
};

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
	Trader trader, match;
	Item item;
	int orderId, quantity, price;
	char type;
	//char status;
};

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