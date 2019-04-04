#include <Arduino.h>
#include <string>
#include <iostream>
#include "website.hpp"

std::string renderWebsite(std::vector<int> data1, std::vector<int> data2) {
    std::string website = "<html><head><title>Statistics</title><meta charset=\"UTF-8\"><script src=\"https://cdnjs.cloudflare.com/ajax/libs/Chart.js/2.5.0/Chart.min.js\"></script></head><body><h1>Number of devices:</h1><div height=\"75%\" width=\"75%\"><canvas id=\"statisticChart\"></canvas><script>var ctx = document.getElementById('statisticChart').getContext('2d');var myChart = new Chart(ctx, {type: 'line', fill: false, data: {labels: ['-30', '-25', '-20', '-15', '-10', '-5', '0'],datasets: [{label: 'A1', fill: false,data: [firstVar1, secondVar1, thirdVar1, fourthVar1, fifthVar1, sixthVar1, seventhVar1],backgroundColor: ['rgba(255, 255, 0, 0)'],borderColor: ['rgba(255, 0, 255, 1)'],borderWidth: 2}, {label: 'A2',fill: false,data: [firstVar2, secondVar2, thirdVar2, fourthVar2, fifthVar2, sixthVar2, seventhVar2],backgroundColor: ['rgba(255, 0, 0, 0)'],borderColor: ['rgba(255, 0, 0, 1)'],borderWidth: 2}]},options: {scales: {yAxes: [{ticks: {beginAtZero: true}}]}}});</script></div></body></html>";
    /*std::ifstream t("website.html");
    std::stringstream buffer;
    buffer << t.rdbuf();*/
    // (*values)[0], (*values)[1], (*values)[2], (*values)[3], (*values)[4], (*values)[5], (*values)[6]
    website.replace(website.find("firstVar1"), 9, String(data1[0]).c_str());
    website.replace(website.find("secondVar1"), 10, String(data1[1]).c_str());
    website.replace(website.find("thirdVar1"), 9, String(data1[2]).c_str());
    website.replace(website.find("fourthVar1"), 10, String(data1[3]).c_str());
    website.replace(website.find("fifthVar1"), 9, String(data1[4]).c_str());
    website.replace(website.find("sixthVar1"), 9, String(data1[5]).c_str());
    website.replace(website.find("seventhVar1"), 11, String(data1[6]).c_str());

    website.replace(website.find("firstVar2"), 9, String(data2[0]).c_str());
    website.replace(website.find("secondVar2"), 10, String(data2[1]).c_str());
    website.replace(website.find("thirdVar2"), 9, String(data2[2]).c_str());
    website.replace(website.find("fourthVar2"), 10, String(data2[3]).c_str());
    website.replace(website.find("fifthVar2"), 9, String(data2[4]).c_str());
    website.replace(website.find("sixthVar2"), 9, String(data2[5]).c_str());
    website.replace(website.find("seventhVar2"), 11, String(data2[6]).c_str());
    return website;
}