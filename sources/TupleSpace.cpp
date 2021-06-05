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
        }else{
            throw TupleSpaceConnectException(" \"" + path + "\" not a tuplespace.");
        }
    }else{
        throw TupleSpaceConnectException(" \"" + path + "\" does not exist.");
    }
}

void Linda::output(Linda::Tuple tuple) {
    // TODO: implement
}

Linda::Tuple Linda::input(Linda::Pattern pattern, std::chrono::milliseconds timeout) {
    // TODO: implement
    return Linda::Tuple();
}

Linda::Tuple Linda::read(Linda::Pattern pattern, std::chrono::milliseconds timeout) {
    // TODO: implement
    return Linda::Tuple();
}
