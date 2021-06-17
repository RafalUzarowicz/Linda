//
// Created by Joanna Sokołowska on 8.06.2021.
//

#include "CLI.h"

void Linda::CLI::run(){
    std::cout<<"$ ";
    while(std::getline(std::cin, line) && line != "quit"){
        if(line == "help"){
            printHelp();
        }else if(line == "reset"){
            reset();
        }else if(line == "show"){
            show();
        }else{
            tokenize();
            try{
                if(!handleCreate()){
                    if(!handleConnect()){
                        if(!handleTuple()){
                            if(!handlePattern()){
                                if(!handleOperations()){
                                    std::cout<<"Unrecognized command: \""<<line_cpy<<"\""<<std::endl;
                                }
                            }
                        }
                    }
                }
            }catch (const Exception::TupleSpaceException& ex){
                std::cout<<"Intercepted exception: "<<ex.what()<<std::endl;
            }catch (const Exception::Tuple::BaseException& ex){
                std::cout<<"Intercepted exception: "<<ex.what()<<std::endl;
            }catch (const Exception::Pattern::BaseException& ex){
                std::cout<<"Intercepted exception: "<<ex.what()<<std::endl;
            }catch (const Exception::Signal::BaseException& ex){
                std::cout<<"Intercepted exception: "<<ex.what()<<std::endl;
            }
        }
        tokens.clear();
        std::cout<<"$ ";
    }
}

bool Linda::CLI::handleCreate(){
    if(tokens[0] != "create") return false;
    if(tokens.size() == 1){
        Linda::create();
        std::cout<<"Created"<<std::endl;
    }else if(tokens.size() == 3){
        Linda::create(tokens[1], tokens[2]);
        std::cout<<"Created"<<std::endl;
    }else{
        std::cout<<"Create requires exactly two arguments or none at all"<<std::endl;
    }
    return true;
}

bool Linda::CLI::handleConnect(){
    if(tokens[0]!= "connect") return false;
    if(tokens.size() == 1){
        Linda::connect();
        std::cout<<"Connected"<<std::endl;
    }else if(tokens.size() == 2){
        Linda::connect(tokens[1]);
        std::cout<<"Connected"<<std::endl;
    }else{
        std::cout<<"Connect requires one or none arguments"<<std::endl;
    }
    return true;
}

bool Linda::CLI::handleTuple(){
    if(tokens[0]!= "tuple") return false;
    if(tokens.size() != 2){
        std::cout<<"tuple operation requires exactly one argument"<<std::endl;
        return true;
    }
    if (tokens[1].find('.')!= std::string::npos){
        try{
            float f = std::stof(tokens[1]);
            t.push(f);
            return true;
        }catch (const std::invalid_argument& argument){}
    }
    try{
        int i = std::stoi(tokens[1]);
        t.push(i);
        return true;
    }catch (const std::invalid_argument& argument){}
    t.push(tokens[1]);
    return true;
}

bool Linda::CLI::handlePattern(){
    if(tokens[0]!= "pattern") return false;
    if(tokens.size()!=3){
        std::cout<<"Pattern operation requires two arguments"<<std::endl;
        return true;
    }
    PatternEntryType type;
    try{
        type = parsePatternEntry();
    }catch (const std::runtime_error& err){
        std::cout<<"Expected one of: ==, =<, >=, <, >, any. Got: "<<tokens[1]<<std::endl;
        return true;
    }
    if(type == PatternEntryType::Any){
        if(tokens[2] == "string"){
            p.add(type, TupleEntryType::String);
        }else  if(tokens[2] == "int"){
            p.add(type, TupleEntryType::Int);
        }else if(tokens[2] == "float"){
            p.add(type, TupleEntryType::Float);
        }else{
            std::cout<<"When pattern is any second argument must be \"float\", \"int\" or \"string\""<<std::endl;
        }

        return true;
    }
    if (tokens[2].find('.')!= std::string::npos){
        try{
            float f = std::stof(tokens[2]);
            p.add(type, f);
            return true;
        }catch (const std::invalid_argument& argument){}
    }
    try{
        int i = std::stoi(tokens[2]);
        p.add(type, i);
        return true;
    }catch (const std::invalid_argument& argument){}
    p.add(type, tokens[2]);
    return true;
}

