#ifndef APPLICATION_H
#define APPLICATION_H
 
#include <bobcat_ui/all.h>
#include "Graph.h"
 
class Application : public bobcat::Application_ {
    bobcat::Window*   window;
    bobcat::Dropdown* fromDropdown;
    bobcat::Dropdown* toDropdown;
    bobcat::Button*   cheapestBtn;
    bobcat::Button*   fastestBtn;
    bobcat::Button*   fewestBtn;
    bobcat::TextBox*  resultBox;
 
    Graph graph;
 
    Vertex* findVertex(std::string code);
    void loadGraph();
    void displayRoute(Waypoint* result, std::string mode);

    Waypoint* getRoot(Waypoint* wp);
    void deleteWaypoints(Waypoint* wp);
 
    void onCheapest(bobcat::Widget* w);
    void onFastest(bobcat::Widget* w);
    void onFewest(bobcat::Widget* w);
 
public:
    Application();
};
 
#endif
