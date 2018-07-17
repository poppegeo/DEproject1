#include <sstream>
#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <vector>
#include <iterator>

using namespace std;

#define SSTR(x) static_cast<std::ostringstream &>(           \
                    (std::ostringstream() << std::dec << x)) \
                    .str()

typedef vector<string> person;
typedef vector<person> drugGroup;

bool costAlphasort(vector<string> &i, vector<string> &j);
bool alphabetacle(vector<string> &i, vector<string> &j);
void getperson(ifstream &infile, string (&persons)[5]);
void printPers(vector<person> persons);
void sortByDrug(vector<person> &persons, vector<string> &drugList, vector<drugGroup> &group);
void dumpToMain(vector<string> &drugList, vector<drugGroup> &topPeople, vector<drugGroup> &tempTopPeople, std::vector<vector<string>> &drugFileNames, vector<vector<int>> &numLines, vector<string> &mainFileNames);
void printGroup(vector<drugGroup> group, int pmax);
void printToFiles(vector<string> &drugList, vector<drugGroup> &group, std::vector<vector<string>> &drugFileNames, vector<vector<int>> &numLines);
void addEmUp(vector<string> &drugList, vector<vector<int>> &numLines, vector<string> &mainFileNames, vector<drugGroup> &finalGroup);
void dotText(string &filename);

int main()
{

    ifstream infile;
    ifstream infile2;
    ofstream infile3;

    infile.open("itcont.txt");
    string pers[5];
    string header[5];
    int const Np = 500; // number of people to be handled at a time

    vector<vector<int>> numLines;
    std::vector<string> v1;
    std::vector<string> mainFileNames;
    vector<person> persons;
    std::vector<vector<string>> drugFileNames;
    vector<drugGroup> group;
    vector<drugGroup> topPeople;
    vector<drugGroup> fileMap;
    vector<drugGroup> finalGroup;
    vector<string> drugList;
    vector<string> fill;
    fill.push_back("eempty");
    vector<person> filler;
    filler.push_back(fill);

    if (infile.is_open())
    {
        getperson(infile, header);

        while (infile.good())
        {
            for (int j = 0; j < Np; j++)
            {
                getperson(infile, pers);
                std::vector<string> tempVec(pers, pers + sizeof pers / sizeof pers[0]);
                persons.push_back(tempVec);

                if (!infile.good())
                    break;
            }

            cout << "test1 here" << endl;
            for (int i = 0; i < drugList.size(); i++)
                cout << "druglist is" << drugList[i] << endl;

            sortByDrug(persons, drugList, group);
            vector<person>().swap(persons);

            while (topPeople.size() < group.size())
                topPeople.push_back(filler);

            for (int j = 0; j < group.size(); j++)
            {
                std::sort(group[j].begin(), group[j].end(), alphabetacle);
                if (topPeople[j][0][0] == "eempty")
                    topPeople[j][0] = group[j][0];
                else
                    topPeople[j].push_back(group[j][0]);
            }

            printToFiles(drugList, group, drugFileNames, numLines);
        }

        vector<drugGroup> tempTopPeople;
        string locStrings[2];
        tempTopPeople = topPeople;

        for (int i = 0; i < tempTopPeople.size(); i++)
        {
            for (int j = 0; j < tempTopPeople[i].size(); j++)
            {
                locStrings[0] = "";
                locStrings[1] = "";
                locStrings[0] = SSTR(locStrings[0] << i);
                locStrings[1] = SSTR(locStrings[1] << j);
                tempTopPeople[i][j].push_back(locStrings[0]);
                tempTopPeople[i][j].push_back(locStrings[1]);
            }
            std::sort(tempTopPeople[i].begin(), tempTopPeople[i].end(), alphabetacle);         

            cout << endl;
        }
        for (int i = 0; i < numLines.size(); i++)
        {
            for (int j = 0; j < numLines[i].size(); j++)
            {
                cout << numLines[i][j] << ",";
            }
            cout << endl;
        }

        dumpToMain(drugList, topPeople, tempTopPeople, drugFileNames, numLines, mainFileNames);

        infile.close();
        addEmUp(drugList, numLines, mainFileNames, finalGroup);
        ofstream outProduct;
        outProduct.open("output.txt");
        cout << "finalGroupsize is " << finalGroup.size() << endl;
        cout << "finalGroup[0][0][0] is " << finalGroup[0][0][0] << endl;
        vector<drugGroup> newFinalGroup;
        vector<person> finalPersons;
        for (int i = 0; i < finalGroup.size(); i++)
        {
            finalPersons.push_back(finalGroup[i][0]);
        }
        std::sort(finalPersons.begin(), finalPersons.end(), costAlphasort);

        for (int i = 0; i < finalPersons.size(); i++)
        {
            for (int k = 0; k < finalPersons[i].size() - 1; k++)
            {
                cout << "finalPersons now is " << finalPersons[i][k] << endl;
                outProduct << finalPersons[i][k] << ',';
            }
            outProduct << finalPersons[i][finalPersons[i].size() - 1] << endl;
        }

        outProduct.close();
    }
    else
    {
        cout << "Error opening file";
    }
    return 0;
}

