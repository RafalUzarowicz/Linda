#include "TupleSpace.h"

#include "Exceptions.h"
#include "Signals.h"

static Linda::Tuple waitForIt(const Linda::Pattern&, char, std::chrono::microseconds);

static void signalHandler(int, siginfo_t*, void*);

class State {
public:
    State(State&) = delete;

    void operator=(const State&) = delete;

private:
    State() : tupleSpacePath(Linda::DEFAULT_TUPLESPACE_DIR + "/" + Linda::DEFAULT_TUPLESPACE_NAME) {
        connected = false;
        depth = 0;
        index = 0;
    }

    static State& getInstance() {
        static State instance;
        return instance;
    }

    //tuplespace  = a dir named tuplespace in specified path, the dir contains file .tuplespace
    std::string tupleSpacePath{};

    bool connected;
    int depth;
    int index;

    friend void Linda::create(bool, const std::string&, const std::string&);

    friend void Linda::connect(const std::string&);

    friend void Linda::output(Linda::Tuple);

    friend Linda::Tuple Linda::input(Linda::Pattern, std::chrono::microseconds);

    friend Linda::Tuple Linda::read(Linda::Pattern, std::chrono::microseconds);

    friend void signalHandler(int, siginfo_t* info, void*);

    friend Linda::Tuple waitForIt(const Linda::Pattern& pattern, char type, std::chrono::microseconds curr_timeout);
};

static Linda::Tuple find(const Linda::Pattern& pattern, const std::string& file_path, bool remove) {
    int fd = open(file_path.c_str(), O_CREAT | O_RDWR, 0666);
    if (fd < 0) {
        std::string errno_msg = strerror(errno);
        throw Linda::Exception::TupleSpaceException("Could not open tuple file. " + errno_msg);
    }

    unsigned char read_buffer[Linda::ENTRY_SIZE];
    memset(&read_buffer, 0, sizeof(read_buffer));

    //set lock
    struct flock lock{};
    memset(&lock, 0, sizeof(lock));
    lock.l_type = F_WRLCK | F_RDLCK;
    lock.l_whence = SEEK_CUR;
    lock.l_len = Linda::ENTRY_SIZE;


    while (::read(fd, read_buffer, Linda::ENTRY_SIZE) == Linda::ENTRY_SIZE) {
        if (!remove) {    //lock can be released immediately if record is not going to be modified
            lock.l_type = F_UNLCK;
            if (fcntl(fd, F_SETLKW, &lock) == -1) {
                close(fd);
                throw Linda::Exception::TupleSpaceException("Error releasing file lock");
            }
        }
        if (read_buffer[0] == Linda::BUSY_FLAG) {
            std::vector<ISerializable::serialization_type> tuple_vec;
            unsigned long i = 1;
            auto c = read_buffer[i];
            while (c != Linda::Tuple::SerializationCodes::END && i < Linda::ENTRY_SIZE - 1) {
                c = read_buffer[i];
                tuple_vec.push_back(c);
                i++;
            }
            Linda::Tuple t;
            t.deserialize(tuple_vec);
            if (pattern.check(t)) {
                if (remove) {
                    //go back one record
                    lseek(fd, -Linda::ENTRY_SIZE, SEEK_CUR);
                    //set flag to empty
                    char busy = Linda::EMPTY_FLAG;
                    write(fd, &busy, sizeof(char));
                    //release lock
                    lock.l_type = F_UNLCK;
                    if (fcntl(fd, F_SETLKW, &lock) == -1) {
                        close(fd);
                        throw Linda::Exception::TupleSpaceException("Error releasing file lock");
                    }
                }
                close(fd);
                return t;
            }
        }
        if (remove) { //release lock for modifying operation
            lock.l_type = F_UNLCK;
            if (fcntl(fd, F_SETLKW, &lock) == -1) {
                close(fd);
                throw Linda::Exception::TupleSpaceException("Error releasing file lock");
            }
        }
    }
    return Linda::Tuple();
}

