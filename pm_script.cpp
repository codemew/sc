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
#define FILEPATH            "" 
#define DEFAULTDERIVEDPATH  "config/InputAdapter/StreamTokenMaps/"

#define FIRSTLINE "1"
#define SECONDLINE "0,PriceMonitor_Fut.log,"

// FILE PATH
string ipfilename = FOFILE;
string opfilename = OTUPUTFILENAME;
string filepath = FILEPATH; // USE THIS FOR CUSTOM PATH OTHER THAN PWD

// DataStructures
map<int, int> token2Stream;
map<int, string> token2Symbol;
set<int> stream_set;

int stream_nos[20];
//default cval, divisor value
int cval_div[]={0,100};
int priceMonitorEntries = 0;

void Print_Usage()
{
    cout << "i: Input File Name                     [REQ - N]|[Default: StreamTokenMapFO.config]\n";
    cout << "s: Stream(s) Comma Seperated           [REQ - Y]\n";
    cout << "p: Path of ConfigFile                  [REQ - N]|[Default: $(PWD)/"<<DEFAULTDERIVEDPATH<<"]\n";
    cout << "w: CVAL,DIVISOR for PriceMonitor       [REQ - N]|[Default: 0,100]\n";
}

// return total number of streams parsed
int Parse_Comma_Separated_String(string str, int *p)
{
    int count = 0;
    char *ch = (char *)str.c_str();

    char *tok = strtok(ch, ",");
    int i = 0;
    while (tok)
    {
        p[i++] = atoi(tok);
        tok = strtok(NULL, ",");
        count++;
    }
    return count;
}

int Write_File(int stream)
{
    ofstream fp;
    string outputfilename = "";
    outputfilename.append(opfilename);

    fp.open(outputfilename, ios::out | ios::app);
    if (!fp.is_open())
    {
        std::cout << "Cannot create file for writting" << endl;
        exit(0);
    }
    int count = 0;
    for (auto it = token2Stream.begin(); it != token2Stream.end(); it++)
    {
        if (it->second == stream)
        {
            fp << 1 << ',' << token2Symbol[it->first] << ',' << cval_div[0] << ',' << cval_div[1] << ',' << endl;
            count++;
        }
    }

    std::cout << "Stream " << stream << " : " << count << " Entries" << endl;

    fp.close();

    return count;
}

void Create_File(string filename)
{

    ofstream fp;
    string outputfilename = "";
    outputfilename.append(filename);

    fp.open(outputfilename, ios::out | ios::trunc);

    if (!fp.is_open())
    {
        std::cout << "Cannot create file for writting";
        exit(0);
    }

    if (FIRSTLINE != "" && SECONDLINE != "")
    {
        fp << FIRSTLINE << endl;
        fp << SECONDLINE << endl;
    }

    fp.close();

    std::cout << filename << " File Created.\n"
              << endl;
}

void File_Parser(string ipfile)
{

    int token, stream;
    string symbol;
    string inputFileName;



        if(filepath=="")
            filepath=DEFAULTDERIVEDPATH;
        
            //adding '/' at the end
            inputFileName = filepath;
                if (inputFileName[inputFileName.length() - 1] != '/')
                    inputFileName.append("/");
            filepath = inputFileName;
    
        inputFileName.append(ipfilename);


    ifstream fip;
    fip.open(inputFileName);

    if (!fip.is_open())
    {
        std::cout << "Error Opening Input File:  " << inputFileName << endl;
        exit(0);
    }
    std::cout << "Opening Input File : " << inputFileName << endl;

    // Parsing FILE
    string line = "DEAFULT";
    // while(fp >> temp )
    while (!fip.eof())
    {

        getline(fip, line);

        if (line == "")
            break;

        char *ch = (char *)line.c_str();
        stream = atoi(strtok(ch, ","));
        // std::cout<<stream<<" ";
        symbol = (string)strtok(NULL, ",");
        // std::cout<<symbol<<" ";
        token = atoi(strtok(NULL, ","));
        // std::cout<<token<<endl;

        token2Stream[token] = stream;
        token2Symbol[token] = symbol;
        stream_set.insert(stream);
    }
    fip.close();
}

int Add_Stream(int *stream, int count)
{
    int entries = 0;
    while (count-- > 0)
    {
        auto pos = stream_set.find(*stream);
        if (pos != stream_set.end())
        {
            entries += Write_File(*stream++);
        }
        else
            std::cout << "Invalid Stream or Stream not found!" << endl;
    }

    return entries;
}

void Print_time(std::chrono::_V2::system_clock::time_point *start_time)
{
    std::chrono::_V2::system_clock::time_point start = *start_time;
    std::chrono::_V2::system_clock::time_point stop = high_resolution_clock::now();
    // auto duration = duration_cast<microseconds>(stop - start);
    duration<double> time_span = duration_cast<duration<double>>(stop - start);
    std::cout << "\nTime taken : " << time_span.count() << " Seconds\n"
              << endl;
}

int main(int argc, char *argv[])
{
    // timer
    std::chrono::_V2::system_clock::time_point start = high_resolution_clock::now();

    int totalentries = 0;  
    int opt,stream_count = 0;
     // new changes --getopt()
    if (argc == 1)
        Print_Usage();
    else
    {
        while ((opt = getopt(argc, argv, "i:s:p:w:h")) != -1)
        {
            switch (opt)
            {
            case 'i':
                printf("Input File name: %s\n", optarg);
                ipfilename = optarg;
                break;
            case 's':
                printf("Stream(s) %s will be added\n", optarg);
                stream_count = Parse_Comma_Separated_String(optarg, stream_nos);
                break;
            case 'p':
                printf("Path: %s\n", optarg);
                filepath = optarg;
                break;
            case 'w':
                printf("CVAL, DIVISOR will be : %s\n", optarg);
                Parse_Comma_Separated_String(optarg, cval_div);
                break;
            case 'h':
                Print_Usage();
                break;
            default:
                printf("Unknown option: %c\n", optopt);
                Print_Usage();
                break;
            }
        }
    }
    // DRIVER CODE
    if (stream_count==0)
    {
        std::cout<<"[-s] missing (Stream is required field)\n";
        Print_Usage();
    }
    else
    {   
        File_Parser(ipfilename);
        Create_File(opfilename);
        totalentries = Add_Stream(stream_nos, stream_count);
        std::cout << endl
                  << opfilename << " : " << totalentries << " Total entires added " << endl;
        // Total TIME
        Print_time(&start);
    }
    // end changes


    return 0;
}