void getperson(ifstream &infile, string (&persons)[5])
{
    char persLine[256];
    infile.getline(persLine, 256);
    string pers(persLine);
    std::stringstream split(pers);
    int i = 0;
    while (getline(split, pers, ','))
    {
        persons[i] = pers;
        i++;
    }
}
void dumpToMain(vector<string> &drugList, vector<drugGroup> &topPeople, vector<drugGroup> &tempTopPeople, std::vector<vector<string>> &drugFileNames, vector<vector<int>> &numLines, vector<string> &mainFileNames)
{
    std::vector<vector<int>> lineNum;

    for (int i = 0; i < drugFileNames.size(); i++)
    {
        vector<int> v;
        for (int j = 0; j < (drugFileNames.size()); j++)
            v.push_back(0);
        lineNum.push_back(v);
    }

    string filename;
    string mainFileName;
    string pers[5];
    vector<person> persons;
    ofstream ouMain;
    ifstream infile;
    ofstream oufile;
    vector<string> fill;
    fill.push_back("eempty");
    vector<person> filler;
    filler.push_back(fill);
    for (int i = 0; i < tempTopPeople.size(); i++)
    {
        cout << "i is " << i << endl;
        bool done = false;
        bool inGood = true;
        mainFileName = tempTopPeople[i][0][3];
        dotText(mainFileName);
        mainFileNames.push_back(mainFileName);
        ouMain.open(mainFileName.c_str());

        vector<int> peopleDone(tempTopPeople[i].size());
        while (tempTopPeople[i][0][0] != "eempty")
        {
            for (int k = 0; k < 4; k++)
                ouMain << tempTopPeople[i][0][k] << ',';
            ouMain << tempTopPeople[i][0][4] << endl;
            int x = 0;
            int y = 0;
            stringstream ss(tempTopPeople[i][0][5]);
            ss >> x;
            stringstream sz(tempTopPeople[i][0][6]);
            sz >> y;
            peopleDone[y] += 1;

            filename = drugFileNames[i][y];
            infile.open(filename.c_str());

            if (peopleDone[y] < numLines[x][y])
            {
                for (int j = 0; j < peopleDone[y]; j++)
                    getperson(infile, pers);

                getperson(infile, pers);

                std::vector<string> tempVec(pers, pers + sizeof pers / sizeof pers[0]);
                tempVec.push_back(tempTopPeople[i][0][5]);
                tempVec.push_back(tempTopPeople[i][0][6]);
                tempTopPeople[i][0].swap(tempVec);
            }
            else
                tempTopPeople[i][0] = fill;
            infile.close();
            std::sort(tempTopPeople[i].begin(), tempTopPeople[i].end(), alphabetacle);
        }
        ouMain << fill[0].c_str();
        ouMain.close();
    }
}