static void enqueue(Linda::Pattern pattern, const std::string& file_path, const char& type) {
    //todo add process entry to first empty space
    pid_t pid = getpid();
    auto serialized_pattern = pattern.serialize();
    unsigned char buffer[Linda::MAX_TUPLE_SIZE + Linda::LIST_HEADER_SIZE + 1];
    memset(buffer, (int) '-', Linda::MAX_TUPLE_SIZE + Linda::LIST_HEADER_SIZE + 1);

    buffer[0] = type;
    memcpy(buffer + 1, &pid, sizeof(pid));
    for (unsigned long i = 0; i < serialized_pattern.size(); i++) {
        buffer[i + Linda::LIST_HEADER_SIZE] = serialized_pattern[i];
    }
    buffer[Linda::LIST_HEADER_SIZE + Linda::MAX_TUPLE_SIZE] = '\n'; //for readability

    int queue_fd = open(file_path.c_str(), O_CREAT | O_APPEND | O_WRONLY, 0666);
    if (queue_fd < 0) {
        std::string errno_msg = strerror(errno);
        throw Linda::Exception::TupleSpaceException("Error when opening queue file. " + errno_msg);
    }

    //lock whole file
    struct flock lock{};
    memset(&lock, 0, sizeof(lock));
    lock.l_type = F_WRLCK | F_RDLCK;
    lock.l_whence = SEEK_SET;

    if (fcntl(queue_fd, F_SETLKW, &lock) == -1) {
        close(queue_fd);
        std::string errno_msg = strerror(errno);
        throw Linda::Exception::TupleSpaceException("Could not acquire queue file lock. " + errno_msg);
    }
    auto written = write(queue_fd, buffer, Linda::MAX_TUPLE_SIZE + Linda::LIST_HEADER_SIZE + 1);
    if (written < 0) {
        lock.l_type = F_UNLCK;
        if (fcntl(queue_fd, F_SETLKW, &lock) == -1) {
            close(queue_fd);
            std::string errno_msg = strerror(errno);
            throw Linda::Exception::TupleSpaceException("Error releasing file lock. " + errno_msg);
        }
        std::string errno_msg = strerror(errno);
        throw Linda::Exception::TupleSpaceException("Error writing process to queue. " + errno_msg);
    }
    //release lock after writing
    lock.l_type = F_UNLCK;
    if (fcntl(queue_fd, F_SETLKW, &lock) == -1) {
        close(queue_fd);
        throw Linda::Exception::TupleSpaceException("Error releasing file lock");
    }
    close(queue_fd);
}

static void dequeue(const std::vector<std::string>& queue_paths) {
    pid_t pid = getpid();
    char buffer[Linda::LIST_HEADER_SIZE];
    memset(buffer, 0, Linda::LIST_HEADER_SIZE);

    //locking whole file
    struct flock lock{};

    for (auto file_path : queue_paths) {
        int fd = open(file_path.c_str(), O_CREAT | O_RDWR, 0666);
        if (fd < 0) {
            std::string errno_msg = strerror(errno);
            throw Linda::Exception::TupleSpaceException("Error when opening queue file. " + errno_msg);
        }

        memset(&lock, 0, sizeof(lock));
        lock.l_type = F_WRLCK | F_RDLCK;
        lock.l_whence = SEEK_SET;
        if (fcntl(fd, F_SETLKW, &lock) == -1) {
            close(fd);
            std::string errno_msg = strerror(errno);
            throw Linda::Exception::TupleSpaceException("Could not acquire queue file lock. " + errno_msg);
        }

        while (::read(fd, buffer, Linda::LIST_HEADER_SIZE) == Linda::LIST_HEADER_SIZE) {
            pid_t temp;
            memcpy(&temp, buffer + 1, sizeof(temp));
            if (buffer[0] != Linda::EMPTY_FLAG && temp == pid) {
                lseek(fd, -static_cast<long>(Linda::LIST_HEADER_SIZE), SEEK_CUR);
                char empty = Linda::EMPTY_FLAG;
                if (write(fd, &empty, sizeof(char)) < 0) {
                    lock.l_type = F_UNLCK;
                    if (fcntl(fd, F_SETLKW, &lock) == -1) {
                        close(fd);
                        std::string errno_msg = strerror(errno);
                        throw Linda::Exception::TupleSpaceException("Could not acquire queue file lock. " + errno_msg);
                    }
                    close(fd);
                    std::string errno_msg = strerror(errno);
                    throw Linda::Exception::TupleSpaceException("Error writing to file. " + errno_msg);
                }
                close(fd);
                return; //there is only one process entry possible - we don't do no threads
            }
        }
        //unlock and move onto the next one
        lock.l_type = F_UNLCK;
        if (fcntl(fd, F_SETLKW, &lock) == -1) {
            close(fd);
            std::string errno_msg = strerror(errno);
            throw Linda::Exception::TupleSpaceException("Could not acquire queue file lock. " + errno_msg);
        }
        close(fd);
    }
}

