#include <iostream>
#include <fstream>
#include <string.h>
#include <map>
#include <set>
#include <chrono>
#include <unistd.h>

using namespace std;
using namespace std::chrono;



#define FOFILE              "StreamTokenMapFO.config"
#define OTUPUTFILENAME      "PriceMonitor.config"
#define FILEPATH            "c:/Users/Administrator/Desktop/sc/"                                              //Keep blank for Current Directory


#define FIRSTCOLUMN         1
#define THIRDCOLUMN         0
#define FOURTHCOLUMN        100

#define FIRSTLINE           "1"
#define SECONDLINE          "0,PriceMonitor_Fut.log,                         //identifier,logfilename,"




//FILE PATH
    string ipfilename   =   FOFILE;
    string opfilename   =   OTUPUTFILENAME;
    string filepath     =   FILEPATH;		                                //USE THIS FOR CUSTOM PATH OTHER THAN PWD



//DataStructures
    map< int, int > token2Stream;
    map< int, string > token2Symbol;
    multiset< int > freqset;
    set< int > stream_set;

    int token,stream;
    string symbol;

    int pricemon=0;



void writefile(int stream) {	

    ofstream fp;
    string outputfilename = filepath;
    outputfilename.append(opfilename);
    

    fp.open(outputfilename,ios::out|ios::app); 
	if(!fp.is_open()){
        cout<<"Cannot create file for writting"<<endl;
        exit(0);
    }
   int count=0;
    for(auto it = token2Stream.begin();it!=token2Stream.end();it++)
     {  
        if(it->second==stream){
            fp<<FIRSTCOLUMN<<','<<token2Symbol[it->first]<<','<<THIRDCOLUMN<<','<<FOURTHCOLUMN<<','<<endl;
            count++;
        }
     }

    cout<<endl<<count<<" Entries add to file.."<<endl;
    pricemon+=count;
    fp.close();
}

void createFile(string filename){

    ofstream fp;
    string outputfilename = filepath;
    outputfilename.append(filename);
    

    fp.open(outputfilename,ios::out|ios::trunc); 

    if(!fp.is_open()){
        cout<<"Cannot create file for writting";
        exit(0);
    }
    
    if(FIRSTLINE!="" && SECONDLINE!=""){
    fp<<FIRSTLINE<<endl;
    fp<<SECONDLINE<<endl;}
    
    fp.close();
    
	cout<<OTUPUTFILENAME<<" File Created."<<endl;
}

void printinfo(){

    cout<<"\nNo. of Symbols found in each Stream: "<<endl;

    cout<<"\n";
    for(auto it=stream_set.begin();it!=stream_set.end();it++){
        cout<<"Strm"<<*it<<"\t";
    }cout<<endl;

    for(auto it=stream_set.begin();it!=stream_set.end();it++){
        cout<<freqset.count(*it)<<"\t";
    }cout<<endl<<endl;

    


}


int main(int argc, char const *argv[]){
    //timer
    auto start = high_resolution_clock::now();


    string inputFileName = filepath;
    inputFileName.append(ipfilename);

    ifstream fip;
    fip.open( inputFileName );
    
    if(!fip.is_open())
    {
        cout<<"Error Opening Input File!!!!";
        exit(0);
    }
    
    
    //Parsing FILE
    
    string line="DEAFULT";
    int symcount=0;
    //while(fp >> temp )
    while(!fip.eof()){
    
    getline(fip,line);

    if(line=="")
        break;

    symcount++;
    
    char* ch =(char*) line.c_str();
    stream = atoi(strtok(ch,","));
        //cout<<stream<<" ";
    symbol=(string)strtok(NULL,",");
        //cout<<symbol<<" ";
    token = atoi(strtok(NULL,","));
       //cout<<token<<endl;

    token2Stream[token] = stream;
    token2Symbol[token] = symbol;
    stream_set.insert(stream);
    freqset.insert(stream);
    }
    fip.close();

    //PARSING TIME
    auto stop = high_resolution_clock::now();
    //auto duration = duration_cast<microseconds>(stop - start);

     duration<double> time_span = duration_cast<duration<double>>(stop - start);

     cout << "Parsing Time taken : " << time_span.count() << " Seconds\n" << endl;

    //Parsing info
    cout<<stream_set.size()<<" Unique Streams"<<endl;
    cout<<symcount<<" Unique Symbols"<<endl;
    printinfo();
    
    
    //DRIVER CODE 
    
    int ch=0;
    int stream;
    createFile(OTUPUTFILENAME);
    cout<<"\nEnter Stream number to add to .config File:";
    	cin>>stream;
    while(ch > -1){    	
    	
    	//check if stream number is present or not
    	auto pos=stream_set.find(stream);
    	if(pos!=stream_set.end()){
    	writefile(stream);    	
    	}
    	else
    	{
    		cout<<"Invalid Stream or Stream not found!"<<endl;
    	}
    	cout<<"Add more Streams? Enter -1 to stop :";
    	cin>>ch;
        stream=ch;
    }
    cout<<pricemon<<" Entries added to PriceMonitor.config"<<endl;
    	

return 0;
}