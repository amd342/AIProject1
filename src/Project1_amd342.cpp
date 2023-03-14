#include <stdlib.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

void identify_poison(vector<vector<string>>&);
void processBC(string, vector<vector<string>>&, vector<vector<vector<string>>>, vector<vector<vector<string>>>, vector<string>, vector<vector<string>>&, vector<string>);
void search_con(string, vector<vector<string>>&, vector<vector<vector<string>>>, vector<vector<vector<string>>>, vector<string>, vector<vector<string>>&, vector<string>);
void rule_to_clause(int, string, vector<vector<string>>&, vector<vector<vector<string>>>, vector<vector<vector<string>>>, vector<string>, vector<vector<string>>&, vector<string>);
void update_VLBC(int, int, string, vector<vector<string>>&, vector<vector<vector<string>>>, vector<vector<vector<string>>>, vector<string>, vector<vector<string>>&, vector<string>);
bool validate_Ri(int, int, int, string, string, string, vector<vector<vector<string>>>, vector<vector<vector<string>>>, vector<vector<string>>, vector<vector<string>>&);
string symptomQuestions(string);
void treatment(vector<vector<string>>&);
void processFC(string, vector<vector<string>>&, vector<vector<vector<string>>>, vector<vector<vector<string>>>, vector<vector<string>>&, vector<string>);
int search_cvl(int, string,  vector<vector<string>>&, vector<vector<vector<string>>>, vector<vector<vector<string>>>, vector<vector<string>>&, vector<string>);
bool update_VLFC(int, int, string, vector<vector<string>>&, vector<vector<vector<string>>>, vector<vector<vector<string>>>, vector<vector<string>>&, vector<string>);
int clause_to_rule(int, string,  vector<vector<string>>&, vector<vector<vector<string>>>, vector<vector<vector<string>>>, vector<vector<string>>&, vector<string>);

//DIFFERENT COMMENT

//create goalList containing intializations state and values of the goals, calls the backward and forward chaining functions, then prints the final results.
int main(){

    vector<vector<string>> goalList;    //holds the values names, initialization state, and values of the goal/conclusion variables.
    vector<string> tokens;              //the following variables are needed to read .txt data into vectors
    stringstream ss;
    string token;
    string str;
    ifstream inFile;
    string input = "r";                //user input, accepts values: "r" = restart program and "q" = quit program

    do{    

    goalList.clear();                  //clears names, initialization states, and values of the goal/conclusion variables for clean restart
    
    inFile.open("data/goalVariList.txt");     //reads .txt data into vector        
    if(!inFile){
        cout<<endl
            <<"*PROGRAM TERMINATED*"<<endl
            <<"inFile (goalList.txt) failed to open"<< endl;
            return 1;
    }
    else{
        while(getline(inFile, str)){
            ss.clear();
            ss.str(str);

            while(getline(ss, token, ' ')){
                tokens.push_back(token);
            }
            goalList.push_back(tokens);
            tokens.clear();
        }                
    }
    inFile.close();
    
    string input;                       

    identify_poison(goalList);          //Start backward chaining to find diagnosis
    treatment(goalList);                //Start forward chaining to find treatment
    
    system("cls");                      //clear questions asked in identify_poison() from screen

    string path = "data/treatmentPlans/" + goalList[1][2];        //Prints the result of treatment()
    inFile.open(path);
    if(!inFile){
        cout<<endl
            <<"*PROGRAM TERMINATED*"<<endl
            <<"inFile (treatment.txt) failed to open"<<endl;
            return 1;
    }
    else{
        while(getline(inFile,str)){
            cout << str << endl;
        }
    }
    inFile.close();

    while((input != "r") && (input != "q")){        //validates user input to restart or quit the program
        cout << "Enter 'r' to restart the program or 'q' to exit." << endl;
        cin >> input;
    }

    system("cls");              //clear the result of treatment() from the screen

    if(input == "q"){           //exit program if input = "q"
        return 0;
    }

    }while(input == "r");       //restart program if input = "r"
}

                                                    //BACKWARD CHAINING METHODS