static void searchQueue(const Linda::Tuple& tuple, const std::string& path, int depth, int idx) {
    //todo search queue from the end of file
    int fd = open(path.c_str(), O_RDWR | O_CREAT, 0666);
    if (fd < 0) {
        throw Linda::Exception::TupleSpaceException("Could not open queue file.");
    }

    //lock whole file
    struct flock lock{};
    memset(&lock, 0, sizeof(lock));
    lock.l_type = F_WRLCK | F_RDLCK;
    lock.l_whence = SEEK_SET;

    unsigned char buffer[Linda::MAX_TUPLE_SIZE + Linda::LIST_HEADER_SIZE];
    if (fcntl(fd, F_SETLKW, &lock) == -1) {
        close(fd);
        std::string errno_msg = strerror(errno);
        throw Linda::Exception::TupleSpaceException("Could not acquire queue file lock. " + errno_msg);
    }

    while (::read(fd, buffer, Linda::MAX_TUPLE_SIZE + Linda::LIST_HEADER_SIZE) > 0) {
        unsigned char type = buffer[0];
        if (type == Linda::READ_FLAG || type == Linda::INPUT_FLAG) {
            pid_t pid;
            memcpy(&pid, buffer + 1, sizeof(pid_t));

            std::vector<ISerializable::serialization_type> tuple_vec;
            unsigned long i = Linda::LIST_HEADER_SIZE;
            auto c = buffer[i];
            while (c != Linda::Pattern::SerializationCodes::END && i < Linda::ENTRY_SIZE - 1) {
                c = buffer[i];
                tuple_vec.push_back(c);
                i++;
            }
            Linda::Pattern p;
            p.deserialize(tuple_vec);
            if (p.check(tuple)) {
                lseek(fd, -static_cast<long>(Linda::MAX_TUPLE_SIZE + Linda::LIST_HEADER_SIZE), SEEK_CUR);
                unsigned char flag = Linda::EMPTY_FLAG;
                if (write(fd, &flag, sizeof(flag)) < 0) {
                    lock.l_type = F_UNLCK;
                    if (fcntl(fd, F_SETLKW, &lock) == -1) {
                        close(fd);
                        std::string errno_msg = strerror(errno);
                        throw Linda::Exception::TupleSpaceException("Error releasing file lock. " + errno_msg);
                    }
                    std::string errno_msg = strerror(errno);
                    close(fd);
                    throw Linda::Exception::TupleSpaceException(
                            "Error while browsing through the process queue. " + errno_msg);
                }
                Linda::Signal::notify(pid, depth, idx);
                if (type == Linda::INPUT_FLAG) {
                    break;
                }
            }
        }

    }
    lock.l_type = F_UNLCK;
    if (fcntl(fd, F_SETLKW, &lock) == -1) {
        close(fd);
        std::string errno_msg = strerror(errno);
        throw Linda::Exception::TupleSpaceException("Could not release queue file lock. " + errno_msg);
    }
    close(fd);
}