bool Linda::CLI::handleOperations(){
    if(tokens[0] == "input"){
        if(tokens.size() == 2){
            std::chrono::microseconds timeout = std::chrono::microseconds(atoi(tokens[1].c_str()));
            auto a = Linda::input(p, timeout);
            std::cout<<"[out]: "<<a<<std::endl;
        }else if(tokens.size() == 1){
            auto a = Linda::input(p);
            std::cout<<"[out]: "<<a<<std::endl;
        }else{
            std::cout<<"Input requires one or two arguments"<<std::endl;
        }
        return true;
    }else if(tokens[0] == "read"){
        if(tokens.size() == 2){
            std::chrono::microseconds timeout = std::chrono::microseconds(atoi(tokens[1].c_str()));
            auto a = Linda::read(p, timeout);
            std::cout<<"[out]: "<<a<<std::endl;
        }else if(tokens.size() == 1){
            auto a = Linda::read(p);
            std::cout<<"[out]: "<<a<<std::endl;
        }else{
            std::cout<<"Read requires one or two arguments"<<std::endl;
        }
        return true;
    }else if(tokens[0] == "output"){
        if(tokens.size() != 1){
            std::cout<<"Output requires no arguments"<<std::endl;
        }else{
            Linda::output(t);
        }
        return true;
    }
    return false;
}

void Linda::CLI::printHelp(){
    std::cout<<"Available commands: "<<std::endl;
    std::cout<<"\t reset - clear tuple and pattern"<<std::endl;
    std::cout<<"\t help - print this help message"<<std::endl;
    std::cout<<"\t show - print current tuple and pattern content"<<std::endl;
    std::cout<<"\t connect <path> - connect to tuplespace"<<std::endl;
    std::cout<<"\t create <path> - create tuplespace"<<std::endl;
    std::cout<<"\t tuple <tuple-element> - add element to tuple"<<std::endl;
    std::cout<<"\t pattern <type> <value> - add element to pattern, type should be one of ==, <, >, <=, >=, any. "
               "Value should be float, integer or string."<<std::endl;
    std::cout<<"\t input [timeout] - read and delete tuple as specified by pattern"<<std::endl;
    std::cout<<"\t read [timeout] - read tuple as specified by pattern"<<std::endl;
    std::cout<<"\t output - put tuple into tuplespace"<<std::endl;
    std::cout<<"\t quit"<<std::endl;
}

void Linda::CLI::reset(){
    t.clear();
    p.clear();
}

void Linda::CLI::tokenize() {
    line_cpy = std::string(line);
    size_t pos;
    while((pos = line.find(' ')) != std::string::npos){
        tokens.push_back(line.substr(0, pos));
        line.erase(0, pos+1);
    }
    tokens.push_back(line);
}

void Linda::CLI::show(){
    std::cout<<"Pattern: "<<p<<std::endl;
    std::cout<<"Tuple: "<<t<<std::endl;
}

Linda::PatternEntryType Linda::CLI::parsePatternEntry(){
    if(tokens[1] == "=="){
        return PatternEntryType::Equal;
    }else if(tokens[1] == "=<"){
        return PatternEntryType::LessOrEqual;
    }else if(tokens[1] == ">="){
        return PatternEntryType::GreaterOrEqual;
    }else if(tokens[1] == "any"){
        return PatternEntryType::Any;
    }else if(tokens[1] == ">"){
        return PatternEntryType::Greater;
    }else if(tokens[1] == "<"){
        return PatternEntryType::Less;
    }else{
        throw std::runtime_error("Wrong pattern type");
    }
}