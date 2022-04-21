#include <SFML/Graphics.hpp>
#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <string>
#include <fstream>
#include <sstream>
#include <chrono>
#include "TextureManager.h"
#include "flightNode.h"
#include "SplayTree.h"
#include "HashTable.h"

using namespace std;


string standardTime(string time)
{
    int len = time.length();
    string out;

    if (len == 1)
    {
        out = "12:0 " + time + " AM";
    }
    else if (len == 2)
    {
        out = "12: " + time + " AM";
    }
    else
    {
        string hour = time.substr(0, len/2);
        string minute = time.substr(len/2, 2);
        if (stoi(hour) / float(12) > 1)
        {
            out = to_string(stoi(hour) % 12) + ":" + minute + " PM";
        }
        else if (stoi(hour) / float(12) == 1)
        {
            out = "1:" + minute + " PM";
        }
        else if (stoi(hour) == 0)
        {
            out = "12:" + minute + "AM";
        }
        else
        {
            out = hour + ":" + minute + " AM";
        }
    }
    return out;
}


int main()
{


    unordered_map<string, sf::Sprite*> airports;

    // Map
    sf::Sprite mapUS;
    mapUS.setTexture(TextureManager::GetTexture("united-states1"));


    string from = "";
    string to = "";

    int h =  mapUS.getGlobalBounds().height;
    int w =  mapUS.getGlobalBounds().width;
    int x = mapUS.getPosition().x;
    int y =  mapUS.getPosition().y;


    float xMax, xMin, xDiv, yMax, yMin, yDiv;
    xMax = 124.8;
    xMin = 66.5;
    xDiv = xMax - xMin;

    yMax = 50;
    yMin = 24.06;
    yDiv = yMax - yMin;

    //For GPS data stuff
    string fname = "../data/airportGPS.csv";
    string line, word;
    fstream file (fname, ios::in);
    if(file.is_open())
    {
        while(getline(file, line))
        {
            string name;
            string latitude;
            string longitude;
            stringstream str(line);
            if(line.length() > 6)
            {
                getline(str, name, ',');
                getline(str, latitude, ',');
                getline(str, longitude);


                float xCalc = float(x) + float(w)*((xMax-stof(longitude))/xDiv);
                float yCalc = float(y) + float(h)*((yMax-stof(latitude))/yDiv);
                // cout << name.length() << endl;

                sf::Sprite* temp = new sf::Sprite;
                temp->setTexture(TextureManager::GetTexture("nodeOrange"));
                temp->setPosition(xCalc,yCalc);
                temp->setScale(0.03, 0.03);
                // temp->setColor(sf::Color::Red);
                airports[name] = temp;
            }
        }
    }

    // Data storage
    unordered_map<string, unordered_map<string, unordered_set<flightNode*>>> data;
    // Structure 1
    unordered_map<string, HashTable> hashes;
    // Structure 2
    unordered_map<string, SplayTree> trees;
    unordered_set<string> fromPorts;
    unordered_set<string> toPorts;

    string dataName = "../data/fldata.csv";
    fstream fdata(dataName, ios::in);
    cout << "Reading flights from data, please wait...\n";
    int count = 0;
    if(fdata.is_open())
    {
        while(getline(fdata, line))
        {
            flightNode* temp = new flightNode();
            string originName;
            string destName;

            stringstream str(line);

            getline(str, word, ',');
            temp -> date = word;
            getline(str, word, ',');
            temp -> carrierAirline = stoi(word);
            getline(str, word, ',');
            temp -> tailNum = word;
            getline(str, word, ',');
            temp -> carrierFlight = stoi(word);
            getline(str, word, ',');
            temp -> originAirport = stoi(word);
            getline(str, word, ',');
            temp -> originAirportSeq = stoi(word);
            getline(str, word, ',');
            temp -> originCity = stoi(word);
            getline(str, word, ',');
            temp -> origin = word;
            originName = word;
            getline(str, word, ',');
            temp -> destAirport = stoi(word);
            getline(str, word, ',');
            temp -> destAirportSeq = stoi(word);
            getline(str, word, ',');
            temp -> destCity = stoi(word);
            getline(str, word, ',');
            temp -> dest = word;
            destName = word;
            getline(str, word, ',');
            temp -> estDepart = stoi(word);
            getline(str, word, ',');
            temp -> actDepart = stoi(word);
            getline(str, word, ',');
            temp -> departDelay = stoi(word);
            getline(str, word, ',');
            temp -> arrTime = stoi(word);
            getline(str, word, ',');
            temp -> arrDelay = stoi(word);

            // nodes.push_back(temp);
            data[originName][destName].insert(temp);
            
            toPorts.insert(destName);
            fromPorts.insert(originName);
            count++;
            if (count % 100000 == 0)
                cout << "Processed " << count << " flights.\n";
        }
    }
    cout << "All flights successfully read from file. Adding to data structures...\n";

    auto start = std::chrono::high_resolution_clock::now();
    for (pair<string, unordered_map<string, unordered_set<flightNode*>>> fromFlights : data) {
        string from = fromFlights.first;
        for (pair<string, unordered_set<flightNode*>> flightSet : fromFlights.second) {
            //cout << "Adding flights from " << from << " to " << flightSet.first << "\n";
            trees[from].insert(flightSet.second);
        }
        //cout << "Added " << trees[from].getSize() << " flights from airport " << from << " to tree in " << trees[from].nodeCount() << " sets.\n";
    }
    auto stop = std::chrono::high_resolution_clock::now();
    auto splayInsert = std::chrono::duration_cast<std::chrono::milliseconds>(stop-start);
    cout << "Added flights to splay trees in " << splayInsert.count() << " milliseconds.\n";

    start = std::chrono::high_resolution_clock::now();
    for (pair<string, unordered_map<string, unordered_set<flightNode*>>> fromFlights : data) {
        string from = fromFlights.first;
        for (pair<string, unordered_set<flightNode*>> flightSet : fromFlights.second) {
            //cout << "Adding flights from " << from << " to " << flightSet.first << "\n";
            hashes[from].insert(flightSet.second);
        }
    }
    stop = std::chrono::high_resolution_clock::now();
    auto hashInsert = std::chrono::duration_cast<std::chrono::milliseconds>(stop-start);
    cout << "Added flights to hash tables in " << hashInsert.count() << " milliseconds.\n";

    int fTotal = 0;
    for (auto a : data)
    {
        for (auto b : a.second)
            fTotal += b.second.size();
    }
    cout << fTotal << endl;

    // CARRIERS
    unordered_map<int, string> carrier;
    string carrierPath = "../data/carriers.csv";
    fstream carrierdata(carrierPath, ios::in);
    if(fdata.is_open())
    {
        while(getline(carrierdata, line))
        {
            stringstream str(line);
            getline(str, word, ',');
            int carVal = stoi(word);
            getline(str, word, ':');
            carrier[carVal] = word;
        }
    }

    // PORTS
    unordered_map<string, string> port;
    string portPath = "../data/ports.csv";
    fstream pathdata(portPath, ios::in);
    if(fdata.is_open())
    {
        while(getline(pathdata, line))
        {
            stringstream str(line);
            getline(str, word, ',');
            string portVal = word;
            getline(str, word, ':');
            port[portVal] = word.substr(1,word.length());
        }
    }


    // GUI setup
    sf::RenderWindow window(sf::VideoMode(1920,1080), "map");


    bool fromSel = false;
    bool toSel = false;
    sf::Font font;
    if (!font.loadFromFile("../data/arial.ttf"))
            std::cout << "can't load" << std::endl;


    sf::Text totalFlights;
    totalFlights.setFont(font);
    totalFlights.setString("Total Flights: " + to_string(fTotal));
    totalFlights.setPosition(10, mapUS.getGlobalBounds().height - 50);
    totalFlights.setCharacterSize(30);
    totalFlights.setFillColor(sf::Color::White);

    sf::Text title1;
    title1.setFont(font);
    title1.setString("Splay Tree");
    title1.setPosition(10, mapUS.getGlobalBounds().height + 10);
    title1.setCharacterSize(40);
    title1.setFillColor(sf::Color::White);
    title1.setStyle(sf::Text::Bold | sf::Text::Underlined);

    sf::Text sInsert;
    sInsert.setFont(font);
    sInsert.setString("Inserted Data In: " + to_string(splayInsert.count()) + " ms");
    sInsert.setPosition(100 + title1.getGlobalBounds().width, mapUS.getGlobalBounds().height + 10);
    sInsert.setCharacterSize(30);
    sInsert.setFillColor(sf::Color::White);

    sf::Text title2;
    title2.setFont(font);
    title2.setString("Hash Table");
    title2.setPosition((mapUS.getGlobalBounds().width)/2 +10, mapUS.getGlobalBounds().height + 10);
    title2.setCharacterSize(40);
    title2.setFillColor(sf::Color::White);
    title2.setStyle(sf::Text::Bold | sf::Text::Underlined);

    sf::Text hInsert;
    hInsert.setFont(font);
    hInsert.setString("Inserted Data In: " + to_string(hashInsert.count()) + " ms");
    hInsert.setPosition(90 + title2.getPosition().x + title2.getGlobalBounds().width, mapUS.getGlobalBounds().height + 10);
    hInsert.setCharacterSize(30);
    hInsert.setFillColor(sf::Color::White);

    sf::Text sAccess;
    sAccess.setFont(font);
    sAccess.setPosition(10, mapUS.getGlobalBounds().height + 20 + title1.getGlobalBounds().height);
    sAccess.setCharacterSize(40);
    sAccess.setFillColor(sf::Color::White);

    sf::Text hAccess;
    hAccess.setFont(font);
    hAccess.setPosition(title2.getPosition().x, mapUS.getGlobalBounds().height + 20 + title1.getGlobalBounds().height);
    hAccess.setCharacterSize(40);
    hAccess.setFillColor(sf::Color::White);




    sf::Text fromText;
    fromText.setFont(font);
    fromText.setString("From:");
    fromText.setPosition((mapUS.getGlobalBounds().width) + 10 , 10);
    fromText.setCharacterSize(40);
    fromText.setFillColor(sf::Color::White);

    sf::Text fromPort;
    fromPort.setFont(font);
    fromPort.setPosition((mapUS.getGlobalBounds().width) + 10 +150, 10);
    fromPort.setCharacterSize(40);
    fromPort.setFillColor(sf::Color::White);

    sf::Text toText;
    toText.setFont(font);
    toText.setString("To:");
    toText.setPosition((mapUS.getGlobalBounds().width) + 10 , mapUS.getGlobalBounds().height / 8);
    toText.setCharacterSize(40);
    toText.setFillColor(sf::Color::White);

    sf::Text toPort;
    toPort.setFont(font);
    toPort.setPosition((mapUS.getGlobalBounds().width) + 10 +150, mapUS.getGlobalBounds().height / 8);
    toPort.setCharacterSize(40);
    toPort.setFillColor(sf::Color::White);

    sf::Text fromDecode;
    fromDecode.setFont(font);
    fromDecode.setPosition((mapUS.getGlobalBounds().width) + 10, 10 + 50);
    fromDecode.setCharacterSize(40);
    fromDecode.setFillColor(sf::Color::White);

    sf::Text toDecode;
    toDecode.setFont(font);
    toDecode.setPosition((mapUS.getGlobalBounds().width) + 10, mapUS.getGlobalBounds().height / 8+50);
    toDecode.setCharacterSize(40);
    toDecode.setFillColor(sf::Color::White);

    sf::Sprite search;
    search.setTexture(TextureManager::GetTexture("search1"));
    search.setPosition((mapUS.getGlobalBounds().width) + 60,220);
    search.setScale(0.5,0.5);

    sf::Sprite test1;
    test1.setTexture(TextureManager::GetTexture("test1"));
    test1.setPosition((mapUS.getGlobalBounds().width) + 95,300);

    sf::Sprite test2;
    test2.setTexture(TextureManager::GetTexture("test2"));
    test2.setPosition((test1.getPosition().x) + 80,300);

    sf::Sprite test3;
    test3.setTexture(TextureManager::GetTexture("test3"));
    test3.setPosition((test2.getPosition().x) + 80,300);

    sf::Sprite next;
    next.setTexture(TextureManager::GetTexture("next"));
    next.setPosition(test3.getPosition().x + 20,mapUS.getGlobalBounds().height + 100);
    next.setScale(0.07,0.07);

    sf::Sprite prev;
    prev.setTexture(TextureManager::GetTexture("prev"));
    prev.setPosition(test1.getPosition().x + 20,mapUS.getGlobalBounds().height + 100);
    prev.setScale(0.07,0.07);

    sf::Text posCount;
    posCount.setFont(font);
    posCount.setCharacterSize(30);
    posCount.setPosition(test1.getPosition().x + 20,mapUS.getGlobalBounds().height + 50);
    posCount.setFillColor(sf::Color::White);


    sf::RectangleShape sepRight;
    sepRight.setSize(sf::Vector2f(3, sf::VideoMode::getDesktopMode().height));
    sepRight.setPosition(sf::Vector2f(mapUS.getGlobalBounds().width, 0));

    sf::RectangleShape sepStructs;
    sepStructs.setSize(sf::Vector2f(3, 300));
    sepStructs.setPosition(sf::Vector2f(mapUS.getGlobalBounds().width/2, mapUS.getGlobalBounds().height+10));

    sf::RectangleShape sepBottom;
    sepBottom.setSize(sf::Vector2f(sf::Vector2f(mapUS.getGlobalBounds().width, 3)));
    sepBottom.setPosition(sf::Vector2f(0, mapUS.getGlobalBounds().height + 10));

    sf::RectangleShape sepMiddle;
    sepMiddle.setSize(sf::Vector2f(sf::Vector2f(mapUS.getGlobalBounds().width, 3)));
    sepMiddle.setPosition(sf::Vector2f((mapUS.getGlobalBounds().width), (test1.getPosition().y) + 80));

    sf::Text info1;
    info1.setFont(font);
    info1.setPosition((mapUS.getGlobalBounds().width) + 10, (sepMiddle.getPosition().y) + 20);
    info1.setCharacterSize(30);
    info1.setFillColor(sf::Color::White);
    info1.setStyle(sf::Text::Bold | sf::Text::Underlined);
    info1.setString("Date:\n\nCarrier:\n\nDeparture:\n\nDeparture Delay:\n\nArrival:\n\nArrival Delay:");

    sf::Text info2;
    info2.setFont(font);
    info2.setPosition((mapUS.getGlobalBounds().width) + 10, (sepMiddle.getPosition().y) + 20);
    info2.setCharacterSize(30);
    info2.setFillColor(sf::Color::White);

    vector<flightNode*> results;
    unordered_set<flightNode*> hashResults;
    unordered_set<flightNode*> treeResults;
    int numResults = 0;
    int pos = 0;

    // bool paused = false;

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
            // else if (event.type == event.MouseButtonPressed && event.type == event.MouseButtonReleased)
            // {
            //     cout << "HI";
            // }
            // else if (event.type == event.MouseButtonPressed)
            // {
            //     cout << "Reset";
            //     paused = false;
            // }
            else if (event.type == event.MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left)
            {
                sf::Vector2f mouseXY = window.mapPixelToCoords(sf::Mouse::getPosition(window));
                for (auto i : airports)
                {
                    if (i.second->getGlobalBounds().contains(mouseXY))
                    {
                        pos = 0;
                        if (!fromSel)
                        {
                            fromSel = true;
                            from = i.first;
                            airports[from]->setTexture(TextureManager::GetTexture("nodeBlue"));
                        }
                        else
                        {
                            if(from == i.first)
                            {
                                airports[from]->setTexture(TextureManager::GetTexture("nodeOrange"));
                                from = "";
                                if(toSel)
                                {
                                    airports[to]->setTexture(TextureManager::GetTexture("nodeOrange"));
                                    to = "";
                                }
                                fromSel = false;
                                toSel = false;
                                results.clear();
                            }
                            else
                            {
                                if (!toSel)
                                {
                                    toSel = true;
                                    to = i.first;
                                    airports[to]->setTexture(TextureManager::GetTexture("nodeBlue"));
                                }
                                else
                                {
                                    toSel = false;
                                    airports[to]->setTexture(TextureManager::GetTexture("nodeOrange"));
                                    to = "";
                                    results.clear();
                                }
                            }
                        }
                    }
                } 
                if (search.getGlobalBounds().contains(mouseXY))
                {


                    // searchFlight(from, to);

                    if (toSel && fromSel)
                    {
                        for(auto i : data[from][to])
                            results.push_back(i);

                        start = std::chrono::high_resolution_clock::now();
                        hashResults = hashes[from].search(to);
                        stop = std::chrono::high_resolution_clock::now();
                        auto hDuration = std::chrono::duration_cast<std::chrono::nanoseconds>(stop-start);
                        cout << "Retrieved flights from hash tables in " << hDuration.count() << " nanoseconds.\n";
                        

                        start = std::chrono::high_resolution_clock::now();
                        treeResults = trees[from].search(to);
                        stop = std::chrono::high_resolution_clock::now();
                        auto sDuration = std::chrono::duration_cast<std::chrono::nanoseconds>(stop-start);
                        cout << "Retrieved flights from splay trees in " << sDuration.count() << " nanoseconds.\n";

                        sAccess.setString("Accessed In: "  + to_string(sDuration.count()) + " ns\n" + "Flights: " + to_string(treeResults.size()));
                        hAccess.setString("Accessed In: "  + to_string(hDuration.count()) + " ns\n" + "Flights: " + to_string(hashResults.size()));

                    }

                }
                else if (prev.getGlobalBounds().contains(mouseXY))
                {
                    if(pos > 0)
                        pos--;

                }
                else if (next.getGlobalBounds().contains(mouseXY))
                {
                    if (pos < results.size()-1)
                    {
                        pos++;
                        // cout << results[pos] -> actDepart << endl << results[pos] -> arrTime << endl;
                    }
                }
                else if (test1.getGlobalBounds().contains(mouseXY) || test2.getGlobalBounds().contains(mouseXY) || test3.getGlobalBounds().contains(mouseXY))
                {

                    if(toSel)
                    {
                        toSel = false;
                        airports[to]->setTexture(TextureManager::GetTexture("nodeOrange"));
                        to = "";
                        results.clear();
                    }
                    if(fromSel)
                    {
                        fromSel = false;
                        airports[from]->setTexture(TextureManager::GetTexture("nodeOrange"));
                        from = "";
                        results.clear();
                    }

                    if (test1.getGlobalBounds().contains(mouseXY) || test2.getGlobalBounds().contains(mouseXY))
                    {
                        if (test1.getGlobalBounds().contains(mouseXY))
                        {
                            from = "MCO";
                            to = "ATL";
                        }
                        if (test2.getGlobalBounds().contains(mouseXY))
                        {
                            from = "LAS";
                            to = "JFK";
                        }

                        fromSel = true;
                        airports[from]->setTexture(TextureManager::GetTexture("nodeBlue"));
                        toSel = true;
                        airports[to]->setTexture(TextureManager::GetTexture("nodeBlue"));

                        for(auto i : data[from][to])
                            results.push_back(i);

                        start = std::chrono::high_resolution_clock::now();
                        hashResults = hashes[from].search(to);
                        stop = std::chrono::high_resolution_clock::now();
                        auto hDuration = std::chrono::duration_cast<std::chrono::nanoseconds>(stop-start);
                        cout << "Retrieved flights from hash tables in " << hDuration.count() << " nanoseconds.\n";
                        

                        start = std::chrono::high_resolution_clock::now();
                        treeResults = trees[from].search(to);
                        stop = std::chrono::high_resolution_clock::now();
                        auto sDuration = std::chrono::duration_cast<std::chrono::nanoseconds>(stop-start);
                        cout << "Retrieved flights from splay trees in " << sDuration.count() << " nanoseconds.\n";

                        sAccess.setString("Accessed In: "  + to_string(sDuration.count()) + " ns\n" + "Flights: " + to_string(treeResults.size()));
                        hAccess.setString("Accessed In: "  + to_string(hDuration.count()) + " ns\n" + "Flights: " + to_string(hashResults.size()));


                    }
                    else if (test3.getGlobalBounds().contains(mouseXY))
                    {
                        //hashes.clear();
                        //trees.clear();

                        start = std::chrono::high_resolution_clock::now();
                        for (pair<string, unordered_map<string, unordered_set<flightNode*>>> fromFlights : data) {
                            string from = fromFlights.first;
                            for (pair<string, unordered_set<flightNode*>> flightSet : fromFlights.second) {
                                trees[from].insert(flightSet.second);
                            }

                        }
                        stop = std::chrono::high_resolution_clock::now();
                        splayInsert = std::chrono::duration_cast<std::chrono::milliseconds>(stop-start);


                        start = std::chrono::high_resolution_clock::now();
                        for (pair<string, unordered_map<string, unordered_set<flightNode*>>> fromFlights : data) {
                            string from = fromFlights.first;
                            for (pair<string, unordered_set<flightNode*>> flightSet : fromFlights.second) {
                                hashes[from].insert(flightSet.second);
                            }
                        }
                        stop = std::chrono::high_resolution_clock::now();
                        hashInsert = std::chrono::duration_cast<std::chrono::milliseconds>(stop-start);

                        sInsert.setString("Inserted Data In: " + to_string(splayInsert.count()) + " ms");
                        hInsert.setString("Inserted Data In: " + to_string(hashInsert.count()) + " ms");


                    } 
                }

            }
            else if (event.type == event.MouseButtonReleased && event.mouseButton.button == sf::Mouse::Right)
            {
                sf::Vector2f mouseXY = window.mapPixelToCoords(sf::Mouse::getPosition(window));
                for (auto i : airports)
                    if (i.second->getGlobalBounds().contains(mouseXY))
                        cout << i.first << endl;
            }
        }

        window.clear(sf::Color(0,0,0));
        // window.clear(sf::Color(188,188,188));
        window.draw(mapUS);

        if (!fromSel)
            for (auto i : airports)
                window.draw(*i.second);
        else
        {
            if (!toSel)
            {
                window.draw(*airports[from]);
                for (auto i : airports)
                    if(data[from].find(i.first) != data[from].end())
                        window.draw(*i.second);
            }
            else
            {
                for (auto i : airports)
                    if(data[from].find(i.first) != data[from].end())
                        window.draw(*i.second);
                window.draw(*airports[from]);
                window.draw(*airports[to]);
            }
        }
        window.draw(totalFlights);
        window.draw(title1);
        window.draw(title2);
        window.draw(sInsert);
        window.draw(hInsert);

        window.draw(toText);
        window.draw(fromText);

        toPort.setString(to);
        window.draw(toPort);
        fromPort.setString(from);
        window.draw(fromPort);

        window.draw(sepRight);
        window.draw(sepBottom);
        window.draw(sepMiddle);
        window.draw(sepStructs);

        if(fromSel)
            fromDecode.setString(port[from]);
        else
            fromDecode.setString("");
        if(toSel)
            toDecode.setString(port[to]);
        else
            toDecode.setString("");

        window.draw(fromDecode);
        window.draw(toDecode);

        window.draw(search);
        window.draw(test1);
        window.draw(test2);
        window.draw(test3);

        if (results.size() != 0)
        {
            window.draw(info1);

            // string resDate = results[pos] -> date;
            // string resCarrier = carrier[results[pos] -> carrierAirline];
            string resDepart = standardTime(to_string(results[pos] -> actDepart));
            int del = results[pos] -> departDelay;
            string resDDelay = to_string(abs(del)) + " minute(s) " + (del >= 0 ? "late" : "early");
            string resArr = standardTime(to_string(results[pos] -> arrTime));
            int arr = results[pos] -> arrDelay;
            string resADelay = to_string(abs(arr)) + " minute(s) " + (arr >= 0 ? "late" : "early");;




            string curResult = "\n" + results[pos] -> date + 
                                "\n\n" +  carrier[results[pos] -> carrierAirline] +
                                "\n\n" + resDepart +
                                "\n\n" + resDDelay +
                                "\n\n" + resArr +
                                "\n\n" + resADelay;
            info2.setString(curResult);
            window.draw(info2);

            string posStr = to_string(pos + 1) + " out of " + to_string(results.size());
            posCount.setString(posStr);
            window.draw(posCount);
            window.draw(next);
            window.draw(prev);

            window.draw(sAccess);
            window.draw(hAccess);

        }



        window.display();

    }


    // Clear out any texture before program terminates
    TextureManager::Clear();
    return 0;
}