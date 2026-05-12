#include "Application.h"
 
// ─────────────────────────────────────────
// Waypoint memory cleanup helpers
// ─────────────────────────────────────────
 
Waypoint* Application::getRoot(Waypoint* wp) {
    if (wp == nullptr) return nullptr;
    while (wp->parent != nullptr)
        wp = wp->parent;
    return wp;
}
 
void Application::deleteWaypoints(Waypoint* wp) {
    if (wp == nullptr) return;
    for (int i = 0; i < wp->children.size(); i++) {
        deleteWaypoints(wp->children[i]);
    }
    delete wp;
}
 
// ─────────────────────────────────────────
// Graph helpers
// ─────────────────────────────────────────
 
Vertex* Application::findVertex(std::string code) {
    // Trim leading spaces
    while (!code.empty() && code[0] == ' ')
        code = code.substr(1);
    // Trim trailing spaces
    while (!code.empty() && code[code.size() - 1] == ' ')
        code = code.substr(0, code.size() - 1);
 
    for (int i = 0; i < graph.vertices.size(); i++) {
        if (graph.vertices[i]->data == code)
            return graph.vertices[i];
    }
    return nullptr;
}
 
void Application::loadGraph() {
    // Add airports
    graph.addVertex(new Vertex("SFO"));
    graph.addVertex(new Vertex("LAX"));
    graph.addVertex(new Vertex("LAS"));
    graph.addVertex(new Vertex("SEA"));
    graph.addVertex(new Vertex("DEN"));
    graph.addVertex(new Vertex("ORD"));
    graph.addVertex(new Vertex("DFW"));
    graph.addVertex(new Vertex("ATL"));
    graph.addVertex(new Vertex("JFK"));
    graph.addVertex(new Vertex("MIA"));
 
    // Grab pointers for convenience
    Vertex* SFO = findVertex("SFO");
    Vertex* LAX = findVertex("LAX");
    Vertex* LAS = findVertex("LAS");
    Vertex* SEA = findVertex("SEA");
    Vertex* DEN = findVertex("DEN");
    Vertex* ORD = findVertex("ORD");
    Vertex* DFW = findVertex("DFW");
    Vertex* ATL = findVertex("ATL");
    Vertex* JFK = findVertex("JFK");
    Vertex* MIA = findVertex("MIA");
 
    // Add routes (cost in $, time in minutes)
    graph.addEdge(SFO, LAX, 120,  90);
    graph.addEdge(SFO, LAS,  80,  95);
    graph.addEdge(SFO, SEA, 140, 120);
    graph.addEdge(SFO, JFK, 500, 360);
    graph.addEdge(LAX, LAS,  70,  75);
    graph.addEdge(LAX, DEN, 160, 140);
    graph.addEdge(LAX, JFK, 420, 330);
    graph.addEdge(LAX, MIA, 460, 340);
    graph.addEdge(LAS, DEN,  90, 115);
    graph.addEdge(LAS, DFW, 150, 155);
    graph.addEdge(SEA, DEN, 130, 165);
    graph.addEdge(SEA, ORD, 220, 240);
    graph.addEdge(SEA, JFK, 430, 335);
    graph.addEdge(DEN, ORD, 120, 130);
    graph.addEdge(DEN, DFW, 110, 120);
    graph.addEdge(DEN, JFK, 300, 240);
    graph.addEdge(ORD, ATL, 160, 130);
    graph.addEdge(ORD, JFK, 140, 115);
    graph.addEdge(ORD, MIA, 280, 190);
    graph.addEdge(DFW, ATL, 130, 120);
    graph.addEdge(DFW, MIA, 170, 145);
    graph.addEdge(DFW, JFK, 260, 120);
    graph.addEdge(ATL, JFK, 150, 125);
    graph.addEdge(ATL, MIA, 100,  95);
    graph.addEdge(JFK, MIA, 220, 180);
}
 
// ─────────────────────────────────────────
// Display route result in the text box
// ─────────────────────────────────────────
 
