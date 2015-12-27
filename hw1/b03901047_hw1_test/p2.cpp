#include<iostream>
#include<fstream>
#include<sstream>
#include<string>
#include<vector>
#include<iomanip>
#include<algorithm>
using namespace std;
class Data{
      public:
              Data(int col)
              {
                       _cols = new int[col];
		       size = col;
              };  
	      const int operator[] (size_t i) const
              {
                    int a = _cols[i];
                    return a;
              };
              int & operator[] (size_t i)
              {
                  return _cols[i];
              };
	     int getSize()
	     {
             return size;
             }
	
      private:
              int *_cols;
	      int size;
};

void PRINT(vector<Data> csvData)
{
	for(int i=0;i<csvData.size();i++)
	{
		for(int j=0;j<csvData[0].getSize();j++)
		{
            if(csvData[i][j] != 200)
            {
                                              cout<<setw(4)<<right<<csvData[i][j];
                                              }
            else
            {
                cout<<setw(4)<<right<<"-";
            }
		}
	cout<<endl;
	}
};
void ADD(vector<Data> &csvData,vector<string> rowString)
{
	Data rowData(rowString.size());
	for(int i=0 ; i < rowString.size() ; i++)
	{
		if(rowString[i].compare("-") == 0 )
		{
			rowData[i] = 200;
		}
		else
		{
			stringstream convert;
			convert << rowString[i];
			convert >> rowData[i];
			convert.str("");
			convert.clear();
		}
	}
	csvData.push_back(rowData);
};
void SUM(vector<Data> csvData,int col)
{
	int colSum = 0;
	for(int i=0;i < csvData.size(); i++)
	{
		if(csvData[i][col] != 200)
		{
			colSum += csvData[i][col];
		}
	}	
	cout<<"The summation of data in column #"<<col<<" is "<<colSum<<"."<<endl;
}
void AVE(vector<Data> csvData,int col)
{
	double colSum = 0;
	double num = 0;
	for(int i=0;i < csvData.size(); i++)
	{
		if(csvData[i][col] != 200)
		{
			colSum += csvData[i][col];
			num++;
		}
	}	
	double average = (colSum/num);
	cout<<"The average of data in column #"<<col<<" is "<<average<<"."<<endl;
}
void MAX(vector<Data> csvData,int col)
{
	double currentMax = -99;
	double num = 0;
	for(int i=0;i < csvData.size(); i++)
	{
		if(csvData[i][col] != 200)
		{
			if(currentMax < csvData[i][col] )
			currentMax =  csvData[i][col];
		}
	}	
	cout<<"The maximum value of data in column #"<<col<<" is "<<currentMax<<"."<<endl;
}
void MIN(vector<Data> csvData,int col)
{
	double currentMin = 100;
	double num = 0;
	for(int i=0;i < csvData.size(); i++)
	{
		if(csvData[i][col] != 200)
		{
			if(currentMin > csvData[i][col] )
			currentMin =  csvData[i][col];
		}
	}	
	cout<<"The minimum value of data in column #"<<col<<" is "<<currentMin<<"."<<endl;
}
void COUNT(vector<Data> csvData,int col)
{
	int numCount = 0;
	bool increOrNot = true;
	double num = 0;
	for(int i=0;i < csvData.size(); i++)
	{
		increOrNot = true;
		if(csvData[i][col] != 200)
		{
			for(int j=0 ; j<i ; j++)
			{
				if(csvData[i][col] == csvData[j][col])
				{
					increOrNot = false;
					break;
				}
			}
			if(increOrNot == true )
			numCount++;
		}
	}	
	cout<<"The distinct count of data in column #"<<col<<" is "<<numCount<<"."<<endl;
}
struct SortData{
	bool operator()(const Data& d1,const Data& d2)
	{
		for(int i=0; i < _sortOrder.size() ; i++){
			if(d1[ _sortOrder[i] ] < d2[ _sortOrder[i] ])	return true;
			if(d1[ _sortOrder[i] ] > d2[ _sortOrder[i] ])	return false;
		}
		return false;
	}/*
	void pushOrder(size_t i){
		_sortOrder.push_back(i);
	}
		vector<size_t>::iterator it = _sortOrder.begin();
		while(  it != _sortOrder.end() )
		{
		//	int index = _sortOrder[i];
			if(d1[*it] < d2[*it])
			{
				return true;
			}
			else if(d1[*it] > d2[*it])
			{
				return false;
			}	
			it++;
		}	
	return false;	
	};*/
	void pushOrder(size_t i)
	{
		_sortOrder.push_back(i);
	};
	vector<size_t> _sortOrder;
};