//Initializes all required data structures (using info from related files) to find a diagnosis, calls processBC() to begin the search.
void identify_poison(vector<vector<string>>& goalList){
    
    vector<string> tokens;                              //the following variables are needed to read .txt data into vectors
    vector<vector<string>> statement;
    stringstream ss;
    string str;
    string token;
    ifstream inFile;

    vector<vector<vector<string>>> ifStates;            //Create ifStates containing symptoms required values for diagnosis to be made.
    inFile.open("data/backwardChaining/ifStatements_BC.txt");
    if(!inFile){
        cout<<endl
            <<"*PROGRAM TERMINATED*"<<endl
            <<"inFile (ifStatements_BC.txt) failed to open"<< endl;
            return;
    }
    else{
        while(getline(inFile, str)){
            while(!str.empty()){
                for(int i = 0; i < 2; i++){
                    tokens.push_back(str);
                    getline(inFile, str);
                }
                statement.push_back(tokens);
                tokens.clear();
            }    
            ifStates.push_back(statement);
            statement.clear();                
        }
    }
    inFile.close();

    vector<vector<vector<string>>> thenStates;          //Create thenStates containing conclusions made if if-statements are met.
    inFile.open("data/backwardChaining/thenStatements_BC.txt");
    if(!inFile){
        cout<<endl
            <<"*PROGRAM TERMINATED*"<<endl
            <<"inFile (thenStatements_BC.txt) failed to open"<< endl;
            return;
    }
    else{
        while(getline(inFile, str)){
            ss.clear();
            ss.str(str);
            if(!str.empty()){
                while(getline(ss, token, ' ')){
                    tokens.push_back(token);
                }
                statement.push_back(tokens);
                tokens.clear();
            }    
            else{
                thenStates.push_back(statement);
                statement.clear();                
            }    
        }
    }
    thenStates.push_back(statement);
    statement.clear();
    inFile.close();
    
    vector<string> conList;                             //Create conList containg the conclusions of each then-statement in each rule.
    inFile.open("data/backwardChaining/conclusionList_BC.txt");
    if(!inFile){
        cout<<endl
            <<"*PROGRAM TERMINATED*"<<endl
            <<"inFile (conclusionList.txt) failed to open"<< endl;
            return;
    }
    else{
        while(getline(inFile, str)){
            conList.push_back(str);
        }
    }
    inFile.close();

    vector<vector<string>> variList;                    //Creates variList contiaing the initialization states and values of the variables.
    inFile.open("data/backwardChaining/variableList_BC.txt");
    if(!inFile){
        cout << endl
            <<"*PROGRAM TERMINATED*"<<endl
            <<"inFile (variableList_BC.txt) failed to open"<< endl;
            return;
    }
    else{
        while(getline(inFile, str)){
            tokens.push_back(str);
            getline(inFile, str);
            ss.clear();
            ss.str(str);
            while(getline(ss, token, ' ')){
                tokens.push_back(token);
            }
            variList.push_back(tokens);
            tokens.clear();
        }   
    }
    inFile.close();
    
    vector<string> clauseList;                          //Creates clauseList containg the variables of each if-statements in each rule.
    inFile.open("data/backwardChaining/clauseList_BC.txt");
    if(!inFile){
        cout<<endl
            <<"*PROGRAM TERMINATED*"<<endl
            <<"inFile (clauseList_BC.txt) failed to open"<< endl;
            return;
    }
    else{
        while(getline(inFile, str)){
            clauseList.push_back(str);
        }
    }
    inFile.close();

    processBC("diagnosis", goalList, ifStates, thenStates, conList, variList, clauseList);      //Call process to begin searching for diagnosis.
}

//Begins backward chaining process 
void processBC(string goal, vector<vector<string>>& goalList, vector<vector<vector<string>>> ifStates, vector<vector<vector<string>>> thenStates, vector<string> conList, vector<vector<string>>& variList, vector<string> clauseList){
    search_con(goal, goalList, ifStates, thenStates, conList, variList, clauseList);
}

