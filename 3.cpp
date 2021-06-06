#include<iostream>
#include<vector>
#include<fstream>
#include<string>
#include<algorithm>
using namespace std;


struct sessionTime{
    double begin;
    double end;
    string day;
};


typedef vector < sessionTime > SessionsTime;


struct course{
    int id;
    string name;
    int units;
    SessionsTime schedule;
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


SessionsTime scheduleInput(string input){
    SessionsTime schedule;
    int i = 0;
    while(i < input.size() && input[i] != ','){
        sessionTime currentItem;
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
        currentItem.grade = -1;
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
        if(findCourse(prerequisities[i], courses).grade < 10)
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


bool comparator(course a, course b){
    if(a.units != b.units)
        return a.units > b.units;
    return a.name < b.name;
}


int findMaxUnitsAllowed(double gpa){
    if(gpa >= 17)
        return 24;
    return 20;
}


double GPA(Courses courses, int termCounter, double &gpa){
    if(termCounter > 1){
        gpa = gpa*1.05;
        return gpa;
    }
    gpa = 0;
    double units = 0;
    for(auto course : courses)
        if(course.grade != -1)
            gpa += course.grade*(double)course.units, units += course.units;
    gpa /= units;
    return gpa;
}


bool overlap(SessionsTime course1, SessionsTime course2){
    for(auto session1 : course1)
        for(auto session2 : course2)
            if(session1.day == session2.day){
                sessionTime a = session1;
                sessionTime b = session2;
                if(session1.begin > session2.begin)
                    a = session2, b = session1;
                if(a.end > b.begin || (a.begin == b.begin && a.end == b.end))
                       return 1;
            }
    return 0;
}


bool isFreeTime(course toCheck, Courses plan){
    for(auto course : plan)
        if(overlap(toCheck.schedule, course.schedule))
            return 0;
    return 1;
}


Courses plan(Courses courses, Courses takableCourses, int termCounter, double &gpa){
    int maxUnitsAllowed = findMaxUnitsAllowed(GPA(courses, termCounter, gpa));
    int unitsCounter = 0;
    Courses nextTermPlan;
    for(auto course : takableCourses){
        if(course.units + unitsCounter <= maxUnitsAllowed && isFreeTime(course, nextTermPlan))
            nextTermPlan.push_back(course), unitsCounter += course.units;
    }
    return nextTermPlan;
}


bool outputSortComparator(course a, course b){
    return a.name < b.name;
}


void updatePassedCourse(course toUpdate, Courses &courses){
    for(int i = 0; i < courses.size(); i++)
        if(courses[i].id == toUpdate.id){
            courses[i].grade = 20;
            courses[i].passed = 1;
            return;
        }
}


string intToString(int number){
    string str = "";
    while(number){
        str = (char)(number%10+'0') + str;
        number /= 10;
    }
    return str;
}


void output(Courses termCourses, Courses &mainCoursesList, int termCounter){
    sort(termCourses.begin(), termCourses.end(), outputSortComparator);
    ofstream out;
    string outputFileString = "semester" + intToString(termCounter) + ".sched";
    out.open(outputFileString);
    for(auto course : termCourses){
        updatePassedCourse(course, mainCoursesList);
        out << course.id << endl;
    }
    out.close();
}


bool passedAllCourses(Courses courses){
    int cnt = 0;
    for(auto course : courses)
        if(!course.passed){
            cnt++;
        }
    return cnt == 0;
}


int main(int argc, char *argv[]){
    Courses courses;
    coursesInput(argv[1], courses);
    gradesInput(argv[2], courses);
    int termCounter = 1;
    double gpa;
    while(!passedAllCourses(courses)){
        Courses takableCourses = findTakableCourses(courses);
        sort(takableCourses.begin(), takableCourses.end(), comparator);
        Courses termPlan = plan(courses, takableCourses, termCounter, gpa);
        output(termPlan, courses, termCounter);
        termCounter++;
    }
}
