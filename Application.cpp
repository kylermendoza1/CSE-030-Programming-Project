#include "Application.h"
#include <fstream>
#include <sstream>

using namespace bobcat;
using namespace std;

// ─────────────────────────────────────────
// Waypoint memory cleanup
// ─────────────────────────────────────────

void Application::deleteWaypoints() {
    for (int i = 0; i < lastSearch.size(); i++) {
        delete lastSearch[i];
    }
    lastSearch = ArrayList<Waypoint*>();
}

// ─────────────────────────────────────────
// Load data from CSV files
// ─────────────────────────────────────────

void Application::initData() {
    fstream file;

    file.open("./assets/airports.csv", ios::in);
    if (file.is_open()) {
        string line;
        while (getline(file, line)) {
            if (!line.empty())
                cities.append(new Vertex(line));
        }
        file.close();
    }

    for (int i = 0; i < cities.size(); i++) {
        g.addVertex(cities[i]);
    }

    file.open("./assets/route.csv", ios::in);
    if (file.is_open()) {
        string line;
        while (getline(file, line)) {
            if (line.empty()) continue;

            stringstream ss(line);
            string from, to, costStr, timeStr;

            getline(ss, from,    ',');
            getline(ss, to,      ',');
            getline(ss, costStr, ',');
            getline(ss, timeStr, ',');

            int fromIdx = stoi(from);
            int toIdx   = stoi(to);
            int cost    = stoi(costStr);
            int time    = stoi(timeStr);

            if (fromIdx < cities.size() && toIdx < cities.size()) {
                g.addEdge(cities[fromIdx], cities[toIdx], cost, time);
            }
        }
        file.close();
    }
}

// ─────────────────────────────────────────
// Build the UI
// ─────────────────────────────────────────

void Application::initInterface() {
    window = new Window(25, 75, 500, 750, "Simple Navigation Project");

    fromDropdown = new Dropdown(25, 25,  450, 25, "Origin");
    toDropdown   = new Dropdown(25, 75,  450, 25, "Destination");

    for (int i = 0; i < cities.size(); i++) {
        fromDropdown->add(cities[i]->data);
        toDropdown->add(cities[i]->data);
    }

    modeDropdown = new Dropdown(25, 125, 450, 25, "Preference");
    modeDropdown->add("Cheapest");
    modeDropdown->add("Fastest");
    modeDropdown->add("Fewest Stops");

    searchButton = new Button(25, 175, 450, 30, "Search");

    resultBox = new bobcat::TextBox(25, 225, 450, 150, "Select airports and click Search.");

    // Canvas sits below the result box

    ON_CLICK(searchButton, Application::onClick);

    window->show();
}

// ─────────────────────────────────────────
// Display route in result box
// ─────────────────────────────────────────

void Application::showRoute(Waypoint* path, string mode) {
    if (!path) {
        resultBox->label("No route found.");
        window->redraw();
        return;
    }

    // Walk parent chain to get ordered steps
    ArrayList<Waypoint*> steps;
    Waypoint* temp = path;
    while (temp != nullptr) {
        steps.prepend(temp);
        temp = temp->parent;
    }

    // Copy all data into plain variables
    ArrayList<string> cityNames;
    ArrayList<int>    legCosts;
    ArrayList<int>    legTimes;

    for (int i = 0; i < steps.size(); i++) {
        cityNames.append(steps[i]->vertex->data);
        if (i < steps.size() - 1) {
            legCosts.append(steps[i+1]->partialCost - steps[i]->partialCost);
            legTimes.append(steps[i+1]->partialTime - steps[i]->partialTime);
        }
    }
    int totalCost  = path->partialCost;
    int totalTime  = path->partialTime;
    int totalStops = path->stops - 1;

    // Show route on canvas


    // Build output into a static buffer
    static char buf[2048];
    int pos = 0;

    auto writeStr = [&](const char* s) {
        while (*s && pos < 2047) buf[pos++] = *s++;
    };
    auto writeInt = [&](int n) {
        char tmp[16];
        int len = 0;
        if (n == 0) { tmp[len++] = '0'; }
        else { while (n > 0) { tmp[len++] = '0' + n % 10; n /= 10; } }
        for (int i = len - 1; i >= 0; i--) buf[pos++] = tmp[i];
    };

    writeStr("[ ");
    writeStr(mode.c_str());
    writeStr(" Route ]\n");
    writeStr("------------------------\n");

    for (int i = 0; i < cityNames.size(); i++) {
        writeStr("  ");
        writeStr(cityNames[i].c_str());
        buf[pos++] = '\n';
        if (i < legCosts.size()) {
            writeStr("    | $");
            writeInt(legCosts[i]);
            writeStr("  ~  ");
            writeInt(legTimes[i]);
            writeStr(" min\n");
            writeStr("    v\n");
        }
    }

    writeStr("------------------------\n");
    writeStr("  Cost:   $"); writeInt(totalCost);  buf[pos++] = '\n';
    writeStr("  Time:    "); writeInt(totalTime);  writeStr(" min\n");
    writeStr("  Stops:   "); writeInt(totalStops);

    buf[pos] = '\0';
    resultBox->label(buf);
    window->redraw();
}

// ─────────────────────────────────────────
// Button click handler
// ─────────────────────────────────────────

void Application::onClick(bobcat::Widget* sender) {
    int fromIndex = fromDropdown->value();
    int toIndex   = toDropdown->value();

    if (fromIndex == toIndex) {
        resultBox->label("Please select different airports.");
        window->redraw();
        return;
    }

    if (fromIndex < 0 || toIndex < 0 ||
        fromIndex >= cities.size() || toIndex >= cities.size()) {
        resultBox->label("Invalid selection.");
        window->redraw();
        return;
    }

    deleteWaypoints();

    Waypoint* path = nullptr;
    string mode    = modeDropdown->text();

    if (mode == "Cheapest") {
        path = g.cheapest(cities[fromIndex], cities[toIndex], lastSearch);
    } else if (mode == "Fastest") {
        path = g.fastest(cities[fromIndex], cities[toIndex], lastSearch);
    } else if (mode == "Fewest Stops") {
        path = g.fewestStops(cities[fromIndex], cities[toIndex], lastSearch);
    }

    showRoute(path, mode);
}

// ─────────────────────────────────────────
// Constructor & Destructor
// ─────────────────────────────────────────

Application::Application() {
    initData();
    initInterface();
}

Application::~Application() {
    deleteWaypoints();
    for (int i = 0; i < cities.size(); i++) {
        delete cities[i];
    }
}