#include <fstream>
#include "TupleSpace.h"
#include "TupleSpaceCreateException.h"
#include "TupleSpaceConnectionException.h"

void Linda::create(const std::string& path, const std::string& name) {
    const std::filesystem::path tuplespace_path{path + "/" + name};

    if(std::filesystem::exists(tuplespace_path) ){
        throw TupleSpaceCreateException("\"" + name + "\" already exists in " + path);
    }else{
        try{
            std::filesystem::create_directories(tuplespace_path);
        }catch (std::filesystem::filesystem_error & ex){
            std::string msg = "Filesystem error. ";
            msg.append(ex.what());
            throw TupleSpaceCreateException(msg);
        }
    }
    //todo should create folder structure?
    std::ofstream file(tuplespace_path.string() + "/.tuplespace");
    file.close();
}

void Linda::connect(const std::string& path) {
    if(std::filesystem::exists(path)){
        if(std::filesystem::exists(path + "./tuplespace")){
            State & state = State::getInstance();
            state.tupleSpacePath = path;
            State::connected = true;
        }else{
            throw TupleSpaceConnectException(" \"" + path + "\" not a tuplespace.");
        }
    }else{
        throw TupleSpaceConnectException(" \"" + path + "\" does not exist.");
    }
}

void Linda::output(Linda::Tuple tuple) {
    if(!State::connected){
        throw TupleSpaceConnectException("Not connected to any tuplespace.");
    }
    State& state = State::getInstance();
    //todo find file, lock file, put tuple, notify processes
    std::string filePath = state.tupleSpacePath + "/" + tuple.path();

    //fixme can this cause process to hang?
    int flags = O_CREAT | O_RDWR;
    int fd = open(filePath.c_str(), O_CREAT);
    if(fd < 0){
        throw TupleSpaceException("Could not open tuple file");
    }



    // TODO: implement
}

Linda::Tuple Linda::input(Linda::Pattern pattern, std::chrono::milliseconds timeout) {
    std::chrono::time_point<std::chrono::system_clock> begin_t = std::chrono::system_clock::now();
    if(!State::connected){
        throw TupleSpaceConnectException("Not connected to any tuplespace.");
    }
    State& state = State::getInstance();



    // TODO: implement
    return Linda::Tuple();
}

Linda::Tuple Linda::read(Linda::Pattern pattern, std::chrono::milliseconds timeout) {
    if(!State::connected){
        throw TupleSpaceConnectException("Not connected to any tuplespace.");
    }
    State& state = State::getInstance();
    // TODO: implement
    return Linda::Tuple();
}