//Searches for the first/next rule number with the desired goal in the conclusion.
void search_con(string goal, vector<vector<string>>& goalList, vector<vector<vector<string>>> ifStates, vector<vector<vector<string>>> thenStates, vector<string> conList, vector<vector<string>>& variList, vector<string> clauseList){
    int i = 0;
    int Ri = 0;
    while(goalList[i][0] != goal){                      //find the goal in goalList
        i++;
    }

    while(goalList[i][1] == "n" && Ri < conList.size()){    //while goal is not initialized and not all rules have been checked
        if(conList[Ri] == goal){                            //if goal is the conclusion of rule Ri
            rule_to_clause(Ri, goal, goalList, ifStates, thenStates, conList, variList, clauseList);
        }
        Ri++;
    }

    if(Ri > conList.size()){
        cout<<endl
            <<"*PROGRAM TERMINATED*"<<endl
            << "End of conclusion list reached without initializing" << goalList[i][0] << endl; 
        return;
    }
}

//Finds the intiail clause number for the rule number
void rule_to_clause(int Ri, string goal, vector<vector<string>>& goalList, vector<vector<vector<string>>> ifStates, vector<vector<vector<string>>> thenStates, vector<string> conList, vector<vector<string>>& variList, vector<string> clauseList){
    int Ci = 6*Ri;                  //Calculate the first clause of rule Ri
    //the given formula was not used because the Ri and Ci are given by the index in the vectors. 
    //As a result for Ri = 0, Ci = 6*((Ri)-1) +1 would result in Ci = -5
    //to adjust for this the formula would have to be altered to Ci-1 = ((Ri+1)-1)+1 to get the
    //the correct clause number, which is equal to Ci = 6*Ri
    update_VLBC(Ci, Ri, goal, goalList, ifStates, thenStates, conList, variList, clauseList);
}

//Updates the variables in the variable list that are relevant to Rule Ri.
void update_VLBC(int Ci, int Ri, string goal, vector<vector<string>>& goalList, vector<vector<vector<string>>> ifStates, vector<vector<vector<string>>> thenStates, vector<string> conList, vector<vector<string>>& variList, vector<string> clauseList){
    string vari;                        //variable name of clause Ci
    int j = 0;                                  
    bool found = false;                 //indicates if vari is found in variList or goalList
    bool match = true;                  //indicates if the value of vari is matching with what is required of rule Ri
    int CiEnd = Ci + 6;                 //the end of the clause list for rule Ri
    for(Ci; Ci < CiEnd; Ci++){          //while the end of the clause list has not been reached
        vari = clauseList[Ci];          //read the first/next variable into vari
        found = false;                  //reinitialize found to false for 'next' variable
        if( (vari == " ") || (match == false) ){   //if there are no more variables or the values of the variables do not match with rule Ri
            Ci = CiEnd;                            //exit the loop
        }
        else{
            for(int i = 0; i < variList.size(); i++){             //iterate through variList    
                if(vari == variList[i][0]){                       //if vari is found in variList
                    found = true;                                 //set found to true
                    if(variList[i][1] == "y"){                    //if vari is initialized
                        match = validate_Ri(Ri, i, Ci, "i", "v", vari, ifStates, thenStates, variList, goalList); //check if the value of vari matches with rule Ri 
                        i = variList.size();                      //exit the loop
                    }
                    else{                                         //if vari is not initialized
                        string value = symptomQuestions(vari);    //ask user for value of vari  
                        variList[i][1] = "y";                     //set vari to initialized  
                        variList[i][2] = value;                   //set value of vari
                        match = validate_Ri(Ri, i, Ci, "i", "v", vari, ifStates, thenStates, variList, goalList); //check if the value of vari matches with rule Ri
                        i = variList.size();                      //exit the loop
                    }
                }
            }
    
            if(found == false){                                   //if vari is not found in variList
                for(int i = 0; i < goalList.size(); i++){         //iterate through goalList
                    if(vari == goalList[i][0]){                   //if vari is found in goalList
                        found == true;                            //set found to true
                        if(goalList[i][1] == "y"){                //if vari is initialized
                            match = validate_Ri(Ri, i, Ci, "i", "g", vari, ifStates, thenStates, variList, goalList); //check if the value of vari matches with rule Ri 
                            i = goalList.size();                  //exit the loop
                        }
                        else{                                     //if vari is not initialized
                            processBC(vari, goalList, ifStates, thenStates, conList, variList, clauseList); //restart process with vari as the goal
                            match = validate_Ri(Ri, i, Ci, "i", "g", vari, ifStates, thenStates, variList, goalList); //check if the value of vari matches with rule Ri
                            i = goalList.size();                  //exit the loop
                        }
                    }
                }
            }
        }
    }
    int i = 0;
    while (goal != goalList[i][0]){                             //find the index of the goal in goalList
        i++;
    }
    if(match == true){                                          //if the vari values match with the rule Ri
        match = validate_Ri(Ri, i, j, "t", "g", goal, ifStates, thenStates, variList, goalList); //set the value of the goal
        return;
    }
    else{                                                       //if the vari values did not match with the rule Ri
        return;                                       
    }
}