static Linda::Tuple get(const Linda::Pattern& pattern, const std::string& tuple_dir, int depth, int idx, bool remove) {
    auto all_paths = pattern.all_paths();
    std::string path;
    for (auto& p : all_paths) {
        if (p.size() == (unsigned long) depth) {
            path = p;
            break;
        }
    }
    std::string filepath = tuple_dir + "/" + path + ".linda";
    int fd = open(filepath.c_str(), O_RDWR);
    if (fd < 0) {
        std::string error_msg = strerror(errno);
        throw Linda::Exception::TupleSpaceException("Could not open tuple file. " + error_msg);
    }
    lseek(fd, static_cast<long>( Linda::ENTRY_SIZE * idx), SEEK_SET);

    unsigned char buffer[Linda::ENTRY_SIZE];
    memset(buffer, 0, Linda::ENTRY_SIZE);

    struct flock lock{};
    memset(&lock, 0, sizeof(lock));
    lock.l_type = F_WRLCK | F_RDLCK;
    lock.l_whence = SEEK_CUR;
    lock.l_len = Linda::LIST_ENTRY_SIZE;

    if (fcntl(fd, F_SETLKW, &lock) == -1) {
        close(fd);
        std::string errno_msg = strerror(errno);
        throw Linda::Exception::TupleSpaceException("Could not acquire queue file lock. " + errno_msg);
    }

    //read tuple
    if (::read(fd, buffer, Linda::ENTRY_SIZE) == Linda::ENTRY_SIZE) {
        if (buffer[0] == Linda::BUSY_FLAG) {
            std::vector<ISerializable::serialization_type> tuple_vec;
            unsigned long i = 1;
            auto c = buffer[i];
            while (c != Linda::Tuple::SerializationCodes::END && i < Linda::ENTRY_SIZE - 1) {
                c = buffer[i];
                tuple_vec.push_back(c);
                i++;
            }
            Linda::Tuple t;
            t.deserialize(tuple_vec);
            if (pattern.check(t)) {
                if (remove) {
                    lseek(fd, -static_cast<long>( Linda::ENTRY_SIZE), SEEK_CUR);
                    if (write(fd, &Linda::EMPTY_FLAG, sizeof(Linda::EMPTY_FLAG)) < 0) {
                        lock.l_type = F_UNLCK;
                        if (fcntl(fd, F_SETLKW, &lock) == -1) {
                            close(fd);
                            throw Linda::Exception::TupleSpaceException("Error releasing file lock");
                        }
                        close(fd);
                        throw Linda::Exception::TupleSpaceException("Could not write to tuple file");
                    }
                }

                lock.l_type = F_UNLCK;
                if (fcntl(fd, F_SETLKW, &lock) == -1) {
                    close(fd);
                    throw Linda::Exception::TupleSpaceException("Error releasing file lock");
                }
                close(fd);
                return t;
            }
        }
    } else {
        lock.l_type = F_UNLCK;
        if (fcntl(fd, F_SETLKW, &lock) == -1) {
            close(fd);
            throw Linda::Exception::TupleSpaceException("Error releasing file lock");
        }
        close(fd);
        std::string errno_msg = strerror(errno);
        throw Linda::Exception::TupleSpaceException("Error while reading from file. " + errno_msg);
    }
    lock.l_type = F_UNLCK;
    if (fcntl(fd, F_SETLKW, &lock) == -1) {
        close(fd);
        throw Linda::Exception::TupleSpaceException("Error releasing file lock");
    }
    close(fd);
    return Linda::Tuple();
}

static void signalHandler(int, siginfo_t* info, void*) {
    int encodedValue = info->si_int, depth, index;
    Linda::Signal::decode(encodedValue, depth, index);

    State& state = State::getInstance();
    state.depth = depth;
    state.index = index;
}

void registerHandler() {
    struct sigaction act = {};
    act.sa_sigaction = signalHandler;
    act.sa_flags = SA_SIGINFO;
    sigaction(Linda::SIGTUPLE, &act, nullptr);
}

static Linda::Tuple waitForIt(const Linda::Pattern& pattern, char type, std::chrono::microseconds curr_timeout) {
    std::chrono::time_point<std::chrono::system_clock> begin_t = std::chrono::system_clock::now();
    State& state = State::getInstance();

    sigset_t sigset;
    sigemptyset(&sigset);
    sigaddset(&sigset, Linda::SIGTUPLE);

    siginfo_t siginfo{};
    memset(&siginfo, 0, sizeof(siginfo_t));

    while (true) {
        std::cout << "Sleeping" << std::endl;
        auto used_time = std::chrono::duration_cast<std::chrono::microseconds>(
                std::chrono::system_clock::now() - begin_t);
        std::chrono::microseconds timeout = curr_timeout - used_time;
        if (timeout.count() >= 0) {
            usleep(timeout.count());
            //block signals
            sigprocmask(SIG_BLOCK, &sigset, nullptr);

            if (state.depth == 0) {
                sigprocmask(SIG_UNBLOCK, &sigset, nullptr);
                break;
            }
            bool rm_flag = type == Linda::READ_FLAG ? false : true;
            Linda::Tuple t = get(pattern, state.tupleSpacePath, state.depth, state.index, rm_flag);
            if (t.size() > 0) {
                sigprocmask(SIG_UNBLOCK, &sigset, nullptr);
                return t;
            }
            auto paths = pattern.all_paths();
            std::string path;
            for (auto& p : paths) {
                if (p.size() == state.depth) {
                    path = p;
                }
            }
            enqueue(pattern, state.tupleSpacePath + "/" + path + "-queue.linda", type);
            sigprocmask(SIG_UNBLOCK, &sigset, nullptr);
        }
    }
    return Linda::Tuple();
}

