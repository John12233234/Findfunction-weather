/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   WeatherData.cpp
 * Author: ambradjo
 * 
 * Created on February 22, 2019, 4:39 PM
 */

#include "WeatherData.h"





WeatherData::WeatherData(){
    
}

 
//write_data is responsible to processing all of the data that was gotten by the libcurl handles
size_t WeatherData::write_data(void* buffer, size_t size, size_t nmemb, void* userp){
    if (buffer && nmemb && userp) {
        MyCustomStruct *pMyStruct = (MyCustomStruct *)userp;

        // Writes to struct passed in from main
        if (pMyStruct->response == nullptr) {
            // Case when first time write_data() is invoked
            pMyStruct->response = new char[nmemb + 1];
            strncpy(pMyStruct->response, (char *)buffer, nmemb);
        }
        else {
            // Case when second or subsequent time write_data() is invoked
            char *oldResp = pMyStruct->response;

            pMyStruct->response = new char[pMyStruct->size + nmemb + 1];

            // Copy old data
            strncpy(pMyStruct->response, oldResp, pMyStruct->size);

            // Append new data
            strncpy(pMyStruct->response + pMyStruct->size, (char *)buffer, nmemb);

            delete []oldResp;
        }

        pMyStruct->size += nmemb;
        pMyStruct->response[pMyStruct->size] = '\n';
    }

    return nmemb;
    
    
    
    
    
    
}
//PrintWeatherInfo is responsible for storing the data into the program, this function is used to store 
    //data such as weather conditions, temperature, humidity, etc. into variables.
void WeatherData::PrintWeatherInfo(ptree &ptRoot){
    
    
    BOOST_FOREACH(ptree::value_type &featVal, ptRoot.get_child("daily")) {
        // "features" maps to a JSON array, so each child should have no name
        if ( !featVal.first.empty() )
            throw "\"features\" child node has a name";

        temperature = featVal.second.get<string>("data.summary");
        

        // Get GPS coordinates (stored as JSON array of 2 values)
        // Sanity checks: Only 2 values
       // ptree coordinates = featVal.second.get_child("geometry.coordinates");
      
    }

    
    
   // temperature = ptRoot.get<string>("daily.data.summary");
    cloudcover = ptRoot.get<string>("currently.cloudCover");
   // alertsTit = ptRoot.get<string>("alerts.Title");
    //alertsTit = ptRoot.get<string>("alerts.title");
    cout<<"Temperature is currently: "<<temperature<<endl<<"The cloud cover percentage is: "<<cloudcover;//<<endl<<alertsTit<<endl;
    return;
}

WeatherData::WeatherData(const WeatherData& orig) {
}



WeatherData::~WeatherData() {
}

//weatherDataExecute initializes all of the libcurl handles which are responsible for going to the website and retrieving the raw data
int WeatherData::weatherDataExecute(string city) {
    string cityN (city);
    CURLcode res = curl_global_init(CURL_GLOBAL_ALL);
    if (res != CURLE_OK) {
        cout << "ERROR: Unable to initialize libcurl" << endl;
        cout << curl_easy_strerror(res) << endl;
        //return 0;
    }

    CURL *curlHandle = curl_easy_init();
    if ( !curlHandle ) {
        cout << "ERROR: Unable to get easy handle" << endl;
        //return 0;
    } else {
        char errbuf[CURL_ERROR_SIZE] = {0};
        MyCustomStruct myStruct;
       // string targetCity = 
        string targetUrl = "https://api.darksky.net/forecast/cb1811f79399ef135226c8480b82f8a3/43.6532,-79.3839";
        //char targetURL[] = &targetUrl[0];
       // cout<<targetUrl<<endl;

        res = curl_easy_setopt(curlHandle, CURLOPT_URL, &targetUrl[0]);
       // res = curl_easy_setopt(curlHandle, CURLOPT_ACCEPT_ENCODING, "");
        if (res == CURLE_OK)
            res = curl_easy_setopt(curlHandle, CURLOPT_ERRORBUFFER, errbuf);
        if (res == CURLE_OK)
            res = curl_easy_setopt(curlHandle, CURLOPT_WRITEFUNCTION, write_data);
        if (res == CURLE_OK)
           res = curl_easy_setopt(curlHandle, CURLOPT_WRITEDATA, &myStruct);

        myStruct.url = &targetUrl[0];

        if (res != CURLE_OK) {
            cout << "ERROR: Unable to set libcurl option" << endl;
            cout << curl_easy_strerror(res) << endl;
        } else {
            res = curl_easy_perform(curlHandle);
        }
        

        cout << endl << endl;
        if (res == CURLE_OK) {
            // Create an empty proper tree
            ptree ptRoot;

            /* Store JSON data into a Property Tree
             *
             * read_json() expects the first parameter to be an istream object,
             * or derived from istream (e.g. ifstream, istringstream, etc.).
             * The second parameter is an empty property tree.
             *
             * If your JSON data is in C-string or C++ string object, you can
             * pass it to the constructor of an istringstream object.
             */
            //go through mystruct.response and find the last occurence of }
            
            string data(myStruct.response);
            int lastpos = data.find_last_of("}");
            for(lastpos++;lastpos<data.size();lastpos++){
                data.erase(lastpos);
            }
            cout<<data;
            istringstream issJsonData(data);
            read_json(issJsonData, ptRoot);

            // Parsing and printing the data
            cout << "Current weather is as follows:" << endl;
            cout << "====================" << endl << endl;
            try {
                PrintWeatherInfo(ptRoot);
            } catch (const char *errMsg) {
                cout << "ERROR: Unable to fully parse the openweathermap JSON data" << endl;
                cout << "Thrown message: " << errMsg << endl;
            }
            cout << endl << "====================" << endl;
            cout << "Done!" << endl;
        } else {
            cout << "ERROR: res == " << res << endl;
            cout << errbuf << endl;
        }

        if (myStruct.response)
            delete []myStruct.response;

        curl_easy_cleanup(curlHandle);
        curlHandle = nullptr;
    }

    curl_global_cleanup();

    //return 0;
    
    
    
}