//Compares the values of the variables in the variable list to the required values in the if-statements forn the then-statement to be true, then updates the goalList with the correct value.
//NOTE: Works for both backward and forward chaining!
bool validate_Ri(int Ri, int i, int j, string ifthen, string goalvari, string vari, vector<vector<vector<string>>> ifStates, vector<vector<vector<string>>> thenStates, vector<vector<string>> variList, vector<vector<string>>& goalList){
    if(ifthen == "i"){                                      //if an if statement is being validated
        j = 0;
        while(vari != ifStates[Ri][j][0]){                  //find the index of the if statement corresponding to vari
            j++;
        }
        if(goalvari == "v"){                                //if vari is in the variList
            if(variList[i][2] == ifStates[Ri][j][1]){       //if the value of vari and the if statement match return true
                return true;                            
            }
            else{                                           //if the value of vari and the if statement do not match return false
                return false;                           
            }
        }
        if(goalvari == "g"){                                //if vari is in the goalList
            if(goalList[i][2] == ifStates[Ri][j][1]){       //if the value of vari and the if statement match return true
                return true;                        
            }
            else{                                           //if the value of vari and the if statement do not match return false
                return false;                           
            }
        }
    }
    if(ifthen == "t"){                                      //if a then statement validated
        goalList[i][2] = thenStates[Ri][0][1];              //set the value of vari to the conclusion value of rule Ri
        goalList[i][1] = "y";                               //set vari to initalized
        return true;    
    }
}

//Asks the user to provide a value for a variable
//NOTE: Works for both backward and forward chaining!
string symptomQuestions(string symptom){
    string input;
    cout << "Is the patient experiencing ";
    if(symptom == "indicQuestion1"){
        cout << "urinary retention, paralysis, red/swollen skin, " << endl 
             << "warts/lesions, swollen digits, or burning eyes, nose, or mouth?"<< endl;
    }
    else if(symptom == "indicQuestion2"){
        cout << "convulsions, fever, irritability, or dehydration?" << endl;
    }
    else if(symptom == "indicQuestion3"){
        cout << "confusion, seizures, headaches, fatigue or arrhythmia?" << endl;
    }
    else{
        cout << symptom << "?" << endl;
    }

    do{
    cout << "Enter 'y' for yes or 'n' for no." << endl;
    
    cin >> input;

    }while(input != "y" && input != "n");

    return input;
}


                                                    //FORWARD CHAINING METHODS
