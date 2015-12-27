/****************************************************************************
  FileName     [ dbTable.cpp ]
  PackageName  [ db ]
  Synopsis     [ Define database Table member functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2015-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <iomanip>
#include <string>
#include <cctype>
#include <cassert>
#include <set>
#include<sstream>
#include <algorithm>
#include<climits>
#include "dbTable.h"

using namespace std;

/*****************************************/
/*          Global Functions             */
/*****************************************/
ostream& operator << (ostream& os, const DBRow& r)
{
	for(int i=0 ; i < r.size() ; i++)
	{
		
		if(r._data[i]>=INT_MIN && r._data[i]<INT_MAX)	os<<r._data[i];
		else	cout<<".";
		if(i !=  r.size()-1)	os<<" ";
	}
   // TODO: to print out a row.
   // - Data are seperated by a space. No trailing space at the end.
   // - Null cells are printed as '.'
   return os;
}

ostream& operator << (ostream& os, const DBTable& t)
{
	for(int i=0 ;i < t.nRows() ;i++)
	{
		for(int j=0;j<t.nCols() ; j++)
		{
			if(t.getData(i,j)>=INT_MIN &&t.getData(i,j)<INT_MAX)	os<<setw(6)<<right<<t.getData(i,j);
			else cout<<" ";
		}
		cout<<endl;
	}
   // TODO: to print out a table
   // - Data are seperated by setw(6) and aligned right.
   // - Null cells should be left blank (printed as ' ').
   return os;
}

ifstream& operator >> (ifstream& ifs, DBTable& t)
{
		
	ifs.seekg(0, ios::end);
    	size_t size = ifs.tellg();
    	string buffer(size, ' ');
    	ifs.seekg(0);
    	ifs.read(&buffer[0], size);
	t.parsingCsv(buffer);
// - You can assume all the data of the table are in a single line.
   return ifs;
}

/*****************************************/
/*   Member Functions for class DBRow    */
/*****************************************/
void
DBRow::removeCell(size_t c)
{
	_data.erase(_data.begin()+c);   
// TODO
}

/*****************************************/
/*   Member Functions for struct DBSort  */
/*****************************************/
bool
DBSort::operator() (const DBRow& r1, const DBRow& r2) const
{
	for(int i=0;i<_sortOrder.size();i++)
	{
		if(r1[ _sortOrder[i]] < r2[ _sortOrder[i]])	return true;
		if(r1[ _sortOrder[i]] > r2[ _sortOrder[i]])	return false;
	}
   // TODO: called as a functional object that compares the data in r1 and r2
   //       based on the order defined in _sortOrder
   return false;
}

/*****************************************/
/*   Member Functions for class DBTable  */
/*****************************************/
void
DBTable::reset()
{
	_table.clear();
   // TODO
}

void
DBTable::addCol(const vector<int>& d)
{
	if(d.size() != (*this).nCols() )	return;
	for(int i=0;i<_table.size();i++)	_table[i].addData(d[i]);	  
 // TODO: add a column to the right of the table. Data are in 'd'.
}

void
DBTable::delRow(int c)
{
	if(c<0 || c>_table.size()-1)	return;
	_table.erase(_table.begin()+c);
	// TODO: delete row #c. Note #0 is the first row.
}

void
DBTable::delCol(int c)
{
   // delete col #c. Note #0 is the first row.
   for (size_t i = 0, n = _table.size(); i < n; ++i)
      _table[i].removeCell(c);
}

// For the following getXXX() functions...  (except for getCount())
// - Ignore null cells
// - If all the cells in column #c are null, return NAN
// - Return "float" because NAN is a float.
float
DBTable::getMax(size_t c) const
{
	int colMax = INT_MIN;
	for(int i=0;i<_table.size();i++)
	{
		if( getData(i,c) > colMax && getData(i,c) != INT_MAX)	colMax = getData(i,c);
	}
	// TODO: get the max data in column #c
   return colMax;
}

float
DBTable::getMin(size_t c) const
{
	int colMin = INT_MAX;
	for(int i=0;i<_table.size();i++)
	{
		if( getData(i,c) < colMin && getData(i,c) != INT_MAX)	colMin = getData(i,c);
	}
	// TODO: get the max data in column #c
   return colMin;
   // TODO: get the min data in column #c
}

float 
DBTable::getSum(size_t c) const
{
	int	colSum  = 0; 
	for(int i=0;i<_table.size();i++)
	{
		if( getData(i,c) != INT_MAX)	colSum += getData(i,c);
	}
   // TODO: compute the sum of data in column #c
   return colSum;
}

int
DBTable::getCount(size_t c) const
{
	int colCount  = 0;
	bool flag = true; 
	for(int i=0;i<_table.size();i++)
	{
		if( getData(i,c) == INT_MAX)
		{
			flag = false;
		}
		else
		{
			for(int j=0;j<i;j++)
			{
				if(getData(i,c) == getData(j,c))
				{
					flag = false;
					break;				
				}
			}
			if(flag == true)	colCount++;	
		}
	}
   return colCount;
   // TODO: compute the number of distinct data in column #c
   // - Ignore null cells
}

float
DBTable::getAve(size_t c) const
{
	float	colSum  = 0;
	float	colCount = 0;	 
	for(int i=0;i<_table.size();i++)
	{
		if( getData(i,c) != INT_MAX)	
		{
			colSum += getData(i,c);
			colCount++;
		}
	}
   // TODO: compute the average of data in column #c
   return colSum/colCount;
}

void
DBTable::sort(const struct DBSort& s)
{
	std::sort(_table.begin(),_table.end(),s);
   // TODO: sort the data according to the order of columns in 's'
}

void
DBTable::printCol(size_t c) const
{
	for(int i=0;i<_table.size();i++)
	{
		if(_table[i][c]>=INT_MIN && _table[i][c]<INT_MAX)	cout<<_table[i][c];
		else	cout<<".";
		if(i !=  _table.size()-1)	cout<<" ";
	}	
   // TODO: to print out a column.
   // - Data are seperated by a space. No trailing space at the end.
   // - Null cells are printed as '.'
}

void
DBTable::printSummary() const
{
   size_t nr = nRows(), nc = nCols(), nv = 0;
   for (size_t i = 0; i < nr; ++i)
      for (size_t j = 0; j < nc; ++j)
         if (_table[i][j] != INT_MAX) ++nv;
   cout << "(#rows, #cols, #data) = (" << nr << ", " << nc << ", "
        << nv << ")" << endl;
}
void
DBTable::parsingCsv(string& csvString)
{
      bool newLine = false;
      string num;
      num.clear();
      vector<string> line;
      vector<vector<string> > lines;
      string::const_iterator aChar = csvString.begin();
      while(aChar<=csvString.end())
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
	line.push_back(num);
	lines.push_back(line);
	num.clear();
	line.clear();      
      stringstream convert;
      int number;
	for(int i=0;i<lines.size();i++)
	{
		DBRow tmpDBRow;
		for(int j=0;j<lines[0].size();j++)
		{
			if(  lines[i][j].compare("\0") != 0 )
			{
				convert << lines[i][j];
				convert >> number;
				tmpDBRow.addData(number);
				convert.str("");
				convert.clear();
			}
			else
			{
				tmpDBRow.addData(INT_MAX);
			}
		}
		(*this).addRow(tmpDBRow);
	}
}

