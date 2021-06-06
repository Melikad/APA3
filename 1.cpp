#include<iostream>
#include<vector>
#include<fstream>
#include<string>
#include<algorithm>
using namespace std;


struct session_time{
    double begin;
    double end;
    string day;
};


struct course{
    int id;
    string name;
    int units;
    vector < session_time > schedule;
    vector < int > prerequisities;
    double grade;
    bool passed;
};


typedef vector < course > Courses;


double timeToInt(string input){
    if(input[1] == ':')
        input = '0' + input;
    return (input[0]-'0')*10 + input[1] - '0' + (double)((input[3] - '0')*10 + (input[4]-'0'))/60;
}


vector < session_time > scheduleInput(string input){
    vector < session_time > schedule;
    int i = 0;
    while(i < input.size() && input[i] != ','){
        session_time currentItem;
        currentItem.day = input.substr(i, 3);
        string beginTime = "";
        i += 4;
        for(i; input[i] != '-'; i++)
            beginTime += input[i];
        currentItem.begin = timeToInt(beginTime);
        string endTime = "";
        i++;
        for(i; i < input.size() && input[i] != '/'; i++)
            endTime += input[i];
        currentItem.end = timeToInt(endTime);
        schedule.push_back(currentItem);
        i++;
    }
    return schedule;
}


int stringToInt(string s){
    int intValue = 0;
    for(int i = 0; i < s.size(); i++){
        intValue *= 10;
        intValue += (s[i]-'0');
    }
    return intValue;
}


vector < int > prerequisitiesInput(string input){
    vector < int > presVec;
    if(input == "0")
        return presVec;
    int i = 0;
    while(i < input.size()){
        int id = 0;
        while(i < input.size() && input[i] != '-'){
            id *= 10, id += input[i]-'0';
            i++;
        }
        presVec.push_back(id);
        i++;
    }
    return presVec;
}


void coursesInput(string fileAddress, Courses &courses){
    ifstream coursesInputFile;
    coursesInputFile.open(fileAddress);
    string input;
    getline(coursesInputFile, input);
    while (!coursesInputFile.eof()) {
        course currentItem;
        getline(coursesInputFile, input, ',');
        currentItem.id = stringToInt(input);
        getline(coursesInputFile, input, ',');
        currentItem.name = input;
        getline(coursesInputFile, input, ',');
        currentItem.units  = stringToInt(input);
        getline(coursesInputFile, input, ',');
        currentItem.schedule = scheduleInput(input);
        getline(coursesInputFile, input);
        currentItem.prerequisities = prerequisitiesInput(input);
        currentItem.passed = 0;
        if(currentItem.name != "")
            courses.push_back(currentItem);
    }
    //courses.pop_back();
}


double stringToDouble(string input){
    int i = 0;
    double doubleValue = 0;
    while(i < input.size() && input[i] != '.'){
        doubleValue *= 10;
        doubleValue += input[i]-'0';
        i++;
    }
    if(i == input.size())
        return doubleValue;
    double decimal = 0;
    i = input.size()-1;
    while(input[i] != '.'){
        decimal += input[i]-'0';
        decimal /= 10;
        i--;
    }
    doubleValue += decimal;
    return doubleValue;
}


void addGrade(Courses &courses, int id, double grade){
    for(int i = 0; i < courses.size(); i++)
        if(courses[i].id == id){
            courses[i].grade = grade;
            if(grade >= 10)
                courses[i].passed = 1;
            return;
        }
    cout << "id not found " << id << endl;
}


void gradesInput(string fileAddress, Courses &courses){
    ifstream gradesInputFile;
    gradesInputFile.open(fileAddress);
    string input;
    getline(gradesInputFile, input);
    while(!gradesInputFile.eof()){
        getline(gradesInputFile, input, ',');
        int id = stringToInt(input);
        getline(gradesInputFile, input);
        double grade = stringToDouble(input);
        if(id != 0)
            addGrade(courses, id, grade);
    }
}


course findCourse(int id, Courses courses){
    for(int i = 0; i < courses.size(); i++)
        if(courses[i].id == id)
            return courses[i];
}


bool passed(vector < int > prerequisities, Courses courses){
    for(int i = 0; i < prerequisities.size(); i++)
        if(findCourse(prerequisities[i], courses).passed == 0)
            return 0;
    return 1;
}


Courses findTakableCourses(Courses courses){
    Courses takableCourses;
    for(int i = 0; i < courses.size(); i++){
        if(!courses[i].passed && passed(courses[i].prerequisities, courses))
            takableCourses.push_back(courses[i]);
    }
    return takableCourses;
}


void output(Courses takableCourses){
    for(int i = 0; i < takableCourses.size(); i++)
        cout << takableCourses[i].id << endl;
}


bool myComparator(course a, course b){
    return a.name < b.name;
}


int main(int argc, char *argv[]){
    vector < course > courses;
    coursesInput(argv[1], courses);
    gradesInput(argv[2], courses);
    Courses takableCourses = findTakableCourses(courses);
    sort(takableCourses.begin(), takableCourses.end(), myComparator);
    output(takableCourses);
}