//Initializes all required data structures (using info from related files) to find a diagnosis, calls processFC() to begin the search.
void treatment(vector<vector<string>>& goalList){

    vector<string> tokens;                                      //the following variables are needed to read .txt data into vectors
    vector<vector<string>> statement;
    stringstream ss;
    string str;
    string token;
    ifstream inFile;

    vector<vector<vector<string>>> ifStates;                    //Create ifStates containing symptoms required values for diagnosis to be made.
    inFile.open("data/forwardChaining/ifStatements_FC.txt");
    if(!inFile){
        cout<<endl
            <<"*PROGRAM TERMINATED*"<<endl
            <<"inFile (ifStatements.txt) failed to open"<< endl;
            return;
    }
    else{
        while(getline(inFile, str)){
             ss.clear();
            ss.str(str);
            if(!str.empty()){
                while(getline(ss, token, ' ')){
                    tokens.push_back(token);
                }
                statement.push_back(tokens);
                tokens.clear();
            }    
            else{
                ifStates.push_back(statement);
                statement.clear();                
            }    
        }            
    }
    ifStates.push_back(statement);
    statement.clear();
    inFile.close();

    vector<vector<vector<string>>> thenStates;                  //Create thenStates containing conclusions made if if-statements are met.
    inFile.open("data/forwardChaining/thenStatements_FC.txt");
    if(!inFile){
        cout<<endl
            <<"*PROGRAM TERMINATED*"<<endl
            <<"inFile (thenStatements_FC.txt) failed to open"<< endl;
            return;
    }
    else{
        while(getline(inFile, str)){
             ss.clear();
            ss.str(str);
            if(!str.empty()){
                while(getline(ss, token, ' ')){
                    tokens.push_back(token);
                }
                statement.push_back(tokens);
                tokens.clear();
            }    
            else{
                thenStates.push_back(statement);
                statement.clear();                
            }    
        }
    }
    thenStates.push_back(statement);
    statement.clear();
    inFile.close();

    vector<string> clauseList;                                  //Create clauseList containg the variables of each if-statements in each rule.
    inFile.open("data/forwardChaining/clauseList_FC.txt");
    if(!inFile){
        cout<<endl
            <<"*PROGRAM TERMINATED*"<<endl
            <<"inFile (clauseList_FC.txt) failed to open"<< endl;
            return;
    }
    else{
        while(getline(inFile, str)){
            clauseList.push_back(str);
        }
    }
    inFile.close();

    vector<vector<string>> variList;                            //Creates variList contiaing the initialization states and values of the variables.
    inFile.open("data/forwardChaining/variableList_FC.txt");
    if(!inFile){
        cout<<endl
            <<"*PROGRAM TERMINATED*"<<endl
            <<"inFile (variableList_FC.txt) failed to open"<< endl;
            return;
    }
    else{
        while(getline(inFile, str)){
            ss.clear();
            ss.str(str);

            while(getline(ss, token, ' ')){
                tokens.push_back(token);
            }
            variList.push_back(tokens);
            tokens.clear();
        }   
    }
    inFile.close();

    processFC("diagnosis", goalList, ifStates, thenStates, variList, clauseList);               //Call process to begin searching for diagnosis.
}

//Controls the flow of control
void processFC(string fact, vector<vector<string>>& goalList, vector<vector<vector<string>>> ifStates, vector<vector<vector<string>>> thenStates, vector<vector<string>>& variList, vector<string> clauseList){
    search_cvl(0, fact, goalList, ifStates, thenStates, variList, clauseList);   
}

//Iterates through the variables of the clause variable list are returns a Ci
int search_cvl(int Ci, string fact,  vector<vector<string>>& goalList, vector<vector<vector<string>>> ifStates, vector<vector<vector<string>>> thenStates, vector<vector<string>>& variList, vector<string> clauseList){
    int Ri;
    bool match = false;             //indicates whether a match to a rule has been made

    while((Ci < clauseList.size()) && (match == false)){        //while not at the end of the clauseList, and while a match has not been made
        if(clauseList[Ci] == fact){                             //If clause Ci = the goal
            Ri = clause_to_rule(Ci, fact, goalList, ifStates, thenStates, variList, clauseList);    //calculate the rule number corresponding to the clause
            match = update_VLFC(Ci, Ri, fact, goalList, ifStates, thenStates, variList, clauseList); //update the values of the variables and check if a match is made                   
        }
        Ci++;
    }
}

