#include "MiniSQL.h"
#include "Interpreter.h"
#include "API_Module.h"
#include "Catalog_Manager.h"
#include "bufferManager.h"
using namespace std;

APIMoudule api;
CatalogManager cm;
bufferManager bm;

int main(void)
{
	string SQL;
	//��ӡ������Ϣ
	cout<<endl;
	cout << "\t\t*************************************************" << endl;
	cout << "\t\t**                   MiniSQL                   **" << endl;
	cout << "\t\t**                Version (1.0)                **" << endl;
	cout << "\t\t**                                             **" << endl;
	cout << "\t\t**     copyright(2014) all right reserved!     **" << endl;
	cout << "\t\t*************************************************" << endl;
	cout << "Type HELP; for instructions." << endl;
	cout << endl;

	while(1)
	{
		cout << "MiniSQL-->> ";
		SQL = Interpreter(api.read_input());
		SQLstatement s(SQL);
		api.API(s);
				
		//s.outputinfo();
		
	}
	return 0;
}

