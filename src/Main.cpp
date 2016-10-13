/*
 * main.cpp

 *
 *  Created on: 29 Eki 2015
 *      Author: Tutku
 */
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <cstring>
#include <iostream>
using namespace std;

/*this function is to return data types*/
string findType(string str)
{
	if(str=="S")
	{
		return "A STRING";
	}
	else if(str=="D")
	{
		return "A DOUBLE";
	}
	else if(str=="I")
	{
		return "AN INTEGER";
	}

	return "\0";/*else*/
}
/*this function is to add data in queue */
int queue_push(string* array, string* arrayType,string file_contents, ofstream& outputFile,int count)
{
	int size, isunknown=0;
	string str_temp;/*this variable for parsed line*/
	string str;
	size=file_contents.size();

	if(file_contents.substr(4,1)=="\"")/*check " character for string control*/
	{
		str_temp=file_contents.substr(4,size);/*read string after command*/
		istringstream string_stream(str_temp);

		while (getline(string_stream, str, '"')) /*read string between "*/
		{
			if(str!="\0")/*if str does not null*/
			{
				array[count]=str;/*push data  to queue*/
				arrayType[count]="S";/*push data stype to type queue*/
				outputFile << "A STRING ADDED TO COLLECTION" <<"\n";
			}
		}

	}
	else/*not string*/
    {
		str_temp=file_contents.substr(4,size);
		const char *pch;/*pointer char*/
		char chrArray[size];
		strcpy(chrArray,str_temp.c_str());

		pch=str_temp.c_str();
		/*check char by char include letter or any character out of dot*/
		for(int i=0; i<size-4; i++)/*first 4 char is put */
		{
            isunknown=0;
			if(!isdigit(chrArray[i]) && chrArray[i]!='.' )
			{
			    isunknown=1;
                break;
			}
		}
		if(isunknown==1)
		{
            outputFile <<"UNKNOWN COMMAND OR DATA TYPE\n";
            return -1;
		}
        else
        {
            if(strstr(pch,".")) /*check double*/
            {
                array[count]=pch;
                arrayType[count]="D";

            }
            else /*integer*/
            {

                array[count]=str_temp;
                arrayType[count]="I";
            }
            if(arrayType[count]=="D")
            {
                outputFile << "A DOUBLE ADDED TO COLLECTION\n";
            }
            if(arrayType[count]=="I")
            {
                outputFile << "AN INTEGER ADDED TO COLLECTION\n";
            }

		}
   }
	count++;
	return 0;
}
/*delete data from queue*/
int queue_pop(string* array, string* arrayType,ofstream& outputFile, int arrayStartpoint)
{
	if(array[arrayStartpoint]!="\0")/*begin of queue does not null*/
	{
		outputFile<<findType(arrayType[arrayStartpoint])<<" DELETED\n";
		arrayStartpoint++;
	}
	else
	{
		outputFile<<"DELETION IGNORED\n";
	}
	return arrayStartpoint;
}
/*add the commands to stack*/
void stack_push(string* arrayStack,string commandline)
{
	for(int i=0;i<100;i++)
	{
		if(arrayStack[i]=="\0")
		{
			arrayStack[i]=commandline;
			break;
		}
	}
}
/*delete the commands from stack*/
string stack_pop(string* arrayStack, int arrayStackTop)
{
	string command;
	if(arrayStack[arrayStackTop]!="\0")/*if stack does not empty*/
	{
		command=arrayStack[arrayStackTop];
		arrayStackTop--;
	}
	return command;
}
/*function for undo command*/
int undo(string *arrayStack,string* array, string * arrayType,int arrayStackTop,ofstream& outputFile, int arrayStartpoint,int count)
{
	string command;
	int diff=0, i=0,isreserved=0;
	command=stack_pop(arrayStack,arrayStackTop);/*take command where is top of stack*/
	if((command=="UNDO")||(command=="REDO"))
	{
	    for(i=1;i<100;i++)/*100 stack size*/
	    {
	        if(command=="UNDO")
	        {
                diff++;
                arrayStack[arrayStackTop-i+1]="\0";/*we are looking at how long go back from undo.*/
                command=stack_pop(arrayStack,arrayStackTop-i);
	        }
	        else if((command=="REDO")&&((stack_pop(arrayStack,arrayStackTop-i)=="UNDO")||(stack_pop(arrayStack,arrayStackTop-i-1)=="UNDO")))
	        {
                diff++;
                arrayStack[arrayStackTop-i+1]="\0";
                isreserved=1;
	        }
	        else if(command=="PRINT")
	        {
                arrayStack[arrayStackTop-i+1]="\0";
                command=stack_pop(arrayStack,arrayStackTop-i);
                diff++;
            }
            else
            {
                if(isreserved==1)
                {
                    outputFile<<"REDO REVERSED\n";
                }
                command=stack_pop(arrayStack,arrayStackTop-diff-1);
	            i=100;
            }
	    }

	}
	if(command=="PRINT")/*if command at the top of the stack is print, go to the one bottom */
	{
		command=stack_pop(arrayStack,arrayStackTop-1);
	}
	if(command=="DELETE")/*if command at the top of the stack is delete, undo delete */
	{
		outputFile<<"DELETION REVERSED\n";
		arrayStartpoint--;
	}
	if (command.substr(0,3)=="PUT")/*if command at the top of the stack is put, undo put */
	{
		outputFile<<"PUTTING REVERSED\n";
		return -2;
	}
	if (command.substr(0,3)=="ADD")/*if command at the top of the stack is add, undo add */
	{
        outputFile<<"ADDITION REVERSED\n";
        return -3;
	}
	return arrayStartpoint;
}
int main (int argc, char* argv[])
{
	string line;
	string file_contents;
	string str;

	string array[100];/*data queue*/
	string arrayType[100];/*data types queue*/
	string arrayStack[100];/*commands stack*/

	int count=0;
	int arrayStartpoint=0;
	int arrayStackTop=-1;
	int isignored=0;/*ignored command*/
	int isunknown=0;/*unknown data type*/

	ifstream inputFile;
	inputFile.open(argv[1]);

	ofstream outputFile;
	outputFile.open(argv[2]);


	while (std::getline(inputFile, line))
	{
	    file_contents = line; /*read input file line by line*/
            isignored=0;
	    	if(file_contents.substr(0,3)=="PUT") /* cut "PUT" word, command is put*/
	    	{
	    		isunknown=0;
	    	    if((count-arrayStartpoint)==20)
	    	    {
                    arrayStartpoint=queue_pop(array,arrayType,outputFile,arrayStartpoint);
	    	    }
	    		isunknown=queue_push(array, arrayType,file_contents,outputFile,count);
	    		if(isunknown!=-1){count++;}


	    	}
	    	else if(file_contents.substr(0,5)=="PRINT")/*command is print*/
	    	{
	    		for(int i=arrayStartpoint; i<count; i++)
	    		{
	    			outputFile << array[i];
	    		}
	    		outputFile<<endl;

	    	}
	    	else if(file_contents.substr(0,3)=="ADD")/*command is add*/
	    	{
	    		if((arrayType[arrayStartpoint]!="\0")&&(arrayType[arrayStartpoint+1]!="\0"))
	    		{
					if((arrayType[arrayStartpoint]=="S")||(arrayType[arrayStartpoint+1]=="S"))
					{
						outputFile<<findType(arrayType[arrayStartpoint+1])<<" ADDED TO "<<findType(arrayType[arrayStartpoint])<<" AND NEW STRING PUT END OF THE COLLECTION\n";
						array[count]=array[arrayStartpoint]+ array[arrayStartpoint+1];
						arrayType[count]="S";
						count++;
					}
					else if((arrayType[arrayStartpoint]=="D")||(arrayType[arrayStartpoint+1]=="D"))
					{
						double firstNum, secondNum, sum;
						outputFile<<findType(arrayType[arrayStartpoint+1])<<" ADDED TO "<<findType(arrayType[arrayStartpoint])<<" AND NEW DOUBLE PUT END OF THE COLLECTION\n";
						istringstream(array[arrayStartpoint])>>firstNum;
						istringstream(array[arrayStartpoint+1])>>secondNum;
						sum=firstNum+secondNum;
						std::ostringstream strs;
						strs << sum;
						array[count] = strs.str();
						arrayType[count]="D";
						count++;
					}
					else if((arrayType[arrayStartpoint]=="I")||(arrayType[arrayStartpoint+1]=="I"))
					{
						int firstintNum, secondintNum, intsum;
						outputFile<<findType(arrayType[arrayStartpoint])<<" ADDED TO "<<findType(arrayType[arrayStartpoint+1])<<" AND NEW INTEGER PUT END OF THE COLLECTION\n";
						istringstream(array[arrayStartpoint])>>firstintNum;
						istringstream(array[arrayStartpoint+1])>>secondintNum;
						intsum=firstintNum+secondintNum;
						std::ostringstream strings;
						strings << intsum;
						array[count] = strings.str();
						arrayType[count]="I";
						count++;
					}
					arrayStartpoint=arrayStartpoint+2;
	    		}
	    		else
	    		{
	    			outputFile<<"ADDITION IGNORED\n";
	    			isignored=1;
	    		}


	    	}
	    	else if(file_contents.substr(0,4)=="UNDO")/*command is undo*/
	    	{
	    		int isChange=0;
	    		isChange=undo(arrayStack,array,arrayType,arrayStackTop,outputFile,arrayStartpoint,count);
	    		if(isChange==-2)
	    		{
	    			count--;
	    		}
	    		else if(isChange==-3)
	    		{
                    count--;
                    arrayStartpoint=arrayStartpoint-2;
	    		}
	    		else
	    		{
	    			arrayStartpoint=isChange;
	    		}
	    	}
	    	else if(file_contents.substr(0,4)=="REDO")/*command is redo*/
	    	{
	    	    int newarrayStackTop=arrayStackTop;
	    	    while(stack_pop(arrayStack,newarrayStackTop)=="PRINT")
	    	    {
                    newarrayStackTop=newarrayStackTop-1;
	    	    }
	    		if(stack_pop(arrayStack,newarrayStackTop)!="UNDO")/*if there is not undo before redo*/
	    		{
	    			outputFile<<"REDO IGNORED\n";
	    			isignored=1;
	    		}
	    		else
	    		{
	    		    outputFile<<"UNDO REVERSED\n";
	    			int i=newarrayStackTop;
	    			while(i>0)
	    			{
	    				if((stack_pop(arrayStack,i)=="UNDO")||(stack_pop(arrayStack,i)=="PRINT"))
	    				{
	    					i--;
	    				}
	    				else
	    				{
	    					break;
	    				}
	    			}
	    			string oldcommand= stack_pop(arrayStack,i);

	    			if(oldcommand.substr(0,3)=="PUT")
					{
						count++;
					}
					else if(oldcommand.substr(0,3)=="ADD")
					{
						count++;
                        arrayStartpoint=arrayStartpoint+2;
					}
					else if(oldcommand.substr(0,6)=="DELETE")
					{
                        arrayStartpoint--;
                    }
	    		}
	    	}
	    	else if(file_contents.substr(0,6)=="DELETE")
	    	{
	    	    if(count!=0)
	    	    {
                    arrayStartpoint=queue_pop(array,arrayType,outputFile,arrayStartpoint);
	    	    }
	    	    else
	    	    {
	    	        isignored=1;
                    outputFile<<"DELETION IGNORED\n";
	    	    }


	    	}
	    	else/*wrong command*/
	    	{
	    		outputFile<<"UNKNOWN COMMAND OR DATA TYPE\n";
	    	}
	    	if(isignored==0)
	    	{
                stack_push(arrayStack,line);
                arrayStackTop++;
	    	}

	}

	inputFile.close();
	outputFile.close();

	return 0;
}

