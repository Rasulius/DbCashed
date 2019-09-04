#include "ChashedDB.h"

ChashedDB::ChashedDB(i_db *db) : database(db)
{
	transactionAction = false;
}


ChashedDB::~ChashedDB()
{

}


bool ChashedDB::begin_transaction(){
	
	std::unique_lock<std::mutex> lock(mutex); //Блокируем
	if (transactionAction) { // Если в транзакции
		return false;
	}

	transactionAction = database->begin_transaction();
	return transactionAction;
}

bool ChashedDB::commit_transaction(){

	std::unique_lock<std::mutex> lock(mutex);
	if (transactionAction == false) {
		return false;
	}

	bool commit = database->commit_transaction();

	if (commit) {
		transactionAction = false;
		for (auto &transact: transaction) {
			cache.insert(transact);
		}
		transaction.clear();
	}
	return !transactionAction;

}
bool ChashedDB::abort_transaction(){

	std::unique_lock<std::mutex> lock(mutex);
	if (!transactionAction) {
		return false;
	}


	if (database->abort_transaction()) {		
		transaction.clear();
		transactionAction = false;
	}
	return !transactionAction;
}


std::string ChashedDB::get(const std::string& key){
	std::unique_lock<std::mutex> lock(mutex);
	auto findCache = cache.find(key);
	if (findCache != cache.end()) {
		return findCache->second;
	}

	std::string temp = database->get(key);
	cache[key] = temp;
	return temp;

}

std::string ChashedDB::set(const std::string& key, const std::string& data){
	std::unique_lock<std::mutex> lock(mutex);
	database->set(key, data); 
	
	if (transactionAction) {
		transaction[key] = data;
	}
	else {
		cache[key] = data;
	}
	return data;
}

std::string ChashedDB::del(const std::string& key){
	
	std::unique_lock<std::mutex> lock(mutex);
	database->del(key); 
	if (transactionAction) {
		transaction[key] = std::string();
	}
	else {
		cache[key] = std::string();
	}
	return std::string();
}



