#include "API_Module.h"
#include "Catalog_Manager.h"
#include "Interpreter.h"
#include "Buffer_Manager.h"
#include "Record_Manager.h"
#include "Index_Manager.cpp"
#include <time.h>

extern CatalogManager cm;
extern APIMoudule api;
RecordManager rm;
IndexManager<int> imInt;
IndexManager<float> imFloat;
IndexManager<string> imString;

extern bool quitFlag;
void APIMoudule::API(SQLstatement &s)
{
	if (s.type == CREATE_TABLE){
		if (cm.API_Catalog(s)){
			Table *t = cm.findTable(s.tableName);
			if (t->primaryKey != "")
			{
				string SQL = "create index " + t->primaryKey + "index on " + t->name + "(" + t->primaryKey + ");";
				SQL = Interpreter(SQL);
				SQLstatement ss(SQL);
				api.API(ss);
			}
		}
		else{
			cout << "create table failed" << endl;
		}
	}
	else if (s.type == CREATE_INDEX){
		if (cm.API_Catalog(s)){
			// 调index
			Index *i = cm.findIndex(s.indexName);
			Table *t = cm.findTable(s.tableName);
			TYPE type = api.getAttributebyi(t, i->index_name)->type;
			if (type == MYINT){
				imInt.createIndex(*t, *i);
			}
			else if (type == MYFLOAT){
				imFloat.createIndex(*t, *i);
			}
			else if (type == MYCHAR){
				imString.createIndexforString(*t, *i);
			}
			else{
				cout << "wrong type." << endl;
			}
		}
		else{
			cout << "create index failed" << endl;
		}
	}
	else if (s.type == DROP_TABLE){
		if (cm.API_Catalog(s)){
			// 调index和record
			rm.dropTable(cm.msg);
		}
		else{
			cout << "drop table failed" << endl;
		}
	}
	else if (s.type == DROP_INDEX){
		if (cm.API_Catalog(s)){
			imInt.dropIndex(s.indexName);
		}
		else{
			cout << "drop index failed" << endl;
		}
	}
	else if (s.type == SELECT){
		if (cm.API_Catalog(s)){
			//cout << "right select statement." << endl;
			// 调index和record
			Table & targetTable = *cm.findTable(s.tableName);
			clock_t start = clock();
			int resultCount = rm.selectWithoutwhere(targetTable, s.attributes);
			clock_t end = clock();
			if (resultCount)
			{
				rm.outputMap(resultCount, targetTable);
				cout << resultCount << " row(s) selected in " << (double)(end - start) / CLOCKS_PER_SEC << " seconds" << endl;
			}
			else
			{
				cout << "no match row" << endl;
			}
		}
		else{
			cout << "select failed." << endl;
		}
	}
	else if (s.type == SELECT_WHERE){
		if (cm.API_Catalog(s)){
			//cout << "right select where statement." << endl;
			// 调index和record
			Table & targetTable = *cm.findTable(s.tableName);
			clock_t start = clock();
			int resultCount = rm.selectWithwhere(targetTable, s.attributes, s.conditions);
			clock_t end = clock();
			if (resultCount)
			{
				rm.outputMap(resultCount, targetTable);
				cout << resultCount << " row(s) selected in " << (double)(end - start) / CLOCKS_PER_SEC << " seconds" << endl;
			}
			else
			{
				cout << "no match row" << endl;
			}
		}
		else{
			cout << "select failed." << endl;
		}
	}
	else if (s.type == INSERT){
		if (cm.API_Catalog(s)){
			//cout << "right insert statement." << endl;
			// 调record和index
			if (rm.insertValue(*cm.findTable(s.tableName), s.content))
			{
				cout << "inserted" << endl;
			}
			else
			{
				cout << "insert failed." << endl;
			}
		}
		else{
			cout << "insert failed." << endl;
		}
	}
	else if (s.type == MYDELETE){
		if (cm.API_Catalog(s)){
			//cout << "right delete statement." << endl;
			// 调record和index
			int deleteCount = rm.deleteWithoutwhere(*cm.findTable(s.tableName));
			if (deleteCount)
			{
				cout << deleteCount << " row(s) deleted" << endl;
			}
			else
			{
				cout << "no row deleted" << endl;
			}
		}
		else{
			cout << "delete failed." << endl;
		}
	}
	else if (s.type == DELETE_WHERE){
		if (cm.API_Catalog(s)){
			//cout << "right delete where statement." << endl;
			// 调record和index
			int deleteCount = rm.deleteWithwhere(*cm.findTable(s.tableName), s.conditions);
			if (deleteCount)
			{
				cout << deleteCount << " row(s) deleted" << endl;
			}
			else
			{
				cout << "no row deleted" << endl;
			}
		}
		else{
			cout << "delete failed." << endl;
		}
	}
	else if (s.type == EXECFILE){
		fstream fin(s.tableName);
		if (fin){
			string sql;
			string temp;
			bool finish = false;
			sql = "";
			//int tempCount = 0;
			while (fin >> temp){
				sql = sql + ' ' + temp;
				if (sql.at(sql.length() - 1) == ';'){
					sql.erase(0, 1);
					sql = lower(sql);
					cout << sql << endl;
					sql = Interpreter(sql);
					SQLstatement s(sql);
					API(s);
					sql = "";

					/*if (!(tempCount % 1000))
					{
						cout << 100 * (double)tempCount / 200000 << "%" << endl;
					}
					tempCount++;*/
				}
			}
		}
		else{
			cout << "no such file." << endl;
		}
	}
	else if (s.type == QUIT){
		quitFlag = true;
		// 保存表信息
		cm.update_tableInfo();
		cm.update_indexInfo();
	}
	else if (s.type == HELP){
		getHelp();
	}
	else if (s.type == ILLEGAL){
		cout << "wrong SQL statement." << endl;
	}
}

void APIMoudule::getHelp()
{
	fstream fin("help.txt");
	string s;
	while (getline(fin, s))
	{
		cout << s << endl;
	}
}

//获取用户输入
string APIMoudule::read_input()
{
	string SQL;
	string temp;
	bool finish = false;
	SQL = "";
	while (!finish)
	{
		cin >> temp;
		SQL = SQL + ' ' + temp;
		if (SQL.at(SQL.length() - 1) == ';'){
			SQL.erase(0, 1);
			finish = true;
		}
	}
	//将输入大写转化为小写
	SQL = lower(SQL);
	//返回用户输入
	return SQL;
}

//改为小写
string APIMoudule::lower(string s)
{
	transform(s.begin(), s.end(), s.begin(), ::tolower);
	return s;
}

Attribute* APIMoudule::getAttributebyi(Table *t, string in)
{
	vector<Attribute>::iterator iter;
	for (iter = t->attributes.begin(); iter != t->attributes.end(); iter++){
		if (iter->indexName == in){
			return &(*iter);
		}
	}
	return NULL;
}