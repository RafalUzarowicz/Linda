#include "TupleSpace.h"

void Linda::create(bool no_exist_err, const std::string& path, const std::string& name) {
    const std::filesystem::path tuplespace_path{path + "/" + name};

    if(std::filesystem::exists(tuplespace_path) ){
        if(no_exist_err){
            return;
        }
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
        if(std::filesystem::exists(path + "/.tuplespace")){
            State & state = State::getInstance();
            state.tupleSpacePath = path;
            //State::connected = true;
        }else{
            throw TupleSpaceConnectException(" \"" + path + "\" not a tuplespace.");
        }
    }else{
        throw TupleSpaceConnectException(" \"" + path + "\" does not exist.");
    }
}

void Linda::output(Linda::Tuple tuple) {
//    if(!State::connected){
//        throw TupleSpaceConnectException("Not connected to any tuplespace.");
//    }
    State& state = State::getInstance();
    //todo find file, lock file, put tuple, notify processe

    //find file
    //fixme can this cause process to hang?
    std::string filePath = state.tupleSpacePath + "/" + tuple.path() + ".linda";
    int flags = O_CREAT | O_RDWR;
    errno = 0;
    int fd = open(filePath.c_str(), flags, 0666);
    if(fd < 0){
        std::cerr<<strerror(errno)<<std::endl;
        throw TupleSpaceException("Could not open tuple file");
    }
    //prep readTuple buffer
    char read_buffer[Linda::ENTRY_SIZE];
    memset(&read_buffer, 0, sizeof (read_buffer));

    //lock file
    struct flock lock;
    memset(&lock, 0, sizeof (lock));
    lock.l_type = F_WRLCK | F_RDLCK;
    lock.l_whence = SEEK_SET;
    lock.l_start = 0;
    lock.l_len = 0; //lock whole file

    if(fcntl(fd, F_SETLKW, &lock) == -1){
        close(fd);
        std::cerr<<strerror(errno)<<std::endl;
        throw TupleSpaceException("Could not acquire file lock");
    }

    //do stuff - find empty record & write
    //todo add max file size check
    ssize_t bytes_read;
    bytes_read = read(fd, read_buffer, Linda::ENTRY_SIZE);
    while(bytes_read == Linda::ENTRY_SIZE){
        if((read_buffer[0] & EMPTY_FLAG )== EMPTY_FLAG){
            break;
        }
        bytes_read = read(fd, read_buffer, Linda::ENTRY_SIZE);
    }

    //todo move padding to tuple class logic
    std::string tuple_str = tuple.to_string();
    char entry[Linda::ENTRY_SIZE];
    memset(entry, int('-'), sizeof(entry));
    entry[0] = Linda::BUSY_FLAG;
    memcpy(entry + 1, tuple_str.c_str(), std::min(Linda::MAX_TUPLE_SIZE, tuple_str.size()));
    entry[Linda::ENTRY_SIZE -1] = '\n';


    if(write(fd, entry, Linda::ENTRY_SIZE) == -1){
        lock.l_type = F_UNLCK;
        if(fcntl(fd, F_SETLKW, &lock) == -1){
            close(fd);
            std::cerr<<strerror(errno)<<std::endl;
            throw TupleSpaceException("Error releasing file lock");
        }
        std::cerr<<strerror(errno)<<std::endl;
        throw TupleSpaceException("Couldn't write tuple to file");
    }

    //unlock
    lock.l_type = F_UNLCK;
    if(fcntl(fd, F_SETLKW, &lock) == -1){
        close(fd);
        throw TupleSpaceException("Error releasing file lock");
    }


    close(fd);
    // TODO: implement notify processes
}

Linda::Tuple Linda::input(Linda::Pattern pattern, std::chrono::milliseconds timeout) {
//    std::chrono::time_point<std::chrono::system_clock> begin_t = std::chrono::system_clock::now();
//    if(!State::connected){
//        throw TupleSpaceConnectException("Not connected to any tuplespace.");
//    }
    State& state = State::getInstance();



    // TODO: implement
    return Linda::Tuple();
}

//don't change back to read cause it clashes with unistd.h read
Linda::Tuple Linda::readTuple(Linda::Pattern pattern, std::chrono::milliseconds timeout) {
//    if(!State::connected){
//        throw TupleSpaceConnectException("Not connected to any tuplespace.");
//    }
//    State& state = State::getInstance();
    // TODO: implement
    return Linda::Tuple();
}
