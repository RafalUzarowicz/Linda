#include "TupleSpace.h"

void Linda::sighandler(int signum, siginfo_t *info, void *ptr) {
    int encodedValue = info->si_int, depth, index;
    Linda::Signal::decode(encodedValue, depth, index);

    State& state = State::getInstance();
    state.depth = depth;
    state.index = index;
    std::cout << "Received depth: " << depth << ", index: " << index << std::endl;
}

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
    auto serialized = tuple.serialize();
    //todo guard against too long tuples
    unsigned char entry[Linda::ENTRY_SIZE];
    memset(entry, int('-'), sizeof(entry));
    entry[0] = Linda::BUSY_FLAG;
    for (unsigned long i = 0; i< serialized.size(); i++){
        entry[i+1] = serialized[i];
    }
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

// TODO: change it to read
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
                unsigned char c = static_cast<unsigned char>(read_buffer[i]);
                while( c != Tuple::SerializationCodes::END && i < ENTRY_SIZE-1){
                    c = static_cast<unsigned char>(read_buffer[i]);
                    tuple_vec.push_back(c);
                    i++;
                }
                Linda::Tuple t;
                t.deserialize(tuple_vec);
                if(pattern.check(t)){
                    //todo release locks here (when they get implemented)
                    close(fd);
                    return t;
                }
            }
        }

        //queue process
        pid_t this_pid = getpid();
        std::cout<<sizeof (pid_t)<<std::endl;
        auto serialized_pattern = pattern.serialize();
        auto pattern_entry_size = serialized_pattern.size() + 1;
        auto* buffer = static_cast<unsigned char *>(malloc(sizeof(unsigned char) * (pattern_entry_size) + sizeof (pid_t)));
        buffer[0] = (unsigned char) this_pid;

        memcpy(buffer, &this_pid, sizeof (pid_t));
        for(unsigned long i = 0; i < pattern_entry_size; i++){
            buffer[i+1] = serialized_pattern[i];
        }
        buffer[pattern_entry_size-1 + sizeof (pid_t)] = '\n';

        std::string queue = state.tupleSpacePath + "/" + path + "-queue.linda";
        int queue_fd = open(queue.c_str(), O_CREAT | O_APPEND | O_WRONLY, 0666);

        struct flock lock;
        memset(&lock, 0, sizeof (lock));
        lock.l_type = F_WRLCK | F_RDLCK;
        lock.l_whence = SEEK_SET;
        lock.l_start = 0;
        lock.l_len = 0; //lock whole file

        if(queue_fd < 0){
            free(buffer);
            std::cout<<strerror(errno)<<std::endl;
            throw Linda::Exception::TupleSpaceException("Exception while opening queue file.");
        }
        if(fcntl(fd, F_SETLKW, &lock) == -1){
            close(fd);
            std::cerr<<strerror(errno)<<std::endl;
            throw Linda::Exception::TupleSpaceException("Could not acquire queue file lock");
        }
        if(write(queue_fd, buffer, pattern_entry_size)< 0){
            free(buffer);
            lock.l_type = F_UNLCK;
            if(fcntl(fd, F_SETLKW, &lock) == -1){
                close(fd);
                throw Linda::Exception::TupleSpaceException("Error releasing file lock");
            }
            std::cout<<strerror(errno)<<std::endl;
            throw Linda::Exception::TupleSpaceException("Exception while queuing the process.");
        }
        lock.l_type = F_UNLCK;
        if(fcntl(fd, F_SETLKW, &lock) == -1){
            close(fd);
            throw Linda::Exception::TupleSpaceException("Error releasing file lock");
        }
        close(queue_fd);
        free(buffer);
    }

    //todo sleep waiting for a signal

    return Linda::Tuple();
}
