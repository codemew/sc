#include <iostream>
#include <fstream>
#include <string.h>
#include <map>
#include <set>
#include <chrono>
#include <unistd.h>

using namespace std;
using namespace std::chrono;

#define FOFILE "StreamTokenMapFO.config"
#define OTUPUTFILENAME "PriceMonitor.config"
#define FILEPATH "C:/Users/Administrator/Desktop/sc" // Keep blank for Current Directory

#define THIRDCOLUMN 0
#define FOURTHCOLUMN 100

#define FIRSTLINE "1"
#define SECONDLINE "0,PriceMonitor_Fut.log,"

// Default columns
int thirdColumn = THIRDCOLUMN;
int fourthColumn = FOURTHCOLUMN;

// FILE PATH
string ipfilename = FOFILE;
string opfilename = OTUPUTFILENAME;
string filepath = FILEPATH; // USE THIS FOR CUSTOM PATH OTHER THAN PWD

// DataStructures
map<int, int> token2Stream;
map<int, string> token2Symbol;
set<int> stream_set;

int stream_no[20];
int column_no[2];
int priceMonitorEntries = 0;

void printusage()
{
    cout << "i: InputFileName\n";
    cout << "s: Stream(s) Comma Seperated\n";
    cout << "p: Path of Input and Output file\n";
    cout << "w: Custom PriceMonitor Params\n";
}

// return total number of streams parsed
int parsecomma(string str, int *p)
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

int writefile(int stream)
{

    ofstream fp;
    string outputfilename = filepath;
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
            fp << 1 << ',' << token2Symbol[it->first] << ',' << thirdColumn << ',' << fourthColumn << ',' << endl;
            count++;
        }
    }

    std::cout << "Stream " << stream << " : " << count << " Entries" << endl;

    fp.close();

    return count;
}

void createFile(string filename)
{

    ofstream fp;
    string outputfilename = filepath;
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

void fileParser(string ipfile)
{

    int token, stream;
    string symbol;
    string inputFileName;

    if (filepath != "")
    {
        inputFileName = filepath;
        if (inputFileName[inputFileName.length() - 1] != '/')
            inputFileName.append("/");
    }
    // one time update of the file path
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

int addStream(int *stream, int count)
{
    int entries = 0;
    while (count-- > 0)
    {
        auto pos = stream_set.find(*stream);
        if (pos != stream_set.end())
        {
            entries += writefile(*stream++);
        }
        else
            std::cout << "Invalid Stream or Stream not found!" << endl;
    }

    return entries;
}

void printTime(std::chrono::_V2::system_clock::time_point *stime)
{
    std::chrono::_V2::system_clock::time_point start = *stime;
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

    // new changes //options parser

    int opt,
        stream_count = 0;
    if (argc == 1)
        printusage();
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
                printf("Stream number %s will be added\n", optarg);
                stream_count = parsecomma(optarg, stream_no);
                break;
            case 'p':
                printf("Path: %s\n", optarg);
                filepath = optarg;
                break;
            case 'w':
                printf("Columns will be : %s\n", optarg);
                parsecomma(optarg, column_no);
                thirdColumn = column_no[0];
                fourthColumn = column_no[1];
                break;
            case 'h':
                printusage();
                break;
            default:
                printf("Unknown option: %c\n", optopt);
                printusage();
                break;
            }
        }
    }
    // DRIVER CODE
    if (argc > 1)
    {
        fileParser(ipfilename);

        createFile(opfilename);
        totalentries = addStream(stream_no, stream_count);

        std::cout << endl
                  << opfilename << " : " << totalentries << " Total entires added " << endl;

        // Total TIME
        printTime(&start);
    }
    // end changes

    // std::cout << pricemon << " Entries added to PriceMonitor.config" << endl;

    return 0;
}