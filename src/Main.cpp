#include "../headers/header.h"

using namespace std;
using json = nlohmann::json;

//global constants
vector<string> lifts {"Deadlift", "Squat", "Bench Press"};

string teststring;

string trimed(string input) {
    for (int i = input.size() - 1; i >= 0; i--) {
        if (input[i] != ' ') {
            input.resize(i + 1);
            break;
        }
        if (i == 0 && input[0] == ' ') return "";
    }
    if (input[0] == ' ') {
        //flemme d'optimiser
        std::reverse(input.begin(), input.end());
        input = trimed(input);
        std::reverse(input.begin(), input.end());
    }
    return input;
}

bool isNumber(string input) {
    regex reg("^[+-]?([0-9]+([.][0-9]*)?|[.][0-9]+)$");
    return regex_match(input, reg);
}

class User {
public:
    string name;
    int weight;
    int age;
    vector<tuple<string, float>> maxLifts;
    User(string inputName, int inputWeight, int inputAge, vector<tuple<string, float>> inputLifts) {
        name = inputName;
        weight = inputWeight;
        age = inputAge;
        maxLifts = inputLifts;
    };
};

void to_json(json& j, const User& user) {
    j = json{{"name", user.name}, {"weight", user.weight}, {"age", user.age}, {"maxLifts", user.maxLifts}};
}

void from_json(const json& j, User& user) {
    j.at("name").get_to(user.name);
    j.at("weight").get_to(user.weight);
    j.at("age").get_to(user.age);
    j.at("maxLifts").get_to(user.maxLifts);
}

//returns a vector of usernames from /data/users.txt
vector<string> loadUsers() {
    vector<string> users{};
    // reads the users from the user file
    ifstream userFileRead("../data/users.txt");
    if (!userFileRead) {
        cout << "Error opening userFile" << endl;
        exit(1);
    }

    string user;
    while (getline(userFileRead, user)) {
        users.push_back(user);
    }
    users.push_back("Add New User");
    userFileRead.close();
    return users;
}

void addUser(User inputUser) {
    vector<string> usernames = loadUsers();
    //if user doesn't exist yet, add to the list of users
    if (find(usernames.begin(), usernames.end(), inputUser.name) == usernames.end()) {
        ofstream userFileWrite("../data/users.txt", std::ios::out | ios::app);
        if (!userFileWrite) {
            cout << "Error opening userFile" << endl;
            exit(1);
        }
        userFileWrite << inputUser.name << endl;
    }
    json j = inputUser;
    ofstream newUserFile("../data/users/" + inputUser.name + ".json");
    if (!newUserFile) {
            cout << "Error writing new user" << endl;
            exit(1);
    }
    newUserFile << j;
}

void deleteUser(string name) {
    vector<string> usernames = loadUsers();
    if (find(usernames.begin(), usernames.end(), name) != usernames.end()) {
        ofstream userFileWrite("../data/users.txt");
        for (string username : usernames) {
            if (username != name && username != "Add New User") {
                userFileWrite << username << endl;
            }
        }
        if (remove(("../data/users/" + name + ".json").c_str()) != 0) {
            cout << "Error deleting file" << endl;
        }
    }
}

// Opens a box which lets you select between choices of an input vector of strings
// Returns the chosen string
string OptionsMenu(vector<string> input) {
    initscr();
    noecho();
    cbreak();
    curs_set(0);
    int maxX, maxY;
    getmaxyx(stdscr, maxY, maxX);
    auto win = newwin(2 + input.size(), maxX-12, maxY-20, 5);
    box(win, 0, 0); // prints a box with win dimensions
    refresh();
    wrefresh(win);
    keypad(win, TRUE);
    int selected = 0; 
    while (true) {
        for (int i = 0; i < input.size(); i++) {
            if (i == selected) {
                wattron(win, A_REVERSE);
            }
            mvwprintw(win, i+1, 1, input[i].c_str());
            wattroff(win, A_REVERSE);
        }

        int choice = wgetch(win);
        switch (choice) {
            case KEY_UP:
                if (selected > 0) selected--; 
                break;
            case KEY_DOWN:
                if (selected < input.size() - 1) selected++;
                break;
            case 'q':
                deleteUser(loadUsers()[selected]);
                clear();
                delwin(win);
                endwin();
                return OptionsMenu(loadUsers());
                break;
            default:
                break;
        }
        // enter is 10, return the lift selected
        if (choice == 10) {
            break;
        }
    }
    clear();
    delwin(win);
    endwin();
    return input[selected];
}