//updates the variables in the variable list that are relevant to rule Ri
bool update_VLFC(int Ci, int Ri, string fact, vector<vector<string>>& goalList, vector<vector<vector<string>>> ifStates, vector<vector<vector<string>>> thenStates, vector<vector<string>>& variList, vector<string> clauseList){
    string vari;                    //variable name of clause Ci
    int j = 0;
    bool found = false;             //indicates if vari is found in goalList or variList
    bool match = true;              //indicates if the value of vari is matching with what is required of rule Ri
    int CiEnd = Ci + 1;             //the end of the clause list for rule Ri
    for(Ci; Ci < CiEnd; Ci++){      //while the end of the clause list has not been reached
        vari = clauseList[Ci];      //read the first/next variable into vari
        found = false;              //reinitialize found to false for 'next' variable
        if( (vari == " ") || (match == false) ){        //if there are no more variables or the values of the variables do not match with rule Ri       
           Ci = CiEnd;                                  //exit the loop   
        }     
        else{
            for(int i = 0; i < goalList.size(); i++){   //iterate through the goalList     
                if(vari == goalList[i][0]){             //if vari is found in goalList      
                    found = true;                       //set found to true
                    if(goalList[i][1] == "y"){          //if vari is initialized
                            match = validate_Ri(Ri, i, Ci, "i", "g", vari, ifStates, thenStates, variList, goalList); //check if the value of vari matches with rule Ri
                        i = goalList.size();            //exit the loop
                    }
                    else{                               //if vari is not initialized        
                        processFC(vari, goalList, ifStates, thenStates, variList, clauseList);  //restart process with vari as the goal
                        match = validate_Ri(Ri, i, Ci, "i", "g", vari, ifStates, thenStates, variList, goalList); //check if the value of vari matches with rule Ri
                        i = goalList.size();            //exit the loop
                    }
                }
            }
            if(found == false){                             //if vari is not found in goalList      
                for(int i = 0; i < variList.size(); i++){   //iterate through variList
                    if(vari == variList[i][0]){             //if vari is found in variList
                        found = true;                       //set found to true
                        if(variList[i][1] == "y"){          //if vari is initalized
                            match = validate_Ri(Ri, i, Ci, "i", "v", vari, ifStates, thenStates, variList, goalList); //check if the value of vari matches with rule Ri
                            i = variList.size();            //exit the loop
                        }
                        else{                               //if vari is not initialized
                            string value = symptomQuestions(vari);  //ask user for value
                            variList[i][1] = "y";           //set vari to initalized
                            variList[i][2] = value;         //set value of vari
                            match = validate_Ri(Ri, i, Ci, "i", "v", vari, ifStates, thenStates, variList, goalList); //check if the value of vari matches with rule Ri
                            i = variList.size();            //exit the loop
                        }
                    }
                }    
            }
        }
    }
    int i = 0;
    while (fact != goalList[i][0]){             //find the index of the goal in goalList
        i++;
    }
    if(match == true){                          //if the vari values match with the rule Ri
        match = validate_Ri(Ri, 1, j, "t", "g", fact, ifStates, thenStates, variList, goalList); //set the value of the goal
        return match;
    }
    else{                                       //if the vari values did not match with the rule Ri    
        return match;                                       
    }
}

//Finds the rule number for the corresponding clause number
int clause_to_rule(int Ci, string fact,  vector<vector<string>>& goalList, vector<vector<vector<string>>> ifStates, vector<vector<vector<string>>> thenStates, vector<vector<string>>& variList, vector<string> clauseList){
    int Ri = Ci;                //Calculate the rule related to clause Ci
    //the given formula was not used because the Ri and Ci are given by the index in the vectors, and the clause to rule ratio are 1 to 1. 
    //As a result for Ci = 0, Ri = ((Ci/1)+1) would result in Ri = 1
    //to adjust for this the formula would have to be altered to Ri-1 = ((Ci/1)+1) to get the
    //the correct clause number, which is equal to Ri = Ci
    return Ri;
}