void csvMenu(vector<Data> &csvData)
{
	    while(1)
	    {
                string input;
		cin >> input;
		if(input.compare("PRINT") == 0)
		{
			PRINT(csvData);
        	}
		else if(input.compare("ADD") == 0)
		{
			vector<string> rowStr;
			string lineStr;
			string singleNum;
			stringstream parseStr;
			getline(cin,lineStr);
			parseStr << lineStr;
			while(parseStr >> singleNum)
			{
				rowStr.push_back(singleNum);
			}
			parseStr.str("");
			parseStr.clear();
			ADD(csvData,rowStr);
		}
		else if(input.compare("SUM") == 0)
		{
			int colSum;
			cin>>colSum;
			SUM(csvData,colSum);
		}
		else if(input.compare("AVE") == 0)
		{
			int colAve;
			cin>>colAve;
			AVE(csvData,colAve);
		}
		else if(input.compare("MAX") == 0)
		{
			int colMax;
			cin>>colMax;
			MAX(csvData,colMax);
		}
		else if(input.compare("MIN") == 0)
		{
			int colMin;
			cin>>colMin;
			MIN(csvData,colMin);
		}
		else if(input.compare("COUNT") == 0)
		{
			int colCount;
			cin>>colCount;
			COUNT(csvData,colCount);
		}
		else if(input.compare("SORT") == 0)
		{
			SortData csvSort;
			string lineStr;
			int singleNum;
			stringstream parseStr;
			getline(cin,lineStr);
			parseStr << lineStr;
			while(parseStr >> singleNum)
			{
				csvSort.pushOrder(singleNum);
			}
			parseStr.str("");
			parseStr.clear();
			sort(csvData.begin(), csvData.end(),csvSort );
		}
   	 }
}
void ParsingCSV(const string& csvString,vector<Data>& csvData, int row, int col)
{
	bool newLine = false;
      string num;
      num.clear();
      vector<string> line;
      vector<vector<string> > lines;
      string::const_iterator aChar = csvString.begin();
      int numCSV;
      while(aChar!=csvString.end())
      {     
	      switch(*aChar)
	      {
		      case ',':
		      		newLine  = false;
			      line.push_back(num);
			      num.clear();
			      break;
		      case '\n':
		      case '\r':
			      if(newLine == false)
			      {
				      
				      line.push_back(num);
				      lines.push_back(line);
				      num.clear();
				      line.clear();
				      newLine = true;
			      }
			      break;
		      default:
			      newLine = false;
			      num += *aChar ;
			      break;
	      }
	      aChar++;
      
      }
      stringstream convert;
      int number = 200;
	for(int i=0;i<row;i++)
	{
		Data rowData(col);
		for(int j=0;j<col;j++)
		{
			char* nullChar = new char[1];
			*nullChar = '\0';  
			const char* numChar = lines[i][j].c_str();
			if(  lines[i][j].compare("\0") != 0 )
			{
				convert<< lines[i][j];
				convert >> number;
				rowData[j] = number;
				convert.str("");
				convert.clear();
			}
			else
			{
				rowData[j] = 200;
			}
		}
		csvData.push_back(rowData);
	}
}
int main(){
	string  fileName;
	int row, column;
	cout<<"Please enter the file name: ";
	cin>>fileName;
	cout<<"Please enter the number of rows and columns:";
    cin>>row>>column;
    const char* c = fileName.c_str();
    ifstream file(c);
    if(file.is_open()) cout<<"File \""<<fileName<<"\" was read in successfully."<<endl;
    file.seekg(0, ios::end);
    size_t size = file.tellg();
    string buffer(size, ' ');
    file.seekg(0);
    file.read(&buffer[0], size);
    vector<Data> csvData; 
    ParsingCSV(buffer,csvData,row,column);
    file.close();
    csvMenu(csvData);
    return 0;

}