bool costAlphasort(vector<string> &i, vector<string> &j)
{
    if (i[2] != j[2])
    {
        int x = 0;
        int y = 0;
        stringstream ss(i[2]);
        ss >> x;
        stringstream sz(j[2]);
        sz >> y;
        return (x > y);
    }
    else
    {
        int isize = i[0].size();
        int jsize = j[0].size();
        int diff = isize - jsize;
        int n = 0;
        if (diff > 0)
            isize = jsize;
        while (i[0][n] == j[0][n] && n < isize)
            n++;
        if (n < isize)
            return (i[0][n] < j[0][n]);
        else if (diff > 0)
            return (false);
        else
            return (true);
    }
}

bool alphabetacle(vector<string> &i, vector<string> &j)
{
    if (i[0] == "eempty")
        return (false);
    else if (j[0] == "eempty")
        return (true);
    else
    {
        int isize = i[1].size();
        int jsize = j[1].size();
        int diff = isize - jsize;
        int n = 0;
        if (diff > 0)
            isize = jsize;
        while (i[1][n] == j[1][n] && n < isize)
            n++;
        if (n < isize)
            return (i[1][n] < j[1][n]);
        else if (diff > 0)
            return (false);
        else
            return (true);
    }
}

void printPers(vector<person> persons)
{
    for (int j = 0; j < persons.size(); j++)
    {
        for (int k = 0; k < persons[j].size(); k++)
        {
            cout << persons[j][k] << ',';
        }
        cout << endl;
    }
}
void printGroup(vector<drugGroup> group, int pmax)
{
    bool pzero;
    int psize;
    if (pmax == 0)
        pzero = true;

    for (int j = 0; j < group.size(); j++)
    {
        cout << "group " << j << endl;
        for (int k = 0; k < group[j].size(); k++)
        {
            if (pzero)
                psize = group[j][k].size();
            else if (pmax < group[j][k].size())
                psize = pmax;
            else
                psize = group[j][k].size();

            for (int p = 0; p < group[j][k].size(); p++)
            {
                cout << group[j][k][p] << ',';
            }
            cout << endl;
        }
    }
}

void sortByDrug(vector<person> &persons, vector<string> &drugList, vector<drugGroup> &group)
{
    int dlSize = drugList.size();

    vector<string> fill;
    fill.push_back("eempty");
    vector<person> filler;
    filler.push_back(fill);
    vector<drugGroup> tempGroup;

    int psize = persons.size();
    vector<string> tempDrugList;

    if (group.size() == 0)
    {
        for (int i = 0; i < psize; i++)
        {
            bool onList = false;
            for (int j = 0; j < tempDrugList.size(); j++)
            {

                if (persons[i][3] == tempDrugList[j])
                {
                    tempGroup[j].push_back(persons[i]);
                    onList = true;
                    j = tempDrugList.size();
                }
            }
            if (!onList)
            {
                vector<person> tempPerson;
                tempPerson.push_back(persons[i]);
                tempGroup.push_back(tempPerson);
                tempDrugList.push_back(persons[i][3]);
            }
        }
        int tdlSize = tempDrugList.size();

        if (dlSize == 0)
        {
            drugList = tempDrugList;
            group = tempGroup;
        }
        else
        {
            while (group.size() < dlSize)
            {
                group.push_back(filler);
            }
            for (int j = 0; j < tdlSize; j++)
            {
                int iprime;
                bool onLists = false;
                for (int i = 0; i < dlSize; i++)
                {
                    if (drugList[i] == tempDrugList[j])
                    {
                        onLists = true;
                        iprime = i;
                    }
                }
                if (onLists)
                {
                    group[iprime] = tempGroup[j];
                }
                if (!onLists)
                {
                    drugList.push_back(tempDrugList[j]);
                    group.push_back(tempGroup[j]);
                    dlSize += 1;
                }
            }
        }
    }
}

