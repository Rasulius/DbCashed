#ifndef ChashedDB_H
#define ChashedDB_H

#include <string>
#include <mutex>
#include <unordered_map>
#include <unordered_set>




struct i_db
{
	bool begin_transaction();
	bool commit_transaction();
	bool abort_transaction();

	std::string get(const std::string& key);
	std::string set(const std::string& key, const std::string& data);
	std::string del(const std::string& key);
};

// Реализуем интерфейс i_DB

class ChashedDB
{
private:
	i_db *database;
	std::unordered_map<std::string, std::string> transaction;
	std::unordered_map<std::string, std::string> cache;
	std::mutex mutex;	
	bool transactionAction;
public:
	explicit ChashedDB(i_db *db);
	~ChashedDB();

	bool begin_transaction();
	bool commit_transaction();
	bool abort_transaction();
	std::string get(const std::string& key);
	std::string set(const std::string& key, const std::string& data);
	std::string del(const std::string& key);

	
};

#endif