string ChooseLiftMenu() {
    return OptionsMenu(lifts);
}

void newUserMenu() {
    initscr();
    noecho();
    cbreak();
    curs_set(1);
    int maxX, maxY;
    getmaxyx(stdscr, maxY, maxX);
    auto win_body = newwin(20, maxX-12, maxY-10, 5);

    keypad(stdscr, TRUE);

    FIELD *fields[7 + lifts.size() * 4];

    //username fields
    fields[0] = new_field(1, 35, maxY-40, 0, 0, 0);
	fields[1] = new_field(1, 35, maxY-40, 40, 0, 0);

    //weight fields
	fields[2] = new_field(1, 35, maxY-38, 0, 0, 0);
	fields[3] = new_field(1, 35, maxY-38, 40, 0, 0);

    fields[4] = new_field(1, 35, maxY-36, 0, 0, 0);
	fields[5] = new_field(1, 35, maxY-36, 40, 0, 0);

    for (int i = 0; i < lifts.size(); i++) {
        fields[6 + i * 4] = new_field(1, 35, maxY - (34 - i * 3), 0, 0, 0);
        fields[7 + i * 4] = new_field(1, 20, maxY - (34 - i * 3), 40, 0, 0);
        fields[8 + i * 4] = new_field(1, 35, maxY - (33 - i * 3), 0, 0, 0);
        fields[9 + i * 4] = new_field(1, 20, maxY - (33 - i * 3), 40, 0, 0);
    }

    //sets the last field to null
	fields[6 + lifts.size() * 4] = NULL;

    set_field_buffer(fields[0], 0, "Username:");
	set_field_buffer(fields[1], 0, "");

	set_field_buffer(fields[2], 0, "Weight (in kg):");
	set_field_buffer(fields[3], 0, "");

    set_field_buffer(fields[4], 0, "Age (optional):");
	set_field_buffer(fields[5], 0, "");

    for (int i = 0; i < lifts.size(); i++) {
        string text = "Best " + lifts[i] + " Set Weight";
        set_field_buffer(fields[6 + i * 4], 0, text.c_str());
        set_field_buffer(fields[7 + i * 4], 0, "");
        set_field_buffer(fields[8 + i * 4], 0, "Number of reps (1-30):");
        set_field_buffer(fields[9 + i * 4], 0, "");
    }

    set_field_opts(fields[0], O_VISIBLE | O_PUBLIC | O_AUTOSKIP);
	set_field_opts(fields[1], O_VISIBLE | O_PUBLIC | O_EDIT | O_ACTIVE);

	set_field_opts(fields[2], O_VISIBLE | O_PUBLIC | O_AUTOSKIP);
	set_field_opts(fields[3], O_VISIBLE | O_PUBLIC | O_EDIT | O_ACTIVE);

    set_field_opts(fields[4], O_VISIBLE | O_PUBLIC | O_AUTOSKIP);
	set_field_opts(fields[5], O_VISIBLE | O_PUBLIC | O_EDIT | O_ACTIVE);

    for (int i = 0; i < lifts.size(); i++) {
        set_field_opts(fields[6 + i * 4], O_VISIBLE | O_PUBLIC | O_AUTOSKIP);
        set_field_opts(fields[7 + i * 4], O_VISIBLE | O_PUBLIC | O_EDIT | O_ACTIVE);
        set_field_opts(fields[8 + i * 4], O_VISIBLE | O_PUBLIC | O_AUTOSKIP);
        set_field_opts(fields[9 + i * 4], O_VISIBLE | O_PUBLIC | O_EDIT | O_ACTIVE);
    }


    set_field_back(fields[1], A_UNDERLINE);
	set_field_back(fields[3], A_UNDERLINE);
    set_field_back(fields[5], A_UNDERLINE);

    for (int i = 0; i < lifts.size(); i++) {
        set_field_back(fields[7 + i * 4], A_UNDERLINE);
        set_field_back(fields[9 + i * 4], A_UNDERLINE); 
    }

    auto form = new_form(fields);
    set_form_win(form, win_body);

    post_form(form);
    refresh();
    wrefresh(win_body);
    //initialises the cursor
    form_driver(form, REQ_NEXT_FIELD);
    form_driver(form, REQ_PREV_FIELD);

    string inputUsername, inputWeight, inputAge;
    vector<tuple<int, int>> weightXreps;

    int c = 0;
    while (true) {
        c = getch();
        switch (c) {
            case KEY_DOWN:
                form_driver(form, REQ_NEXT_FIELD);
                form_driver(form, REQ_END_LINE);
                break;
            case KEY_UP:
                form_driver(form, REQ_PREV_FIELD);
                form_driver(form, REQ_END_LINE);
                break;
            case KEY_LEFT:
                form_driver(form, REQ_PREV_CHAR);
                break;
            case KEY_RIGHT:
                form_driver(form, REQ_NEXT_CHAR);
                break;
            case KEY_BACKSPACE: case 127:
                form_driver(form, REQ_DEL_PREV);
                break;
            default:
                form_driver(form, c);
                break;
        }
        if (c == 10) {
            form_driver(form, REQ_VALIDATION);
            inputUsername = field_buffer(fields[1], 0);
            inputWeight = field_buffer(fields[3], 0);
            inputAge = field_buffer(fields[5], 0);
            weightXreps = {};
            for (int i = 0; i < lifts.size(); i++) {
                string liftWeightStr = trimed(field_buffer(fields[7 + i * 4], 0));
                string liftRepsStr = trimed(field_buffer(fields[9 + i * 4], 0));
                float liftWeight = 0;
                int liftReps = 0;
                if (liftWeightStr != "" && !isNumber(liftWeightStr)) {
                    mvwprintw(win_body, 1, 1, "Please Input Correct Exercise Weights");
                    wrefresh(win_body);
                    goto exitLabel;
                }
                if (liftRepsStr != "" && !isNumber(liftRepsStr)) {
                    mvwprintw(win_body, 1, 1, "Please Input Correct Reps");
                    wrefresh(win_body);
                    goto exitLabel;
                }
                if (isNumber(liftWeightStr)) liftWeight = stof(liftWeightStr);
                if (isNumber(liftRepsStr)) liftReps = stoi(liftRepsStr);
                tuple<float, int> liftInfo(liftWeight, liftReps);
                weightXreps.push_back(liftInfo);
            }

            if (trimed(inputUsername) == "") {
                mvwprintw(win_body, 1, 1, "Please Input A Username");
                wrefresh(win_body);
                continue;
            }
            if (trimed(inputWeight) == "" || !isNumber(trimed(inputWeight))) {
                mvwprintw(win_body, 1, 1, "Please Input A Correct Weight");
                wrefresh(win_body);
                continue;
            }
            if (trimed(inputAge) != "" && !isNumber(trimed(inputAge))) {
                mvwprintw(win_body, 1, 1, "Please Input A Correct Age");
                wrefresh(win_body);
                continue;
            }
            break;
        }
        exitLabel:;
    }

    int age = (trimed(inputAge) == "") ? -1 : stoi(trimed(inputAge));
    vector<tuple<string, float>> maxLifts{};
    for (int i = 0; i < lifts.size(); i++) {
        float oneRepMax = 100 * get<0>(weightXreps[i]) / (48.8 + 53.8 * exp(-0.075 * get<1>(weightXreps[i])));
        if (get<1>(weightXreps[i]) == 0) oneRepMax = 0;
        maxLifts.push_back(tuple<string, float>(lifts[i], oneRepMax));
    }

    User newUser = User(trimed(inputUsername), 
                        stoi(trimed(inputWeight)),
                        age,
                        maxLifts);

    addUser(newUser);


    unpost_form(form);
    free_form(form);
    for (auto field: fields) free_field(field);

    clear();
    endwin();
}

void loadUserMenu() {
    initscr();
    noecho();
    cbreak();
    curs_set(1);
    int maxX, maxY;
    getmaxyx(stdscr, maxY, maxX);
    auto win_body = newwin(20, maxX-12, maxY-10, 5);

    keypad(stdscr, TRUE);

    clear();
    endwin();
}

int main() {
    vector<string> users = loadUsers();

    string chosenUser = OptionsMenu(users);
    while (chosenUser == "Add New User") {
        newUserMenu();
        users = loadUsers();
        chosenUser = OptionsMenu(users);
    }
    //user is chosen
    loadUserMenu();

    // cout << teststring << endl;
    // cout << ChooseLiftMenu() << endl;
    
    
}

