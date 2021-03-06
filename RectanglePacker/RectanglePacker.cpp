#include <fstream>
#include <iostream>
#include <sstream>
#include <time.h>
#include <random>

#include "../json11/json11.hpp"

#include "../Solvers/Solvers.h"
#include "../Solvers/MaxRectSolver.h"
#include "../Solvers/SkylineSolver.h"
#include "../Solvers/ShelfSolver.h"
#include "RectanglePacker.h"

namespace rectpack {

RectanglePacker::RectanglePacker(Rectangle _bin, int _mask, float _maxTime, int _seed, unsigned long long (*_evaluator)(const Rectangle &))
    : shapes(), bin(_bin), mask(_mask), maxTime(_maxTime), seed(_seed), evaluator(_evaluator), packed() {}
RectanglePacker::RectanglePacker(const RectanglePacker &other)
    : shapes(other.shapes), bin(other.bin), mask(other.mask), maxTime(other.maxTime), seed(other.seed), evaluator(other.evaluator), packed(other.packed) {}

RectanglePacker::~RectanglePacker() {}

//Inputs settings and different shapes from istream in json format
void RectanglePacker::inputFromJSON(std::istream &in) {
    std::stringstream inStream;
    inStream << in.rdbuf();
    std::string inputString = inStream.str();

    std::string err;
    auto json = json11::Json::parse(inputString, err);

    if(err != "") {
        std::cout << "There was an error : " << err << std::endl;
        exit(0);
    }

    this->bin = Rectangle(json["Bin"]["W"].number_value(), json["Bin"]["H"].number_value());
    this->mask = json["Settings"]["Mask"].int_value();
    this->maxTime = json["Settings"]["MaxTime"].number_value();
    this->seed = json["Settings"]["Seed"].int_value();

    std::string evaluatorName = json["Settings"]["Evaluator"].string_value();
    if(evaluatorName == "Area") {
        this->evaluator = rectangleEvaluators::areaEvaluator;
    } else if(evaluatorName == "Perimeter") {
        this->evaluator = rectangleEvaluators::perimeterEvaluator;
    } else if(evaluatorName == "Width") {
        this->evaluator = rectangleEvaluators::widthEvaluator;
    } else if(evaluatorName == "Height") {
        this->evaluator = rectangleEvaluators::heightEvaluator;
    } else {
        std::cout << "No evaluator name specified. Using areaEvaluator instead." << std::endl;
        this->evaluator = rectangleEvaluators::areaEvaluator;
    }

    int ind = 0;
    for(const auto &shp : json["Shapes"].array_items()) {
        cordType w = shp["W"].number_value(), h = shp["H"].number_value(); int count = shp["Count"].int_value();
        ind ++;
        for(int i = 0; i < count; i ++) {
            this->shapes.push_back(Rectangle(w, h, ind));
        }
    }
}

//Main function which calculates the best packing
void RectanglePacker::execute() {
    srand(this->seed);
    if(this->mask & 1) {
        std::cout << "Started skyline solver " << std::endl;
        SkylineSolver solver(this->bin);
        solver.solve(this->shapes, this->maxTime, this->evaluator);
        this->packed.compareAndSwap(solver.packed);
    }
    if(this->mask & 2) {
        std::cout << "Started maxrect solver " << std::endl;
        MaxRectSolver solver(this->bin);
        solver.solve(this->shapes, this->maxTime, this->evaluator);
        this->packed.compareAndSwap(solver.packed);
    }
    if(this->mask & 4) {
        std::cout << "Started shelf solver " << std::endl;
        ShelfSolver solver(this->bin);
        solver.solve(this->shapes, this->maxTime, this->evaluator);
        this->packed.compareAndSwap(solver.packed);
    }
}

//Prints answer to ostream in svg format
void RectanglePacker::outputToSvg(std::ostream &out) {
    out << "<svg xmlns=\"http://www.w3.org/2000/svg\" xmlns:xlink=\"http://www.w3.org/1999/xlink\" width='2000' height='2000' viewBox=\"0 0 2000 2000\">" << std::endl;

    this->bin.printToSvg(out);

    this->packed.printToSvg(out);

    out << "\t<text x = \"" << 2 << "\" y = \"" << this->bin.height + 10 << "\" fill = \"red\" style=\"font-size: 4pt;\">" << this->packed.score << "from " << this->bin.getArea() << "</text>" << std::endl;
    out << "</svg>" << std::endl;
}

};