namespace Linda {

    void create(bool no_exist_err, const std::string& path, const std::string& name) {
        const std::filesystem::path tuplespace_path{path + "/" + name};

        if (std::filesystem::exists(tuplespace_path)) {
            if (no_exist_err) {
                return;
            }
            throw Linda::Exception::TupleSpaceCreateException("\"" + name + "\" already exists in " + path);
        } else {
            try {
                std::filesystem::create_directories(tuplespace_path);
            } catch (std::filesystem::filesystem_error& ex) {
                std::string msg = "Filesystem error. ";
                msg.append(ex.what());
                throw Linda::Exception::TupleSpaceCreateException(msg);
            }
        }
        std::ofstream file(tuplespace_path.string() + "/.tuplespace");
        file.close();
    }

    void connect(const std::string& path) {
        if (std::filesystem::exists(path)) {
            if (std::filesystem::exists(path + "/.tuplespace")) {
                State& state = State::getInstance();
                state.tupleSpacePath = path;
                state.connected = true;
            } else {
                throw Linda::Exception::TupleSpaceConnectException(" \"" + path + "\" not a tuplespace.");
            }
        } else {
            throw Linda::Exception::TupleSpaceConnectException(" \"" + path + "\" does not exist.");
        }
    }

    void output(Tuple tuple) {
        if (tuple.size() == 0) {
            throw Linda::Exception::TupleSpaceException("Empty tuple");
        }
        if (tuple.getSerializedLength() > MAX_TUPLE_SIZE) {
            throw Linda::Exception::TupleSpaceException("Exceeding max length after serialization");
        }
        if (!State::getInstance().connected) {
            throw Linda::Exception::TupleSpaceConnectException("Not connected to any tuplespace.");
        }
        State& state = State::getInstance();

        //find file
        //fixme can this cause process to hang?
        std::string filePath = state.tupleSpacePath + "/" + tuple.path() + ".linda";
        int flags = O_CREAT | O_RDWR;
        int fd = open(filePath.c_str(), flags, 0666);
        if (fd < 0) {
            std::cerr << strerror(errno) << std::endl;
            throw Linda::Exception::TupleSpaceException("Could not open tuple file");
        }

        //prep read buffer
        unsigned char read_buffer[Linda::ENTRY_SIZE];
        memset(&read_buffer, (int) '-', sizeof(read_buffer));

        //lock file
        struct flock lock{};
        memset(&lock, 0, sizeof(lock));
        lock.l_type = F_WRLCK | F_RDLCK;
        lock.l_whence = SEEK_SET;
        lock.l_start = 0;
        lock.l_len = 0; //lock whole file

        if (fcntl(fd, F_SETLKW, &lock) == -1) {
            close(fd);
            std::cerr << strerror(errno) << std::endl;
            throw Linda::Exception::TupleSpaceException("Could not acquire file lock");
        }

        //todo add max file size check
        int idx = 0;
        ssize_t bytes_read;
        bytes_read = ::read(fd, read_buffer, Linda::ENTRY_SIZE);
        while (bytes_read == Linda::ENTRY_SIZE) {
            if (read_buffer[0] == Linda::EMPTY_FLAG) {
                lseek(fd, -static_cast<long>(ENTRY_SIZE), SEEK_CUR);
                break;
            }
            idx++;
            bytes_read = ::read(fd, read_buffer, Linda::ENTRY_SIZE);
        }

        auto serialized = tuple.serialize();
        unsigned char entry[Linda::ENTRY_SIZE];
        memset(entry, int('-'), sizeof(entry));
        entry[0] = Linda::BUSY_FLAG;
        for (unsigned long i = 0; i < serialized.size(); i++) {
            entry[i + 1] = serialized[i];
        }
        entry[Linda::ENTRY_SIZE - 1] = '\n';

        lock.l_type = F_UNLCK;
        if (write(fd, entry, Linda::ENTRY_SIZE) == -1) {
            if (fcntl(fd, F_SETLKW, &lock) == -1) {
                close(fd);
                std::cerr << strerror(errno) << std::endl;
                throw Linda::Exception::TupleSpaceException("Error releasing file lock");
            }
            std::cerr << strerror(errno) << std::endl;
            throw Linda::Exception::TupleSpaceException("Couldn't write tuple to file");
        }

        //unlock
        if (fcntl(fd, F_SETLKW, &lock) == -1) {
            close(fd);
            throw Linda::Exception::TupleSpaceException("Error releasing file lock");
        }
        close(fd);

        std::string queue_path = state.tupleSpacePath + "/" + tuple.path() + "-queue.linda";
        searchQueue(tuple, queue_path, tuple.size(), idx);
    }

