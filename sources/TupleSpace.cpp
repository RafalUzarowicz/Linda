#include "TupleSpace.h"

void Linda::create(bool no_exist_err, const std::string& path, const std::string& name) {
    const std::filesystem::path tuplespace_path{path + "/" + name};

    if(std::filesystem::exists(tuplespace_path) ){
        if(no_exist_err){
            return;
        }
        throw Linda::Exception::TupleSpaceCreateException("\"" + name + "\" already exists in " + path);
    }else{
        try{
            std::filesystem::create_directories(tuplespace_path);
        }catch (std::filesystem::filesystem_error & ex){
            std::string msg = "Filesystem error. ";
            msg.append(ex.what());
            throw Linda::Exception::TupleSpaceCreateException(msg);
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
            state.connected = true;
        }else{
            throw Linda::Exception::TupleSpaceConnectException(" \"" + path + "\" not a tuplespace.");
        }
    }else{
        throw Linda::Exception::TupleSpaceConnectException(" \"" + path + "\" does not exist.");
    }
}

void Linda::output(Linda::Tuple tuple) {
    if(!State::getInstance().connected){
        throw Linda::Exception::TupleSpaceConnectException("Not connected to any tuplespace.");
    }
    State& state = State::getInstance();

    //find file
    //fixme can this cause process to hang?
    std::string filePath = state.tupleSpacePath + "/" + tuple.path() + ".linda";
    int flags = O_CREAT | O_RDWR;
    int fd = open(filePath.c_str(), flags, 0666);
    if(fd < 0){
        std::cerr<<strerror(errno)<<std::endl;
        throw Linda::Exception::TupleSpaceException("Could not open tuple file");
    }
    //prep readTuple buffer
    unsigned char read_buffer[Linda::ENTRY_SIZE];
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
        throw Linda::Exception::TupleSpaceException("Could not acquire file lock");
    }

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
    unsigned char entry[Linda::ENTRY_SIZE];
    memset(entry, int('-'), sizeof(entry));
    entry[0] = Linda::BUSY_FLAG;
    memcpy(entry + 1, tuple_str.c_str(), std::min(Linda::MAX_TUPLE_SIZE, tuple_str.size()));
    entry[Linda::ENTRY_SIZE -1] = '\n';


    if(write(fd, entry, Linda::ENTRY_SIZE) == -1){
        lock.l_type = F_UNLCK;
        if(fcntl(fd, F_SETLKW, &lock) == -1){
            close(fd);
            std::cerr<<strerror(errno)<<std::endl;
            throw Linda::Exception::TupleSpaceException("Error releasing file lock");
        }
        std::cerr<<strerror(errno)<<std::endl;
        throw Linda::Exception::TupleSpaceException("Couldn't write tuple to file");
    }

    //unlock
    lock.l_type = F_UNLCK;
    if(fcntl(fd, F_SETLKW, &lock) == -1){
        close(fd);
        throw Linda::Exception::TupleSpaceException("Error releasing file lock");
    }
    close(fd);


    //todo find waiting processes and send signals
}

Linda::Tuple Linda::input(Linda::Pattern pattern, std::chrono::milliseconds timeout) {
    std::chrono::time_point<std::chrono::system_clock> begin_t = std::chrono::system_clock::now();
    if(!State::getInstance().connected){
        throw Linda::Exception::TupleSpaceConnectException("Not connected to any tuplespace.");
    }



    return Linda::Tuple();
}

//don't change back to read cause it clashes with unistd.h read
Linda::Tuple Linda::readTuple(Linda::Pattern pattern, std::chrono::milliseconds timeout) {
    std::chrono::time_point<std::chrono::system_clock> begin_t = std::chrono::system_clock::now();
    if(!State::getInstance().connected){
        throw Linda::Exception::TupleSpaceConnectException("Not connected to any tuplespace.");
    }
    State& state = State::getInstance();
    // TODO: implement - read and delete
    const std::vector<std::string > & filePaths = pattern.all_paths();
    for(auto & path : filePaths){
        std::string filePath = state.tupleSpacePath + "/" + path + ".linda";
        int flags = O_CREAT | O_RDWR;
        int fd = open(filePath.c_str(), flags, 0666);
        if(fd < 0){
            std::cerr<<strerror(errno)<<std::endl;
            throw Linda::Exception::TupleSpaceException("Could not open tuple file");
        }

        //todo add file locking -> at least a read lock started and then released with each read iteration
        char read_buffer[Linda::ENTRY_SIZE];
        memset(&read_buffer, 0, sizeof (read_buffer));
        while(read(fd, read_buffer, Linda::ENTRY_SIZE) > 0){
            if(read_buffer[0] == Linda::BUSY_FLAG){
                std::vector<ISerializable::serialization_type> tuple_vec;
                int i = 1;
                //fixme handle unsigned chars -> can't really read them from file or do proper substring comparison
                while( read_buffer[i]!= Tuple::SerializationCodes::END && i < ENTRY_SIZE){
                    tuple_vec.push_back((unsigned char)i);
                    i++;
                }
                tuple_vec.push_back(Tuple::SerializationCodes::END);
                Linda::Tuple t;
                t.deserialize(tuple_vec);
                if(pattern.check(t)){
                    //todo release locks here (when they get implemented)
                    close(fd);
                    return t;
                }
            }
        }

        //todo implement queuing process in every loop
        pid_t this_pid = getpid();
        std::string process_entry = pattern.to_string();
        auto pattern_entry_size = process_entry.size() + 2;
        auto* buffer = static_cast<unsigned char *>(malloc(sizeof(unsigned char) * (pattern_entry_size)));
        buffer[0] = (unsigned char) this_pid;
        memcpy(buffer +1, process_entry.c_str(), process_entry.size());
        buffer[pattern_entry_size-1] = '\n';

        std::string queue = state.tupleSpacePath + "/" + path + "-queue.linda";
        int queue_fd = open(queue.c_str(), O_CREAT | O_APPEND, 0666);
        write(queue_fd, buffer, pattern_entry_size);
        close(queue_fd);
        free(buffer);

    }

    //todo sleep waiting for a signal

    return Linda::Tuple();
}
