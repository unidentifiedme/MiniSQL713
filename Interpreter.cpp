#include "Interpreter.h"
using namespace std;
string allchar = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

/////////////////////////////////////////////////////////////////////////////////////////////
//获取用户输入，并对输入作有效性检查，若正确，返回语句的内部表示形式
string Interpreter()	
{
	string SQL;
	string temp;
	int start=0, end;
	SQL = read_input();
	//获取输入的第一个单词
	start = SQL.find_first_not_of(' ',0);
	end = SQL.find_first_of(' ',start);
	temp = SQL.substr(start, end - start);//第一个词
	//获取第二个单词
	start = end + 1;
	
	//若为create语句
	if(temp=="create")
		SQL=create_clause(SQL,start);
	//若为drop语句
	else if(temp=="drop")
		SQL=drop_clause(SQL,start);
	//若为select语句
	else if(temp=="select")
		SQL=select_clause(SQL,start);
	//若为insert语句
	else if(temp=="insert")
		SQL=insert_clause(SQL,start);
	//若为delete语句
	else if(temp=="delete")
		SQL=delete_clause(SQL,start);
	//若为use语句
	else if(temp=="use")
		SQL=use_clause(SQL,start);
	//若为execfile语句
	else if(temp=="execfile")
		SQL=execfile_clause(SQL,start);
	//若为quit语句
	else if(temp=="quit")
		SQL=quit_clause(SQL,start);
	//获取帮助
	else if(temp=="help")
		SQL="80";
	//若为非法语句
	else
	{
		cout<<"syntax error:"<<" "<<temp<<"---is not a valid key word!"<<endl;
		SQL="99";
	}
	//返回输入语句的内部形式
	return SQL;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//获取用户输入
string read_input()
{
	string SQL;
	string temp;
	bool finish = false;
	SQL = "";
	while (!finish)
	{
		cin >> temp;
		SQL = SQL + ' ' + temp;
		if (SQL.at(SQL.length()-1) == ';'){
			SQL.erase(0,1);
			finish = true;
		}
	}
	//将输入大写转化为小写
	SQL = lower(SQL);
	//返回用户输入
	return SQL;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//改为小写
string lower(string s)
{
	transform(s.begin(), s.end(), s.begin(), ::tolower);
	return s;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//验证create语句是否有效
string create_clause(string SQL,int start)
{
	string temp;
	int end;
	//获取第二个单词
	end = SQL.find_first_of(' ', start);
	temp = SQL.substr(start, end - start);
	start=end+1;  //start从第三个词开始

	//若无，打印出错信息
	if (start == 0 || temp.empty())
	{
		cout<<"syntax error: syntax error for create statement!"<<endl;
		SQL="99";
	}
	//若为database,继续验证
	else if(temp=="database")
		SQL=create_database(SQL,start);
	//若为table,继续验证
	else if(temp=="table")
		SQL=create_table(SQL,start);
	//若为index,继续验证
	else if(temp=="index")
		SQL=create_index(SQL,start);
	//若为错误信息，打印出错信息
	else
	{
		cout<<"syntax error:"<<" "<<temp<<"---is not a valid key word!"<<endl;
		SQL="99";
	}
	//返回create语句的内部形式
	return SQL;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//验证create database语句是否有效
string create_database(string SQL, int start)
{
	string temp;
	int end;
	//获取第三个单词
	end = SQL.find_first_of(';', start);
	temp = SQL.substr(start, end - start);  //数据库名
	start = end + 1;  //start从第四个词开始

	//若无，打印出错信息
	if (start == 0 || temp.empty())
	{
		cout << "error: database name has not been specified!" << endl;
		SQL = "99";
	}
	else
	{
		//若为非法信息，打印出错信息
		if (SQL.at(end) != ';' || end != SQL.length() - 1)
		{
			cout << "error12:" << temp << "---is not a valid database name!" << endl;
			SQL = "99";
		}
		//返回create database语句的内部形式
		else
			SQL = "00" + temp;
	}
	return SQL;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//验证create table语句是否有效
string create_table(string SQL, int start)
{
	string temp, sql, T;
	int end;
	//获取表名
	end = SQL.find_first_not_of(allchar, start);
	temp = SQL.substr(start, end - start);// 表名
		
	if ((SQL.find('(', end)) == -1)
	{
		cout << "error: missing ( in the statement!" << endl;
		SQL = "99";
		return SQL;
	}
	start = SQL.find_first_of('(', end) + 1;

	//若无，打印出错信息
	if (start == 0 || temp.empty())
	{
		cout << "error: error in create table statement!" << endl;
		SQL = "99";
		return SQL;
	}
	else 
	{
		sql = temp + ",";
		start = SQL.find_first_of(allchar, start);
		//获取每个属性
		while ((end = SQL.find(',', start)) != -1)
		{
			temp = SQL.substr(start, end - start);  //第一个属性
			start = end + 1;
			//若有空属性，打印出错信息
			if (start == 0 || temp.empty())
			{
				cout << "error: error in create table statement!" << endl;
				SQL = "99";
				return SQL;
			}
			//保存属性
			else
			{
				sql = get_attribute(temp, sql);
				if (sql == "99")
					return sql;
			}
			while (SQL.at(start) == ' ' || SQL.at(start) == '\n')
				start++;
		}
		//获得最后属性
		end = SQL.find_last_of(')');
		temp = SQL.substr(start, end - start);
		//若无，打印出错信息
		if(temp.empty())
		{
			cout << "error: error in create table statement!" << endl;
			SQL = "99";
			return SQL;
		}
		//存储属性
		else
		{
			end = SQL.find_first_of(' ', start);
			T = SQL.substr(start, end - start);
			start = end + 1;
			//若为主键定义
			if (T == "primary")
			{
				//判断是否有关键字key
				while (SQL.at(start) == ' ')
					start++;
				end = SQL.find_first_of('(', start);
				T = SQL.substr(start, end - start);
				start = end + 1;
				
				//若为空，打印出错信息
				if(T.empty())
				{
					cout << "syntax error: syntax error in create table statement!" << endl;
					cout << "\t missing key word key!" << endl;
					SQL="99";
					return SQL;
				}
				//若有，继续验证
				else if(T=="key")
				{
					//获取主键属性名
					while (SQL.at(start) == ' ')
						start++;
					end = SQL.find(')', start);
					T = SQL.substr(start, end - start);
					//若无，打印出错信息
					if(T.empty())
					{
						cout<<"error : missing primary key attribute name!"<<endl;
						SQL="99";
						return SQL;
					}
					//若为非法信息，打印出错信息
					else if (T.find(' ') != -1 || (sql.find(T) == -1))
					{
						cout<<"error : "<<T<<"---is not a valid primary key attribute name!"<<endl;
						SQL="99";
						return SQL;
					}
					//保存主键
					else
					{
						sql += T + " #,";
						SQL="01"+sql;
					}
				}
				//若为非法信息，打印出错信息
				else
				{
					cout << "error : " << T << "---is not a valid key word!" << endl;
					SQL = "99";
					return SQL;
				}				
			}
			//若为一般属性
			else   //还可以不定义主键？
			{
				sql = get_attribute(temp, sql);
				if (sql == "99")
				{
					SQL = "99";
					return SQL;
				}
				else
					SQL="01"+sql;
			}
		}
	}
	return SQL;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//获得属性
string get_attribute(string temp, string sql)
{
	int start = 0, end, index;
	string name, type, C, uni;
	temp += " ";
	//获得属性名
	end = temp.find_first_of(' ', start);
	name = temp.substr(start, end - start);
	start = end + 1;
	sql = sql + name + ' ';
	//获得数据类型
	while (temp.at(start) == ' ')
		start++;
	end = temp.find(' ', start);
	type = temp.substr(start, end - start);
	start = end;
	//若无，打印出错信息
	if (name.empty() || type.empty())
	{
		cout << "error : error in create table statement!" << endl;
		sql = "99";
		return sql;
	}
	//若为int 
	else if (type == "int")
		sql += "+";
	//若为float
	else if (type == "float")
		sql += "-";
	//其他
	else
	{
		index = type.find('(');
		C = type.substr(0, index);
		index++;
		//若有误，打印出错信息
		if (C.empty())
		{
			cout << "error: " << type << "---is not a valid data type definition!" << endl;
			sql = "99";
			return sql;
		}
		//若为char
		else if (C == "char")
		{
			C = type.substr(index, type.length() - index - 1);
			if (C.empty())
			{
				cout << "error: the length of the data type char has not been specified!" << endl;
				sql = "99";
				return sql;
			}
			else
				sql += C;
		}
		//若为非法信息，打印出错信息
		else
		{
			cout << "error: " << C << "---is not a valid key word!" << endl;
			sql = "99";
			return sql;
		}
	}
	//是否有附加信息
	while (start < temp.length() - 1 && temp.at(start) == ' ')
		start++;
	if (start < temp.length() - 1)
	{
		//若为unique定义，保存信息
		end = temp.find_first_of(' ', start);
		uni = temp.substr(start, end - start);
		if (uni == "unique")
		{
			sql += " 1,";
		}
		//若为非法信息，打印出错信息
		else
		{
			cout << "error: " << uni << "---is not a valid key word!" << endl;
			sql = "99";
			return sql;
		}
	}
	//若无附加信息
	else
		sql += " 0,";
	return sql;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//验证create index语句是否有效
string create_index(string SQL,int start)
{
	string temp, sql;
	int end;
	//获取第三个单词
	while(SQL.at(start)==' ')
		start++;
	end = SQL.find_first_of(' ', start);
	temp = SQL.substr(start, end - start);
	start = end + 1;
	//若无，打印出错信息
	if (start == 0 || temp.empty())
	{
		cout << "syntax error: syntax error for create index statement!" << endl;
		SQL = "99";
	}
	else
	{
		sql = temp;
		//获取第四个单词
		while (SQL.at(start) == ' ')
			start++;
		end = SQL.find_first_of(' ', start);
		temp = SQL.substr(start, end - start);
		start=end+1;
		//若无，打印出错信息
		if (start == 0 || temp.empty())
		{
			cout << "syntax error: syntax error for create index statement!" << endl;
			SQL = "99";
		}
		//若为on,继续验证
		else if (temp == "on")
			SQL = create_index_on(SQL, start, sql);
		//若为非法信息，打印非法信息
		else
		{
			cout << "syntax error:" << " " << temp << "---is not a valid key word!" << endl;
			SQL = "99";
		}			
	}
	return SQL;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//验证create index on语句是否有效
string create_index_on(string SQL,int start,string sql)
{
	string temp;
	int end, length;
	//获取表名
	while (SQL.at(start) == ' ')
		start++;
	end = SQL.find_first_of('(', start);
	temp = SQL.substr(start, end - start);
	start = end + 1;
	//若无，打印出错信息
	if (start == 0 || temp.empty())
	{
		cout << "syntax error: syntax error for create index statement!" << endl;
		cout << "\t missing ( !" << endl;
		SQL="99";
		return SQL;
	}
	else
	{
		//检验是否为有效文件名
		length = temp.length() - 1;
		while (temp.at(length) == ' ')
			length--;
		temp = temp.substr(0, length + 1);
		//有效                            //?
		if (temp.find(' ') == -1) // 还需检查表是否存在
		{
			sql += " " + temp;
			//获取属性名
			while (SQL.at(start) == ' ')
				start++;
			end = SQL.find(')', start);
			temp = SQL.substr(start, end - start);
			start = end + 1;
			//若无，打印出错信息
			if (start == 0 || temp.empty())
			{
				cout<<"syntax error: syntax error for create index statement!"<<endl;
				cout<<"\t missing ) !"<<endl;
				SQL="99";
				return SQL;
			}
			else
			{
				//检验是否为有效属性名
				length = temp.length() - 1;
				while (temp.at(length) == ' ')
					length--;
				temp = temp.substr(0, length + 1);
				//有效
				if (temp.find(' ') == -1)	// 还需检测表中属性是否存在
				{
					sql += " " + temp;
					while (SQL.at(start) == ' ')
						start++;
					if (SQL.at(start) != ';' || start != SQL.length() - 1)
					{
						cout << "syntax error: syntax error for create index on statement!" << endl;
						SQL = "99";
						return SQL;
					}
					//返回create index语句的内部形式
					else
						SQL = "02" + sql;
				}
				//无效,打印出错信息
				else
				{
					cout << "error:" << " " << temp << "---is not a valid attribute name!" << endl;
					SQL = "99";
					return SQL;
				}
			}
		}
		//无效,打印出错信息
		else
		{
			cout << "error:" << " " << temp << "---is not a valid table name!" << endl;
			SQL = "99";
			return SQL;
		}
	}
	return SQL;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//验证drop语句是否有效
string drop_clause(string SQL,int start)
{

	string temp;
	int end;
	//获取第二个单词
	end = SQL.find_first_of(' ', start);
	temp = SQL.substr(start, end - start);
	start = end + 1;  //start从第三个词开始
	//若无，打印出错信息
	if(start == 0 || temp.empty())
	{
		cout<<"syntax error: syntax error for drop statement!"<<endl;
		SQL="99";
	}
	//若为database,继续验证
	else if(temp=="database")
		SQL=drop_database(SQL,start);
	//若为table,继续验证
	else if(temp=="table")
		SQL=drop_table(SQL,start);
	//若为index,继续验证
	else if(temp=="index")
		SQL=drop_index(SQL,start);
	//若为错误信息，打印出错信息
	else
	{
		cout<<"syntax error:"<<" "<<temp<<"---is not a valid key word!"<<endl;
		SQL="99";
	}
	//返回drop语句的内部形式
	return SQL;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//验证drop database语句是否有效
string drop_database(string SQL,int start)
{
	string temp;
	int end;
	//获取第三个单词
	end = SQL.find_first_of(';', start);
	temp = SQL.substr(start, end - start);  //数据库名
	start = end + 1;  //start从第四个词开始
	//若无，打印出错信息
	if (start == 0 || temp.empty())
	{
		cout << "error: database name has not been specified!" << endl;
		SQL = "99";
	}
	else
	{
		//若为非法信息，打印出错信息
		if (SQL.at(end) != ';' || end != SQL.length() - 1)
		{
			cout << "error:" << temp << "---is not a valid database name!" << endl;
			SQL = "99";
		}
		//返回drop database语句的内部形式
		else
			SQL = "10" + temp;
	}
	return SQL;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//验证drop table语句是否有效
string drop_table(string SQL,int start)  //drop table 表名 ;
{
	string temp;
	int end;
	//获取第三个单词
	end = SQL.find_first_of(';', start);
	temp = SQL.substr(start, end - start);  //数据库名
	start = end + 1;  //start从第四个词开始
	//若无，打印出错信息
	if (start == 0 || temp.empty())
	{
		cout << "error: table name has not been specified!" << endl;
		SQL = "99";
	}
	else
	{
		//若为非法信息，打印出错信息
		if (SQL.at(end) != ';' || end != SQL.length() - 1)
		{
			cout << "error:" << temp << "---is not a valid table name!" << endl;
			SQL = "99";
		}
		//返回drop table语句的内部形式
		else
			SQL = "11" + temp;
	}
	return SQL;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//验证drop index语句是否有效
string drop_index(string SQL,int start)  //drop index 索引名 ;
{
	string temp;
	int end;
	//获取第三个单词
	end = SQL.find_first_of(';', start);
	temp = SQL.substr(start, end - start);  //数据库名
	start = end + 1;  //start从第四个词开始
	//若无，打印出错信息

	if (start == 0 || temp.empty())
	{
		cout << "error: index name has not been specified!" << endl;
		SQL = "99";
	}
	else
	{
		//若为非法信息，打印出错信息
		if (SQL.at(end) != ';' || end != SQL.length() - 1)
		{
			cout << "error:" << temp << "---is not a valid index name!" << endl;
			SQL = "99";
		}
		//返回drop index语句的内部形式
		else
			SQL = "12" + temp;
	}
	return SQL;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//验证select 语句是否有效
string select_clause(string SQL,int start)
{
	return SQL;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//获得属性组或文件组的每一项
string get_part(string temp,string sql,string kind)
{
	return sql;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//验证insert语句是否有效
string insert_clause(string SQL, int start)  //insert into 表名 values ( 值1 , 值2 , … , 值n ) ;
{
	string temp;
	string sql = "";
	int end;


	start = SQL.find_first_not_of(' ', start);	//第二个单词
	end = SQL.find(' ', start);
	temp = SQL.substr(start, end - start);  //temp = "into"

	start = end + 1;
	//若无，打印出错信息
	if (temp.empty())
	{
		cout << "syntax error: can't find the keyword 'into' " << endl;
		SQL = "99";
	}
	else if (temp != "into")
	{
		cout << "syntax error: might be 'into' " << endl;
		SQL = "99";
	}
	else
	{
		//获取第三个单词
		start = SQL.find_first_not_of(' ', start);
		//index = start;
		end = SQL.find(' ', start);
		temp = SQL.substr(start, end - start);  //temp = table名
		sql += temp;   //sql = table名
		start = end + 1;

		//若无，打印出错信息
		if (temp.empty())
		{
			cout << "syntax error: can't find the target Table" << endl;
			SQL = "99";
		}
		else if (temp == "values" || temp == "value" || temp == "into")
		{
			cout << "syntax error: can't find the target Table" << endl;
			SQL = "99";
		}
		else
		{

			//获取第四个单词
			start = SQL.find_first_not_of(' ', start);
			//index = start;
			end = SQL.find_first_not_of(allchar, start);
			temp = SQL.substr(start, end - start);  //temp = value
			start = end;                    //start = (

			//若无，打印出错信息
			if (temp.empty())
			{
				cout << "syntax error: can't find keyword 'values' or 'value' " << endl;
				SQL = "99";
			}
			else if (temp != "values" && temp != "value")
			{
				cout << "syntax error: maybe 'value' or 'values' " << endl;
				SQL = "99";
			}
			else
			{
				//cout << SQL;
				SQL = insert_into_values(SQL, start, sql);
			}
		}
	}

	return SQL;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//验证insert into values语句是否有效
string insert_into_values(string SQL, int start, string sql)
{
	string temp;                      //insert into 表名 values ( 值1 , 值2 , … , 值n ) ;
	string T;
	int end;
	//获取(
	start = SQL.find_first_not_of(' ', start);
	end = SQL.find_first_not_of('(', start);
	temp = SQL.substr(start, end - start);  //temp = (
	start = end;
	//若无，打印出错
	if (temp.empty())
	{
		cout << "syntax error: lack of '('" << endl;
		SQL = "99";
	}
	else if (temp != "(")
	{
		cout << "syntax error: multiple '(' have been found" << endl; //左括号缺失或过多
		SQL = "99";
	}
	else
	{
		end = SQL.rfind(')', SQL.length());	//最后一个右括号
		if (end == -1)
		{
			cout << "syntax error: lack of ')'" << endl;
			SQL = "99";
		}
		else
		{
			temp = SQL.substr(start, end - start);  //temp = ( 值1 , 值2 , … , 值n )
			start = end + 1;
			//若无，打印出错
			if (temp.empty())
			{
				cout << "syntax error: syntax error for insert into statement!" << endl;
				SQL = "99";
			}
			else
			{
				//replace(temp.begin(), temp.end(), ' ', ''); // replace allchar 'x' to 'y'
				SQL = "30" + sql + ' ' + temp;
				//cout << SQL;
			}
		}
	}
	return SQL;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//验证delete语句是否有效
string delete_clause(string SQL,int start)
{
	return SQL;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//验证 delete from where 语句是否有效
string delete_from_where(string SQL,int start,string sql)
{
	return SQL;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//将表达式转化为内部形式
string get_expression(string temp,string sql)
{
	return sql;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//获取表达式组的每个表达式
string get_each(string T,string sql,string condition)
{
	return sql;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//验证use语句是否有效
string use_clause(string SQL,int start)
{
	return SQL;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//验证execfile语句是否有效
string execfile_clause(string SQL,int start)
{
	return SQL;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//验证quit语句是否有效
string quit_clause(string SQL,int start)
{
	return SQL;
}