void printToFiles(vector<string> &drugList, vector<drugGroup> &group, std::vector<vector<string>> &drugFileNames, vector<vector<int>> &numLines)
{
    int dsize = drugList.size();
    int psize = 5;
    int dfnsize;
    bool izero = true;
    string endstr = ".txt";
    dfnsize = 0;
    for (int i = 0; i < dsize; i++)
    {
        if (group[i][0][0] != "eempty")
        {
            string filename = drugList[i];
            if (drugFileNames.size() == i)
            {
                dfnsize = 0;
                filename = SSTR(filename << dfnsize);
                for (int j = 0; j < 4; j++)
                    filename += endstr[j];

                std::string filenameArr[1];
                filenameArr[0] = filename;
                std::vector<string> filenameVec(filenameArr, filenameArr + sizeof filenameArr / sizeof filenameArr[0]);
                drugFileNames.push_back(filenameVec);
            }
            else
            {
                dfnsize = drugFileNames[i].size();
                filename = SSTR(filename << dfnsize);
                for (int j = 0; j < 4; j++)
                    filename += endstr[j];
                ;
                drugFileNames[i].push_back(filename);
            }

            ofstream oufile;
            oufile.open(filename.c_str());
            int numLinesCount = 0;
            for (int j = 0; j < group[i].size(); j++)
            {
                for (int k = 0; k < psize - 1; k++)
                {
                    oufile << group[i][j][k] << ',';
                }
                oufile << group[i][j][psize - 1] << endl;
                numLinesCount++;
            }
            cout << "i is " << i << endl;
            cout << "numlines.size is " << numLines.size() << endl;
            cout << "numlinesCount is " << numLinesCount << endl;

            if (numLines.size() > i)
            {
                vector<int> v;
                v.push_back(numLinesCount);
                cout << "numlines.size is " << numLines.size() << endl;
                numLines[i].push_back(v[0]);
            }
            else
            {
                vector<int> v;
                v.push_back(numLinesCount);
                numLines.push_back(v);
            }

            oufile.close();
        }
        else
        {
            drugFileNames[i].push_back("eempty");
            if (numLines.size() == i)
            {
                vector<int> v;
                v.push_back(0);
                numLines.push_back(v);
            }
            else
            {
                numLines[i].push_back(0);
            }
        }
    }
    vector<drugGroup>().swap(group);
}

void addEmUp(vector<string> &drugList, vector<vector<int>> &numLines, vector<string> &mainFileNames, vector<drugGroup> &finalGroup)
{
    string filename;
    string mainFileName;
    int pmax = 500;
    int j = 0;
    string pers[5];
    vector<person> persons;
    ifstream inMain;

    for (int i = 0; i < mainFileNames.size(); i++)
    {

        int costCount = 0;
        int peopleCount = 0;
        bool isDupe;

        vector<string> drugFill;
        drugFill.push_back(drugList[i]);
        vector<person> drugName;

        mainFileName = mainFileNames[i];
        inMain.open(mainFileName.c_str());
        getperson(inMain, pers);
        while (pers[0] != "eempty")
        {
            isDupe = false;
            peopleCount += 1;
            std::vector<string> tempVec(pers, pers + sizeof pers / sizeof pers[0]);
            persons.push_back(tempVec);
            int x = 0;
            stringstream ss(pers[4]);
            ss >> x;
            costCount += x;
            int p = persons.size() - 1;
            if (p > pmax)
            {
                persons.erase(persons.begin());
                p = p - 1;
            }
            for (int j = 0; j < p; j++)
            {
                if (persons[p][1] == persons[j][1])
                {
                    if (persons[p][2] == persons[j][2])
                    {
                        isDupe = true;
                    }
                }
            }
            if (isDupe)
                peopleCount = peopleCount - 1;
            getperson(inMain, pers);
        }
        inMain.close();
        int totalPrescribers = peopleCount;
        string totalPrescStr = "";
        totalPrescStr = SSTR(totalPrescStr << totalPrescribers);
        string totalCostStr = SSTR(totalCostStr << costCount);
        drugFill.push_back(totalPrescStr);
        drugFill.push_back(totalCostStr);
        drugName.push_back(drugFill);
        finalGroup.push_back(drugName);
    }
}
void dotText(string &filename)
{
    string endstr = "MAIN.txt";
    for (int j = 0; j < 8; j++)
    {
        filename += endstr[j];
    }
}
