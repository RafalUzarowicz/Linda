//
// Created by Joanna Sokołowska on 8.06.2021.
//

#include "CLI.h"

void Linda::CLI::run(){
    std::cout<<"$ ";
    while(std::getline(std::cin, line) && line != "quit"){
        if(line == "help"){
            print_help();
        }else if(line == "reset"){
            reset();
        }else{
            tokenize();
            if(tokens.empty()) {
                tokens.push_back(line);
            }
            try{
                if(!handle_create()){
                    if(!handle_connect()){
                        if(!handle_tuple()){
                            if(!handle_pattern()){
                                if(!handle_operations()){
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
        std::cout<<"$ ";
    }
}

bool Linda::CLI::handle_create(){
    if(tokens[0] != "create") return false;
    if(tokens.size() == 1){
        Linda::create();
        std::cout<<"Created"<<std::endl;
    }else if(tokens.size() == 3){
        Linda::create(tokens[1].c_str(), tokens[2]);
        std::cout<<"Created"<<std::endl;
    }else{
        std::cout<<"Create requires exactly two arguments or none at all"<<std::endl;
    }
    return true;
}

bool Linda::CLI::handle_connect(){
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

bool Linda::CLI::handle_tuple(){
    //todo
    // - check if string is float or int if not assume a string argument, add it to this.t
    if(tokens[0]!= "tuple") return false;
    if(tokens.size() != 2){
        std::cout<<"tuple operation requires exactly one argument"<<std::endl;
        return true;
    }
    return true;
}

bool Linda::CLI::handle_pattern(){
    //todo
    // - check if token[1] contains a valid pattern type
    // - parse token[2] value
    // - add to pattern
    if(tokens[0]!= "pattern") return false;


    return true;
}

bool Linda::CLI::handle_operations(){
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

void Linda::CLI::print_help(){
    std::cout<<"Available commands: "<<std::endl;
    std::cout<<"\t reset - clear tuple and pattern"<<std::endl;
    std::cout<<"\t help - print this help message"<<std::endl;
    std::cout<<"\t connect <path> - connect to tuplespace"<<std::endl;
    std::cout<<"\t create <path> - create tuplespace"<<std::endl;
    std::cout<<"\t tuple <tuple-element> - add element to tuple"<<std::endl;
    std::cout<<"\t pattern <type> <value> - add element to pattern"<<std::endl;
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
}