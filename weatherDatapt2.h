/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   WeatherData.h
 * Author: ambradjo
 *
 * Created on February 22, 2019, 4:39 PM
 */


#include <iostream>
#include <string.h>
#include <curl/curl.h>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/foreach.hpp>
#include <string>

#ifndef WEATHERDATA_H
#define WEATHERDATA_H

using namespace std;
using boost::property_tree::ptree;
using boost::property_tree::read_json;



/*WeatherData class is where we are handling all of the data retrieval from the web 
 * 
 * 
 * 
 */



class WeatherData {
public:
    
    
//This struct is responsible for writing data into the callback function, it can also read data in from it
    typedef struct MyCustomStruct {
    char *url = nullptr;
    unsigned int size = 0;
    char *response = nullptr;
} MyCustomStruct;

    string cityCond;
    string temperature;
    string weatherDescription;
    string cloudcover;
    string alertTit;
    string alertTim;
    string alertExp;
    string alertDescrip;
    string nearestStorm;
    string Precipitation;
 
    WeatherData();
    //weatherDataExecute initializes all of the libcurl handles which are responsible for going to the website and retrieving the raw data
    int weatherDataExecute(string city);
    
    //write_data is responsible to processing all of the data that was gotten by the libcurl handles
    static size_t write_data(void *buffer, size_t size, size_t nmemb, void *userp);
    WeatherData(const WeatherData& orig);
    //PrintWeatherInfo is responsible for storing the data into the program, this function is used to store 
    //data such as weather conditions, temperature, humidity, etc. into variables.
    void retrieveWeatherInfo(ptree &ptRoot);
    virtual ~WeatherData();
   // int WeatherData1();
    
private:

};

extern WeatherData * dataW;
#endif /* WEATHERDATA_H */