    //to jest usuwające
    Tuple input(Pattern pattern, std::chrono::microseconds timeout) {
        registerHandler();
        std::chrono::time_point<std::chrono::system_clock> begin_t = std::chrono::system_clock::now();
        if (pattern.getSerializedLength() > MAX_TUPLE_SIZE) {
            throw Linda::Exception::TupleSpaceException("Exceeding max length after serialization");
        }
        if (pattern.size() == 0) {
            throw Linda::Exception::TupleSpaceException("Empty pattern");
        }
        if (!State::getInstance().connected) {
            throw Linda::Exception::TupleSpaceConnectException("Not connected to any tuplespace.");
        }
        State& state = State::getInstance();

        std::vector<std::string> enqeued_in;
        try {
            const std::vector<std::string>& node_paths = pattern.all_paths();
            for (auto& path : node_paths) {
                std::string node_path = state.tupleSpacePath + "/" + path + ".linda";
                Tuple t = find(pattern, node_path, true);
                if (t.size() > 0) {
                    //tuple found, remove from all kłełełes and return
                    dequeue(enqeued_in);
                    return t;
                }
                std::string queue_path = state.tupleSpacePath + "/" + path + "-queue.linda";;
                enqueue(pattern, queue_path, Linda::INPUT_FLAG);
                enqeued_in.push_back(queue_path);
            }
        } catch (const Linda::Exception::TupleSpaceException& ex) {
            //todo remove process from kłełełes and re-throw exception?
            dequeue(enqeued_in);
            throw ex;   //todo nie wiem czy to na pewno tak jak się powinno to robić
        }

        std::chrono::microseconds used_time = std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::system_clock::now() - begin_t);
        return waitForIt(pattern, Linda::INPUT_FLAG, timeout - used_time);
    }

    //to jest nieusuwające
    Tuple read(Pattern pattern, std::chrono::microseconds timeout) {
        registerHandler();
        std::chrono::time_point<std::chrono::system_clock> begin_t = std::chrono::system_clock::now();
        if (pattern.getSerializedLength() > MAX_TUPLE_SIZE) {
            throw Linda::Exception::TupleSpaceException("Exceeding max length after serialization");
        }
        if (pattern.size() == 0) {
            throw Linda::Exception::TupleSpaceException("Empty pattern");
        }
        if (!State::getInstance().connected) {
            throw Linda::Exception::TupleSpaceConnectException("Not connected to any tuplespace.");
        }
        State& state = State::getInstance();

        std::vector<std::string> enqeued_in;
        try {
            const std::vector<std::string>& node_paths = pattern.all_paths();
            for (auto& path : node_paths) {
                std::string node_path = state.tupleSpacePath + "/" + path + ".linda";
                Tuple t = find(pattern, node_path, false);
                if (t.size() > 0) {
                    //tuple found, remove from kłełełes and return
                    dequeue(enqeued_in);
                    return t;
                }
                std::string queue_path = state.tupleSpacePath + "/" + path + "-queue.linda";;
                enqueue(pattern, queue_path, Linda::READ_FLAG);
                enqeued_in.push_back(queue_path);
            }
        } catch (const Linda::Exception::TupleSpaceException& ex) {
            //todo remove process from kłełełes and re-throw exception?
            dequeue(enqeued_in);
            throw ex;   //todo nie wiem czy to na pewno tak jak się powinno to robić
        }
        std::chrono::microseconds used_time = std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::system_clock::now() - begin_t);
        return waitForIt(pattern, Linda::READ_FLAG, timeout - used_time);
    }
}