#ifndef APPLICATION_H
#define APPLICATION_H

#include <bobcat_ui/all.h>
#include "Graph.h"

class Application : public bobcat::Application_ {
    bobcat::Window*   window;
    bobcat::Dropdown* fromDropdown;
    bobcat::Dropdown* toDropdown;
    bobcat::Dropdown* modeDropdown;
    bobcat::Button*   searchButton;
    bobcat::TextBox*  resultBox;
    

    Graph g;
    ArrayList<Vertex*>   cities;
    ArrayList<Waypoint*> lastSearch;

    void initData();
    void initInterface();

    void deleteWaypoints();
    void showRoute(Waypoint* path, std::string mode);

    void onClick(bobcat::Widget* sender);

public:
    Application();
    ~Application();
};

#endif