void Application::displayRoute(Waypoint* result, std::string mode) {
    if (result == nullptr) {
        resultBox->label("No route found.");
        return;
    }
 
    // Walk parent chain to reconstruct path
    ArrayList<std::string> path;
    Waypoint* current = result;
    while (current != nullptr) {
        path.prepend(current->vertex->data);
        current = current->parent;
    }
 
    // Build output string with no library calls
    static char buf[1024];
    int pos = 0;
 
    // Write mode label
    const char* modeLabel = "Mode: ";
    for (int i = 0; modeLabel[i]; i++) buf[pos++] = modeLabel[i];
    for (int i = 0; i < (int)mode.size(); i++) buf[pos++] = mode[i];
    buf[pos++] = '\n';
 
    // Write route
    const char* routeLabel = "Route: ";
    for (int i = 0; routeLabel[i]; i++) buf[pos++] = routeLabel[i];
    for (int i = 0; i < path.size(); i++) {
        for (int j = 0; j < (int)path[i].size(); j++)
            buf[pos++] = path[i][j];
        if (i < path.size() - 1) {
            buf[pos++] = ' ';
            buf[pos++] = '-';
            buf[pos++] = '>';
            buf[pos++] = ' ';
        }
    }
    buf[pos++] = '\n';
 
    // Helper lambda to write an int into buf
    auto writeInt = [&](int n) {
        char tmp[16];
        int len = 0;
        if (n == 0) { tmp[len++] = '0'; }
        else { while (n > 0) { tmp[len++] = '0' + n % 10; n /= 10; } }
        for (int i = len - 1; i >= 0; i--) buf[pos++] = tmp[i];
    };
 
    // Write cost
    const char* costLabel = "Total cost:  $";
    for (int i = 0; costLabel[i]; i++) buf[pos++] = costLabel[i];
    writeInt(result->partialCost);
    buf[pos++] = '\n';
 
    // Write time
    const char* timeLabel = "Total time:  ";
    for (int i = 0; timeLabel[i]; i++) buf[pos++] = timeLabel[i];
    writeInt(result->partialTime);
    const char* minLabel = " min\n";
    for (int i = 0; minLabel[i]; i++) buf[pos++] = minLabel[i];
 
    // Write stops (stops counts hops, subtract 1 for layovers)
    const char* stopsLabel = "Stops:       ";
    for (int i = 0; stopsLabel[i]; i++) buf[pos++] = stopsLabel[i];
    writeInt(result->stops - 1);
 
    buf[pos] = '\0';
    resultBox->label(buf);
}
 
// ─────────────────────────────────────────
// Button handlers
// ─────────────────────────────────────────
 
void Application::onCheapest(bobcat::Widget* w) {
    Vertex* f = findVertex(fromDropdown->text());
    Vertex* t = findVertex(toDropdown->text());
    if (!f || !t) {
        resultBox->label("Could not find airports.");
        window->redraw();
        return;
    }
    Waypoint* result = graph.cheapest(f, t);
    displayRoute(result, "Cheapest");
    deleteWaypoints(getRoot(result));
    window->redraw();
}
 
void Application::onFastest(bobcat::Widget* w) {
    Vertex* f = findVertex(fromDropdown->text());
    Vertex* t = findVertex(toDropdown->text());
    if (!f || !t) {
        resultBox->label("Could not find airports.");
        window->redraw();
        return;
    }
    Waypoint* result = graph.fastest(f, t);
    displayRoute(result, "Fastest");
    deleteWaypoints(getRoot(result));
    window->redraw();
}
 
void Application::onFewest(bobcat::Widget* w) {
    Vertex* f = findVertex(fromDropdown->text());
    Vertex* t = findVertex(toDropdown->text());
    if (!f || !t) {
        resultBox->label("Could not find airports.");
        window->redraw();
        return;
    }
    Waypoint* result = graph.fewestStops(f, t);
    displayRoute(result, "Fewest Stops");
    deleteWaypoints(getRoot(result));
    window->redraw();
}
 
// ─────────────────────────────────────────
// Constructor
// ─────────────────────────────────────────
 
Application::Application() {
    window = new bobcat::Window(100, 100, 800, 400, "Flight Planner");
 
    loadGraph();
 
    // --- Dropdowns ---
    fromDropdown = new bobcat::Dropdown(20,  40, 200, 30, "From");
    toDropdown   = new bobcat::Dropdown(240, 40, 200, 30, "To");
    for (int i = 0; i < graph.vertices.size(); i++) {
        fromDropdown->add(graph.vertices[i]->data);
        toDropdown->add(graph.vertices[i]->data);
    }
 
    // --- Search buttons ---
    cheapestBtn = new bobcat::Button(20,  110, 130, 35, "Cheapest");
    fastestBtn  = new bobcat::Button(160, 110, 130, 35, "Fastest");
    fewestBtn   = new bobcat::Button(300, 110, 130, 35, "Fewest Stops");
 
    ON_CLICK(cheapestBtn, Application::onCheapest);
    ON_CLICK(fastestBtn,  Application::onFastest);
    ON_CLICK(fewestBtn,   Application::onFewest);
 
    // --- Result text box ---
    resultBox = new bobcat::TextBox(20, 165, 740, 200, "Select airports and a preference.");
 
    window->show();
}